
#include "ut_sys_ctrl.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS+CLI includes. */
#include "../../srv/cli/cli_if.h"
#include "../../srv/log/log.h"
#include "../stub/stub_code.h"
#include "../../drv/awmf_0165/awmf_0165_drv.h"
#include "../../app/Wave_Configuration/wave_config.h"

static BaseType_t single_wave_cfg_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t target_wave_cfg_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t read_2492_register_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t write_2492_register_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t write_2442_register_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t log_lvl_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t log_port_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t lmx2492_wave_cfg_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);


static const CLI_Command_Definition_t sgl_wave_cfg_cmd =
{
	"SglWaveCfg",
	"\r\nSglWaveCfg <frequencyPoint>:\r\n Single frequency point waveform configuration\r\n",
	single_wave_cfg_handler,
	1
};

static const CLI_Command_Definition_t tgt_wave_cfg_cmd =
{
	"TgtWaveCfg",
	"\r\nTgtWaveCfg <isTriggerMode>:\r\n Target frequency point waveform configuration\r\n",
	target_wave_cfg_handler,
	1
};

static const CLI_Command_Definition_t lmx2492_wave_cfg_cmd =
{
	"lmx2492",
	"\r\nlmx2492 <wave file>:\r\n lmx2492 frequency point waveform configuration\r\n",
	lmx2492_wave_cfg_handler,
	1
};

static const CLI_Command_Definition_t get_2492_reg_cmd =
{
	"LMXSglRegGet",
	"\r\nLMXSglRegGet <register>:\r\n Get single LMX2492 register\r\n",
	read_2492_register_handler,
	1
};

static const CLI_Command_Definition_t set_2492_reg_cmd =
{
	"LMXSglRegSet",
	"\r\nLMXSglRegSet <register> <value>:\r\n Set single LMX2492 register\r\n",
	write_2492_register_handler,
	2
};

static const CLI_Command_Definition_t set_2442_reg_cmd =
{
	"CHCRegSet",
	"\r\nCHCRegSet <value>:\r\n Set CHC2442 register\r\n",
	write_2442_register_handler,
	1
};

static const CLI_Command_Definition_t log_lvl_command =
{
    "log_lvl",
    "\r\nlog_lvl (level):\r\n Set or get the log level\r\n",
    log_lvl_command_handler,
    -1
};

static const CLI_Command_Definition_t log_port_command =
{
    "log_port",
    "\r\nlog_port:\r\n Set the log port\r\n",
	log_port_command_handler,
    1
};

static BaseType_t single_wave_cfg_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	uint8_t fre_point = 0;

	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		fre_point = strtoul(param1, NULL, 0);
		if (fre_point >= 0 && fre_point < 3)
		{
			(void)single_frequency_waveform_configuration(fre_point);
			snprintf(pcWriteBuffer, xWriteBufferLen, "The single-frequency point waveform configuration successfully");
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "The single-frequency point waveform configuration failed");
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "The single-frequency point waveform configuration failed");
	}

	return pdFALSE;

}

static BaseType_t target_wave_cfg_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	bool isTrigger = false;

	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		isTrigger = (bool)strtoul(param1, NULL, 0);

		(void)target_waveform_configuration(isTrigger);

		snprintf(pcWriteBuffer, xWriteBufferLen, "Target waveform configuration successfully");
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "Target waveform configuration failed");
	}

	return pdFALSE;
}

#include "../../common/table_2d/table_2d.h"
static BaseType_t lmx2492_wave_cfg_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	bool isTrigger = false;

	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		char strName[256] = { 0 };
		snprintf( strName, sizeof(strName), "/home/root/conf/lmx2492/%s.txt", param1 );	
		printf( "file %s\n", strName );
		struct table_2d* psWaveCfg = file_to_table_2d( strName );
		if( psWaveCfg )
		{
			if( 1 < psWaveCfg->uRowNum )
			{
				for( uint32_t i = 0; psWaveCfg->uLineNum > i; ++i )
				{
					set_single_2492_register( psWaveCfg->aiTable[psWaveCfg->uRowNum*i], psWaveCfg->aiTable[psWaveCfg->uRowNum*i + 1] );
				}

				snprintf(pcWriteBuffer, xWriteBufferLen, "lmx2492 waveform configuration successfully");
			}
			else
			{
				snprintf(pcWriteBuffer, xWriteBufferLen, "lmx2492 waveform file error");
			}

			table_2d_free( psWaveCfg );
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "lmx2492 waveform file error");
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "lmx2492 waveform configuration failed");
	}

	return pdFALSE;
}

static BaseType_t read_2492_register_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	uint8_t value = 0;

	BaseType_t len1 = 0;
	uint16_t regAddr = 0;

	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		regAddr = strtoul(param1, NULL, 0);

		value = get_single_2492_register(regAddr);

		snprintf(pcWriteBuffer, xWriteBufferLen, "LMX2492 register 0x%x value: 0x%x", regAddr, value);
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "Read LMX2492 register failed");
	}

	return pdFALSE;
}


static BaseType_t write_2492_register_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	BaseType_t len2 = 0;
	uint16_t regAddr = 0;
	uint8_t value = 0;

	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);

	if (len1 > 0 && len2 > 0)
	{
		regAddr = strtoul(param1, NULL, 0);
		value = strtoul(param2, NULL, 0);

		set_single_2492_register(regAddr, value);

		snprintf(pcWriteBuffer, xWriteBufferLen, "Set LMX2492 register 0x%x to value: 0x%x", regAddr, value);
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "Set LMX2492 register failed");
	}

	return pdFALSE;
}

static BaseType_t write_2442_register_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	uint32_t value = 0;

	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		value = strtoul(param1, NULL, 0);

		set_chc2442_value(value);

		snprintf(pcWriteBuffer, xWriteBufferLen, "Set CHC2442 register to value: 0x%x", value);
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "Set CHC2442 register failed");
	}

	return pdFALSE;
}

static BaseType_t log_lvl_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
    BaseType_t len1;
    uint32_t lvl;
    const char *lvl_str = NULL;
    const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

    // if (len1 > 0)
    // {
    //     lvl = strtoul(param1, NULL, 0);
    //     log_set_lvl((log_level_t)lvl);
    //     snprintf(pcWriteBuffer, xWriteBufferLen, "\r\nOK\r\n");
    // }
    // else
    // {
    //     lvl = (uint32_t)log_get_lvl_num();
    //     lvl_str = log_get_lvl_str();
    //     snprintf(pcWriteBuffer, xWriteBufferLen, "\r\n%u: %s\r\n", lvl, lvl_str);
    // }

    return pdFALSE;
}

// extern volatile uint8_t byFlagLogPort;
static BaseType_t log_port_command_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	uint32_t value = 0;

	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	// if (len1 > 0)
	// {
	// 	value = strtoul(param1, NULL, 0);

	// 	switch( value )
	// 	{
	// 		case 0:
	// 			byFlagLogPort = 0;
	// 			snprintf(pcWriteBuffer, xWriteBufferLen, "LOG PORT NULL");
	// 			break;
	// 		case 1:
	// 			byFlagLogPort = 1;
	// 			snprintf(pcWriteBuffer, xWriteBufferLen, "LOG PORT UART 0");
	// 			break;
	// 		case 2:
	// 			byFlagLogPort = 2;
	// 			snprintf(pcWriteBuffer, xWriteBufferLen, "LOG PORT UDP 192.168.235.55:8080");
	// 			break;
	// 		default:
	// 			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	// 			break;
	// 	}
	// }
	// else
	// {
	// 	snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	// }

	return pdFALSE;
}

void ut_sys_ctrl_init(void)
{
	FreeRTOS_CLIRegisterCommand(&sgl_wave_cfg_cmd);
	FreeRTOS_CLIRegisterCommand(&tgt_wave_cfg_cmd);
	FreeRTOS_CLIRegisterCommand(&lmx2492_wave_cfg_cmd);
	FreeRTOS_CLIRegisterCommand(&get_2492_reg_cmd);
	FreeRTOS_CLIRegisterCommand(&set_2492_reg_cmd);
	FreeRTOS_CLIRegisterCommand(&set_2442_reg_cmd);

	FreeRTOS_CLIRegisterCommand(&log_lvl_command);
	FreeRTOS_CLIRegisterCommand(&log_port_command);

	return;
}

