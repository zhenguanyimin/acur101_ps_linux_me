/*
* skyfend 
* @author: chenyili@autel.com
* 
*/
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <net/if.h>

#include <linux/socket.h>
#include <linux/sockios.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <mqueue.h>

#include "witd.h"

#define WIT_POWER_5V_NAME    "/run/gpio/USB_5V/value"
#define WIT_USB_MUX_OEN_NAME "/run/gpio/USBMUX_OEN/value"
#define WIT_USB_SEL_NAME     "/run/gpio/USB_SEL/value"

#define WIT_DEFAULT_IP "192.168.1.10"

struct witd_s
{
	char ip[16];
	char if_name[IFNAMSIZ];
	unsigned short port;
	char mq_name[128];
	char log_file_name[128];

	char myself_ip[16];
	
	mqd_t req_mq;
	mqd_t rsp_mq;

	bool poweron;
};



struct cmd_funcs_s
{
	int cmd;
	int (*f)(struct witd_s *witd, const struct wit_msg_req *req, int req_msg_size, struct wit_msg_rsp *rsp, int *rsp_msg_size);
};


static int set_pin_value(const char *pin_name, const char *value)
{
	int fd;
	int len;
	int ret;
	
	fd = open(pin_name, O_RDWR);
	if (fd < 0)
	{
		return -1;
	}

	len = write(fd, value, strlen(value));
	if (len != strlen(value))
	{
		perror("set fail");
		ret = -1;
	}
	else
	{
		ret = 0;
	}

	close(fd);
	return ret;
}

static int poweron(struct witd_s *witd, const struct wit_msg_req *req, int req_msg_size, struct wit_msg_rsp *rsp, int *rsp_msg_size)
{
	int ret;
	struct if_nameindex *if_names;
	int i;
	bool ready = false;
	
	*rsp_msg_size = sizeof(*rsp);
	rsp->req_cmd = req->req_cmd;

	ret = set_pin_value(WIT_POWER_5V_NAME, "1");
	if (ret)
	{		
		rsp->rsp_result = WIT_RSP_RESULT_HW_ERROR;
		goto error;
	}

	ret = set_pin_value(WIT_USB_MUX_OEN_NAME, "1");
	if (ret)
	{		
		rsp->rsp_result = WIT_RSP_RESULT_HW_ERROR;
		goto error;
	}
	
	ret = set_pin_value(WIT_USB_SEL_NAME, "1");
	if (ret)
	{		
		rsp->rsp_result = WIT_RSP_RESULT_HW_ERROR;
		goto error;
	}

	#if 0
	for (int i = 0; i < 60; i++)
	{
		if_names = if_nameindex();
		for (struct if_nameindex *p = if_names; p->if_index && (p->if_name != NULL); p++)
		{			
			if (0 == memcmp(p->if_name, witd->if_name, strlen(witd->if_name)))
			{
				printf("%s ready, index is:%d\n", p->if_name, p->if_index);
				ready = true;
				break;
			}
		}
		if_freenameindex(if_names);

		if (ready)
		{
			break;
		}

		sleep(1);
	}

	if (ready)
	{
		printf("WIT power on!!\n");
		rsp->rsp_result = WIT_RSP_RESULT_OK;
		rsp->param_len = 0;
	}
	else
	{
		printf("WIT: wait too much time to power on, but it not ready, power off it\n");
		set_pin_value(WIT_USB_SEL_NAME, "0");
		set_pin_value(WIT_USB_MUX_OEN_NAME, "0");
		set_pin_value(WIT_POWER_5V_NAME, "0");
		
		rsp->rsp_result = WIT_RSP_RESULT_HW_ERROR;
		rsp->param_len = 0;
	}
	#endif 
	
	rsp->rsp_result = WIT_RSP_RESULT_OK;
	rsp->param_len = 0;
	return 0;

	error:
	return -1;
}

static int poweroff(struct witd_s *witd, const struct wit_msg_req *req, int req_msg_size, struct wit_msg_rsp *rsp, int *rsp_msg_size)
{
	int ret;
	
	*rsp_msg_size = sizeof(*rsp);
	rsp->req_cmd = req->req_cmd;

	ret = set_pin_value(WIT_USB_SEL_NAME, "0");
	if (ret)
	{		
		rsp->rsp_result = WIT_RSP_RESULT_HW_ERROR;
		goto error;
	}

	ret = set_pin_value(WIT_USB_MUX_OEN_NAME, "0");
	if (ret)
	{		
		rsp->rsp_result = WIT_RSP_RESULT_HW_ERROR;
		goto error;
	}
	
	ret = set_pin_value(WIT_POWER_5V_NAME, "0");
	if (ret)
	{		
		rsp->rsp_result = WIT_RSP_RESULT_HW_ERROR;
		goto error;
	}

	sleep(1);

	printf("WIT power off!!!!\n");
	rsp->rsp_result = WIT_RSP_RESULT_OK;
	rsp->param_len = 0;
	return 0;

	error:
	return -1;
}

static int get_if_ip(const char *if_name, struct sockaddr_in *addr)
{
	int fd;
	struct ifreq ifr;
	
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0)
	{
		return -1;
	}

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, if_name, IFNAMSIZ -1);
	if (ioctl(fd, SIOCGIFADDR, &ifr) < 0)
	{
		perror("get if IP address error:");
		close(fd);
		return -1;
	}

	memcpy(addr, &ifr.ifr_ifru.ifru_addr, sizeof(struct sockaddr_in));

	printf("my ip:%s\n", inet_ntoa(addr->sin_addr));

	close(fd);
	return 0;
}

static int set_if_ip(const char *if_name, const char *ip)
{
	int fd;
	struct ifreq ifr;
	struct sockaddr_in addr;
	
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0)
	{
		return -1;
	}

	memset(&ifr, 0, sizeof(ifr));
	memset(&addr, 0, sizeof(addr));
	strncpy(ifr.ifr_ifrn.ifrn_name, if_name, IFNAMSIZ - 1);

	addr.sin_family = AF_INET;
	addr.sin_port = 0;
	inet_aton(ip, &addr.sin_addr);
	memcpy(&ifr.ifr_ifru.ifru_addr, &addr, sizeof(struct sockaddr));
	
	if (ioctl(fd, SIOCSIFADDR, &ifr) < 0)
	{
		perror("set IP address error:");
		goto error;
	}

	close(fd);
	return 0;

	error:
	close(fd);
	return -1;
}

static int is_if_exist(const char *if_name)
{
	struct if_nameindex *if_names;
	int i;
	int exist = 0;

	if_names = if_nameindex();
	for (struct if_nameindex *p = if_names; p->if_index && (p->if_name != NULL); p++)
	{			
		if (0 == memcmp(p->if_name, if_name, strlen(if_name)))
		{
			printf("%s ready, index is:%d\n", p->if_name, p->if_index);
			exist = 1;
			break;
		}
	}
	if_freenameindex(if_names);

	return exist;
}

static int if_check(const char *if_name, const char *ip)
{
	int ret;
	struct sockaddr_in s_addr;
	
	if (!is_if_exist(if_name))
	{
		return -1;
	}

	memset(&s_addr, 0, sizeof(s_addr));
	ret = get_if_ip(if_name, &s_addr);
	if (0 == ret)
	{
		return 0;
	}

	if (0 == s_addr.sin_addr.s_addr)
	{
		printf("%s has no IP, set to %s\n", if_name, ip);

		ret = set_if_ip(if_name, ip);
		if (ret)
		{
			return -1;
		}
	}

	return 0;
}

static int start_connect(struct witd_s *witd, const struct wit_msg_req *req, int req_msg_size, struct wit_msg_rsp *rsp, int *rsp_msg_size);
static int check_connect(struct witd_s *witd, const struct wit_msg_req *req, int req_msg_size, struct wit_msg_rsp *rsp, int *rsp_msg_size);
static int ready_check(struct witd_s *witd, const struct wit_msg_req *req, int req_msg_size, struct wit_msg_rsp *rsp, int *rsp_msg_size);

static struct cmd_funcs_s cmd_funcs[] = 
{
	{WIT_REQ_CMD_NET_CONNECT, start_connect},
	{WIT_REQ_CMD_NET_CHECK, check_connect},
	{WIT_REQ_CMD_POWERON, poweron},
	{WIT_REQ_CMD_POWEROFF,poweroff},
	{WIT_REQ_CMD_READY_CHECK, ready_check},
};

#define ATCMD_OK_STRING "\r\r\nOK\r\r\n"

static int tcp_connect(const char *ifname, const char *ip, unsigned short port)
{
	int ret;
	struct sockaddr_in s_addr;
	int fd;	
	
	printf("connect to %s::%d\n", ip, port);
	
	s_addr.sin_addr.s_addr = inet_addr(ip);
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = ntohs(port);
	
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
	{
		perror("socket error");
		return -1;
	}

	if (ifname)
	{
		setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen(ifname));
	}

	ret = connect(fd, (struct sockaddr *)&s_addr, sizeof(struct sockaddr));
	if (ret < 0)
	{
		close(fd);
		return -1;
	}

	return fd;
}

static int tcp_disconnect(int socketfd)
{
	close(socketfd);
	return 0;
}


static int send_atcmd(int shell_fd, const char *atcmd, char *rsp, int size)
{
	int ret;
	ssize_t all;

	printf("send:%s\n", atcmd);
	
	ret = send(shell_fd, atcmd, strlen(atcmd), 0);
	if (ret != strlen(atcmd))
	{
		return -1;
	}

	memset(rsp, 0, size);
	all = 0;
	ret = recv(shell_fd, rsp, size, 0);
	if (ret > 0)
	{
		all = ret;
		printf("got:%d\n", (int)all);
		while((all < size) && (!strstr(rsp, ATCMD_OK_STRING)))
		{
			usleep(200*1000);
			ret = recv(shell_fd, &rsp[all], size - all, MSG_DONTWAIT);
			if (ret > 0)
			{
				all += ret;
				continue;
			}

			break;
		}
	}
	
	//for debug	
	printf("got:\n");
	{
		int i;

		for (i=0;i<strlen(rsp); i++)
		{
			if (rsp[i] == '\r')
			{
				printf("\\r");
			}
			else
			{	
				if (rsp[i] == '\n')
					printf("\\n");
				
				printf("%c", rsp[i]);
			}
		}
	}
//	printf("recv:%s\n", rsp);
	
	return 0;
}

static int start_connect(struct witd_s *witd, const struct wit_msg_req *req, int req_msg_size, struct wit_msg_rsp *rsp, int *rsp_msg_size)
{
	char buffer[512];
	int ret;
	int fd;

	*rsp_msg_size = sizeof(*rsp);
	rsp->req_cmd = req->req_cmd;

	if (if_check(witd->if_name, witd->myself_ip))
	{
		rsp->rsp_result = WIT_RSP_RESULT_ERROR;
		return -1;
	}

	fd = tcp_connect(witd->if_name, witd->ip, witd->port);
	if (fd < 0)
	{
		rsp->rsp_result = WIT_RSP_RESULT_HW_ERROR;
		return -1;
	}
	
	ret = send_atcmd(fd, "atcmd at+cfun=0\n", buffer, sizeof(buffer));
	if (ret)
	{
		rsp->rsp_result = WIT_RSP_RESULT_HW_ERROR;
		goto error;
	}
	
	if (!strstr(buffer, ATCMD_OK_STRING))
	{
		rsp->rsp_result = WIT_RSP_RESULT_ERROR;
		goto error;
	}

	ret = send_atcmd(fd, "atcmd at^ddtc=1\n", buffer, sizeof(buffer));
	if (ret)
	{
		rsp->rsp_result = WIT_RSP_RESULT_HW_ERROR;
		goto error;
	}
	
	if (!strstr(buffer, ATCMD_OK_STRING))
	{
		rsp->rsp_result = WIT_RSP_RESULT_ERROR;
		goto error;
	}

	ret = send_atcmd(fd, "atcmd at+cfun=1\n", buffer, sizeof(buffer));
	if (ret)
	{
		rsp->rsp_result = WIT_RSP_RESULT_HW_ERROR;
		goto error;
	}
	
	if (!strstr(buffer, ATCMD_OK_STRING))
	{
		rsp->rsp_result = WIT_RSP_RESULT_ERROR;
		goto error;
	}

	ret = send_atcmd(fd, "atcmd at+cfun=0\n", buffer, sizeof(buffer));
	if (ret)
	{
		rsp->rsp_result = WIT_RSP_RESULT_HW_ERROR;
		goto error;
	}
	
	if (!strstr(buffer, ATCMD_OK_STRING))
	{
		rsp->rsp_result = WIT_RSP_RESULT_ERROR;
		goto error;
	}

	ret = send_atcmd(fd, "atcmd at^dfhc=1\n", buffer, sizeof(buffer));
	if (ret)
	{
		rsp->rsp_result = WIT_RSP_RESULT_HW_ERROR;
		goto error;
	}
	
	if (!strstr(buffer, ATCMD_OK_STRING))
	{
		rsp->rsp_result = WIT_RSP_RESULT_ERROR;
		goto error;
	}

	ret = send_atcmd(fd, "atcmd at+cfun=1\n", buffer, sizeof(buffer));
	if (ret)
	{
		rsp->rsp_result = WIT_RSP_RESULT_HW_ERROR;
		goto error;
	}
	
	if (!strstr(buffer, ATCMD_OK_STRING))
	{
		rsp->rsp_result = WIT_RSP_RESULT_ERROR;
		goto error;
	}
	
	rsp->rsp_result = WIT_RSP_RESULT_OK;
	rsp->param_len = 0;
	tcp_disconnect(fd);
	return 0;

error:
	tcp_disconnect(fd);
	return -1;
}


static int check_connect(struct witd_s *witd, const struct wit_msg_req *req, int req_msg_size, struct wit_msg_rsp *rsp, int *rsp_msg_size)
{
	char buffer[512];
	int ret;
	int fd;

	*rsp_msg_size = sizeof(*rsp);
	rsp->req_cmd = req->req_cmd;

	if (if_check(witd->if_name, witd->myself_ip))
	{
		rsp->rsp_result = WIT_RSP_RESULT_ERROR;
		return -1;
	}

	fd = tcp_connect(witd->if_name, witd->ip, witd->port);
	if (fd < 0)
	{
		rsp->rsp_result = WIT_RSP_RESULT_HW_ERROR;
		return -1;
	}
	
	ret = send_atcmd(fd, "atcmd at^dacs?\n", buffer, sizeof(buffer));
	if (ret)
	{
		rsp->rsp_result = WIT_RSP_RESULT_HW_ERROR;
		goto error;
	}
	
	if (!strstr(buffer, ATCMD_OK_STRING))
	{
		rsp->rsp_result = WIT_RSP_RESULT_ERROR;
		goto error;
	}

	if (strstr(buffer, "\r\r\n^DACS: 1,1\r\r\n"))
	{
		rsp->rsp_result = WIT_RSP_RESULT_CONNECTED;
	}
	else
	{
		rsp->rsp_result = WIT_RSP_RESULT_CONNECTING;
	}

	rsp->param_len = 0;
	tcp_disconnect(fd);
	return 0;

	error:
	tcp_disconnect(fd);
	return -1;
}

static int ready_check(struct witd_s *witd, const struct wit_msg_req *req, int req_msg_size, struct wit_msg_rsp *rsp, int *rsp_msg_size)
{
	char buffer[512];
	int ret;
	int fd;
	
	*rsp_msg_size = sizeof(*rsp);
	rsp->req_cmd = req->req_cmd;

	if (if_check(witd->if_name, witd->myself_ip))
	{
		rsp->rsp_result = WIT_RSP_RESULT_ERROR;
		return -1;
	}

	fd = tcp_connect(witd->if_name, witd->ip, witd->port);
	if (fd < 0)
	{
		rsp->rsp_result = WIT_RSP_RESULT_HW_ERROR;
		return -1;
	}
	
	ret = send_atcmd(fd, "atcmd at\n", buffer, sizeof(buffer));
	if (ret)
	{
		rsp->rsp_result = WIT_RSP_RESULT_HW_ERROR;
		goto error;
	}
	
	if (!strstr(buffer, ATCMD_OK_STRING))
	{
		rsp->rsp_result = WIT_RSP_RESULT_ERROR;
		goto error;
	}

	rsp->rsp_result = WIT_RSP_RESULT_OK;
	rsp->param_len = 0;
	tcp_disconnect(fd);
	return 0;

	error:
	tcp_disconnect(fd);
	return -1;
}


static void witd_loop(struct witd_s *witd)
{
	int i;
	int ret;
	struct wit_msg_req req;
	struct wit_msg_rsp rsp;
	int req_len;
	int rsp_len;
	int got;

	printf("waitting cmd...\n");
	while(1)
	{
		//recv mq
		req_len = mq_receive(witd->req_mq, (char *)&req, sizeof(req), NULL);
		if (req_len <= 0)
		{
			continue;
		}

		printf("recv cmd:%d\n", req.req_cmd);

		got = false;
		for (i=0; i<sizeof(cmd_funcs)/sizeof(cmd_funcs[0]); i++)
		{
			if (req.req_cmd == cmd_funcs[i].cmd)
			{
				got = true;
				ret = cmd_funcs[i].f(witd, &req, req_len, &rsp, &rsp_len);
				break;
			}
		}

		//send mq
		if (got)
		{
			rsp.req_cmd = req.req_cmd;
		}
		else
		{
			rsp.req_cmd = req.req_cmd;
			rsp.rsp_result = WIT_RSP_RESULT_UNSUPPORTED;
			rsp.param_len = 0;
			rsp_len = sizeof(rsp);
		}

		mq_send(witd->rsp_mq, (char *)&rsp, rsp_len, 0);
	}
}


static int create_mq(const char *req_mq_name, const char *rsp_mq_name, mqd_t *req_md, mqd_t *rsp_md)
{
	mqd_t mq;
	struct mq_attr attr;

	printf("req name:%s, rsp name:%s\n", req_mq_name, rsp_mq_name);

	attr.mq_maxmsg = WIT_MSG_CNT;
	attr.mq_msgsize = sizeof(struct wit_msg_rsp);
	mq = mq_open(rsp_mq_name, O_CREAT|O_RDWR|O_CLOEXEC, 0666, &attr); 
	if (mq == (mqd_t)-1)
	{
		perror("mq open error\n");
		return -1;
	}
	*rsp_md = mq;

	attr.mq_maxmsg = WIT_MSG_CNT;
	attr.mq_msgsize = sizeof(struct wit_msg_req);
	mq = mq_open(req_mq_name, O_CREAT|O_RDWR|O_CLOEXEC, 0666, &attr); 
	if (mq == (mqd_t)-1)
	{
		perror("mq open error\n");
		mq_unlink(rsp_mq_name);
		return -1;
	}

	*req_md = mq;

	return 0;
}

int witd_main(struct witd_s *witd)
{
	int ret;
	char req_msg_name[256];
	char rsp_msg_name[256];

	memset(req_msg_name, 0, sizeof(req_msg_name));
	memset(rsp_msg_name, 0, sizeof(rsp_msg_name));

	sprintf(req_msg_name, "/%s-req.msg", witd->mq_name);
	sprintf(rsp_msg_name, "/%s-rsp.msg", witd->mq_name);
	
	ret = create_mq(req_msg_name, rsp_msg_name, &witd->req_mq, &witd->rsp_mq);
	if (ret)
	{
		printf("create mq fail:%d, req-msg:%s, rsp-msg:%s\n", ret, req_msg_name, rsp_msg_name);
		return -1;
	}

	while(1)
	{
		witd_loop(witd);
	}

	return 0; 
}

#define APP_PARAMETERS "i:d:p:m:g:s:h"

static void usage(const char *app_name)
{
	printf("Usage: %s OPTIONS\n"
		   "wireless image transmition module control deamon\n"
		    "\t-d dest IPv4: \tthe dest ip v4 address\n"
			"\t-p port: \tthe shell console wnet port\n"
			"\t-m message-queue-name: \tthe comminication message queue name\n"			
			"\t[-i interface]: \tthe wnet interface which order a shell console\n"
			"\t[-g log-file-name]: \tthe log file name\n"
			"\t[-s ip]: \tthe IP after reboot. set to %s is not this parameter\n",
			app_name, WIT_DEFAULT_IP);
}


int main(int argc, char **argv)
{
	int ret;	
	static struct witd_s witd;

	opterr = 0;
	memset(&witd, 0, sizeof(witd));
	
	while((ret = getopt(argc, argv, APP_PARAMETERS)) != -1)
	{
		switch(ret)
		{
			case 'i':
				if (strlen(optarg) > sizeof(witd.if_name))
				{
					printf("-i parameter too long!!\n");
				}
				else
				{
					memcpy(witd.if_name, optarg, strlen(optarg));
				}
				break;
			case 'd':
				if (strlen(optarg) > sizeof(witd.ip))
				{
					printf("-d parameter too long!!\n");
				}
				else
				{
					memcpy(witd.ip, optarg, strlen(optarg));
				}
				break;	
			case 'p':
				witd.port = atoi(optarg);
				break;

			case 'm':
				if (strlen(optarg) > sizeof(witd.mq_name))
				{
					printf("-m parameter too long!!\n");
				}
				else
				{
					memcpy(witd.mq_name, optarg, strlen(optarg));
				}
				break;
			case 'g':
				if (strlen(optarg) > sizeof(witd.log_file_name))
				{
					printf("-g parameter too long!!\n");
				}
				else
				{
					memcpy(witd.log_file_name, optarg, strlen(optarg));
				}
				break;
			case 's':
				if (strlen(optarg) > sizeof(witd.myself_ip))
				{
					printf("-s parameter too long!!\n");
				}
				else
				{
					memcpy(witd.myself_ip, optarg, strlen(optarg));
				}
				break;	
			case 'h':
			case '?':
				usage(argv[0]);
				exit(0);
				break;
		}
	}

	if ((strlen(witd.ip) <= 0) || (0 == witd.port) || (strlen(witd.mq_name) <= 0))
	{
		printf("parameter error\n");
		usage(argv[0]);
		exit(0);
	}

	if (0 == strlen(witd.myself_ip))
	{
		strcpy(witd.myself_ip, WIT_DEFAULT_IP);
	}

	witd_main(&witd);
	return 0;
}



#if 0
#include <mqueue.h>
#include "witd.h"

struct cmd_and_string_s 
{
	int cmd;
	char *string;
};

struct cmd_and_string_s cmds[] = 
{
	{WIT_REQ_CMD_NET_CONNECT, "net-connect"},
	{WIT_REQ_CMD_NET_CHECK, "net-check" },
	{WIT_REQ_CMD_READY_CHECK, "ready-check"},
	{WIT_REQ_CMD_POWERON, "poweron"},
	{WIT_REQ_CMD_POWEROFF, "poweroff"},
};

static void usage(const char *app_name)
{
	printf("%s OPTION\nOPTION is\n", app_name);
	for (int i = 0; i < (sizeof(cmds)/sizeof(cmds[0])); i++)
	{
		printf("\t%s\n", cmds[i].string);
	}
}

int client_main(int argc, char **argv)
{
	int ret;
	mqd_t req_mq;
	mqd_t rsp_mq;
	int i;
	
	struct wit_msg_req req;
	struct wit_msg_rsp rsp;

	if (argc != 2)
	{
		usage(argv[0]);
		return -1;
	}
	
	printf("arg:%s\n", argv[1]);

	for (i = 0; i < (sizeof(cmds)/sizeof(cmds[0])); i++)
	{
		if ( 0 == strcmp(cmds[i].string, argv[1]))
		{
			req.req_cmd = cmds[i].cmd;
			req.param_len = 0;
			break;
		}
	}

	if (i >= (sizeof(cmds)/sizeof(cmds[0])))
	{
		usage(argv[0]);
		return -1;
	}
	
	req_mq = mq_open("/witd-req.msg", O_RDWR); 
	if (req_mq == (mqd_t)-1)
	{
		perror("mq open error\n");
		return -1;
	}
	printf("req_mq:%d\n", req_mq);

	rsp_mq = mq_open("/witd-rsp.msg", O_RDWR);
	if (rsp_mq == (mqd_t)-1)
	{
		perror("mq open error\n");
		mq_close(req_mq);
		return -1;
	}
	printf("req_mq:%d\n", req_mq);


	ret = mq_send(req_mq, (char *)&req, sizeof(req), 0);
	printf("mq send:%d\n", ret);

	while(1)
	{
		ret = mq_receive(rsp_mq, (char *)&rsp, sizeof(rsp), NULL);

		printf("mq recv:%d\n", ret);

		if (rsp.req_cmd != req.req_cmd)
		{
			continue;
		}

		if ((WIT_RSP_RESULT_OK == rsp.rsp_result) || (WIT_RSP_RESULT_CONNECTED == rsp.rsp_result))
		{
			printf("cmd:%d, Done!\n",  rsp.req_cmd);
		}
		else
		{
			printf("cmd:%d, result:%d\n", rsp.req_cmd, rsp.rsp_result);
		}

		break;
	}

	mq_close(req_mq);
	mq_close(rsp_mq);

	return 0;
}



#endif

