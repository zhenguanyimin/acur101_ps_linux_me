#ifndef RADAR_POSE_ESTIMATION_H_
#define RADAR_POSE_ESTIMATION_H_

#include "../../../utilities/common_struct.h"

#define MAGDATASCALE (1.0)
#define ACCELDATASCALE (1.0)
typedef struct sMagCaliParam {
	float hardIron[3];
	float softIron[9];
	double chi2;
}sMagCaliParam;

typedef struct sMagCaliData {
	/*int mag_x[60];
	int mag_y[60];
	int mag_z[60];*/
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
	sMagCaliParam* magCaliParam;
	sMagCaliData* magCaliData;
	sGyroCaliParam* gyroCaliParam;
	sAccelCaliParam* accelCaliParam;
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

typedef struct sGyroCaliData {
	int angVel_x[120];
	int angVel_y[120];
	int angVel_z[120];
}sGyroCaliData;

typedef struct sAccelCaliData {
	int accel_x[120];
	int accel_y[120];
	int accel_z[120];
}sAccelCaliData;

void calcMagCaliParam(sRadarPoseEstimation* radarPoseEstimation_inst);
void calcRadarPose(sRadarPoseEstimation* radarPoseEstimation_inst, float g, sAlgObjData* algObj, float dt);
void passYPR2algo(sAlgObjData* algObj, sRadarPoseEstimation* radarPoseEstimation_inst);
#if defined (DEBUG_LOG_GYRO)
void testRadarPoseEstimation(sAlgObjData* algObj);
#endif
void calcGyroCaliParam(sGyroCaliData* gyroCaliData, sRadarPoseEstimation* radarPoseEstimation_inst);
void calcAccelCaliParam(sAccelCaliData* accelCaliData, sRadarPoseEstimation* radarPoseEstimation_inst, float g);
#endif
