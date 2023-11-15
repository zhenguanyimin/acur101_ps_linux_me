
#include "ut_0165_drv.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS+CLI includes. */
#include "../../srv/cli/cli_if.h"

#include "../../drv/awmf_0165/awmf_0165_drv.h"
#include "../../drv/awmf_0165/awmf_0165_chip.h"
#include "../../drv/awmf_0165/awmf_0165_io.h"
#include "../../drv/awmf_0165/awmf_0165_def.h"
#include "../../app/Wave_Configuration/wave_config.h"

typedef struct awmf_name_id_map {
	uint32_t name;
	uint32_t id;
} awmf_name_id_map_t;

static BaseType_t awmf_workmode_set_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t awmf_workmode_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t awmf_reg_set_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t awmf_reg_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t awmf_temp_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t awmf_atc_mode_en_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t awmf_atc_mode_dis_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);


STATIC awmf_name_id_map_t g_awmf_name_id_map_tbl[] = {
	/* SPI1 */
	{8, AWMF_0165_TX_U08}, // U8
	/* SPI2 */
	{12, AWMF_0165_TX_U12}, // U12
	{10, AWMF_0165_TX_U10}, // U10
	{9, AWMF_0165_TX_U09}, // U9
	{11, AWMF_0165_TX_U11}, // U11
	/* SPI3 */
	{16, AWMF_0165_TX_U16}, // U16
	{14, AWMF_0165_TX_U14}, // U14
	{13, AWMF_0165_TX_U13}, // U13
	{15, AWMF_0165_TX_U15}, // U15
	/* SPI4 */
	{19, AWMF_0165_RX_U19}, // U19
	{17, AWMF_0165_RX_U17}, // U17
	{18, AWMF_0165_RX_U18}, // U18
	{20, AWMF_0165_RX_U20}, // U20
	/* SPI5 */
	{23, AWMF_0165_RX_U23}, // U23
	{21, AWMF_0165_RX_U21}, // U21
	{22, AWMF_0165_RX_U22}, // U22
	{24, AWMF_0165_RX_U24}, // U24
};

static const CLI_Command_Definition_t awmf_workmode_set_cmd =
{
	"AwmfWorkmodeSet",
	"\r\nAwmfWorkmodeSet <ChipName> <Workmode>:\r\n Set awmf0165 workmode\r\n",
	awmf_workmode_set_cmd_handler,
	2
};

static const CLI_Command_Definition_t awmf_workmode_get_cmd =
{
	"AwmfWorkmodeGet",
	"\r\nAwmfWorkmodeGet <ChipName>:\r\n Get awmf0165 workmode\r\n",
	awmf_workmode_get_cmd_handler,
	1
};

static const CLI_Command_Definition_t awmf_reg_set_cmd =
{
	"AwmfRegSet",
	"\r\nAwmfRegSet <ChipName> <RegAddr> <RegValHigh> <RegValLow>:\r\n Set awmf0165 register value\r\n",
	awmf_reg_set_cmd_handler,
	4
};

static const CLI_Command_Definition_t awmf_reg_get_cmd =
{
	"AwmfRegGet",
	"\r\nAwmfRegGet <ChipName> <RegAddrBase> [<RegAddrCnt>]:\r\n Get awmf0165 register value\r\n",
	awmf_reg_get_cmd_handler,
	-1
};
static const CLI_Command_Definition_t awmf_temp_get_cmd =
{
	"AwmfTempGet",
	"\r\nAwmfRegGet :\r\n Get awmf0165 temperature value\r\n",
	awmf_temp_cmd_handler,
	0
};


static const CLI_Command_Definition_t awmf_atc_mode_en_cmd =
{
	"AwmfAtcModeEnable",
	"\r\nAwmfRegGet :\r\n enable awmf0165 Automatic temperature compesation !\r\n",
	awmf_atc_mode_en_handler,
	0
};


static const CLI_Command_Definition_t awmf_atc_mode_dis_cmd =
{
	"AwmfAtcModeDisable",
	"\r\nAwmfRegGet :\r\n enable awmf0165 Automatic temperature compesation !\r\n",
	awmf_atc_mode_dis_handler,
	0
};

ret_code_t awmf_get_chip_id_by_name(uint32_t awmf_name, uint32_t *awmf_id)
{
	ret_code_t ret = RET_OK;
	uint32_t i = 0;

	for (i = 0; i < ARRAY_SIZE(g_awmf_name_id_map_tbl); i++)
	{
		if (awmf_name == g_awmf_name_id_map_tbl[i].name)
		{
			*awmf_id = g_awmf_name_id_map_tbl[i].id;
			break;
		}
	}

	if (i >= ARRAY_SIZE(g_awmf_name_id_map_tbl))
	{
		ret = RET_GENERAL_ERR;
	}

	return ret;
}

static BaseType_t awmf_workmode_set_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0, len2 = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
	uint32_t awmf_name = 0, workmode = 0, awmf_id = 0;
	ret_code_t ret = RET_OK;

	if ((len1 > 0) && (len2 > 0))
	{
		awmf_name = strtoul(param1, NULL, 0);
		workmode = strtoul(param2, NULL, 0);

		ret = awmf_get_chip_id_by_name(awmf_name, &awmf_id);
		if ((ret != RET_OK) || (!IS_0165_WORKMODE_VALID(workmode)))
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
		else
		{
			ret = awmf_0165_drv_workmode_set(awmf_id, workmode);
			if (ret == RET_OK)
			{
				snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
			}
			else
			{
				snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
			}
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

static BaseType_t awmf_workmode_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	uint32_t awmf_name = 0, workmode = 0, awmf_id = 0;
	ret_code_t ret = RET_OK;

	if (len1 > 0)
	{
		awmf_name = strtoul(param1, NULL, 0);

		ret = awmf_get_chip_id_by_name(awmf_name, &awmf_id);
		if (ret != RET_OK)
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
		else
		{
			ret = awmf_0165_drv_workmode_get(awmf_id, &workmode);
			printf("awmf_workmode_get_cmd_handler ret: %d, workmode: %d\n", ret, workmode);
			if (ret == RET_OK)
			{
				snprintf(pcWriteBuffer, xWriteBufferLen, "OK\r\n%u (0x%x) %u", awmf_name, awmf_id, workmode);
			}
			else
			{
				snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
			}
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

static BaseType_t awmf_reg_set_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0, len2 = 0, len3 = 0, len4 = 0;;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
	const char *param3 = FreeRTOS_CLIGetParameter(pcCommandString, 3, &len3);
	const char *param4 = FreeRTOS_CLIGetParameter(pcCommandString, 4, &len4);
	uint32_t awmf_name = 0, reg_addr = 0, reg_val_high = 0, reg_val_low = 0, awmf_id = 0;
	ret_code_t ret = RET_OK;

	if ((len1 > 0) && (len2 > 0) && (len3 > 0) && (len4 > 0))
	{
		awmf_name = strtoul(param1, NULL, 0);
		reg_addr = strtoul(param2, NULL, 0);
		reg_val_high = strtoul(param3, NULL, 0);
		reg_val_low = strtoul(param4, NULL, 0);

		ret = awmf_get_chip_id_by_name(awmf_name, &awmf_id);
		if ((ret != RET_OK) || (!IS_0165_REG_ADDR_VALID(reg_addr)))
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
		else
		{
			ret = awmf_0165_drv_reg_set(awmf_id, reg_addr, reg_val_high, reg_val_low);
			if (ret == RET_OK)
			{
				snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
			}
			else
			{
				snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
			}
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

static BaseType_t awmf_reg_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0, len2 = 0, len3 = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
	const char *param3 = FreeRTOS_CLIGetParameter(pcCommandString, 3, &len3);
	uint32_t awmf_name = 0, reg_addr_base = 0, reg_addr_cnt = 0, awmf_id = 0;
	uint32_t reg_val_high = 0, reg_val_low = 0;
	ret_code_t ret = RET_OK;
	static uint32_t reg_idx = 0;
	BaseType_t ret_val = pdFALSE;

	if ((len1 > 0) && (len2 > 0))
	{
		awmf_name = strtoul(param1, NULL, 0);
		reg_addr_base = strtoul(param2, NULL, 0);

		ret = awmf_get_chip_id_by_name(awmf_name, &awmf_id);
		if ((ret != RET_OK) || (!IS_0165_REG_ADDR_VALID(reg_addr_base)))
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
		else
		{
			if (len3 > 0)
			{
				reg_addr_cnt = strtoul(param3, NULL, 0);
			}
			else
			{
				reg_addr_cnt = 1;
			}

			if (reg_idx < reg_addr_cnt)
			{
				ret = awmf_0165_drv_reg_get(awmf_id, reg_addr_base + reg_idx, &reg_val_high, &reg_val_low);
				if (ret == RET_OK)
				{
					snprintf(pcWriteBuffer, xWriteBufferLen, "0x%x 0x%x 0x%x\r\n", reg_addr_base + reg_idx, reg_val_high, reg_val_low);
					reg_idx++;
				}
				else
				{
					snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
					reg_idx = 0;
					ret_val = pdFALSE;
				}
			}

			if (reg_idx == reg_addr_cnt)
			{
				strncat(pcWriteBuffer, "DONE", xWriteBufferLen);
				reg_idx = 0;
				ret_val = pdFALSE;
			}
			else
			{
				ret_val = pdTRUE;
			}
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return ret_val;
}

int FractionToInt(float FloatNum)
{
	float Temp;

	Temp = FloatNum;
	if (FloatNum < 0) {
		Temp = -(FloatNum);
	}

	return( ((int)((Temp -(float)((int)Temp)) * (1000.0f))));
}
float temperature[16];
static BaseType_t awmf_temp_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	awmf0165_get_temperature( &temperature[0] );

//	AWMF_0165_TX_CHAIN_U12_U10_U09_U11 = 2, /* SPI2 */
//	AWMF_0165_TX_CHAIN_U16_U14_U13_U15 = 1, /* SPI3 */
//	AWMF_0165_RX_CHAIN_U19_U17_U18_U20 = 4, /* SPI4 */
//	AWMF_0165_RX_CHAIN_U23_U21_U22_U24 = 3, /* SPI5 */
	snprintf(pcWriteBuffer, xWriteBufferLen,
			"\r\nawmf0165 temperature is :\r\n"
			"	U16:%0d.%03d\r\n	U14:%0d.%03d\r\n	U13:%0d.%03d\r\n	U15:%0d.%03d \r\n"
			"	U12:%0d.%03d\r\n	U10:%0d.%03d\r\n	U09:%0d.%03d\r\n	U11:%0d.%03d \r\n"
			"	U23:%0d.%03d\r\n	U21:%0d.%03d\r\n	U22:%0d.%03d\r\n	U24:%0d.%03d \r\n"
			"	U19:%0d.%03d\r\n	U17:%0d.%03d\r\n	U18:%0d.%03d\r\n	U20:%0d.%03d \r\n",
			(int)(temperature[0]), FractionToInt(temperature[0]),
			(int)(temperature[1]), FractionToInt(temperature[1]),
			(int)(temperature[2]), FractionToInt(temperature[2]),
			(int)(temperature[3]), FractionToInt(temperature[3]),
			(int)(temperature[4]), FractionToInt(temperature[4]),
			(int)(temperature[5]), FractionToInt(temperature[5]),
			(int)(temperature[6]), FractionToInt(temperature[6]),
			(int)(temperature[7]), FractionToInt(temperature[7]),
			(int)(temperature[8]), FractionToInt(temperature[8]),
			(int)(temperature[9]), FractionToInt(temperature[9]),
			(int)(temperature[10]), FractionToInt(temperature[10]),
			(int)(temperature[11]), FractionToInt(temperature[11]),
			(int)(temperature[12]), FractionToInt(temperature[12]),
			(int)(temperature[13]), FractionToInt(temperature[13]),
			(int)(temperature[14]), FractionToInt(temperature[14]),
			(int)(temperature[15]), FractionToInt(temperature[15])
	);

	return pdFALSE;
}
static BaseType_t awmf_atc_mode_en_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	STATIC awmf_0165_reg_init_data_t l_atc_mode_data[] = {
		{AWMF_0165_REG_MODE, 0x0000, 0x40010000},
		{0x000d, 0x0000, 0x002c0000},
		{0x001f, 0x0000, 0x000003e8},
		{0x0021, 0x0000, 0x0000001f},
		{0x000d, 0x0000, 0x000c0000},
	};

	awmf_0165_dev_chain_data_t chain_data = {
			AWMF_0165_TX_CHAIN_U16_U14_U13_U15,
			AWMF_0165_MAX_CHIP_NUM_PER_CHAIN,
			{ { 0x00 , 0x00 , AWMF_0165_REG_MODE , 0x00 , 0x40010000 },
			  { 0x00 , 0x00 , AWMF_0165_REG_MODE , 0x00 , 0x40010000 },
			  { 0x00 , 0x00 , AWMF_0165_REG_MODE , 0x00 , 0x40010000 },
			  { 0x00 , 0x00 , AWMF_0165_REG_MODE , 0x00 , 0x40010000 }
			},
	};

	for( chain_data.chain_id = AWMF_0165_TX_CHAIN_U16_U14_U13_U15 ; chain_data.chain_id <= AWMF_0165_RX_CHAIN_U19_U17_U18_U20 ; chain_data.chain_id++ )
	{
		for( uint8_t reg_cnt = 0x00 ; reg_cnt < ARRAY_SIZE(l_atc_mode_data) ; reg_cnt++ )
		{
			for( uint8_t i = 0x00 ; i < 4 ; i++ )
			{
				chain_data.chip_data[i].addr_offset = l_atc_mode_data[reg_cnt].reg_addr ;
				chain_data.chip_data[i].data_high = l_atc_mode_data[reg_cnt].data_high ;
				chain_data.chip_data[i].data_low = l_atc_mode_data[reg_cnt].data_low ;
			}
			awmf_0165_drv_chain_serial_write(&chain_data);
		}
	}


	return pdFALSE;
}
static BaseType_t awmf_atc_mode_dis_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

	awmf_0165_dev_chain_data_t chain_data = {
			AWMF_0165_TX_CHAIN_U16_U14_U13_U15,
			AWMF_0165_MAX_CHIP_NUM_PER_CHAIN,
			{ { 0x00 , 0x00 , AWMF_0165_REG_MODE , 0x00 , 0x00010000 },
			  { 0x00 , 0x00 , AWMF_0165_REG_MODE , 0x00 , 0x00010000 },
			  { 0x00 , 0x00 , AWMF_0165_REG_MODE , 0x00 , 0x00010000 },
			  { 0x00 , 0x00 , AWMF_0165_REG_MODE , 0x00 , 0x00010000 }
			},
	};

	for( chain_data.chain_id = AWMF_0165_TX_CHAIN_U16_U14_U13_U15 ; chain_data.chain_id <= AWMF_0165_RX_CHAIN_U19_U17_U18_U20 ; chain_data.chain_id++ )
	{
		awmf_0165_drv_chain_serial_write(&chain_data);
	}

	return pdFALSE;
}
void ut_0165_drv_init(void)
{
	FreeRTOS_CLIRegisterCommand(&awmf_workmode_set_cmd);
	FreeRTOS_CLIRegisterCommand(&awmf_workmode_get_cmd);
	FreeRTOS_CLIRegisterCommand(&awmf_reg_set_cmd);
	FreeRTOS_CLIRegisterCommand(&awmf_reg_get_cmd);
	FreeRTOS_CLIRegisterCommand(&awmf_temp_get_cmd);
	FreeRTOS_CLIRegisterCommand(&awmf_atc_mode_en_cmd);
	FreeRTOS_CLIRegisterCommand(&awmf_atc_mode_dis_cmd);

	return;
}
