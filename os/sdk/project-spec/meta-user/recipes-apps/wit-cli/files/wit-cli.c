
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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

int main(int argc, char **argv)
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


