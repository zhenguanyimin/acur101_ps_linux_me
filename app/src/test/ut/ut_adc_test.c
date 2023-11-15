
#include "ut_adc_test.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS+CLI includes. */
#include "../../srv/cli/cli_if.h"

#include "../stub/stub_code.h"

static BaseType_t adc_mode_set_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);


static const CLI_Command_Definition_t adc_mode_set_cmd =
{
	"AdcModeSet",
	"\r\nAdcModeSet:<RegVal>:\r\n set adc work mode\r\n",
	adc_mode_set_cmd_handler,
	1
};

static BaseType_t adc_mode_set_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

	BaseType_t len1 = 0 ;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	uint32_t value = 0 ;

	// if ( len1 > 0 )
	// {
	// 	value = strtoul(param1, NULL, 0);

	// 	Xil_Out32(0X80020200, ( 0x00001600 | ( value & 0xff ) ) );
	// 	sleep(1);
	// 	Xil_Out32(0X80020200,0x000014d0);
	// 	sleep(1);
	// 	Xil_Out32(0X80020200,0x00001548);

	// 	snprintf(pcWriteBuffer, xWriteBufferLen, "OK");

	// }
	// else
	// {
	// 	snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR : command error");
	// }
	snprintf(pcWriteBuffer, xWriteBufferLen, "OK");

	return pdFALSE;
}


void ut_adc_test_init(void)
{
	FreeRTOS_CLIRegisterCommand(&adc_mode_set_cmd);

	return;
}
