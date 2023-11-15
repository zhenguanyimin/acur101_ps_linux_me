
#include <string.h>
#include <math.h>

#include "../include/tracking_common.h"
#include "../include/tracking_int.h"


/**
*  @b Description
*  @n
*		tracking Module calls this function to start target tracking. This function is called during modules' allocation step, 
*		once new set of points passes allocation thresholds 
*
*  @param[in]  handle
*		This is handle to tracking unit
*  @param[in]  timeStamp
*		This is an allocation time stamp
*  @param[in]  tid
*		This is a target identifier given to a unit
*  @param[in]  uCenter
*		This is a pointer to the centroid in measurement coordinates
*
*  \ingroup tracking_ALG_UNIT_FUNCTION
*
*  @retval
*      None
*/

int tracking_unitStart(void *handle, sTracking_platformInfo *platformInfo, sMeasurement_SphPoint *measurement, \
	uint16_t* bestIndex, uint32_t wholeSpaceScanCycleCnt, sMeasurement_enum_wavetype waveType, uint32_t tid)
{
    sTracking_objectUnit *inst;
    sTracking_measurementSphUnion u;
    int n, m;
	float gC[MSIZE_SPH*MSIZE_SPH] = { 0.f };
	int i;
	sTracking_gatingParams gatingParams;
	int iSameRoot = 0;
    inst = (sTracking_objectUnit *)handle;
	// init current target
//#ifdef DEMO_ACUR
//	if (measurement->array[0] > 225 && measurement->array[0] < 241) // ´ËÇø¼äÄÚ²»ÔÊÐí½¨º½
//	{
//		return 1;
//	}
//#endif
	inst->tid = tid;
	memcpy(u.array, measurement->array, sizeof(measurement->array));
	// init state values
	tracking_spherical2cartesian(inst->tracking_params->stateVectorType, u.array, inst->kalman_state.S_hat);
	/* P_apriori_hat = diag([0,0,0.5,0.5,1,1]) */
	memset(inst->kalman_state.P_hat, 0, sizeof(inst->kalman_state.P_hat));
	for (i = 0; i < SSIZE_XY; i++)
	{
		inst->kalman_state.P_hat[i*(SSIZE_XY + 1)] = pInit[i];
	}
	/* inst->kalman_state.P_hat[0] = 0.000025f * u.vector.range * u.vector.range + 0.01f * u.vector.range + 1.f;
	inst->kalman_state.P_hat[10] = 0.0004f * u.vector.range * u.vector.range + 0.04f * u.vector.range + 1.f;
	inst->kalman_state.P_hat[20] = 0.0004f * u.vector.range * u.vector.range + 0.04f * u.vector.range + 1.f;
	inst->kalman_state.P_hat[30] = 1.f;
	inst->kalman_state.P_hat[40] = 0.000025f * u.vector.range * u.vector.range + 0.01f * u.vector.range + 1.f;
	inst->kalman_state.P_hat[50] = 0.000025f * u.vector.range * u.vector.range + 0.01f * u.vector.range + 1.f;
	inst->kalman_state.P_hat[60] = 1.f;
	inst->kalman_state.P_hat[70] = 1.f;
	inst->kalman_state.P_hat[80] = 1.f; */
	/* P_apriori_hat = diag([0,0,0.5,0.5,1,1]) */
    memcpy(inst->kalman_state.H_s.array, u.array, sizeof(u.array)); /* Initialize Hs to measurment vector */

	//init association properties
	tracking_calcMeasurementLimits(u.vector.range, &inst->tracking_params->advParams.gatingParams.limits, \
		&inst->assoProperty.H_limits.vector);
	inst->assoProperty.G = inst->tracking_params->advParams.gatingParams.gain;
	memset(gC, 0, sizeof(float)*MSIZE_SPH*MSIZE_SPH);
	for (n = 0; n < MSIZE_SPH; n++) {
		gC[n*MSIZE_SPH + n] = 2.f;
	}
	tracking_matrixInv(gC, &inst->assoProperty.gC_det, inst->assoProperty.gC_inv);
	memset(inst->assoProperty.bitIndicatorsOfDetAsso, 0, sizeof(inst->assoProperty.bitIndicatorsOfDetAsso));
	m = measurement->detId;
	inst->assoProperty.bitIndicatorsOfDetAsso[m >> 3] |= (1 << (m & 0x7));
	/*for (n = 0; n < measurement->detectionNum; n++)
	{
		m = measurement->detectionId[n];
		inst->assoProperty.bitIndicatorsOfDetAsso[m >> 3] |= (1 << (m & 0x7));
	}*/
	bestIndex[measurement->detId] = inst->uid;
	for (iSameRoot = 0; iSameRoot < NUM_EXTERN_DOPPLER; iSameRoot++)
	{
		inst->assoProperty.sameRootTracker.sameRootTrackerId[iSameRoot] = -1;
	}
	inst->assoProperty.sameRootTracker.sameRootNum = 0;
	inst->assoProperty.sameRootTracker.mdScore = 1000.f;
	inst->assoProperty.sameRootTracker.visited = 0;
	inst->assoProperty.assoCondenceMeasId = measurement->detId;
	inst->assoProperty.assoThisScan = 0;
	memcpy(&gatingParams, &inst->tracking_params->advParams.gatingParams, sizeof(sTracking_gatingParams));
	getGatingParams(inst->kalman_state.H_s.array, &gatingParams, &inst->tracking_params->advParams, platformInfo);
	inst->assoProperty.assocGating.depth = gatingParams.limits.depth;
	inst->assoProperty.assocGating.width = gatingParams.limits.width;
	inst->assoProperty.assocGating.vel = gatingParams.limits.vel;
	inst->assoProperty.assocGating.height = gatingParams.limits.height;
	inst->assoProperty.assocGating.depth = gatingParams.limits.depth;
	inst->assoProperty.assocGating.width = gatingParams.limits.width;
	inst->assoProperty.assocGating.vel = gatingParams.limits.vel;
	inst->assoProperty.assocGating.height = gatingParams.limits.height;
	inst->assoProperty.assocDynamicGating.depth = inst->assoProperty.assocGating.depth*0.7f;
	inst->assoProperty.assocDynamicGating.width = inst->assoProperty.assocGating.width*0.7f;
	inst->assoProperty.assocDynamicGating.vel = inst->assoProperty.assocGating.vel*0.7f;
	inst->assoProperty.assocDynamicGating.height = inst->assoProperty.assocGating.height*0.7f;
	tracking_calcMeasurementLimits(u.vector.range, &inst->assoProperty.assocDynamicGating, \
		&inst->assoProperty.assocDynamic_H_limits.vector);
	inst->assoProperty.assMahDistance = 0.f;
	inst->curTarget.mdScore = 0.f;
	inst->assoProperty.lastSeenTime = g_curTimestamp;
	inst->assoProperty.lastSeenWholeScanId = wholeSpaceScanCycleCnt;
	inst->assoProperty.assoUnambiVel = measurement->vector.doppler;
	//inst->assoProperty.assoDistribution.uSectorCountMaxIndex = 0U;
	//inst->assoProperty.assoDistribution.uSectorOccupationSum = 0U;
	//memset(inst->assoProperty.assoDistribution.uSectorOccupation, 0, sizeof(uint8_t)*TRACKING_NUM_SECTORS);
	inst->timestamp = g_curTimestamp;
	// Init object management
	inst->objManagement.heartBeatCount = 1;
	inst->objManagement.allocationTime = g_curTimestamp;
	inst->objManagement.allocationRange = measurement->vector.range;
	inst->objManagement.allocationVelocity = measurement->vector.doppler;
	inst->objManagement.allocationWavetype = waveType;
	inst->objManagement.estNumOfPoints = 1;
	inst->objManagement.detect2activeCount = 0;
	inst->objManagement.detect2freeCount = 0;
	inst->objManagement.active2freeCount = 0;
	inst->objManagement.lifetime = 0;
	inst->objManagement.lifetime_old = 0;
	inst->objManagement.updatePast8frame = 0;/* The update status in the past 8 frames,by hxj */
	inst->objManagement.past8FrameBitNum = 0;
	if (gVelunambigType == WHOLE_SCAN)
	{
		inst->objManagement.state = TRACK_STATE_INIT;
	}
	else
	{
		inst->objManagement.state = TRACK_STATE_DETECTION;
	}
	inst->objManagement.isNewTracker = 1;
	inst->objManagement.tasFlag = 0;
	inst->objManagement.forcastFrameNum = 0;
//	inst->objManagement.abnormalTrackerCnter = 0;

	// Init object merge information
	inst->objMerge.isCurrentScanMerge = 0;
	inst->objMerge.mergeScanCycleCnt = 0;

	// Init object kinematic parameters
	/* Radial Velocity initialization */
	/* Radial Velocity handling is set to start with range rate filtering */
	inst->objKinematic.velocityHandling = VELOCITY_LOCKED;
	inst->objKinematic.maxDistance = 0.f;
	inst->objKinematic.initPosition.x0 = measurement->x;
	inst->objKinematic.initPosition.y0 = measurement->y;
	inst->objKinematic.initPosition.z0 = measurement->z;

	inst->objKinematic.trajDistance = 0.f;
	inst->objKinematic.lastPosition.x0 = measurement->x;
	inst->objKinematic.lastPosition.y0 = measurement->y;
	inst->objKinematic.lastPosition.z0 = measurement->z;

	inst->assoProperty.cumDetNum = 0;
	inst->assoProperty.predictedThisScan = 0;

	inst->objKinematic.z_mean = inst->kalman_state.S_hat[2];
	inst->objKinematic.z_var = 0.f;
	inst->objKinematic.sampleNum = 1;

	inst->curTarget.fRCS = measurement->rcs;
	inst->curTarget.RCSSampleNum = 1;
	inst->objKinematic.velUnambigProp.velUnambigDone = measurement->unambigVelDone;
	inst->objKinematic.velUnambigProp.waveType = measurement->waveType;

	inst->objKinematic.x_mean300 = 0.f;
	inst->objKinematic.y_mean300 = 0.f;
	inst->objKinematic.z_mean300 = 0.f;
	inst->objKinematic.count300 = 0U;
	if (inst->kalman_state.H_s.array[0] < 300)
	{
		inst->objKinematic.restrict800 = 1U;
	}
	else
	{
		inst->objKinematic.restrict800 = 0U;
	}

	inst->assoProperty.confirmBeam = CFM_INITIAL_STATE;
	inst->assoProperty.cfm_times = 0;
	if (measurement->waveType == 0 && measurement->unambigVelDone == 0)
	{
		inst->assoProperty.waveInfo = 0;
	}
	else if (measurement->waveType == 1 && measurement->unambigVelDone == 0)
	{
		inst->assoProperty.waveInfo = 1;
	}
	else if (measurement->unambigVelDone == 1)
	{
		inst->assoProperty.waveInfo = 2;
	}
	return 0;
}