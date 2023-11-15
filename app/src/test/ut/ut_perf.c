
#include "ut_protocol.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* FreeRTOS+CLI includes. */
#include "../../srv/cli/cli_if.h"

// #include "../../srv/log/log.h"


extern int udp_perf_client(char *pcWriteBuffer, size_t xWriteBufferLen);

static BaseType_t lwip_udp_perf_client_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	int ret;

	memset(pcWriteBuffer, 0, xWriteBufferLen);

	ret = udp_perf_client(pcWriteBuffer, xWriteBufferLen);
	snprintf(pcWriteBuffer + strlen(pcWriteBuffer), xWriteBufferLen - strlen(pcWriteBuffer), "ret:%d\r\n%s\r\n", ret, (0 == ret)?"OK":"ERROR");

	return pdFALSE;
}

static const CLI_Command_Definition_t lwip_udp_perf_client_cmd =
{
	"UDP_Perf_Client",
	"\r\nUDP_Perf_Client \r\n UDP perf client, server port is 5001\r\n",
	lwip_udp_perf_client_cmd_handler,
	-1
};


void ut_perf_init(void)
{
	FreeRTOS_CLIRegisterCommand(&lwip_udp_perf_client_cmd);
}

