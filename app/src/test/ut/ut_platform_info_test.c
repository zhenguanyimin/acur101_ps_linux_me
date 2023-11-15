
#include "ut_platform_info_test.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>

/* FreeRTOS+CLI includes. */
#include "../../srv/cli/cli_if.h"
#include "../../drv/gps/gps_dev.h"
#include "../../srv/protocol/protocol_system.h"
#include "../../app/DevInfoProcess/DevInfoProcess.h"


static BaseType_t platform_info_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t gps_info_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t gps_reset_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t gps_standby_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t gps_fullon_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t attitude_info_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t imu_info_start_output_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

static const CLI_Command_Definition_t platform_info_get_cmd =
{
	"platformInfoGet",
	"\r\nplatformInfoGet <time>:\r\n  get platform information in <time> second \r\n",
	platform_info_get_cmd_handler,
	0
};
static const CLI_Command_Definition_t gps_info_get_cmd =
{
	"GpsInfoGet",
	"\r\nGpsInfoGet :\r\n  get gps information  \r\n",
	gps_info_get_cmd_handler,
	0
};

static const CLI_Command_Definition_t attitude_info_get_cmd =
{
	"AttitudeInfoGet",
	"\r\nAttitudeInfoGet :\r\n  get Attitude information  \r\n",
	attitude_info_get_cmd_handler,
	0
};


static const CLI_Command_Definition_t gps_reset_cmd =
{
	"GpsReset",
	"\r\nGpsReset:\r\n  Reset gps! \r\n",
	gps_reset_cmd_handler,
	0
};


static const CLI_Command_Definition_t gps_standby_cmd =
{
	"GpsStandby",
	"\r\nGpsStandby:\r\n  gps Standby \r\n",
	gps_standby_cmd_handler,
	0
};


static const CLI_Command_Definition_t gps_fullon_cmd =
{
	"GpsFullOn",
	"\r\nGpsFullOn:\r\n  gps full on \r\n",
	gps_fullon_cmd_handler,
	0
};


static const CLI_Command_Definition_t imu_info_start_output_cmd =
{
	"ImuInfoStartOutput",
	"\r\nImuInfoStartOutput:\r\n  imu_info start output \r\n",
	imu_info_start_output_cmd_handler,
	0
};


static BaseType_t platform_info_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	protocol_radar_platfrom_info_t platformInfo[1];
	ret_code_t ret = RET_OK;

	ret = getPlatformInfo(platformInfo);
	if (ret == RET_OK)
	{
		snprintf(pcWriteBuffer, xWriteBufferLen,
					"\r\ngPlatformInfo data:\r\n"
					"\rheading				= %0d;\r\n"
					"\rpitching				= %0d;\r\n"
					"\rrolling				= %0d;\r\n"
					"\rlongitude			= %0d;\r\n"
					"\rlatitude				= %0d;\r\n"
					"\raltitude				= %0d;\r\n"
					"\rvelocityNavi			= %0d;\r\n"
					"\rtargetTimeMark[0]	= %0d;\r\n"
					"\rtargetTimeMark[1]	= %0d;\r\n"
					"\rtargetTimeMark[2]	= %0d;\r\n"
					"\rtargetTimeMark[3]	= %0d;\r\n"
					"\rtargetTimeMark[4]	= %0d;\r\n"
					"\rtargetTimeMark[5]	= %0d;\r\n"
					"\rsigProcRelativeTime	= %0d;\r\n",
					(int)platformInfo[0].heading,
					(int)platformInfo[0].pitching,
					(int)platformInfo[0].rolling,
					(int)platformInfo[0].longitude,
					(int)platformInfo[0].latitude,
					(int)platformInfo[0].altitude,
					(int)platformInfo[0].velocityNavi,
					(int)platformInfo[0].targetTimeMark[0],
					(int)platformInfo[0].targetTimeMark[1],
					(int)platformInfo[0].targetTimeMark[2],
					(int)platformInfo[0].targetTimeMark[3],
					(int)platformInfo[0].targetTimeMark[4],
					(int)platformInfo[0].targetTimeMark[5],
					(int)platformInfo[0].sigProcRelativeTime);
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}

static BaseType_t gps_info_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	GPS_INFO gpsInfo[1];
	ret_code_t ret = RET_GENERAL_ERR;

	ret = getGpsInfo(gpsInfo);

	char Post_sys_status[2] = { gpsInfo->RMC.Post_sys_status, 0x00 };
	char RMC_NS[2] = { gpsInfo->RMC.NS, 0x00 };
	char RMC_EW[2] = { gpsInfo->RMC.EW, 0x00 };
	char VTG_Mode[2] = { gpsInfo->VTG.Mode, 0x00 };
	char GGA_NS[2] = { gpsInfo->GGA.NS, 0x00 };
	char GGA_EW[2] = { gpsInfo->GGA.EW, 0x00 };

	if (ret == RET_OK)
	{
		snprintf(pcWriteBuffer, xWriteBufferLen,
					"\r\n RMC Info data:\r\n"
					"\rPost_sys_status	= %s;\r\n"
					"\rlongitude		= %f(%s);\r\n"
					"\rlatitude			= %f(%s);\r\n"
					"\rUTC time			= %0d.%0d.%0d  %0d:%0d:%0d;\r\n\r\n"
					"\r\n VTG Info data  :\r\n"
					"\rvelocityNavi		= %f km/h;\r\n"
					"\rmode				= %s;\r\n"
					"\r\n GGA Info data:\r\n"
					"\rnum_of_sate		= %d;\r\n"
					"\rlongitude		= %f(%s);\r\n"
					"\rlatitude			= %f(%s);\r\n"
					"\raltitude			= %f m;\r\n"
					"\rUTC time			= %0d:%0d:%0d;\r\n",
					Post_sys_status,
					gpsInfo->RMC.latitude/100,
					RMC_NS,
					gpsInfo->RMC.longitude/100,
					RMC_EW,
					gpsInfo->RMC.D.year,
					gpsInfo->RMC.D.month,
					gpsInfo->RMC.D.day,
					gpsInfo->RMC.D.hour,
					gpsInfo->RMC.D.minute,
					gpsInfo->RMC.D.second,
					gpsInfo->VTG.kilometers_speed,
					VTG_Mode,
					gpsInfo->GGA.num_of_sate,
					gpsInfo->GGA.latitude/100,
					GGA_NS,
					gpsInfo->GGA.longitude/100,
					GGA_EW,
					gpsInfo->GGA.Altitude,
					gpsInfo->GGA.D.hour,
					gpsInfo->GGA.D.minute,
					gpsInfo->GGA.D.second);
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE;
}


static BaseType_t attitude_info_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	float radarPoseEstimation[3];	// 0: yaw 1: pitch 2: roll
	ret_code_t ret = RET_GENERAL_ERR;

	ret = getRadarPoseEstimation(radarPoseEstimation);
	if (ret == RET_OK)
	{
		snprintf(pcWriteBuffer, xWriteBufferLen,
					"\r\n attitude information:\r\n"
					"\rRadar_yaw	= %0f;\r\n"
					"\rRadar_pitch	= %0f;\r\n"
					"\rRadar_roll	= %0f;\r\n"
					"\rRadar_yaw_deg	= %0f;\r\n"
					"\rRadar_pitch_deg	= %0f;\r\n"
					"\rRadar_roll_deg	= %0f;\r\n",
					radarPoseEstimation[0],
					radarPoseEstimation[1],
					radarPoseEstimation[2],
					radarPoseEstimation[0] * RAD2DEG,
					radarPoseEstimation[1] * RAD2DEG,
					radarPoseEstimation[2] * RAD2DEG);
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}
	return pdFALSE;

}

static BaseType_t gps_reset_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

	GPS_reset();

	snprintf(pcWriteBuffer, xWriteBufferLen,	"\r\n DONE \r\n"		);

	return pdFALSE;

}

static BaseType_t gps_standby_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

	GPS_enable_standby_mode();

	snprintf(pcWriteBuffer, xWriteBufferLen,	"\r\n DONE \r\n"		);

	return pdFALSE;

}

static BaseType_t gps_fullon_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

	GPS_enable_full_on_mode();

	snprintf(pcWriteBuffer, xWriteBufferLen,	"\r\n DONE \r\n"		);

	return pdFALSE;

}


static BaseType_t imu_info_start_output_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t ret = pdTRUE;
	static BaseType_t print_flag = pdFALSE;
	static uint32_t cnt_c = 0x00;

	usleep( 20 * 1000 );
	if( print_flag != pdTRUE )
	{
		snprintf(pcWriteBuffer, xWriteBufferLen,
								"\rtimer(ms)	"
								"acc_x(mg)	acc_y(mg)	acc_z(mg)	"
								"mag_x(mguss)	mag_y(mguss)	mag_z(mguss)	"
								"gyro_x(dps)	gyro_y(dps)	gyro_z(dps)	"
								"roll	pitch	yaw	"
								"movingflag");
		print_flag = pdTRUE;
	}
	else
	{
		if( cnt_c < 300 )
		{
			att_info_t *att_info = (att_info_t *)malloc(sizeof(att_info_t));
			if (NULL == att_info)
			{
				snprintf(pcWriteBuffer, xWriteBufferLen,	"\r\n ERROR \r\n"		);
				return pdFALSE;
			}

			memset(att_info, 0, sizeof(att_info_t));
			getRadarAttInfo(cnt_c, att_info);
			snprintf(pcWriteBuffer, xWriteBufferLen,
								"\r\n %f	%d	%d	%d	%d	%d	%d	%f	%f	%f	%f	%f	%f	%d	",
								att_info->time,
								att_info->acc_x,
								att_info->acc_y,
								att_info->acc_z,
								att_info->mag_x,
								att_info->mag_y,
								att_info->mag_z,
								att_info->gyro_x,
								att_info->gyro_y,
								att_info->gyro_z,
								att_info->roll,
								att_info->pitch,
								att_info->yaw,
								att_info->movinflag);
			cnt_c++;

			free(att_info);
			att_info = NULL;
		}
		else
		{
			cnt_c = 0x00;
			ret = pdFALSE;
			print_flag = pdFALSE;
			snprintf(pcWriteBuffer, xWriteBufferLen,	"\r\n DONE \r\n"		);
		}
	}

	return ret;
}

void ut_platform_info_test_init(void)
{
	FreeRTOS_CLIRegisterCommand(&platform_info_get_cmd);
	FreeRTOS_CLIRegisterCommand(&gps_info_get_cmd);
	FreeRTOS_CLIRegisterCommand(&attitude_info_get_cmd);
	FreeRTOS_CLIRegisterCommand(&gps_standby_cmd);
	FreeRTOS_CLIRegisterCommand(&gps_reset_cmd);
	FreeRTOS_CLIRegisterCommand(&gps_fullon_cmd);
	FreeRTOS_CLIRegisterCommand(&imu_info_start_output_cmd);

	return;
}
