
/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "stdbool.h"
/* FreeRTOS+CLI includes. */
#include "ut_lsm303ah_test.h"
#include "../../srv/cli/cli_if.h"
#include "../../srv/flash_nv/flash_raw.h"
#include "../../drv/lsm303ah/lsm303ah_dev.h"
#include "../../app/DevInfoProcess/DevInfoProcess.h"



static BaseType_t lsm303ah_self_test_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t lsm303ah_reg_set_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t lsm303ah_reg_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t lsm303ah_value_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t lsm303ah_mag_data_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t lsm303ah_acc_data_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t lsm303ah_data_print_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t lsm303ah_acc_cali_rst_erase_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t lsm303ah_mag_cali_rst_erase_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t lsm303ah_reset_movingflag_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t lsm303ah_manual_calibrite_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

static const CLI_Command_Definition_t lsm303ah_self_test_cmd =
{
	"Lsm303ahSelfTest",
	"\r\nLsm303ahSelfTest :\r\n Lsm303ah Self Test\r\n",
	lsm303ah_self_test_cmd_handler,
	0
};

static const CLI_Command_Definition_t lsm303ah_reg_set_cmd =
{
	"Lsm303ahRegSet",
	"\r\nLsm303ahRegSet <RegAddr> <RegVal>:\r\n Set Lsm303ah register value\r\n",
	lsm303ah_reg_set_cmd_handler,
	2
};

static const CLI_Command_Definition_t lsm303ah_reg_get_cmd =
{
	"Lsm303ahRegGet",
	"\r\nLsm303ahRegGet <RegAddrBase> :\r\n Get Lsm303ah register value\r\n",
	lsm303ah_reg_get_cmd_handler,
	1
};

static const CLI_Command_Definition_t lsm303ah_value_get_cmd =
{
	"Lsm303ahValueGet",
	"\r\nLsm303ahValueGet :\r\n Get Lsm303ah acc&mag value\r\n",
	lsm303ah_value_get_cmd_handler,
	0
};

static const CLI_Command_Definition_t lsm303ah_mag_data_get_cmd =
{
	"Lsm303ahGetMagCalibrationData",
	"\r\nLsm303ahGetMagCalibrationData :\r\n Get mag Calibration Data\r\n",
	lsm303ah_mag_data_get_cmd_handler,
	0
};

static const CLI_Command_Definition_t lsm303ah_acc_data_get_cmd =
{
	"Lsm303ahGetAccCalibrationData",
	"\r\nLsm303ahGetAccCalibrationData <direction> :\r\n Get acc Calibration Data\r\n",
	lsm303ah_acc_data_get_cmd_handler,
	1
};

static const CLI_Command_Definition_t lsm303ah_acc_cali_result_erase_cmd =
{
	"Lsm303ahAccEraseCaliRst",
	"\r\nLsm303ahAccEraseCalibrationResult :\r\n Erase acc Calibration result \r\n",
	lsm303ah_acc_cali_rst_erase_cmd_handler,
	0
};

static const CLI_Command_Definition_t lsm303ah_mag_cali_result_erase_cmd =
{
	"Lsm303ahMagEraseCaliRst",
	"\r\nLsm303ahAccEraseCalibrationResult :\r\n Erase mag Calibration result \r\n",
	lsm303ah_mag_cali_rst_erase_cmd_handler,
	0
};

static const CLI_Command_Definition_t lsm303ah_data_print_cmd =
{
	"Lsm303ahDataPrint",
	"\r\nLsm303ahDataPrint :\r\n print origin data and calibration data \r\n",
	lsm303ah_data_print_cmd_handler,
	0
};
static const CLI_Command_Definition_t lsm303ah_reset_movingflag_cmd =
{
	"Lsm303ahResetmovingflag",
	"\r\nLsm303ahResetmovingflag :\r\n  Reset movingflag \r\n",
	lsm303ah_reset_movingflag_cmd_handler,
	0
};

static const CLI_Command_Definition_t lsm303ah_manual_calibrite_cmd =
{
	"Lsm303ahManualCalibrite",
	"\r\nLsm303ahManualCalibrite :\r\n  Manual Calibrite \r\n",
	lsm303ah_manual_calibrite_cmd_handler,
	6
};

static BaseType_t lsm303ah_self_test_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

	uint16_t acc_abs[3], mag_abs[3];

	// i2c_1_init(XPAR_PSU_I2C_1_DEVICE_ID);
	// ACC_sensor_self_test(acc_abs);
	// MAG_sensor_self_test(mag_abs);

	// Acc_init();
	// Mag_init();
	
	snprintf(pcWriteBuffer, xWriteBufferLen,
				"\r\nlsm303ah absolute error is:\r\n"
				"	ACC_X:%0dmg is %s\r\n"
				"	ACC_Y:%0dmg is %s\r\n"
				"	ACC_Z:%0dmg is %s\r\n \r\n"
				"	MAG_X:%0dmgauss is %s\r\n"
				"	MAG_Y:%0dmgauss is %s\r\n"
				"	MAG_Z:%0dmgauss is %s\r\n",
				(int)acc_abs[0],(acc_abs[0] < 1500) ? "OK" : "ERROR",
				(int)acc_abs[1],(acc_abs[0] < 1500) ? "OK" : "ERROR",
				(int)acc_abs[2],(acc_abs[0] < 1500) ? "OK" : "ERROR",
				(int)mag_abs[0],(acc_abs[0] < 500 ) ? "OK" : "ERROR",
				(int)mag_abs[1],(acc_abs[0] < 500 ) ? "OK" : "ERROR",
				(int)mag_abs[2],(acc_abs[0] < 500 ) ? "OK" : "ERROR");
	return pdFALSE;
}

static BaseType_t lsm303ah_reg_set_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0, len2 = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
	uint32_t reg_addr = 0, reg_val = 0;
	ret_code_t ret = RET_GENERAL_ERR;

	if ((len1 > 0) && (len2 > 0))
	{
		reg_addr = strtoul(param1, NULL, 0);
		reg_val = strtoul(param2, NULL, 0);

		reg_addr &= 0xff;
		reg_val &= 0xff;

		if (!IS_LSM303AH_REG_ADDR_VALID(reg_addr))
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
		else
		{
			ret = LSM303AH_Reg_Write((uint8_t)reg_addr, (uint8_t)reg_val);
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

static BaseType_t lsm303ah_reg_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	uint32_t reg_addr = 0;
	uint8_t reg_val = 0;
	ret_code_t ret = RET_GENERAL_ERR;

	if (len1 > 0)
	{
		reg_addr = strtoul(param1, NULL, 0);

		reg_addr &= 0xff;

		if (!IS_LSM303AH_REG_ADDR_VALID(reg_addr))
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
		else
		{
			ret = LSM303AH_Reg_Read((uint8_t)reg_addr, &reg_val);
			if (ret == RET_OK)
			{
				snprintf(pcWriteBuffer, xWriteBufferLen, "register[0x%x] value is 0x%x\r\n", reg_addr, reg_val);

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

static BaseType_t lsm303ah_value_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

	int Acceleration_G[3];
	int Magnetic_mGa[3];

	get_acc_mag_data(&Acceleration_G[0], &Magnetic_mGa[0]);

	snprintf(pcWriteBuffer, xWriteBufferLen,
				"\r\nlsm303ah value is:\r\n"
				"	ACC_X: %0dmg\r\n"
				"	ACC_Y: %0dmg\r\n"
				"	ACC_Z: %0dmg\r\n \r\n"
				"	MAG_X: %0dmgauss\r\n"
				"	MAG_Y: %0dmgauss\r\n"
				"	MAG_Z: %0dmgauss\r\n \r\n",
				(int)Acceleration_G[0], (int)Acceleration_G[1], (int)Acceleration_G[2],
				(int)Magnetic_mGa[0],   (int)Magnetic_mGa[1],   (int)Magnetic_mGa[2]);

	return pdFALSE;
}

static BaseType_t lsm303ah_mag_data_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

	static ret_code_t ret = RET_GENERAL_ERR;
	BaseType_t ret_val = pdTRUE;
	static int loop = 0;
	int l_Magnetic_mGa[120][3];

	if (loop == 0)
	{
		ret = get_mag_calibration_data(l_Magnetic_mGa);
	}

	if(ret == RET_OK)
	{
		if (loop == 0)
		{
			snprintf(pcWriteBuffer, xWriteBufferLen,
					"\r\nlsm303ah mag calibration data:\r\n"
					"MAG_X(mgauss)		MAG_Y(mgauss)		MAG_Z(mgauss)\r\n"
					"%0d,			%0d,			%0d;\r\n%0d,			%0d,			%0d;\r\n"
					"%0d,			%0d,			%0d;\r\n%0d,			%0d,			%0d;\r\n"
					"%0d,			%0d,			%0d;\r\n%0d,			%0d,			%0d;\r\n"
					"%0d,			%0d,			%0d;\r\n%0d,			%0d,			%0d;\r\n"
					"%0d,			%0d,			%0d;\r\n%0d,			%0d,			%0d;\r\n",
					l_Magnetic_mGa[loop+0][0], l_Magnetic_mGa[loop+0][1], l_Magnetic_mGa[loop+0][2],
					l_Magnetic_mGa[loop+1][0], l_Magnetic_mGa[loop+1][1], l_Magnetic_mGa[loop+1][2],
					l_Magnetic_mGa[loop+2][0], l_Magnetic_mGa[loop+2][1], l_Magnetic_mGa[loop+2][2],
					l_Magnetic_mGa[loop+3][0], l_Magnetic_mGa[loop+3][1], l_Magnetic_mGa[loop+3][2],
					l_Magnetic_mGa[loop+4][0], l_Magnetic_mGa[loop+4][1], l_Magnetic_mGa[loop+4][2],
					l_Magnetic_mGa[loop+5][0], l_Magnetic_mGa[loop+5][1], l_Magnetic_mGa[loop+5][2],
					l_Magnetic_mGa[loop+6][0], l_Magnetic_mGa[loop+6][1], l_Magnetic_mGa[loop+6][2],
					l_Magnetic_mGa[loop+7][0], l_Magnetic_mGa[loop+7][1], l_Magnetic_mGa[loop+7][2],
					l_Magnetic_mGa[loop+8][0], l_Magnetic_mGa[loop+8][1], l_Magnetic_mGa[loop+8][2],
					l_Magnetic_mGa[loop+9][0], l_Magnetic_mGa[loop+9][1], l_Magnetic_mGa[loop+9][2]);

		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen,
					"%0d,			%0d,			%0d;\r\n%0d,			%0d,			%0d;\r\n"
					"%0d,			%0d,			%0d;\r\n%0d,			%0d,			%0d;\r\n"
					"%0d,			%0d,			%0d;\r\n%0d,			%0d,			%0d;\r\n"
					"%0d,			%0d,			%0d;\r\n%0d,			%0d,			%0d;\r\n"
					"%0d,			%0d,			%0d;\r\n%0d,			%0d,			%0d;\r\n",
					l_Magnetic_mGa[loop+0][0], l_Magnetic_mGa[loop+0][1], l_Magnetic_mGa[loop+0][2],
					l_Magnetic_mGa[loop+1][0], l_Magnetic_mGa[loop+1][1], l_Magnetic_mGa[loop+1][2],
					l_Magnetic_mGa[loop+2][0], l_Magnetic_mGa[loop+2][1], l_Magnetic_mGa[loop+2][2],
					l_Magnetic_mGa[loop+3][0], l_Magnetic_mGa[loop+3][1], l_Magnetic_mGa[loop+3][2],
					l_Magnetic_mGa[loop+4][0], l_Magnetic_mGa[loop+4][1], l_Magnetic_mGa[loop+4][2],
					l_Magnetic_mGa[loop+5][0], l_Magnetic_mGa[loop+5][1], l_Magnetic_mGa[loop+5][2],
					l_Magnetic_mGa[loop+6][0], l_Magnetic_mGa[loop+6][1], l_Magnetic_mGa[loop+6][2],
					l_Magnetic_mGa[loop+7][0], l_Magnetic_mGa[loop+7][1], l_Magnetic_mGa[loop+7][2],
					l_Magnetic_mGa[loop+8][0], l_Magnetic_mGa[loop+8][1], l_Magnetic_mGa[loop+8][2],
					l_Magnetic_mGa[loop+9][0], l_Magnetic_mGa[loop+9][1], l_Magnetic_mGa[loop+9][2]);
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

static BaseType_t lsm303ah_acc_data_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

	ret_code_t ret = RET_GENERAL_ERR;
	int l_acc_print[60];
	BaseType_t len1 = 0;
	const char *direction = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if (len1 > 0)
	{
		ret = get_acc_calibration_data(direction, len1, l_acc_print);
		if(ret == RET_OK)
		{
			snprintf(pcWriteBuffer, xWriteBufferLen,
					"\r\nlsm303ah acc %s data(mg):\r\n"
					"%0d \r\n%0d \r\n%0d \r\n%0d \r\n%0d \r\n"
					"%0d \r\n%0d \r\n%0d \r\n%0d \r\n%0d \r\n"
					"%0d \r\n%0d \r\n%0d \r\n%0d \r\n%0d \r\n"
					"%0d \r\n%0d \r\n%0d \r\n%0d \r\n%0d \r\n"
					"%0d \r\n%0d \r\n%0d \r\n%0d \r\n%0d \r\n"
					"%0d \r\n%0d \r\n%0d \r\n%0d \r\n%0d \r\n"
					"%0d \r\n%0d \r\n%0d \r\n%0d \r\n%0d \r\n"
					"%0d \r\n%0d \r\n%0d \r\n%0d \r\n%0d \r\n"
					"%0d \r\n%0d \r\n%0d \r\n%0d \r\n%0d \r\n"
					"%0d \r\n%0d \r\n%0d \r\n%0d \r\n%0d \r\n"
					"%0d \r\n%0d \r\n%0d \r\n%0d \r\n%0d \r\n"
					"%0d \r\n%0d \r\n%0d \r\n%0d \r\n%0d \r\n",	
					direction, 
					l_acc_print[0],  l_acc_print[1],  l_acc_print[2],  l_acc_print[3],  l_acc_print[4],
					l_acc_print[5],  l_acc_print[6],  l_acc_print[7],  l_acc_print[8],  l_acc_print[9],
					l_acc_print[10], l_acc_print[11], l_acc_print[12], l_acc_print[13], l_acc_print[14],
					l_acc_print[15], l_acc_print[16], l_acc_print[17], l_acc_print[18], l_acc_print[19],
					l_acc_print[20], l_acc_print[21], l_acc_print[22], l_acc_print[23], l_acc_print[24],
					l_acc_print[25], l_acc_print[26], l_acc_print[27], l_acc_print[28], l_acc_print[29],
					l_acc_print[30], l_acc_print[31], l_acc_print[32], l_acc_print[33], l_acc_print[34],
					l_acc_print[35], l_acc_print[36], l_acc_print[37], l_acc_print[38], l_acc_print[39],
					l_acc_print[40], l_acc_print[41], l_acc_print[42], l_acc_print[43], l_acc_print[44],
					l_acc_print[45], l_acc_print[46], l_acc_print[47], l_acc_print[48], l_acc_print[49],
					l_acc_print[50], l_acc_print[51], l_acc_print[52], l_acc_print[53], l_acc_print[54],
					l_acc_print[55], l_acc_print[56], l_acc_print[57], l_acc_print[58], l_acc_print[59]);
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "Get acc calibration data error");
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

#define FLASH_NV_NAME_ACC_CALIB_DATA        "ACC_CALIB_DATA"
static BaseType_t lsm303ah_acc_cali_rst_erase_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	reset_acc_cali_data();

	FlashRaw_Del(FLASH_NV_NAME_ACC_CALIB_DATA);

	snprintf(pcWriteBuffer, xWriteBufferLen, "OK");

	return pdFALSE;
}

#define FLASH_NV_NAME_MAG_CALIB_DATA        "MAG_CALIB_DATA"
static BaseType_t lsm303ah_mag_cali_rst_erase_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

	reset_mag_cali_data();

	FlashRaw_Del(FLASH_NV_NAME_MAG_CALIB_DATA);

	snprintf(pcWriteBuffer, xWriteBufferLen, "OK");

	return pdFALSE;
}

static BaseType_t lsm303ah_data_print_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	ret_code_t ret = RET_GENERAL_ERR;
	sRadarPoseEstimation radarPoseEstimation[1];
	sMagCaliParam magCaliparam[1];
	sMagCaliData magCaliData[1];
	sGyroCaliParam gyroCaliParam[1];
	sAccelCaliParam accelCaliParam[1];

	ret = get_radar_pose_estimation(radarPoseEstimation, magCaliparam, magCaliData, gyroCaliParam, accelCaliParam);

	if (ret == RET_OK)
	{
		radarPoseEstimation->magCaliParam = magCaliparam;
		radarPoseEstimation->magCaliData = magCaliData;
		radarPoseEstimation->gyroCaliParam = gyroCaliParam;		
		radarPoseEstimation->accelCaliParam = accelCaliParam;

		snprintf(pcWriteBuffer, xWriteBufferLen,
					"\r\n lsm303am relation data:\r\n"
					"\rhardIron[0]	= %0f;\r\n"
					"\rhardIron[1]	= %0f;\r\n"
					"\rhardIron[2]	= %0f;\r\n"
					"\rsoftIron[0]	= %0f;\r\n"
					"\rsoftIron[1]	= %0f;\r\n"
					"\rsoftIron[2]	= %0f;\r\n"
					"\rsoftIron[3]	= %0f;\r\n"
					"\rsoftIron[4]	= %0f;\r\n"
					"\rsoftIron[5]	= %0f;\r\n"
					"\rsoftIron[6]	= %0f;\r\n"
					"\rsoftIron[7]	= %0f;\r\n"
					"\rsoftIron[8]	= %0f;\r\n"
					"\rbias_x	= %0f;\r\n"
					"\rbias_y	= %0f;\r\n"
					"\rbias_z	= %0f;\r\n"
					"\rscale_x	= %0f;\r\n"
					"\rscale_x	= %0f;\r\n"
					"\rscale_x	= %0f;\r\n"
					"\rRadar_yaw	= %0f;\r\n"
					"\rRadar_pitch	= %0f;\r\n"
					"\rRadar_roll	= %0f;\r\n"
					"\rRadar_yaw_deg	= %0f;\r\n"
					"\rRadar_pitch_deg	= %0f;\r\n"
					"\rRadar_roll_deg	= %0f;\r\n"
					"\rmovingFlag	= %0d;\r\n",
					radarPoseEstimation->magCaliParam->hardIron[0],
					radarPoseEstimation->magCaliParam->hardIron[1],
					radarPoseEstimation->magCaliParam->hardIron[2],
					radarPoseEstimation->magCaliParam->softIron[0],
					radarPoseEstimation->magCaliParam->softIron[1],
					radarPoseEstimation->magCaliParam->softIron[2],
					radarPoseEstimation->magCaliParam->softIron[3],
					radarPoseEstimation->magCaliParam->softIron[4],
					radarPoseEstimation->magCaliParam->softIron[5],
					radarPoseEstimation->magCaliParam->softIron[6],
					radarPoseEstimation->magCaliParam->softIron[7],
					radarPoseEstimation->magCaliParam->softIron[8],
					radarPoseEstimation->accelCaliParam->bias_x,
					radarPoseEstimation->accelCaliParam->bias_y,
					radarPoseEstimation->accelCaliParam->bias_z,
					radarPoseEstimation->accelCaliParam->scale_x,
					radarPoseEstimation->accelCaliParam->scale_y,
					radarPoseEstimation->accelCaliParam->scale_z,
					radarPoseEstimation->yaw,
					radarPoseEstimation->pitch,
					radarPoseEstimation->roll,
					radarPoseEstimation->yaw * RAD2DEG,
					radarPoseEstimation->pitch * RAD2DEG,
					radarPoseEstimation->roll * RAD2DEG,
					radarPoseEstimation->movingFlag);
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}


	return pdFALSE;
}

static BaseType_t lsm303ah_reset_movingflag_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

	reset_moving_flag();

	snprintf(pcWriteBuffer, xWriteBufferLen, "OK");

	return pdFALSE;
}

static BaseType_t lsm303ah_manual_calibrite_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0, len2 = 0, len3 = 0, len4 = 0, len5 = 0, len6 = 0;
	float iLongitude = -1;
	float iLatitude = -1;
	float iAltitude = -1;
	float iHeading = -1;
	float iPitching = -1;
	float iRolling = -1;
	float poseVal[3];
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
	const char *param3 = FreeRTOS_CLIGetParameter(pcCommandString, 3, &len3);
	const char *param4 = FreeRTOS_CLIGetParameter(pcCommandString, 4, &len4);
	const char *param5 = FreeRTOS_CLIGetParameter(pcCommandString, 5, &len5);
	const char *param6 = FreeRTOS_CLIGetParameter(pcCommandString, 6, &len6);

	if ((len1 > 0) && (len2 > 0) && (len3 > 0) && (len4 > 0) && (len5 > 0) && (len6 > 0))
	{
		iHeading = strtof(param1, NULL);
		iPitching = strtof(param2, NULL);
		iRolling = strtof(param3, NULL);
		iLongitude = strtof(param4, NULL);
		iLatitude = strtof(param5, NULL);
		iAltitude = strtof(param6, NULL);

		poseVal[0] = (iHeading * DEG2RAD);
		poseVal[1] = (iPitching * DEG2RAD);
		poseVal[2] = (iRolling * DEG2RAD);
		set_radar_pose_value(poseVal);

		set_angle_done_value(true);

		snprintf(pcWriteBuffer, xWriteBufferLen,
				"OK yaw %f, pitch %f, roll %f, iLongitude %f, iLatitude %f, iAltitude %f",
				iHeading, iPitching, iRolling, iLongitude, iLatitude, iAltitude);
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}
	return pdFALSE;
}

void ut_lsm303ah_init(void)
{

	FreeRTOS_CLIRegisterCommand(&lsm303ah_self_test_cmd);
	FreeRTOS_CLIRegisterCommand(&lsm303ah_reg_set_cmd);
	FreeRTOS_CLIRegisterCommand(&lsm303ah_reg_get_cmd);
	FreeRTOS_CLIRegisterCommand(&lsm303ah_value_get_cmd);
	FreeRTOS_CLIRegisterCommand(&lsm303ah_mag_data_get_cmd);
	FreeRTOS_CLIRegisterCommand(&lsm303ah_acc_data_get_cmd);
	FreeRTOS_CLIRegisterCommand(&lsm303ah_acc_cali_result_erase_cmd);
	FreeRTOS_CLIRegisterCommand(&lsm303ah_mag_cali_result_erase_cmd);
	FreeRTOS_CLIRegisterCommand(&lsm303ah_data_print_cmd);
	FreeRTOS_CLIRegisterCommand(&lsm303ah_reset_movingflag_cmd);
	FreeRTOS_CLIRegisterCommand(&lsm303ah_manual_calibrite_cmd);
	return;
}
