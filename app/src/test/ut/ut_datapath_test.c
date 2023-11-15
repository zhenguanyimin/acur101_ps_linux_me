

/* Standard includes. */
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

/* FreeRTOS+CLI includes. */
#include "../../srv/cli/FreeRTOS_CLI.h"
#include "../../app/DataPath/data_path.h"

#include "../../app/Wave_Configuration/wave_config.h"
#include "../../drv/awmf_0165/awmf_0165_drv.h"


static BaseType_t datapath_output_en_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0 ;
	int type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if(len1>0)
	{
		type = strtoul(param1, NULL, 0);
		if (1==type)
		{
//			data_path_init(ADC_DATA);
//			vTaskDelay(1000/portTICK_RATE_MS );

			data_path_out_en(type/*ADC_DATA*/);
			data_path_start(DATA_PATH_ADC_TO_UDP/*ADC_DATA*/); //adc
		}else if ((2==type)||(3==type))
		{
//			data_path_init(RDMAP_DATA);
//			vTaskDelay(1000/portTICK_RATE_MS );
			data_path_out_en(type);
			data_path_start(DATA_PATH_RDMAP_TO_UDP/*RDMAP_DATA*/); //rdm
		}
		else if (4 == type)
		{
			data_path_out_en(type);
			data_path_start(DATA_PATH_LOCAL_TO_PSPL);
		} else if(5 == type){
			data_path_out_en(type);
			data_path_start(DATA_PATH_ADC_TO_UDP|DATA_PATH_RDMAP_TO_UDP);
		}

		if ((1==type)||(2==type)||(3==type)||(4==type)||(5==type))
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}else
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}
	
	return pdFALSE ;
}

static const CLI_Command_Definition_t datapath_out_enable =
{
	"DatapathOutEn",
	"\r\nDatapathOutEn <type>:\r\n type:0 for disable ,1 for adc ,2 for rdmap\r\n",
	datapath_output_en_handler,
	1
};

static const CLI_Command_Definition_t datapath_set_debug =
{
	"DatapathSetDebug",
	"\r\nDatapathOutEn <type>:\r\n type:0 for disable ,1 for adc ,2 for rdmap\r\n",
	datapath_output_en_handler,
	1
};

void ut_datapath_init(void)
{
	FreeRTOS_CLIRegisterCommand(&datapath_out_enable);
}
