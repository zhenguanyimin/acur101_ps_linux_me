/**
 * @file net_cfg.c 
 * @date 2023.9.4
 * @author tangsongquan tangsongquan A01136/autel@autel
 *
 * @description:
 *			setting radar's network according to user. it's for radar network configuration.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>  
#include <asm/types.h>  
#include <linux/netlink.h>  
#include <linux/rtnetlink.h>  
#include <sys/ioctl.h>  
#include <linux/if.h>  
#include <pthread.h>
#include <sys/wait.h>   
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/route.h>
#include <linux/if.h>
#include <linux/if_arp.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <arpa/inet.h>

#include "../../inc/radar_error.h"
#include "../log/log.h"
#include "../../service/storage/file/storage_file.h"
#include "net_cfg.h"

#define NETWORK_NETCARD_NAME "eth0"
#define NETLINK_STATUS_NODE "/sys/class/net/eth0/carrier"
#define RADAR_CMD_MAX_LEN		128
#define NET_CFG_FHELL_FILE_NAME	"netcfg.sh"
#define NET_CFG_FILE_NAME 		"netcfg.dat"
#define NET_CFG_MODE_DHCP 		"dhcp"
#define NET_CFG_MODE_STATIC 	"static"
#define NETLINK_STATUS_MAX_LEN  20480  
#define NETLINK_STATUS_CHECK_TIME (500 * 1000) // 500MS, we'll check network status per 500ms
#define NETLINK_STATUS_CLEAN_TIME	100
//#define RADAR_NETWORK_DEFAULT_MODE NET_CFG_MODE_STATIC 
#define RADAR_NETWORK_DEFAULT_MODE NET_CFG_MODE_DHCP

typedef struct {
	char ip[RADAR_IP_STRING_MAX_LEN];
	char net_flag[RADAR_NET_CFG_FLAG_LEN];
}net_file_info;

struct net_cfg{
	bool res_rel;
	net_file_info net_user_cfg;
	int netlink_status;
	int netcard_status;
	bool network_fault;
	pthread_t 	net_pid;
	int netlink_valid_disconnect_cnt;
	pthread_mutex_t 	mutex;
	char *netcard_name;
	char netcard_ip[RADAR_IP_STRING_MAX_LEN];
	char netcard_mac[RADAR_NETCARD_MAC_MAC_LEN];
};

static struct net_cfg app_net_cfg = {0};

/*
 * @fn net_shell_file_generate
 * @param:
 * 		filename: the shell file name wanted.
 * @brief:
 *		currently we use a shell file to modify network configuration file, which located in 
 *		/etc/network/interfaces. this function is used to generate such shell file.
 */
static int net_shell_file_generate(const char *filename)
{
	int fd = -1;
	int len = 0, ret = -1;

	char *filecmd = "#!/bin/bash\n \
					 set -e\n \
					 RADAR_NETWORK_CONFIG_FILE=/etc/network/interfaces\n \
					 NETWORK_MODE=$1\n \
					 IP=$2\n \
					 sed -i '/auto eth0/,/iface eth1/{/auto eth0/!{/iface eth1/!d}}' ${RADAR_NETWORK_CONFIG_FILE}\n \
					 if [ \"${NETWORK_MODE}\" == \"dhcp\" ]; then\n \
						 sed -i '/auto eth0/a iface eth0 inet dhcp' ${RADAR_NETWORK_CONFIG_FILE}\n \
					 else\n \
						 sed -i '/auto eth0/a iface eth0 inet static \\naddress '${IP}'\\nnetmask 255.255.255.0' ${RADAR_NETWORK_CONFIG_FILE}\n \
							 fi\n \
							 /etc/init.d/networking stop\n \
							 /etc/init.d/networking start\n \
							 echo -e \"result $?\"\n \
							 if [ $? -eq 0 ];then\n \
								 exit 0\n \
							 else\n \
								 exit 1\n \
									 fi\n";

	fd = open( filename, O_RDWR | O_CREAT, 0755 );
	if(fd < 0) {
		APP_LOG_ERROR("generate network config file fail %d\n", fd);
		return fd;
	}

	len = strlen(filecmd);
	ret = write(fd, filecmd, len);
	if(ret <= 0) {
		APP_LOG_ERROR("write file error");
		return ret;
	}

	close(fd);

	return 0;
}

static int check_cfg_file(void)
{
	int ret = -1;

	ret =access(NET_CFG_FHELL_FILE_NAME, R_OK|W_OK|X_OK|F_OK);
	if(0 != ret) {
		ret = net_shell_file_generate(NET_CFG_FHELL_FILE_NAME);
	}

	return ret;
}

static int load_user_config(net_file_info *pnfi)
{
	int ret = -1;

	if(NULL == pnfi) {
		return RET_INVALID_PARAM;
	}
	ret = storage_file_read(NET_CFG_FILE_NAME, pnfi, sizeof(net_file_info));
	if(ret <= 0) {
		//APP_LOG_INFO("network user configuration empty, use default=> flag: %s, ip: %s\n", NET_CFG_MODE_STATIC, RADAR_DEFAULT_IP);
		memcpy(pnfi->ip, RADAR_DEFAULT_IP, strlen(RADAR_DEFAULT_IP)); 
		memcpy(pnfi->net_flag, RADAR_NETWORK_DEFAULT_MODE, strlen(RADAR_NETWORK_DEFAULT_MODE));
		ret = storage_file_write(NET_CFG_FILE_NAME, pnfi, sizeof(net_file_info));		
	}else {
		ret = 0;
	}

	return ret;
}

static int record_user_config(net_file_info *pnfi)
{
	int ret = -1;

	if(NULL == pnfi) {
		return RET_INVALID_PARAM;
	}
	ret = storage_file_write(NET_CFG_FILE_NAME, pnfi, sizeof(net_file_info));		

	return ret;
}

static int network_execute_config(struct net_cfg *pnc)
{
	int ret = -1;
	char exe_cmd[RADAR_CMD_MAX_LEN] = {0};

	sprintf(exe_cmd, "./%s %s %s", NET_CFG_FHELL_FILE_NAME, pnc->net_user_cfg.net_flag, pnc->net_user_cfg.ip);

	ret = system(exe_cmd);
	if(0 > ret) {
		return ret;
	}
	if(WIFEXITED(ret)) {
		ret = 0;
	}

	return ret;
}

static int net_eth_get_ipv4_addr(char *eth_name, unsigned char *ip)
{
	int ret = -1;
	int fd = -1;
	struct ifreq ifreq = {0};
	char *cip = NULL;

	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	memset(&ifreq, 0x00, sizeof(struct ifreq));
	strcpy(ifreq.ifr_name, eth_name);
	ret = ioctl(fd, SIOCGIFADDR, &ifreq);
	if (ret < 0) {
		perror("SIOCGIFADDR: ");
		goto ERROR;
	}
	if (ifreq.ifr_addr.sa_family != AF_INET) {
		ret = -1;
		goto ERROR;
	}
	struct sockaddr_in *addr = (struct sockaddr_in *)&(ifreq.ifr_addr);
	cip = inet_ntoa(addr->sin_addr);
	if(cip) {
		memcpy(ip, cip, strlen(cip));
	} else {
		APP_LOG_ERROR("convert ip fail\n");
	}

	ret = 0;

ERROR:
	close(fd);

	return ret;
}

int net_eth_get_netcard_mac(char *netcard_name, unsigned char *mac)
{
	int ret;
	int fd;
	struct ifreq ifreq;

	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	memset(&ifreq, 0x00, sizeof(struct ifreq));
	strcpy(ifreq.ifr_name, netcard_name);
	ret = ioctl(fd, SIOCGIFHWADDR, &ifreq);
	if (ret < 0) {
		perror("SIOCGIFHWADDR: ");
	}
	if (ret == 0 && ifreq.ifr_hwaddr.sa_family == ARPHRD_ETHER) {
		//memcpy(mac, ifreq.ifr_hwaddr.sa_data, 6);
		sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x",
            (unsigned char)ifreq.ifr_hwaddr.sa_data[0],
            (unsigned char)ifreq.ifr_hwaddr.sa_data[1],
            (unsigned char)ifreq.ifr_hwaddr.sa_data[2],
            (unsigned char)ifreq.ifr_hwaddr.sa_data[3],
            (unsigned char)ifreq.ifr_hwaddr.sa_data[4],
            (unsigned char)ifreq.ifr_hwaddr.sa_data[5]
            );

	} else {
		ret = -1;
	}
	close(fd);

	return ret;
}

/*
 * @fn check_config_result
 * @parma:
 * 	in netcard_name, the network card name in device, currently it's eth0
 * @note:
 *		we check configuration result by attemping to get netcard ip, if it's
 *		successful, then everything went well, or regarding fail.
 */
static int check_config_result(struct net_cfg* pcf)
{
	int ret = 0;

	ret = net_eth_get_ipv4_addr(pcf->netcard_name, pcf->netcard_ip);
	if(0 == ret) {
		APP_LOG_INFO("netcard %s's ip is %s\n", pcf->netcard_name, pcf->netcard_ip);
	}

	return ret;
}

static int get_netlink_status(void)
{
	char eth_status[2] = {0};
	int netstatus_fd = 0;
	int st = -1; 
	int retval = 0;

	netstatus_fd = open(NETLINK_STATUS_NODE, O_RDONLY);
	retval = read(netstatus_fd, eth_status, 2);
	//APP_LOG_DEBUG("ret = %d,status [0]=%02x,%c;[1]=%02x,%c", retval, eth_status[0], eth_status[0], eth_status[1], eth_status[1]);
	if(retval < 0) {
		return retval;
	}
	close(netstatus_fd);
	st = eth_status[0]-0x30;

	return st; 
}

static int clean_netlink_status(int x)
{
	int dtime = NETLINK_STATUS_CLEAN_TIME;
	int status = -1;

	do {
		status = get_netlink_status();
		if(x == status) {
			break;
		}
		usleep(1000 * 100);
		dtime--;
	} while(dtime > 0);

	return status;
}

static int check_load_user_config_by_status(struct net_cfg* pcf, int status)
{
	int ret = 0;
	net_file_info nfi = {0};
	char net_flag[RADAR_NET_CFG_FLAG_LEN] = {0};

	pthread_mutex_lock(&pcf->mutex);

	if(NETLINK_STATUS_DISABLE == status) {
		if(pcf->netlink_valid_disconnect_cnt > 0) {
			net_eth_get_ipv4_addr(pcf->netcard_name, pcf->netcard_ip);//update ip info
			pcf->netlink_status = NETLINK_STATUS_DISCONNECTED;
			pcf->netlink_valid_disconnect_cnt--; //current operation has handled one of down event.
		}
	} else {
		if(NETLINK_STATUS_DISCONNECTED == pcf->netlink_status && get_netlink_status() == NETLINK_STATUS_CONNECTED) {
			APP_LOG_DEBUG("network load=> flag: %s, ip:%s \n", pcf->net_user_cfg.net_flag, pcf->net_user_cfg.ip);
			
			app_net_cfg.network_fault = false;
			pcf->netlink_status = NETLINK_STATUS_CONNECTED;
			ret = network_execute_config(pcf);
			if(0 != ret) {
				APP_LOG_INFO("warmming!!! network configuration shell file execuate fail %d\n", ret);	
			}
			ret = check_config_result(pcf);
			if(0 != ret && strncmp(pcf->net_user_cfg.net_flag, NET_CFG_MODE_DHCP, strlen(NET_CFG_MODE_DHCP)) == 0) {
				memcpy(net_flag, pcf->net_user_cfg.net_flag, strlen(pcf->net_user_cfg.net_flag));
				memcpy(pcf->net_user_cfg.net_flag, NET_CFG_MODE_STATIC, strlen(NET_CFG_MODE_STATIC));
				ret = network_execute_config(pcf);
				memcpy(pcf->net_user_cfg.net_flag, net_flag, RADAR_NET_CFG_FLAG_LEN);
				if(0 == ret) {
					ret = check_config_result(pcf);
					if(0 != ret) {
						app_net_cfg.network_fault = true;
						APP_LOG_ERROR("config IP fail %d, flag %s, ip:%s\n", ret, NET_CFG_MODE_STATIC, pcf->net_user_cfg.ip);
					} else {
						app_net_cfg.network_fault = false;
					}
				} else {
					app_net_cfg.network_fault = true;
				}
			} else if(0 != ret) {
				app_net_cfg.network_fault = true;
			}
			/*
			 * this is extremely important to check network status with timeout after network reset.
			 * cause network reset will due to cache status in netlink node.
			 * we check the status with timeout value NETLINK_STATUS_CLEAN_TIME, whose uint is (1000 * 100 us) 
			 */
			if(clean_netlink_status(NETLINK_STATUS_CONNECTED) != NETLINK_STATUS_CONNECTED) {
				ret = -1; 
				APP_LOG_INFO("warmming!!! clean netlink status fail\n");
			}
		}
	}
	pthread_mutex_unlock(&pcf->mutex);

	return ret;
}

static void* network_monitor_thread(void *arg) 
{
	int fd = 0, retval = 0;  
	char buf[NETLINK_STATUS_MAX_LEN] = {0};
	int len = NETLINK_STATUS_MAX_LEN;
	struct sockaddr_nl addr = {0};
	struct nlmsghdr *nh = NULL;
	struct ifinfomsg *ifinfo = NULL;
	struct rtattr *attr = NULL;
	struct net_cfg *pnet_cfg = (struct net_cfg*)arg;
	fd_set readset;
	struct timeval timeout;

	if(NULL == pnet_cfg) {
		return NULL;
	}

	fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &len, sizeof(len));
	memset(&addr, 0, sizeof(addr));
	addr.nl_family = AF_NETLINK;
	addr.nl_groups = RTNLGRP_LINK;
	bind(fd, (struct sockaddr*)&addr, sizeof(addr));

	APP_LOG_DEBUG("network thread monitor loaded\n");

	while(!app_net_cfg.res_rel) {
		FD_ZERO(&readset);
		FD_SET(fd, &readset);
		timeout.tv_sec			= 0;
		timeout.tv_usec			= NETLINK_STATUS_CHECK_TIME;
		retval = select(fd + 1, &readset, NULL, NULL, &timeout);
		if(retval > 0) {
			if ((retval = read(fd, buf, NETLINK_STATUS_MAX_LEN)) > 0) {  
				for (nh = (struct nlmsghdr *)buf; NLMSG_OK(nh, retval); nh = NLMSG_NEXT(nh, retval))  {
					if (nh->nlmsg_type == NLMSG_DONE)
						break;
					else if (nh->nlmsg_type == NLMSG_ERROR)
						break;
					else if (nh->nlmsg_type != RTM_NEWLINK)
						continue;
					ifinfo = NLMSG_DATA(nh);
					if(ifinfo->ifi_flags & IFF_LOWER_UP) {
						//pnet_cfg->netlink_valNETLINK_ROUTEid_up_cnt++;
					} else {
						/**
						 * link down isn't mean that netlink is unconnected. we should check netlink status.
						 */
						if(get_netlink_status() == 0) {
							pnet_cfg->netlink_valid_disconnect_cnt++;
						}
					}

					APP_LOG_DEBUG("%u: %s(dic:%d)", ifinfo->ifi_index, (ifinfo->ifi_flags & IFF_LOWER_UP) ? "up" : "down", pnet_cfg->netlink_valid_disconnect_cnt);
					pnet_cfg->netcard_status = (ifinfo->ifi_flags & IFF_LOWER_UP) ? NETLINK_STATUS_ENABLE:NETLINK_STATUS_DISABLE;
					check_load_user_config_by_status(pnet_cfg, pnet_cfg->netcard_status);
				}
			}

		}
	}

	close(fd);

	return NULL;
}

static int network_cfg_thread_load(struct net_cfg *pnc)
{
	int ret = 0;

	ret = pthread_create(&(pnc->net_pid), NULL, network_monitor_thread, pnc);
	if(0 != ret) {
		APP_LOG_DEBUG("network status monitor thread loaded fail, ret = %d\n", ret);
		return -1;
	}

	return 0;
}

static int net_cfg_load_execute(struct net_cfg *pnc)
{
	int ret = -1;

	ret = load_user_config(&pnc->net_user_cfg);
	if(0 != ret) {
		APP_LOG_ERROR("load user config error %d\n", ret);
		return ret;
	}
	
	//only when netlink is connected, we'll config network
	pnc->network_fault = false;
	pnc->netcard_status = NETLINK_STATUS_DISABLE;
	pnc->netlink_status =  get_netlink_status();
	APP_LOG_DEBUG("initialized network, current netlink status %d\n", pnc->netlink_status);
	if(pnc->netlink_status == NETLINK_STATUS_CONNECTED) {
		pnc->netlink_status = NETLINK_STATUS_DISCONNECTED;//for update current network config.
		ret = check_load_user_config_by_status(pnc, NETLINK_STATUS_ENABLE);
		if(0 == ret) {
			pnc->netcard_status = NETLINK_STATUS_ENABLE;
			pnc->network_fault = false;
		} else {
			pnc->network_fault = true;
		}
	}

	return ret;
}

int radar_network_init(void)
{
	int ret = -1;

	memset((char*)&app_net_cfg, 0, sizeof(struct net_cfg));
	ret = check_cfg_file(); 
	if(0 != ret) {
		APP_LOG_INFO("warmming!!! cfg error %d\n", ret);
		//we'll still use the default network configuration, keep moving to next.
	}
	if( 0 != pthread_mutex_init( &(app_net_cfg.mutex), NULL ) ) {
		APP_LOG_INFO("warmming!!! initialzed network configuration thread mutex fail\n");
	}
	memset(app_net_cfg.net_user_cfg.net_flag, 0, RADAR_NET_CFG_FLAG_LEN);
	app_net_cfg.netcard_name = NETWORK_NETCARD_NAME;
	app_net_cfg.netlink_valid_disconnect_cnt = 0;
	app_net_cfg.res_rel = false;
	ret = net_eth_get_netcard_mac(app_net_cfg.netcard_name, app_net_cfg.netcard_mac);
	if(0 != ret) {
		memset(app_net_cfg.netcard_mac, 0, RADAR_NETCARD_MAC_MAC_LEN);
		APP_LOG_INFO("warmming !!! get mac error, ret = %d\n", ret);
	}

	ret = net_cfg_load_execute(&app_net_cfg);
	ret |= network_cfg_thread_load(&app_net_cfg);
	
	APP_LOG_INFO("radar network initialized finish %d\n", ret);
	//debug, force to use dhcp
	//memcpy(app_net_cfg.net_user_cfg.net_flag, NET_CFG_MODE_DHCP, strlen(NET_CFG_MODE_DHCP));

	return ret;
}

bool get_network_fault_status(void)
{
	return app_net_cfg.network_fault;
}

int get_netcard_mac(unsigned char *netcard_mac)
{
	int ret = 0;

	if(strlen(app_net_cfg.netcard_mac) == 0) {
		ret = net_eth_get_netcard_mac(app_net_cfg.netcard_name, app_net_cfg.netcard_mac);
		if(0 == ret) {
			memcpy(netcard_mac, app_net_cfg.netcard_mac, strlen(app_net_cfg.netcard_mac));
		}
	} else {
		memcpy(netcard_mac, app_net_cfg.netcard_mac, strlen(app_net_cfg.netcard_mac));
	}

	return ret;
}

void get_netcard_ip(unsigned char *netcard_ip)
{
	if(netcard_ip) {
		memcpy(netcard_ip, app_net_cfg.netcard_ip, strlen(app_net_cfg.netcard_ip));
	}
}

/*
 * @func get_netcard_status
 * @param
 *		null
 * @return
 *		NETLINK_STATUS_ENABLE: netlink enabled 
 *		NETLINK_STATUS_DISABLE netlink disabled 
 * @brief
 *		this is using radar configuration status as netlink status, netlink
 *	status includes up/down and cable connect/disconnect.
 */
int get_netcard_status(void)
{
	if(app_net_cfg.netcard_status == NETLINK_STATUS_ENABLE) {
		return NETLINK_STATUS_ENABLE;
	} else {
		return NETLINK_STATUS_DISABLE;
	}
}

/*
 * @func get_netlink_connnect_status
 * @param
 *		null
 * @return
 *		NETLINK_STATUS_DISCONNECTED:  netlink connected
 *		NETLINK_STATUS_CONNECTED  netlink unconnected
 * @brief
 *		netlink_valid_disconnect record current netlink connected status, once cable was conencted
 *		, this field will be set to 0, or it'll increase one by one.
 */
int get_netlink_connect_status(void)
{
	if(app_net_cfg.netlink_valid_disconnect_cnt > 0 || app_net_cfg.netlink_status == NETLINK_STATUS_DISCONNECTED) {
			return NETLINK_STATUS_DISCONNECTED; 
	} else {
		return NETLINK_STATUS_CONNECTED; 
	}
}

/*
 * @func radar_network_enable_flag
 * @param
 *		flag: NET_CFG_MODE_DHCP or NET_CFG_MODE_STATIC
 *		ip: ip configured by user.
 * @return
 *		0: success
 *		others: fail
 * @brief
 *		provided to user with CMD ipop. used to configurate network by parameters
 */
int radar_network_enable_flag(const char *flag, const char *ip)
{
	int ret = -1;
	net_file_info nfi = {0};

	if(NULL == flag || NULL == ip) {
		return RET_INVALID_PARAM;
	}

	if(strncmp(flag, NET_CFG_MODE_STATIC, strlen(flag)) != 0 && strncmp(flag, NET_CFG_MODE_DHCP, strlen(flag)) != 0) {
		APP_LOG_ERROR("network configuration flag(flag:%s, ip:%s) from user invalid\n", flag, ip);
		return RET_INVALID_PARAM;
	}

	memcpy(nfi.ip, ip, strlen(ip));
	memcpy(nfi.net_flag, flag, strlen(flag));

	ret = record_user_config(&nfi);
	if(0 == ret) {
		memcpy(app_net_cfg.net_user_cfg.ip, ip, strlen(ip));
		memcpy(app_net_cfg.net_user_cfg.net_flag, flag, strlen(flag));
		ret = net_cfg_load_execute(&app_net_cfg);
	}

	APP_LOG_DEBUG("enable flag = %s, ip = %s, ret = %d", nfi.net_flag, nfi.ip, ret);

	return ret;
}

/*
 * @func radar_network_cmd_flag_get
 * @param
 *		flag: NET_CFG_MODE_DHCP or NET_CFG_MODE_STATIC
 *		ip: ip currently configurated
 * @return
 *		0: success
 *		others: fail
 * @brief
 *		provided to user with CMD ipop. used to get current network configuration parameters
 */
int radar_network_cmd_flag_get(char *flag, char *ip)
{
	int ret = -1;
	net_file_info nfi = {0};

	ret = load_user_config(&nfi);
	if(0 != ret) {
		APP_LOG_ERROR("load user config error %d\n", ret);
		return ret;
	}

	memcpy(flag, nfi.net_flag, strlen(nfi.net_flag));
	memcpy(ip, nfi.ip, strlen(nfi.ip));

	APP_LOG_DEBUG("current flag = %s, ip = %s", nfi.net_flag, nfi.ip);

	return 0;
}

int radar_network_cfg_uninit(void)
{
	app_net_cfg.res_rel = true;
	pthread_join(app_net_cfg.net_pid, NULL);
	pthread_mutex_destroy(&app_net_cfg.mutex);

	return 0;
}

