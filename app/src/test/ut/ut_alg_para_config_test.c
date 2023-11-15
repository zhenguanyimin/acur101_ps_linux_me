
#include "ut_alg_para_config_test.h"
/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS+CLI includes. */
#include "../../srv/cli/cli_if.h"

#include "../stub/stub_code.h"
#include "../../app/Detection/detection_interface.h"

static BaseType_t Cfar_Para_Cfg_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t Cfar_Para_Get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

//static sAlgoParameter_t algoPara;

static const CLI_Command_Definition_t cfar_para_config_cmd =
{
	"CfarParaCfg",
	"\r\nCfarParaCfg:<parameter><value>:\r\n config algorithm parameter\r\n",
	Cfar_Para_Cfg_cmd_handler,
	2
};

static const CLI_Command_Definition_t cfar_para_get_cmd =
{
	"CfarParaGet",
	"\r\nCfarParaGet:\r\n get algorithm parameter\r\n",
	Cfar_Para_Get_cmd_handler,
	0
};

static BaseType_t Cfar_Para_Get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
#if 0
	memset(&algoPara, 0, sizeof(algoPara));
	get_algo_parameter(&algoPara);
	snprintf(pcWriteBuffer, xWriteBufferLen,
			"\rgRangeCfarMethod = %d\r\n"
			"gDopplerCfarMethod = %d\r\n"
			"gRangeCfarThreshold[6] = { %d , %d , %d , %d , %d , %d }\r\n"
			"gDopplerCfarThreshold[6] = { %d , %d , %d , %d , %d , %d }\r\n"
			"gGlobalCfarThreshold[6] = { %d , %d , %d , %d , %d , %d }\r\n"
			"gRangeBinCutIdx[6] = { %d , %d , %d , %d , %d , %d }\r\n"
			"rangeWin = %d\r\n"
			"rangeGuard = %d\r\n"
			"dopplerWin = %d\r\n"
			"dopplerGuard = %d\r\n",
			algoPara.rangeCfarMethod,
			algoPara.dopplerCfarMethod,
			algoPara.rangeCfarThreshold[0],
			algoPara.rangeCfarThreshold[1],
			algoPara.rangeCfarThreshold[2],
			algoPara.rangeCfarThreshold[3],
			algoPara.rangeCfarThreshold[4],
			algoPara.rangeCfarThreshold[5],
			algoPara.dopplerCfarThreshold[0],
			algoPara.dopplerCfarThreshold[1],
			algoPara.dopplerCfarThreshold[2],
			algoPara.dopplerCfarThreshold[3],
			algoPara.dopplerCfarThreshold[4],
			algoPara.dopplerCfarThreshold[5],
			algoPara.globalCfarThreshold[0],
			algoPara.globalCfarThreshold[1],
			algoPara.globalCfarThreshold[2],
			algoPara.globalCfarThreshold[3],
			algoPara.globalCfarThreshold[4],
			algoPara.globalCfarThreshold[5],
			algoPara.rangeBinCutIdx[0],
			algoPara.rangeBinCutIdx[1],
			algoPara.rangeBinCutIdx[2],
			algoPara.rangeBinCutIdx[3],
			algoPara.rangeBinCutIdx[4],
			algoPara.rangeBinCutIdx[5],
			algoPara.cfarPara.rangeWin,
			algoPara.cfarPara.rangeGuard,
			algoPara.cfarPara.dopplerWin,
			algoPara.cfarPara.dopplerGuard
	);
#endif

	return pdFALSE;
}

static BaseType_t Cfar_Para_Cfg_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0, len2 = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);

	if ((len1 > 0) && (len2 > 0))
	{
		int32_t value = strtol(param2, NULL, 0);
//		cfar_parameter_configuration(param1, value, len1);
		snprintf(pcWriteBuffer, xWriteBufferLen, "OK ");
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR : command error");
	}

	return pdFALSE;
}


void ut_alg_para_config_test_init(void)
{
	FreeRTOS_CLIRegisterCommand(&cfar_para_config_cmd);
	FreeRTOS_CLIRegisterCommand(&cfar_para_get_cmd);

	return;
}
