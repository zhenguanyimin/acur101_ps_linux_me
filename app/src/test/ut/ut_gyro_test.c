
/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* FreeRTOS+CLI includes. */
#include "ut_gyro_test.h"
#include "../../srv/cli/cli_if.h"
#include "../../drv/gyro/gyro_uart.h"
#include "../../srv/flash_nv/flash_raw.h"
#include "../../app/DevInfoProcess/DevInfoProcess.h"

static BaseType_t gyro_reg_set_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t gyro_reg_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t gyro_value_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t gyro_data_print_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t gyro_calibration_data_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t gyro_calibration_result_erase_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t gyro_angle_reset_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);


static const CLI_Command_Definition_t gyro_reg_set_cmd =
{
	"GyroRegSet",
	"\r\nGyroRegSet <RegAddr> <RegVal>:\r\n Set gyro register value\r\n",
	gyro_reg_set_cmd_handler,
	2
};


static const CLI_Command_Definition_t gyro_reg_get_cmd =
{
	"GyroRegGet",
	"\r\nGyroRegSet <RegAddrBase> :\r\n Get gyro register value\r\n",
	gyro_reg_get_cmd_handler,
	1
};

static const CLI_Command_Definition_t gyro_value_get_cmd =
{
	"GyroValueGet",
	"\r\nGyroValueGet <period> <count> :\r\n Get gyro value\r\n",
	gyro_value_get_cmd_handler,
	2
};

static const CLI_Command_Definition_t gyro_data_get_cmd =
{
	"GyroCalibrationData",
	"\r\nGyroCalibrationData :\r\n Get gyro Calibration Data\r\n",
	gyro_calibration_data_get_cmd_handler,
	0
};

static const CLI_Command_Definition_t Gyro_cali_result_erase_cmd =
{
	"GyroEraseCaliRst",
	"\r\nGyroEraseCaliRst :\r\n Erase Calibration result \r\n",
	gyro_calibration_result_erase_cmd_handler,
	0
};

static const CLI_Command_Definition_t gyro_data_print_cmd =
{
	"GyroDataPrint",
	"\r\nGyroDataPrint :\r\n print origin data and calibration data \r\n",
	gyro_data_print_cmd_handler,
	0
};

static const CLI_Command_Definition_t gyro_angle_reset_cmd =
{
	"GyroAngleReset",
	"\r\nGyroAngleReset :\r\n radar attitude angle reset \r\n",
	gyro_angle_reset_cmd_handler,
	0
};

static BaseType_t gyro_reg_set_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0, len2 = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
	uint32_t reg_addr = 0, reg_val = 0;
	ret_code_t ret = RET_GENERAL_ERR;

	if ((len1 > 0) && (len2 > 0) )
	{
		reg_addr = strtoul(param1, NULL, 0);
		reg_val = strtoul(param2, NULL, 0);

		reg_addr &= 0xff;
		reg_val &= 0xff;

		ret = Gyro_Reg_Write((uint8_t)reg_addr , (uint8_t)reg_val);
		if (ret == RET_OK)
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}

	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

static BaseType_t gyro_reg_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	uint32_t reg_addr = 0;
	uint8_t reg_val = 0;
	ret_code_t ret = RET_GENERAL_ERR;

	if ( len1 > 0 )
	{
		reg_addr = strtoul(param1, NULL, 0);

		reg_addr &= 0xff;

		ret = Gyro_Reg_Read((uint8_t)reg_addr , &reg_val);
		if (ret == RET_OK)
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "\r	register[0x%x] value is 0x%x	\r\n", reg_addr, reg_val );

		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}
	return pdFALSE;
}

static BaseType_t gyro_value_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0, len2 = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
	uint32_t period = 0, count = 0 ;
	static uint32_t index = 0;
	BaseType_t ret_val = pdTRUE;

	int l_gyro_data[3];

	if (len1 > 0)
	{
		period = strtoul(param1, NULL, 0);
		if (len2 > 0)
		{
			count = strtoul(param2, NULL, 0);
		}
		else
		{
			count = 1;
		}

		Get_gyro_date( l_gyro_data );
		usleep( period * 1000 );

		if (index == 0x00)
		{
			index++;
			snprintf(pcWriteBuffer, xWriteBufferLen,
						"\r\n gyro data:\r\n"
						"\r	GYRO_X(dps)	GYRO_Y(dps)	GYRO_Z(dps) \r\n"
						"	%f,	%f,	%f \r\n",
						l_gyro_data[0]/1000.0,	l_gyro_data[1]/1000.0,	l_gyro_data[2]/1000.0);
		}
		else if (index < count)
		{
			index++;
			snprintf(pcWriteBuffer, xWriteBufferLen,
						"\r	%f,	%f,	%f \r\n",
						l_gyro_data[0]/1000.0,	l_gyro_data[1]/1000.0,	l_gyro_data[2]/1000.0);
		}
		else if (index == count)
		{
			strncat(pcWriteBuffer, "DONE", xWriteBufferLen);
			index = 0;
			ret_val = pdFALSE;
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		ret_val = pdFALSE;
	}

	return ret_val;
}

static BaseType_t gyro_calibration_data_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

	static ret_code_t ret = RET_GENERAL_ERR;
	BaseType_t ret_val = pdTRUE;
	static int loop = 0;
	int l_gyro_data[120][3];

	if (loop == 0)
	{
		ret = get_gyro_calibration_data(l_gyro_data);
	}

	if(ret == RET_OK)
	{
		if (loop == 0)
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "\r\ngyro calibration data:\r\n"
					"GYRO_X(dps)    GYRO_Y(dps)    GYRO_Z(dps)\r\n"
					"%0f,	%0f,	%0f;\r\n%0f,	%0f,	%0f;\r\n"
					"%0f,	%0f,	%0f;\r\n%0f,	%0f,	%0f;\r\n"
					"%0f,	%0f,	%0f;\r\n%0f,	%0f,	%0f;\r\n"
					"%0f,	%0f,	%0f;\r\n%0f,	%0f,	%0f;\r\n"
					"%0f,	%0f,	%0f;\r\n%0f,	%0f,	%0f;\r\n",
					l_gyro_data[loop+0][0]/1000.0, l_gyro_data[loop+0][1]/1000.0, l_gyro_data[loop+0][2]/1000.0,
					l_gyro_data[loop+1][0]/1000.0, l_gyro_data[loop+1][1]/1000.0, l_gyro_data[loop+1][2]/1000.0,
					l_gyro_data[loop+2][0]/1000.0, l_gyro_data[loop+2][1]/1000.0, l_gyro_data[loop+2][2]/1000.0,
					l_gyro_data[loop+3][0]/1000.0, l_gyro_data[loop+3][1]/1000.0, l_gyro_data[loop+3][2]/1000.0,
					l_gyro_data[loop+4][0]/1000.0, l_gyro_data[loop+4][1]/1000.0, l_gyro_data[loop+4][2]/1000.0,
					l_gyro_data[loop+5][0]/1000.0, l_gyro_data[loop+5][1]/1000.0, l_gyro_data[loop+5][2]/1000.0,
					l_gyro_data[loop+6][0]/1000.0, l_gyro_data[loop+6][1]/1000.0, l_gyro_data[loop+6][2]/1000.0,
					l_gyro_data[loop+7][0]/1000.0, l_gyro_data[loop+7][1]/1000.0, l_gyro_data[loop+7][2]/1000.0,
					l_gyro_data[loop+8][0]/1000.0, l_gyro_data[loop+8][1]/1000.0, l_gyro_data[loop+8][2]/1000.0,
					l_gyro_data[loop+9][0]/1000.0, l_gyro_data[loop+9][1]/1000.0, l_gyro_data[loop+9][2]/1000.0);
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen,
					"%0f,	%0f,	%0f;\r\n%0f,	%0f,	%0f;\r\n"
					"%0f,	%0f,	%0f;\r\n%0f,	%0f,	%0f;\r\n"
					"%0f,	%0f,	%0f;\r\n%0f,	%0f,	%0f;\r\n"
					"%0f,	%0f,	%0f;\r\n%0f,	%0f,	%0f;\r\n"
					"%0f,	%0f,	%0f;\r\n%0f,	%0f,	%0f;\r\n",
					l_gyro_data[loop+0][0]/1000.0, l_gyro_data[loop+0][1]/1000.0, l_gyro_data[loop+0][2]/1000.0,
					l_gyro_data[loop+1][0]/1000.0, l_gyro_data[loop+1][1]/1000.0, l_gyro_data[loop+1][2]/1000.0,
					l_gyro_data[loop+2][0]/1000.0, l_gyro_data[loop+2][1]/1000.0, l_gyro_data[loop+2][2]/1000.0,
					l_gyro_data[loop+3][0]/1000.0, l_gyro_data[loop+3][1]/1000.0, l_gyro_data[loop+3][2]/1000.0,
					l_gyro_data[loop+4][0]/1000.0, l_gyro_data[loop+4][1]/1000.0, l_gyro_data[loop+4][2]/1000.0,
					l_gyro_data[loop+5][0]/1000.0, l_gyro_data[loop+5][1]/1000.0, l_gyro_data[loop+5][2]/1000.0,
					l_gyro_data[loop+6][0]/1000.0, l_gyro_data[loop+6][1]/1000.0, l_gyro_data[loop+6][2]/1000.0,
					l_gyro_data[loop+7][0]/1000.0, l_gyro_data[loop+7][1]/1000.0, l_gyro_data[loop+7][2]/1000.0,
					l_gyro_data[loop+8][0]/1000.0, l_gyro_data[loop+8][1]/1000.0, l_gyro_data[loop+8][2]/1000.0,
					l_gyro_data[loop+9][0]/1000.0, l_gyro_data[loop+9][1]/1000.0, l_gyro_data[loop+9][2]/1000.0);
		}

		loop++;
		if (loop >= 12)
		{
			ret_val = pdFALSE;
		}
	}
	else
	{
		ret_val = pdFALSE;
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return ret_val;

}

#define FLASH_NV_NAME_ANGLE_CALIB_DATA      "ANGLE_CALIB_DATA"
static BaseType_t gyro_calibration_result_erase_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

	eraseGyroCaliParam();

	FlashRaw_Del(FLASH_NV_NAME_ANGLE_CALIB_DATA);

	snprintf(pcWriteBuffer, xWriteBufferLen, "OK");

	return pdFALSE;
}
static BaseType_t gyro_data_print_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	float bias[3] = {0.0};
	ret_code_t ret = RET_GENERAL_ERR;

	ret = getGyroCaliParamBias(bias);
	if (ret == RET_OK)
	{
		snprintf(pcWriteBuffer, xWriteBufferLen,
					"\r\n gyro relation data:\r\n"
					"\rbias_x = %0f;\r\n"
					"\rbias_y = %0f;\r\n"
					"\rbias_z = %0f;\r\n",
					bias[0], bias[1], bias[2]);
		strncat(pcWriteBuffer, "DONE", xWriteBufferLen);
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}
	
	return pdFALSE;
}

static BaseType_t gyro_angle_reset_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	ret_code_t ret = RET_GENERAL_ERR;

	ret = resetGyroAngleData();

	if (ret == RET_OK)
	{
		snprintf(pcWriteBuffer, xWriteBufferLen,
					"\r\n radar attitude angle :\r\n"
					"\ryaw   = %0f;\r\n"
					"\rpitch = %0f;\r\n"
					"\rroll  = %0f;\r\n",
					0.0, 0.0, 0.0);
		strncat(pcWriteBuffer, "DONE", xWriteBufferLen);
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

void ut_gyro_init(void)
{

	FreeRTOS_CLIRegisterCommand(&gyro_reg_set_cmd);
	FreeRTOS_CLIRegisterCommand(&gyro_reg_get_cmd);
	FreeRTOS_CLIRegisterCommand(&gyro_value_get_cmd);
	FreeRTOS_CLIRegisterCommand(&gyro_data_get_cmd);
	FreeRTOS_CLIRegisterCommand(&Gyro_cali_result_erase_cmd);
	FreeRTOS_CLIRegisterCommand(&gyro_data_print_cmd);
	FreeRTOS_CLIRegisterCommand(&gyro_angle_reset_cmd);

	return;
}
