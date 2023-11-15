
#include <stdio.h>

#include "ut_wit_test.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* FreeRTOS+CLI includes. */
#include "../../srv/cli/FreeRTOS_CLI.h"
#include "../../srv/wit/wit_service.h"


static BaseType_t wit_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

static const CLI_Command_Definition_t wit_cmd =
{
	"wit",
	"\r\nwit <action>:\r\n wireless image trasition module test\r\n"
	"action: \r\n"
	"\tenable: enable wit, it will auto connnect\r\n"
	"\tdisable: disable wit and poweroff it\r\n"
	"\tstatus: get status\r\n",
	wit_cmd_handler,
	1
};

static BaseType_t wit_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len = 0;
	int ret;
	WIT_STATUS_t status;
	const char *action = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len);

	if (0 == strcasecmp(action, "enable"))
	{
		ret = wit_enable();
		if (0 == ret)
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "enable OK!");
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "enable ERROR!");
		}
	}
	else if (0 == strcasecmp(action, "disable"))
	{
		ret = wit_disable();
		if (0 == ret)
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "disable OK!");
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "disable ERROR!");
		}
	}
	else if (0 == strcasecmp(action, "status"))
	{
		status = wit_status();
		if (WIT_STATUS_ERROR == status)
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "WIT hardware error!!");
		}
		else if (WIT_STATUS_DISABLE == status)
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "WIT is disable!");
		}
		else if (WIT_STATUS_ENABLE_NO_CONNECT == status)
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "WIT is connecting...");
		}
		else if (WIT_STATUS_ENABLE_CONNECTED == status)
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "WIT CONNECTED!!!");
		}
		else 
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "unkown status");
		}
	}

	return pdFALSE;
}

void ut_wit_test_init(void)
{
	FreeRTOS_CLIRegisterCommand(&wit_cmd);
}

