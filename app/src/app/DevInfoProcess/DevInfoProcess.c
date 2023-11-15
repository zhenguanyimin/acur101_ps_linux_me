#include <unistd.h>
#include <pthread.h>
#include "DevInfoProcess.h"
#include "../../srv/alink/command/track/alink_track.h"

ret_code_t GetMagCaliData(void)
{
	ret_code_t ret = RET_OK;

//	ret = GetMagCaliData_rpc_m();

	return ret;
}

ret_code_t GetGyroCaliData(void)
{
	ret_code_t ret = RET_OK;

//	ret = GetGyroCaliData_rpc_m();

	return ret;
}

ret_code_t GetAccCaliData(uint8_t direction)
{
	ret_code_t ret = RET_OK;

//	ret = GetAccCaliData_rpc_m(direction);

	return ret;
}

void plt_cali_cmd_pro(void)
{
//	plt_cali_cmd_pro_rpc_m();
}

ret_code_t get_gyro_calibration_data(int (*gyroData)[3])
{
	ret_code_t ret = RET_OK;
	
//	ret = get_gyro_calibration_data_rpc_m(gyroData);

	return ret;
}

ret_code_t eraseGyroCaliParam(void)
{
	ret_code_t ret = RET_OK;

//	ret = eraseGyroCaliParam_rpc_m();

	return ret;
}

ret_code_t getGyroCaliParamBias(float bias[3])
{
	ret_code_t ret = RET_OK;
	
//	ret = getGyroCaliParamBias_rpc_m(bias);

	return ret;
}

ret_code_t resetGyroAngleData(void)
{
//	return resetGyroAngleData_rpc_m();
}

ret_code_t getPlatformInfo(protocol_radar_platfrom_info_t *platformInfo)
{
	ret_code_t ret = RET_OK;

//	ret = getPlatformInfo_rpc_m(platformInfo);

	return ret;
}

ret_code_t getRadarPoseEstimation(float radarPoseEstimation[3])
{
	ret_code_t ret = RET_OK;

//	ret = getRadarPoseEstimation_rpc_m(radarPoseEstimation);

	return ret;
}

ret_code_t getRadarAttInfo(uint32_t idx, att_info_t *att_info)
{
	ret_code_t ret = RET_OK;

//	ret = getRadarAttInfo_rpc_m(idx, att_info);

	return ret;
}

ret_code_t get_mag_calibration_data(int (*magData)[3])
{
	ret_code_t ret = RET_OK;

//	ret = get_mag_calibration_data_rpc_m(magData);

	return ret;
}

ret_code_t get_acc_calibration_data(const char *direction, uint32_t len, int *accPrint)
{
	ret_code_t ret = RET_OK;

//	ret = get_acc_calibration_data_rpc_m(direction, len, accPrint);

	return ret;
}

ret_code_t reset_acc_cali_data(void)
{
//	return reset_acc_cali_data_rpc_m();
}

ret_code_t reset_mag_cali_data(void)
{
//	return reset_mag_cali_data_rpc_m();
}

ret_code_t get_radar_pose_estimation(sRadarPoseEstimation *postEstimation, sMagCaliParam *magCaliparam, sMagCaliData *magCaliData, \
										sGyroCaliParam *gyroCaliParam, sAccelCaliParam *accelCaliParam)
{
	ret_code_t ret = RET_OK;

//	ret = get_radar_pose_estimation_rpc_m(postEstimation, magCaliparam, magCaliData, gyroCaliParam, accelCaliParam);

	return ret;
}

ret_code_t reset_moving_flag(void)
{
//	return reset_moving_flag_rpc_m();
}

ret_code_t set_radar_pose_value(float *poseVal)
{
	ret_code_t ret = RET_OK;

//	ret = set_radar_pose_value_rpc_m(poseVal);

	return ret;
}

ret_code_t set_angle_done_value(bool value)
{
	ret_code_t ret = RET_OK;

//	ret = set_angle_done_value_rpc_m(value);

	return ret;
}

#include "../../app/Tracking/src/alg_init/alg_init.h"
#include "../../app/Tracking/src/utilities/common_define.h"
//#include "../../app/Tracking/src/target_process/radarPoseEstimation/include/radarPoseEstimation.h"

static uint32_t g_angle_done = false;
static float longitude_buf = 0;
static float latitude_buf = 0;
static float altitude_buf = 0;
static sRadarPoseEstimation g_RadarPose = { 0 };

extern protocol_radar_platfrom_info_t gPlatformInfo[1];
extern sAlgObjData *gAlgData;
extern void passYPR2algo(sAlgObjData* algObj, sRadarPoseEstimation* radarPoseEstimation_inst);

static void *DevInfo_task(void *p_arg)
{
	for( ; ; )
	{
		usleep( 200000 );
		if (g_angle_done == true)
		{
			gPlatformInfo[0].heading  = (int32_t)(g_RadarPose.yaw * RAD2DEG * 0x8000);
			gPlatformInfo[0].pitching = (int32_t)(g_RadarPose.pitch * RAD2DEG * 0x8000);
			gPlatformInfo[0].rolling  = (int32_t)(g_RadarPose.roll * RAD2DEG * 0x8000);
			passYPR2algo(gAlgData, &g_RadarPose);
		}
		else
		{
			gPlatformInfo[0].heading  = (int32_t)(0 * RAD2DEG * 0x8000);
			gPlatformInfo[0].pitching = (int32_t)(0 * RAD2DEG * 0x8000);
			gPlatformInfo[0].rolling  = (int32_t)(0 * RAD2DEG * 0x8000);
			passYPR2algo(gAlgData, &g_RadarPose);
		}
		
		if( g_angle_done == true)
		{
			gPlatformInfo[0].longitude = (int32_t)(longitude_buf * 1000000);
			gPlatformInfo[0].latitude = (int32_t)(latitude_buf * 1000000);
			gPlatformInfo[0].altitude = (int32_t)(altitude_buf * 0x80);
		}
		else
		{
			gPlatformInfo[0].longitude = (int32_t)(0 * 0x8000);
			gPlatformInfo[0].latitude = (int32_t)(0 * 0x8000);
			gPlatformInfo[0].altitude = (int32_t)(0 * 0x80);
		}
		alink_upload_attitude( gPlatformInfo );
	}
}

int Devinfo_cal_data( void )
{
	g_angle_done == true;
	gPlatformInfo[0].heading  = (int32_t)(0 * RAD2DEG * 0x8000);
	gPlatformInfo[0].pitching = (int32_t)(0 * RAD2DEG * 0x8000);
	gPlatformInfo[0].rolling  = (int32_t)(0 * RAD2DEG * 0x8000);
	gPlatformInfo[0].longitude = (int32_t)(0 * 1000000);
	gPlatformInfo[0].latitude = (int32_t)(0 * 1000000);
	gPlatformInfo[0].altitude = (int32_t)(0 * 0x80);
}

ret_code_t plt_cali_data_rcv(uint32_t longitude, uint32_t latitude, uint32_t altitude, uint32_t heading, uint32_t pitching, uint32_t rolling)
{
	g_RadarPose.yaw = (((int32_t)heading) * DEG2RAD / 32768.0);
	g_RadarPose.pitch = (((int32_t)pitching) * DEG2RAD / 32768.0);
	g_RadarPose.roll = (((int32_t)rolling) * DEG2RAD / 32768.0);

	longitude_buf = (int32_t)longitude / 1000000.0;
	latitude_buf = (int32_t)latitude / 1000000.0;
	altitude_buf = (int32_t)altitude / 32768.0;

	g_angle_done = true;

	return RET_OK;
}

static void *DevInfo_task_1(void *p_arg)
{
	
	for( ; ; )
	{
		usleep( 20000 );

		
	}
}

int DevInfo_init( void )
{
	int32_t eRet = 0;

	pthread_t pid;
	if( 0 == pthread_create(&pid, NULL, DevInfo_task, NULL) )
	{
		;
	}

#if 0
	pthread_t pid1;
	if( 0 == pthread_create(&pid1, NULL, DevInfo_task_1, NULL) )
	{
		;
	}
#endif
	
	return eRet;
}


