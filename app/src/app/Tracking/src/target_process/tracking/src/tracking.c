
#include "../include/tracking.h"
#include"../../dispatch/include/dispatch.h"
#include "../include/tracking_int.h"
#include <stdlib.h>
#include <time.h>

#ifdef ENABLE_ADTF_LOG
#include <adtf_platform_inc.h>
#include <adtf_plugin_sdk.h>
#endif

/*********** Right Hand Coordinate System (AUTOSAR) *******
*
*                  ^ x
*                  |
*                  |
*      y           |
*      <------------------------
*                  |
*  Angle/Azimuth: left side is position(+)
*
*************************************************/

float *const_process_noise = NULL;
float *const_max_acceleration = NULL;
float *const_tws_measurement_noise = NULL;
float *const_tas_measurement_noise = NULL;
sTracking_sceneryParams *const_roiArea = NULL;
sTracking_insideScratchPad *gScratchPadData = NULL;
sTrackingInst* gTracking_inst = NULL;

float g_average_dt = 0.075f;
int g_frame_dtCnt = 0;
float g_curTimestamp = 0.f;
float g_aziBeam = 0.f;

//clock_t tracking_startTime;
//clock_t tracking_endTime;
#ifdef AEB_PORTING
extern uint32_t OSIF_GetMilliseconds(void);
extern uint32_t tracking_startTime, tracking_endTime, tracking_deltaTime;
#endif

int data_idx_gyro = 0;
sSmoothTrackingPosition smoothTraj[MAX_NUM_TRAJS] = { 0 };
sSmoothTrackingPosition smoothTraj_previous_4[MAX_NUM_TRAJS] = { 0 };
sSmoothTrackingPosition smoothTraj_previous_3[MAX_NUM_TRAJS] = { 0 };
sSmoothTrackingPosition smoothTraj_previous_2[MAX_NUM_TRAJS] = { 0 };
sSmoothTrackingPosition smoothTraj_previous_1[MAX_NUM_TRAJS] = { 0 };

//sTracking_objectUnit* gUnitTrack_debug[MAX_NUM_TRAJS] = { 0 };

int tracking_init(sAlgObjData *algObj) 
{
	gTracking_inst = (sTrackingInst*)(algObj->tracking_inst);
	if (gTracking_inst == NULL) {
		return -1;
	}
	const_process_noise = gTracking_inst->init_process_noise;
	const_max_acceleration = gTracking_inst->init_max_acceleration;
	const_tws_measurement_noise = gTracking_inst->init_tws_measurement_noise;
	const_tas_measurement_noise = gTracking_inst->init_tas_measurement_noise;
	const_roiArea = &gTracking_inst->init_roiArea;
	gScratchPadData = gTracking_inst->scratchPadData;
	memcpy(&(gTracking_inst->trackingConfig.advParams.sceneryParams), const_roiArea, sizeof(sTracking_sceneryParams));
	gTracking_inst->handle = tracking_create(&gTracking_inst->trackingConfig, \
		&gTracking_inst->errorCode, gScratchPadData);
	if (gTracking_inst->handle == NULL) {
		return -1;
	}
	return 0;
}

// Run it to cluster points from one frame data
void tracking_process(sAlgObjData *algObj)
{
	sTrackingInst* tracking_inst = (sTrackingInst*)(algObj->tracking_inst);
	sMeasProcessInst* measurementProcess_inst = (sMeasProcessInst*)algObj->measurementProcess_inst;
	sMeasOutput *measInfo = measurementProcess_inst->measOutput;
	// TODO: condenceDone???
	/*if (measInfo->condenceDone == 0)
		return;*/
#ifndef _WINDOWS
	clock_gettime(CLOCK_MONOTONIC, &g_tUsTrackProcCur[4]);
#endif

	tracking_preProcess(algObj);
#ifndef _WINDOWS
	clock_gettime(CLOCK_MONOTONIC, &g_tUsTrackProcEnd[4]);
	g_tUsTrackProcUsed[4] = ((g_tUsTrackProcEnd[4].tv_sec - g_tUsTrackProcCur[4].tv_sec) * 1000000000LL +
					   (g_tUsTrackProcEnd[4].tv_nsec - g_tUsTrackProcCur[4].tv_nsec))/1000;
	clock_gettime(CLOCK_MONOTONIC, &g_tUsTrackProcCur[5]);
#endif

    // Runs a single step of the given algorithm instance with input point cloud data
	tracking_step(tracking_inst->handle, measInfo, algObj);
#ifndef _WINDOWS
	clock_gettime(CLOCK_MONOTONIC, &g_tUsTrackProcEnd[5]);
	g_tUsTrackProcUsed[5] = ((g_tUsTrackProcEnd[5].tv_sec - g_tUsTrackProcCur[5].tv_sec) * 1000000000LL +
					   (g_tUsTrackProcEnd[5].tv_nsec - g_tUsTrackProcCur[5].tv_nsec))/1000;
	clock_gettime(CLOCK_MONOTONIC, &g_tUsTrackProcCur[6]);
#endif

	tracking_report(algObj, tracking_inst->handle);
#ifndef _WINDOWS
	clock_gettime(CLOCK_MONOTONIC, &g_tUsTrackProcEnd[6]);
	g_tUsTrackProcUsed[6] = ((g_tUsTrackProcEnd[6].tv_sec - g_tUsTrackProcCur[6].tv_sec) * 1000000000LL +
					   (g_tUsTrackProcEnd[6].tv_nsec - g_tUsTrackProcCur[6].tv_nsec))/1000;
#endif
}

void tracking_preProcess(sAlgObjData* algObj)
{
	int j = 0;
	int i = 0;
	int tasObjNum = 0;
	int count = 0;
	sTracking_moduleInstance *inst = NULL;
	sTrackingTarget* tracker = NULL;
    sTracking_objectUnit *inst_tracker = NULL;
	sMeasurement_SphPoint *measurement = NULL;
	sDotCohPointsIdInfo *pointsIdInfo = NULL;
	sTracking_ListElem* tElem = NULL;
	sTracking_ListElem* tElemToMove = NULL;
	sTrackingInst* tracking_inst = (sTrackingInst*)(algObj->tracking_inst);
	sMeasProcessInst* measurementProcess_inst = (sMeasProcessInst*)algObj->measurementProcess_inst;
	sDispatchInst* dispatch_inst = (sDispatchInst*)algObj->dispatch_inst;
	uint32_t wholeSpaceScanCycleCnt = dispatch_inst->dispatchOutput->wholeSpaceScanCycleCnt;
	uint32_t wholeSpaceScanCycleFinalBeamFlag = dispatch_inst->dispatchOutput->wholeSpaceScanCycleFinalBeamFlag;
	sTracking_boundaryBox* boundaryBox = tracking_inst->init_roiArea.boundaryBox;
	sTracking_scanBoundary *scanBoundary = &tracking_inst->trackingConfig.advParams.sceneryParams.scanBoundary;
	inst = (sTracking_moduleInstance*)tracking_inst->handle;
	inst->tasTargetId = dispatch_inst->dispatchOutput->tasObjId;
	inst->wholeSpaceScanCycleCnt = wholeSpaceScanCycleCnt;
	inst->wholeSpaceScanCycleFinalBeamFlag = wholeSpaceScanCycleFinalBeamFlag;
	inst->endOfInitTrackScan = dispatch_inst->dispatchOutput->tasAndInitTrackFinishBeamFlag;
	tracking_inst->curTimestamp = (float)(algObj->detectsListOutput->timestamp) / 1000.f;
	tracking_inst->trackingConfig.condenceDone = measurementProcess_inst->measOutput->condenceDone;
	tracking_inst->trackingConfig.tasTargetId = measurementProcess_inst->measOutput->tasTargetId;
	g_aziBeam = asinf((float)(dispatch_inst->dispatchOutput->aziBeamSin) * INV_ONE15FORMAT_SCALE);
	if( g_byDummyTest )
		tracking_inst->dt = CONST_DELTA_T;

	g_curTimestamp = tracking_inst->curTimestamp;
#ifdef DEBUG_LOG_ZQ
	my_printf("measOutput num %d", measurementProcess_inst->measOutput->num);	
#endif
	
	if (inst->tracking_params->workMode == 0)
	{
		tElem = tracking_listGetFirst(&inst->tasTrackList);
		tasObjNum = &inst->tasTrackList.count;
		while (tElem != 0 && count < tasObjNum)
		{
			tElemToMove = tElem;
			tElem = tracking_listGetNext(tElem);
			tracking_listRemoveElement(&inst->tasTrackList, tElemToMove);
			tracking_listEnqueue(&inst->twsTrackList, tElemToMove);

			count++;
		}
	}

	tracking_inst->runningStatus = MODULE_OK;
	tracking_inst->frame_cnt++;
	for (i = 0; i < measurementProcess_inst->measOutput->num; i++)
	{
		measurement = &measurementProcess_inst->measOutput->measurement[i];
		pointsIdInfo = &measurementProcess_inst->measOutput->pointsIdInfo[i];
#ifdef DEBUG_LOG_ZQ
		my_printf("waveType %d measId %d r a p v [%.2f %.2f %.2f %.2f]  x y z[%.2f %.2f %.2f] velUnambiDone %d ",
			measurement->waveType, i, \
			measurement->vector.range, \
			measurement->vector.azimuthRad*RAD2DEG, \
			measurement->vector.pitchRad*RAD2DEG, \
			measurement->vector.doppler, \
			measurement->x, measurement->y, measurement->z,
			measurement->unambigVelDone);
#endif
		if (fabsf(measurement->vector.doppler) < 1.f)
		{
			inst->bestIndex[i] = TRACKING_ID_POINT_BEHIND_THE_WALL;
#ifdef DEBUG_LOG_ZQ
			my_printf("det %d is low speed measurement", i);
#endif
		}
		if (fabsf(measurement->vector.azimuthRad) > scanBoundary->aziScanScope*DEG2RAD || \
			fabsf(measurement->vector.pitchRad) > scanBoundary->eleScanScope*DEG2RAD || \
			measurement->vector.range > 1100.f)
		{
			inst->bestIndex[i] = TRACKING_ID_POINT_BEHIND_THE_WALL;
#ifdef DEBUG_LOG_ZQ
			my_printf("det %d is behind the ScanScope wall", i);
#endif
		}
		else
		{
			inst->bestIndex[i] = TRACKING_ID_POINT_NOT_ASSOCIATED;
		}
		if (tracking_inst->init_roiArea.numBoundaryBoxes)
		{
			if (measurement->x > boundaryBox[0].x1&&measurement->x < boundaryBox[0].x2&&\
				measurement->y> boundaryBox[0].y1&&measurement->y < boundaryBox[0].y2&&\
				measurement->z> boundaryBox[0].z1&&measurement->z < boundaryBox[0].z2)
			{
				inst->bestIndex[i] = TRACKING_ID_POINT_NOT_ASSOCIATED;
			}
			else
			{
				inst->bestIndex[i] = TRACKING_ID_POINT_BEHIND_THE_WALL;
#ifdef DEBUG_LOG_ZQ
				my_printf("det %d is behind the boundaryBox wall ", i);
#endif

			}
		}
		// abort the dot if cohesion measurement number = 1, do not use this condition with fix beam mode!
		/*if (pointsIdInfo->detectionNum <= 1)
		{
			inst->bestIndex[i] = TRACKING_ID_POINT_BEHIND_THE_WALL;
		}*/
	}
#ifdef  DUMMY_TEST_PRINT_TXT
	if (measurementProcess_inst->measOutput->num > 0)
	{
		FILE *fp;
		fopen_s(&fp, "D:/work/acur100/dummyData/dummyCohData.txt", "a+");
		/*if (algObj->detectsListOutput->frameID == 1)
		{
			fprintf(fp, "frameId   x   y   z   vx   vy   vz range doppler azi pitch\n");
		}*/
		fprintf(fp, "%d %.2f %.2f %.2f %.2f %.2f %.2f %.2f \n", algObj->detectsListOutput->frameID,
			measurement->x, measurement->y, measurement->z,
			measurement->vector.range, measurement->vector.doppler, measurement->vector.azimuthRad*RAD2DEG,
			measurement->vector.pitchRad*RAD2DEG);
		fclose(fp);
	}
	
#endif
}

// return tracking result
void tracking_report(sAlgObjData *algObj, void *handle)
{
	sTracking_moduleInstance *inst = NULL;
	sTracking_ListElem *tElem;
	uint16_t uid;
	uint16_t num = 0;
	uint16_t tasTrackNum = 0;
	uint16_t twsTrackNum = 0;
	int ret = -1;
	sTrackingInst* tracking_inst = (sTrackingInst*)(algObj->tracking_inst);
	inst = (sTracking_moduleInstance *)handle;
	tracking_inst->trajInfoOutput->timestamp = (uint32_t)(tracking_inst->curTimestamp*1000.f);
	tracking_inst->trajInfoOutput->frameID = algObj->detectsListOutput->frameID;
	tElem = tracking_listGetFirst(&inst->tasTrackList);
	data_idx_gyro++;
	while (tElem != 0)
	{
		uid = tElem->data;
		ret = tracking_unitReport(inst->hTrack[uid], algObj, num);
		if (ret == 0)
		{
			tasTrackNum++;
			num++;
		}
		tElem = tracking_listGetNext(tElem);
	}
	tElem = tracking_listGetFirst(&inst->twsTrackList);
	while (tElem != 0)
	{
		uid = tElem->data;
		ret = tracking_unitReport(inst->hTrack[uid], algObj, num);
		if (ret == 0)
		{
			twsTrackNum++;
			num++;
		}
		tElem = tracking_listGetNext(tElem);
	}
	if (gVelunambigType == WHOLE_SCAN)
	{
		tElem = tracking_listGetFirst(&inst->initTrackList);
		while (tElem != 0)
		{
			uid = tElem->data;
			ret = tracking_unitReport(inst->hTrack[uid], algObj, num);
			if (ret == 0)
			{
				tracking_inst->trajInfoOutput->trajList[num].twsTasFlag = 1;
				// TODO: tasTrackNum need not ++ finally
				tasTrackNum++;
				num++;
			}
			tElem = tracking_listGetNext(tElem);
		}
	}
	tracking_inst->trajInfoOutput->trackObjNum = num;
	tracking_inst->trajInfoOutput->trackTasNum = tasTrackNum;
	tracking_inst->trajInfoOutput->trackTwsNum = twsTrackNum;
	tracking_inst->trajInfoOutput->trackTwsTasFlag = (uint16_t)g_scanType;
}

int tracking_unitReport(void *handle, sAlgObjData *algObj, uint16_t num)
{
    sTracking_objectUnit *inst;
	sTrackingTarget* tracker = NULL;
	sTrackingInst* tracking_inst; 
	sTracking_moduleInstance *inst_module = NULL;
	sPlatformInfo* platformInfo = algObj->platformInfo;
	sPlatformData* platFormData = &platformInfo->platformData;
	float pitching = (float)platFormData->pitching*INV_ONE15FORMAT_SCALE*DEG2RAD*-1.f;
	float rolling= (float)platFormData->rolling*INV_ONE15FORMAT_SCALE*DEG2RAD;
	float heading = (float)platFormData->heading*INV_ONE15FORMAT_SCALE*DEG2RAD;
	//float rolling = 0;
	//float pitching = 0;
	//float heading = 0;

	int i = 0;
	float cosPitch = cosf(pitching);
	float sinPitch = sinf(pitching);
	float cosYaw = cosf(heading);
	float sinYaw = sinf(heading);
	float cosRoll = cosf(rolling);
	float sinRoll = sinf(rolling);
	float rotMatrix[TRACKING_DIMENSION*TRACKING_DIMENSION] = { 0.f };
	float rotTracking2Sensor[TRACKING_DIMENSION * TRACKING_DIMENSION] = { 0.f };
	float rotTracking2Global[TRACKING_DIMENSION * TRACKING_DIMENSION] = { 0.f };
	float rotPosState[TRACKING_DIMENSION] = { 0.f };
	float rotVelState[TRACKING_DIMENSION] = { 0.f };
	float rotAccState[TRACKING_DIMENSION] = { 0.f };
	float rangexy = 0.f;
	uint8_t mNum = algObj->detectsListOutput->detectObjNum;
	sDetectPoint* detPointsList = algObj->detectsListOutput->detPointsList;
#if defined (DEBUG_LOG_TRK2GLOBAL)
	FILE* fp1 = NULL;
	/*float fakeTarget[3] = { 1,2,5 };*/
	float fakeTarget[3] = { 1,0,0 };
#endif
	tracking_inst = (sTrackingInst*)(algObj->tracking_inst);
	inst_module = (sTracking_moduleInstance *)tracking_inst->handle;
	tracker = &(tracking_inst->trajInfoOutput->trajList[num]);
	inst = (sTracking_objectUnit *)handle;
	/*if (fabsf(inst->kalman_state.S_hat[4]) > 10 && inst->kalman_state.H_s.array[0] <= 600)
	{
		return -1;
	}
	else if (fabsf(inst->kalman_state.S_hat[4]) > 15 && inst->kalman_state.H_s.array[0] > 600)
	{
		return -1;
	}*/

	//if (2 == g_byDevHwGen)
	//{
	//	if (inst->objKinematic.z_mean < -40)
	//		return -1;
	//	else if (inst->objKinematic.z_var > 1000)
	//		return -1;
	//}
	//else if (3 == g_byDevHwGen)
	//{
	//	;
	//}

#ifdef STATE_DETECTION_OUTPUT
	if(1)
#else
	if(inst->objManagement.state==TRACK_STATE_ACTIVE || inst->objManagement.state==TRACK_STATE_NEW_ACTIVE)
#endif
	{
		//sinPitch = -sinPitch;		// the following rotMatrix is derived using counterclockwise for yaw/pitch/roll,
		//							// but acturally we define pitch by clockwise.
		//rotMatrix[0] = cosPitch * cosYaw;
		//rotMatrix[1] = -sinYaw * cosRoll + cosYaw * sinPitch*sinRoll;
		//rotMatrix[2] = sinYaw * sinRoll + cosYaw * sinPitch*cosRoll;
		//rotMatrix[3] = sinYaw * cosPitch;
		//rotMatrix[4] = cosYaw * cosRoll + sinYaw * sinPitch*sinRoll;
		//rotMatrix[5] = -cosYaw * sinRoll + sinYaw * sinPitch * cosRoll;
		//rotMatrix[6] = -sinPitch;
		//rotMatrix[7] = cosPitch * sinRoll;
		//rotMatrix[8] = cosPitch * cosRoll;
		//tracking_matrixMultiply(TRACKING_DIMENSION, TRACKING_DIMENSION, 1, rotMatrix, &inst->kalman_state.S_hat[0], rotPosState);
		//tracking_matrixMultiply(TRACKING_DIMENSION, TRACKING_DIMENSION, 1, rotMatrix, &inst->kalman_state.S_hat[3], rotVelState);
		//tracking_matrixMultiply(TRACKING_DIMENSION, TRACKING_DIMENSION, 1, rotMatrix, &inst->kalman_state.S_hat[6], rotAccState);

		rotMatrix[0] = cosPitch * cosYaw;
		rotMatrix[1] = -sinYaw * cosRoll + cosYaw * sinPitch * sinRoll;
		rotMatrix[2] = sinYaw * sinRoll + cosYaw * sinPitch * cosRoll;
		rotMatrix[3] = sinYaw * cosPitch;
		rotMatrix[4] = cosYaw * cosRoll + sinYaw * sinPitch * sinRoll;
		rotMatrix[5] = -cosYaw * sinRoll + sinYaw * sinPitch * cosRoll;
		rotMatrix[6] = -sinPitch;
		rotMatrix[7] = cosPitch * sinRoll;
		rotMatrix[8] = cosPitch * cosRoll;
		/*rotTracking2Sensor[1] = 1.f;
		rotTracking2Sensor[5] = 1.f;
		rotTracking2Sensor[6] = 1.f;*/
		rotTracking2Sensor[0] = 1.f;
		rotTracking2Sensor[4] = 1.f;
		rotTracking2Sensor[8] = 1.f;
		tracking_matrixMultiply(TRACKING_DIMENSION, TRACKING_DIMENSION, TRACKING_DIMENSION, rotMatrix, rotTracking2Sensor, rotTracking2Global);
//		tracking_matrixMultiply(TRACKING_DIMENSION, TRACKING_DIMENSION, 1, rotTracking2Global, &inst->kalman_state.S_hat[0], rotPosState);
//#if defined DEBUG_LOG_TRK2GLOBAL
//		/*tracking_matrixMultiply(TRACKING_DIMENSION, TRACKING_DIMENSION, 1, rotTracking2Global, fakeTarget, rotPosState);*/
//#endif
//		tracking_matrixMultiply(TRACKING_DIMENSION, TRACKING_DIMENSION, 1, rotTracking2Global, &inst->kalman_state.S_hat[3], rotVelState);
//		tracking_matrixMultiply(TRACKING_DIMENSION, TRACKING_DIMENSION, 1, rotTracking2Global, &inst->kalman_state.S_hat[6], rotAccState);
		tracking_matrixMultiply(TRACKING_DIMENSION, TRACKING_DIMENSION, 1, rotTracking2Global, &smoothTraj[inst->uid].array[0], rotPosState);
#if defined DEBUG_LOG_TRK2GLOBAL
		/*tracking_matrixMultiply(TRACKING_DIMENSION, TRACKING_DIMENSION, 1, rotTracking2Global, fakeTarget, rotPosState);*/
#endif
		tracking_matrixMultiply(TRACKING_DIMENSION, TRACKING_DIMENSION, 1, rotTracking2Global, &smoothTraj[inst->uid].array[3], rotVelState);
		tracking_matrixMultiply(TRACKING_DIMENSION, TRACKING_DIMENSION, 1, rotTracking2Global, &inst->kalman_state.S_hat[6], rotAccState);
		inst_module->uidTrack[num] = inst->uid;
		//tracker->id = (uint16_t)(inst->uid);
		tracker->id = (uint16_t)(inst->tid);
		tracker->traj.x = (int32_t)(rotPosState[0] * ONE6FORMAT_SCALE);
		tracker->traj.y = (int32_t)(rotPosState[1] * ONE6FORMAT_SCALE);
		tracker->traj.z = (int32_t)(rotPosState[2] * ONE6FORMAT_SCALE);
		tracker->traj.vx = (int16_t)(rotVelState[0] * ONE6FORMAT_SCALE);
		tracker->traj.vy = (int16_t)(rotVelState[1] * ONE6FORMAT_SCALE);
		tracker->traj.vz = (int16_t)(rotVelState[2] * ONE6FORMAT_SCALE);
		tracker->traj.ax = (int16_t)(rotAccState[0] * ONE6FORMAT_SCALE);
		tracker->traj.ay = (int16_t)(rotAccState[1] * ONE6FORMAT_SCALE);
		tracker->traj.az = (int16_t)(rotAccState[2] * ONE6FORMAT_SCALE);
		for (i = 0; i < SSIZE_XY; i++)
		{
			tracker->variance[i] = (uint16_t)(sqrtf(inst->kalman_state.P_hat[i*(SSIZE_XY + 1)]) * ONE6FORMAT_SCALE);
		}
		//tracker->variance[8] = (uint16_t)(inst->assoProperty.assMahDistance * ONE6FORMAT_SCALE);
		/*tracker->range = (uint32_t)(inst->kalman_state.H_s.vector.range*ONE6FORMAT_SCALE);*/
		tracker->range = (uint32_t)(smoothTraj[inst->uid].range * ONE6FORMAT_SCALE);
		//tracker->azimuth = (int16_t)(inst->kalman_state.H_s.vector.azimuthRad*RAD2DEG*ONE6FORMAT_SCALE );			
		//tracker->elevation = (int16_t)((inst->kalman_state.H_s.vector.pitchRad + pitching)*RAD2DEG*ONE6FORMAT_SCALE);
		rangexy = sqrtf(rotPosState[0] * rotPosState[0] + rotPosState[1] * rotPosState[1]);
		tracker->azimuth = (int16_t)(acosf(rotPosState[0] / rangexy)*RAD2DEG*ONE6FORMAT_SCALE);
		if (rotPosState[1] < 0.f)
		{
			tracker->azimuth = -tracker->azimuth;
		}
		//tracker->elevation = (int16_t)(asinf(rotPosState[2] / inst->kalman_state.H_s.vector.range)*RAD2DEG*ONE6FORMAT_SCALE);
		tracker->elevation = (int16_t)(asinf(rotPosState[2] / smoothTraj[inst->uid].range) * RAD2DEG * ONE6FORMAT_SCALE);
		//tracker->velocity = (int16_t)(inst->kalman_state.H_s.vector.doppler*ONE6FORMAT_SCALE);
		tracker->velocity = (int16_t)(smoothTraj[inst->uid].doppler * ONE6FORMAT_SCALE);
#if defined DEBUG_LOG_TRK2GLOBAL
		fopen_s(&fp1, "E:/record/20230306/record2.txt", "a+");
		fprintf(fp1, "%d	%u	%.3f	%.3f	%.3f	%.3f	%.3f	%.3f	%.3f	%.3f	%.3f	%.3f\n", \
			data_idx_gyro, inst->uid,rotPosState[0], rotPosState[1], rotPosState[2], rotVelState[0], rotVelState[1], rotVelState[2], \
			rotAccState[0], rotAccState[1], rotAccState[2], tracker->range*INV_ONE6FORMAT_SCALE, tracker->azimuth * INV_ONE6FORMAT_SCALE,\
			tracker->elevation * INV_ONE6FORMAT_SCALE, tracker->velocity * INV_ONE6FORMAT_SCALE);
		fclose(fp1);
#endif
		//tracker->mag = (uint16_t)(0.0f*ONE6FORMAT_SCALE);
		tracker->mag = (uint16_t)(inst->objKinematic.maxDistance*ONE6FORMAT_SCALE);
		tracker->ambiguous = inst->objKinematic.velUnambigProp.velUnambigDone;
		tracker->alive = inst->objManagement.lifetime;
		/*tracker->variance[0] = (uint16_t)(inst->objManagement.updatePast8frame * ONE6FORMAT_SCALE);
		tracker->variance[1] = (uint16_t)(inst->curTarget.mdScore * ONE6FORMAT_SCALE); 
		tracker->variance[2] = (uint16_t)(inst->assoProperty.assoThisScan * ONE6FORMAT_SCALE);*/
		//tracker->classification = (uint8_t)(inst->objManagement.classification);

		if (2 == g_byDevHwGen)
		{
			if (inst->objKinematic.z_mean < -40)
				tracker->classification = 2U;
			else if (inst->objKinematic.z_var > 1000)
				tracker->classification = 2U;
			else
				tracker->classification = 1U;
		}
		else if (3 == g_byDevHwGen)
		{
			if (inst->objKinematic.z_mean < -40)
				tracker->classification = 2U;
			else
				tracker->classification = 1U;
		}

		tracker->probOfExisting = (uint8_t)(inst->objManagement.probOfExisting * ONE6FORMAT_SCALE);
		tracker->mag = (uint16_t)(inst->curTarget.fRCS * ONE6FORMAT_SCALE);
		switch (inst->objManagement.state)
		{
#ifdef STATE_DETECTION_OUTPUT
			if (gVelunambigType == WHOLE_SCAN)
			{
				case TRACK_STATE_INIT:
				{
					tracker->stateType = UNSTABLE_TRAJ;
					break;
				}
			}
			case TRACK_STATE_DETECTION:
			{
				tracker->stateType = UNSTABLE_TRAJ;
				break;
			}
#endif
			case TRACK_STATE_NEW_ACTIVE:
			{
			}
			case TRACK_STATE_ACTIVE:
			{
				tracker->stateType = STABLE_TRAJ;
				break;
			}
			default:
			{
				tracker->stateType = FREE_TRAJ;
				break;
			}
		}
		tracker->assocBit0 = 0;
		tracker->assocBit1 = 0;
		if (inst->assoProperty.assoCondenceMeasId >= 0)
		{
			tracker->associationNum = inst->assoProperty.assoCondenceMeasId;
			for (i = 0; i < mNum; i++) {
				if (inst->assoProperty.bitIndicatorsOfDetAsso[i >> 3] & (0x1 << (i & 0x7))) {
					if (detPointsList[i].id < 32) {
						tracker->assocBit0 |= (0x1 << detPointsList[i].id);
					}
					else if (detPointsList[i].id < 64) {
						tracker->assocBit1 |= (0x1 << (detPointsList[i].id - 32));
					}
#ifdef DEBUG_LOG_ZQ
					//my_printf("id %d bit0 bit1 %d %d", tracker->id, tracker->assocBit0, tracker->assocBit1);
#endif // DEBUG_LOG_ZQ

				}
			}

		}
		else
		{
			tracker->associationNum = 255;
			tracker->assocBit0 = 0;
			tracker->assocBit1 = 0;
		}
		tracker->forcastFrameNum = (uint16_t)(inst->objManagement.forcastFrameNum);
		return 0;
	}
	return -1;
}

// check input data
uint8_t tracking_dataCheck(int input_n)
{
	if (input_n == 0) {
		return false;
	}
	return true;
}

// reset this module
void tracking_reset()
{
}

// check the running status
MODULE_STATUS tracking_checkRunningStatus()
{
	return gTracking_inst->runningStatus;
}
