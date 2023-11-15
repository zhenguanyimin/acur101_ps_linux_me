/* Standard includes. */
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

/* FreeRTOS+CLI includes. */
#include "../../srv/cli/FreeRTOS_CLI.h"
#include "../../service/monitor/power/monitor_power.h"
#include "../../service/monitor/temp/monitor_temp.h"
#include "ut_power_dissipation.h"

static BaseType_t temp_vco_get_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	int ret = -1;
	float vco = 0;

    ret = get_temp_vco(&vco);
	if(0 == ret) {
		snprintf(pcWriteBuffer, xWriteBufferLen, "vco: %f", vco);
	} else {
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR %d", ret);
	}

	return pdFALSE;
}

static BaseType_t temp_code_get_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	int ret = -1;
	char codestr[1024] = {0};

    ret = get_temp_code(codestr, sizeof(codestr));
	if(0 == ret) {
		snprintf(pcWriteBuffer, xWriteBufferLen, "%s", codestr);
	} else {
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR %d", ret);
	}

	return pdFALSE;
}

static BaseType_t temp_tx_get_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	int ret = -1;
	BaseType_t len1 = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	int uindex = 0;
	char txbuf[1024] = {0};

	if(len1 > 0 && param1) {
		uindex = strtoul(param1, NULL, 0);
		ret = get_temp_tx_fu(uindex, txbuf, sizeof(txbuf));	
		if(0 == ret) {
			snprintf(pcWriteBuffer, xWriteBufferLen, "%s", txbuf);
		} else {
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR %d", ret);
		}
	} else {
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE; 
}

static BaseType_t power_vco_get_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	int ret = -1;
	float vco = 0;

    ret = get_power_vco(&vco);
	if(0 == ret) {
		snprintf(pcWriteBuffer, xWriteBufferLen, "vco: %f", vco);
	} else {
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR %d", ret);
	}

	return pdFALSE;
}

static BaseType_t power_code_get_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	int ret = -1;
	char codestr[1024] = {0};

    ret = get_power_code(codestr, sizeof(codestr));
	if(0 == ret) {
		snprintf(pcWriteBuffer, xWriteBufferLen, "%s", codestr);
	} else {
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR %d", ret);
	}

	return pdFALSE;
}

static BaseType_t power_tx_get_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	int ret = -1;
	BaseType_t len1 = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	int uindex = 0;
	char txbuf[1024] = {0};

	if(len1 > 0 && param1) {
		uindex = strtoul(param1, NULL, 0);
		ret = get_power_tx_fu(uindex, txbuf, sizeof(txbuf));	
		if(0 == ret) {
			snprintf(pcWriteBuffer, xWriteBufferLen, "%s", txbuf);
		} else {
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR %d", ret);
		}
	} else {
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE; 
}

static const CLI_Command_Definition_t power_code_get =
{
	"PowerCodeGet",
	"\r\nPowerCodeGet\r\n none parameters requested \r\n",
	power_code_get_handler,
	0	
};

static const CLI_Command_Definition_t power_tx_get =
{
	"PowerTxGet",
	"\r\nPowerTxGet\r\n PowerTxGet <uindex>\r\nuindex: the power uint index \r\n",
	power_tx_get_handler,
	1	
};
static const CLI_Command_Definition_t power_vco_get =
{
	"PowerVcoGet",
	"\r\nPowerVcoGet\r\n PowerVcoGet \r\n",
	power_vco_get_handler,
	0	
};

static const CLI_Command_Definition_t temp_code_get =
{
	"TempCodeGet",
	"\r\nTempCodeGet\r\n none parameters requested \r\n",
	temp_code_get_handler,
	0	
};

static const CLI_Command_Definition_t temp_tx_get =
{
	"TempTxGet",
	"\r\nTempTxGet\r\n PowerTxGet <uindex>\r\nuindex: the power uint index \r\n",
	temp_tx_get_handler,
	1	
};
static const CLI_Command_Definition_t temp_vco_get =
{
	"TempVcoGet",
	"\r\nTempVcoGet\r\n PowerVcoGet \r\n",
	temp_vco_get_handler,
	0	
};

void ut_power_dissipation_init(void)
{
	FreeRTOS_CLIRegisterCommand(&power_tx_get);
	FreeRTOS_CLIRegisterCommand(&power_code_get);
	FreeRTOS_CLIRegisterCommand(&power_vco_get);
	FreeRTOS_CLIRegisterCommand(&temp_tx_get);
	FreeRTOS_CLIRegisterCommand(&temp_code_get);
	FreeRTOS_CLIRegisterCommand(&temp_vco_get);
}

