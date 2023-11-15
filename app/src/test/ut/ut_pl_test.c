
#include "ut_adc_test.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS+CLI includes. */
#include "../../srv/cli/cli_if.h"
#include "../../srv/pl_test/pl_test.h"
#include "../stub/stub_code.h"

static BaseType_t pl_reg_write_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t pl_reg_read_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);


static const CLI_Command_Definition_t pl_reg_write_cmd =
{
	"WriteRegister",
	"\r\nWriteRegister:<RegAddr> <RegVal>:\r\n write PL register value\r\n",
	pl_reg_write_cmd_handler,
	2
};

static const CLI_Command_Definition_t pl_reg_read_cmd =
{
	"ReadRegister",
	"\r\nReadRegister:<RegAddr> :\r\n read PL register value\r\n",
	pl_reg_read_cmd_handler,
	1
};


static BaseType_t pl_reg_write_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0, len2 = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
	uint32_t reg_addr = 0, value = 0 ;

	if ((len1 > 0) && (len2 > 0))
	{
		reg_addr = strtoul(param1, NULL, 0);
		value = strtoul(param2, NULL, 0);

		pl_reg_write(reg_addr, value);
		snprintf(pcWriteBuffer, xWriteBufferLen, "OK");

	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR : command error");
	}

	return pdFALSE;
}

static BaseType_t pl_reg_read_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0 ;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	uint32_t reg_addr = 0, value = 0 ;

	if ( len1 > 0 )
	{
		reg_addr = strtoul(param1, NULL, 0);

		pl_reg_read(reg_addr, &value);
		snprintf(pcWriteBuffer, xWriteBufferLen, "[ 0x%x ] = 0x%x\r\n", reg_addr, value );

	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR : command error");
	}

	return pdFALSE;
}

void ut_pl_test_init(void)
{

	FreeRTOS_CLIRegisterCommand(&pl_reg_write_cmd);
	FreeRTOS_CLIRegisterCommand(&pl_reg_read_cmd);

	return;
}
