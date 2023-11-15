

#include "ut_net_cfg.h"




/* Standard includes. */
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

/* FreeRTOS+CLI includes. */
#include "../../srv/cli/FreeRTOS_CLI.h"
#include "../../srv/net_cfg/net_cfg.h"

static BaseType_t net_cfg_enable_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0, len2 = 0;
	int type = -1, ret = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
	char flag[RADAR_NET_CFG_FLAG_LEN] = {0};
	char ip[RADAR_IP_STRING_MAX_LEN] = {0};
	char *pos = NULL;

	if(len1 > 0 && len2 > 0) {
		pos = strstr(param1, " ");
		if(NULL != pos && param2 != NULL) {
			memcpy(flag, param1, pos - param1);
			memcpy(ip, param2, strlen(param2));
			ret = radar_network_enable_flag(flag, ip);
			if(0 == ret) {
				snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
			} else {
				snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR %d", ret);
			}
		}else {
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR %d", ret);
		}

	} else {
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}
	
	return pdFALSE;
}

static BaseType_t net_cfg_get_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	int ret = -1;
	char flag[RADAR_NET_CFG_FLAG_LEN] = {0};
	char ip[RADAR_IP_STRING_MAX_LEN] = {0};

    ret = radar_network_cmd_flag_get(flag, ip);
	if(0 == ret) {
		snprintf(pcWriteBuffer, xWriteBufferLen, "%s %s", flag, ip);
	} else {
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR %d", ret);
	}

	return pdFALSE; 
}

static const CLI_Command_Definition_t net_cfg_enable_flag =
{
	"NetCfgEnable",
	"\r\nNetCfgEnable <flag> <ip>:\r\n type:dhcp or static, when it's dhcp, set ip to you default ip \r\n",
	net_cfg_enable_handler,
	2
};

static const CLI_Command_Definition_t net_cfg_flag_get =
{
	"NetCfgGet",
	"\r\nNetCfgGet\r\n none parameters request\r\n",
	net_cfg_get_handler,
	0	
};

void ut_net_cfg_init(void)
{
	FreeRTOS_CLIRegisterCommand(&net_cfg_enable_flag);
	FreeRTOS_CLIRegisterCommand(&net_cfg_flag_get);
}

