#ifndef DEV_INFO_PROCESS_H
#define DEV_INFO_PROCESS_H

#include "../../inc/radar_error.h"
#include "../../inc/radar_common.h"
#include "../../drv/gps/gps_dev.h"
#include "../../srv/protocol/protocol_system.h"


#ifndef RAD2DEG
#define RAD2DEG 						(57.2957795f) //180.0f/M_PI
#endif

#ifndef DEG2RAD
#define DEG2RAD 						(0.01745329f) //M_PI/180.0f
#endif


typedef struct{
	uint32_t longitude ;
	uint32_t latitude ;
	uint32_t altitude ;
	uint32_t heading ;
	uint32_t pitching ;
	uint32_t rolling ;
}cali_info_t;

typedef struct{
	float time;
	int acc_x;
	int acc_y;
	int acc_z;
	int mag_x;
	int mag_y;
	int mag_z;
	float gyro_x;
	float gyro_y;
	float gyro_z;
	float roll;
	float pitch;
	float yaw;
	uint32_t movinflag;
}att_info_t;

typedef enum
{
	coord_x = 0,
	coord_y,
	coord_z,
}coordinate_id_t;


typedef struct sMagCaliParam {
	float hardIron[3];
	float softIron[9];
	double chi2;
}sMagCaliParam;

typedef struct sMagCaliData {
	int mag_x[120];
	int mag_y[120];
	int mag_z[120];
	int GT_mag;
}sMagCaliData;

typedef struct sGyroCaliParam {
	float bias_x;
	float bias_y;
	float bias_z;
}sGyroCaliParam;

typedef struct sAccelCaliParam {
	float bias_x;
	float bias_y;
	float bias_z;
	float scale_x;
	float scale_y;
	float scale_z;
}sAccelCaliParam;

typedef struct sRadarPoseEstimation {
	sMagCaliParam *magCaliParam;
	sMagCaliData *magCaliData;
	sGyroCaliParam *gyroCaliParam;
	sAccelCaliParam *accelCaliParam;
	int a[3];
	int mag[3];
	int gyro[3];
	float roll;
	float pitch;
	float yaw;
	float magnetic_declination;
	uint16_t caliDone;
	int movingFlag;
}sRadarPoseEstimation;


ret_code_t GetMagCaliData(void);
ret_code_t GetGyroCaliData(void);
ret_code_t GetAccCaliData(uint8_t direction);
void plt_cali_cmd_pro(void);
//ret_code_t plt_cali_data_rcv(uint32_t longitude, uint32_t latitude, uint32_t altitude, uint32_t heading, uint32_t pitching, uint32_t rolling);
ret_code_t get_gyro_calibration_data(int (*gyroData)[3]);
ret_code_t eraseGyroCaliParam(void);
ret_code_t getGyroCaliParamBias(float bias[3]);
ret_code_t resetGyroAngleData(void);
ret_code_t getPlatformInfo(protocol_radar_platfrom_info_t *platformInfo);
ret_code_t getRadarPoseEstimation(float radarPoseEstimation[3]);
ret_code_t getRadarAttInfo(uint32_t idx, att_info_t *att_info);
ret_code_t get_mag_calibration_data(int (*magData)[3]);
ret_code_t get_acc_calibration_data(const char *direction, uint32_t len, int *accPrint);
ret_code_t reset_acc_cali_data(void);
ret_code_t reset_mag_cali_data(void);
ret_code_t get_radar_pose_estimation(sRadarPoseEstimation *postEstimation, sMagCaliParam *magCaliparam, sMagCaliData *magCaliData, \
										sGyroCaliParam *gyroCaliParam, sAccelCaliParam *accelCaliParam);
ret_code_t reset_moving_flag(void);
ret_code_t set_radar_pose_value(float *poseVal);
ret_code_t set_angle_done_value(bool value);

int DevInfo_init( void );
int Devinfo_cal_data( void );
int plt_cali_data_rcv(uint32_t longitude, uint32_t latitude, uint32_t altitude, uint32_t heading, uint32_t pitching, uint32_t rolling);

#endif
