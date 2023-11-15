
#include <string.h>
#include <math.h>
#include <float.h>

#include "../include/tracking_common.h"
#include "../include/tracking_int.h"

/**
*  @b Description
*  @n
*		tracking Module calls this function to run tracking unit prediction step 
*
*  @param[in]  handle
*		This is handle to tracking unit
*
*  \ingroup tracking_ALG_UNIT_FUNCTION
*
*  @retval
*      None
*/

extern uint32_t g_detFrameId;
extern float g_detTimestamp;
extern float g_aziBeam;
uint32_t predictAll = 0;
uint32_t predictOmit = 0;
uint32_t omitThisFrame = 0;

void tracking_unitPredict(void *handle, sTracking_platformInfo* platformInfo)
{
    sTracking_objectUnit *inst;
	float temp1[SSIZE_XY*SSIZE_XY] = { 0.f };
	float temp3[SSIZE_XY*SSIZE_XY] = { 0.f };
	float S_apriori[SSIZE_XY] = { 0 };
	int dimLen = 0;
	float Q[SSIZE_XY*SSIZE_XY];
	float F[SSIZE_XY*SSIZE_XY];
    inst = (sTracking_objectUnit *)handle;
	dimLen = inst->tracking_params->stateVectorDimLength;
	inst->timestamp = g_curTimestamp;
	//inst->objManagement.lifetime++;
	memcpy(F, inst->tracking_params->F, sizeof(inst->tracking_params->F));
	memcpy(Q, inst->tracking_params->Q, sizeof(inst->tracking_params->Q));
	memset((uint8_t *)(inst->assoProperty.bitIndicatorsOfDetAsso), 0, sizeof(inst->assoProperty.bitIndicatorsOfDetAsso));
	inst->assoProperty.sameRootTracker.mdScore = 1000.f;
	//Q[0] = const_process_noise_stat[0] * 50.f;
	//Q[7] = const_process_noise_stat[1] * 30.f;
	//Q[14] = const_process_noise_stat[2] * 150.f;
	//Q[21] = const_process_noise_stat[3] * 100.f;
	//Q[28] = const_process_noise_stat[4] * 1500.f;
	//Q[35] = const_process_noise_stat[5] * 1000.f;
	/* obj.S_apriori_hat(1:mSize) = obj.F(1:mSize,1:mSize) * obj.S_hat(1:mSize) */
	tracking_matrixMultiply(SSIZE_XY, SSIZE_XY, 1, F, inst->kalman_state.S_hat, S_apriori);
	memcpy(inst->kalman_state.S_hat, S_apriori, sizeof(inst->kalman_state.S_hat));
	tracking_matrixMultiply(SSIZE_XY, SSIZE_XY, SSIZE_XY, F, inst->kalman_state.P_hat, temp1);
	tracking_matrixTransposeMultiply(SSIZE_XY, SSIZE_XY, SSIZE_XY, temp1, F, temp3);
//	/* obj.P_apriori(1:mSize,1:mSize) = obj.F(1:mSize,1:mSize) * obj.P(1:mSize,1:mSize) * obj.F(1:mSize,1:mSize)' + obj.Q(1:mSize,1:mSize) */
	tracking_matrixAdd(SSIZE_XY, SSIZE_XY, temp3, Q, temp1);
	tracking_matrixMakeSymmetrical(SSIZE_XY, temp1, inst->kalman_state.P_hat);

	tracking_cartesian2spherical(inst->tracking_params->stateVectorType, \
		inst->kalman_state.S_hat, inst->kalman_state.H_s.array);
	inst->objManagement.isNewTracker = 0;
#ifdef DEBUG_LOG_ZQ
		my_printf("--predInfo-- id %d, x %.2f, y%.2f,z %.2f,vx %.2f,vy %.2f,vz %.2f,ax %.2f,ay %.2f,az %.2f ,covY %.2f", \
		inst->tid, inst->kalman_state.S_hat[0], inst->kalman_state.S_hat[1], inst->kalman_state.S_hat[2], \
		inst->kalman_state.S_hat[3], inst->kalman_state.S_hat[4], inst->kalman_state.S_hat[5], \
		inst->kalman_state.S_hat[6], inst->kalman_state.S_hat[7], inst->kalman_state.S_hat[8],inst->kalman_state.P_hat[0]);
	
#endif // DEBUG_LOG

	//inst->objGeometryProperty.hasTrackerOfNearRegion = 0U;
	// update Vabs
	//tracking_calculateTrackerAbsVelocity(inst, platformInfo);

		calcZMeanVar(inst);
}

void tracking_tasPredict(sTracking_moduleInstance *inst, sMeasOutput *measInfo)
{
	sTracking_ListElem *tElem = NULL;
	uint16_t uid;
	int sameRootuid = 0;
	sTracking_objectUnit *tasTracker = NULL;
	sTracking_objectUnit *cfmTracker = NULL;
	sTracking_objectUnit *cfmSameRootTracker = NULL;
	int iSameRoot = 0;
#ifdef DEBUG_TIMESTAMP
	FILE* fp1 = NULL;
	fopen_s(&fp1, "E:/record/20230405/timestamp.txt", "a+");
#endif // DEBUG_TIMESTAMP
	tElem = tracking_listGetFirst(&inst->tasTrackList);
	while (tElem != 0)
	{
		uid = tElem->data;
		if (uid == inst->tasTargetId)
		{
			tasTracker = (sTracking_objectUnit *)inst->hTrack[uid];
			if (measInfo->condenceDone == 1U)
			{
				tasTracker->objManagement.lifetime++;
			}
#ifdef DEBUG_TIMESTAMP
			fprintf(fp1, "%u	%u	%.3f	%u	%.3f	%.3f	%.3f\n",g_scanType, g_detFrameId, g_detTimestamp, uid, g_curTimestamp, tasTracker->timestamp, g_curTimestamp - tasTracker->timestamp);
#endif // DEBUG_TIMESTAMP
			tracking_unitCalDtTransMatrix(inst, tasTracker);
			tracking_unitPredict(inst->hTrack[uid], &inst->platformInfo);

			break;
		}
		tElem = tracking_listGetNext(tElem);
	}
	if (gCfmBeamFlag == 1 && gVelunambigType == STEP_SCAN)
	{
		tElem = tracking_listGetFirst(&inst->twsTrackList);
		while (tElem != 0)
		{
			uid = tElem->data;
			if (uid == inst->tasTargetId)
			{
				cfmTracker = (sTracking_objectUnit *)inst->hTrack[uid];
				if (measInfo->condenceDone == 1U)
				{
					cfmTracker->objManagement.lifetime++;
				}
				tracking_unitCalDtTransMatrix(inst, cfmTracker);
				tracking_unitPredict(inst->hTrack[uid], &inst->platformInfo);
				if (cfmTracker->assoProperty.sameRootTracker.sameRootNum > 0)
				{
					for (iSameRoot = 0; iSameRoot < NUM_EXTERN_DOPPLER; iSameRoot++)
					{
						sameRootuid = cfmTracker->assoProperty.sameRootTracker.sameRootTrackerId[iSameRoot];
						if (sameRootuid != -1)
						{
							cfmSameRootTracker = (sTracking_objectUnit *)inst->hTrack[sameRootuid];
							cfmSameRootTracker->objManagement.lifetime++;
							tracking_unitCalDtTransMatrix(inst, cfmSameRootTracker);
							tracking_unitPredict(inst->hTrack[sameRootuid], &inst->platformInfo);
						}
					}
				}
				break;
			}
			tElem = tracking_listGetNext(tElem);
		}
	}
#ifdef DEBUG_TIMESTAMP
	fclose(fp1);
#endif // DEBUG_TIMESTAMP
}
void tracking_twsPredict(sTracking_moduleInstance *inst, sMeasOutput* measInfo)
{
	sTracking_ListElem *tElem = NULL;
	uint16_t uid;
	sTracking_objectUnit *twsTracker = NULL;
	sTracking_objectUnit *tasTracker = NULL;
	uint8_t num = measInfo->num;
	//sMeasurement_SphPoint u_tilda;
	sMeasurement_SphPoint* measurement;
	//float measArray[MSIZE_SPH] = { 0.f };
	int i;
	float range = 0.f, aziBeam = 0.f;
	uint8_t predictFlag = 0;
#ifdef DEBUG_TIMESTAMP
	FILE* fp1 = NULL;
	fopen_s(&fp1, "E:/record/20230417/timestamp6.txt", "a+");
#endif // DEBUG_TIMESTAMP
	omitThisFrame = 0;
	//aziBeam = asinf((float)(measInfo->aziBeamSin) * INV_ONE15FORMAT_SCALE);
	tElem = tracking_listGetFirst(&inst->twsTrackList);
	while (tElem != 0)
	{
		predictAll++;
		uid = tElem->data;
		twsTracker = (sTracking_objectUnit *)inst->hTrack[uid];
		if (inst->wholeSpaceScanCycleFinalBeamFlag == 1)  // start of current scan
		{
			twsTracker->objManagement.lifetime++;
			twsTracker->assoProperty.predictedThisScan = 0; //±£µ×Ô¤²â±êÖ¾
		}
		predictFlag = 0;
		//memset(&u_tilda, 0, sizeof(sMeasurement_SphPoint));
		if (inst->wholeSpaceScanCycleFinalBeamFlag == 1 && twsTracker->assoProperty.predictedThisScan == 0) {
			predictFlag = 1;
			twsTracker->assoProperty.predictedThisScan = 1;
		}
		else if (g_aziBeam >= twsTracker->kalman_state.H_s.array[1] && twsTracker->assoProperty.predictedThisScan == 0)
		//else if (aziBeam >= twsTracker->kalman_state.H_s.array[1] && twsTracker->assoProperty.predictedThisScan == 0)
		{
			predictFlag = 1;
			twsTracker->assoProperty.predictedThisScan = 1;
		}
		else
		{
			for (i = 0; i < num; i++)
			{
				measurement = &measInfo->measurement[i];
				measurement->unambigVel = measurement->vector.doppler;
				/*memcpy(measArray, measurement->array, sizeof(measurement->array));
				tracking_vectorSub(MEASUREMENT_VECTOR_SIZE, measArray, twsTracker->kalman_state.H_s.array, u_tilda.array);*/
				if (!g_byDummyTest)
				{
					//if (measArray[0] < 200 && measInfo->pointsIdInfo[i].detectionNum == 1 && measInfo->scanType == TWS_SCAN)	// TODO
					if (measurement->array[0] < 200 && measInfo->pointsIdInfo[i].detectionNum == 1 && measInfo->scanType == TWS_SCAN)
					{
#ifdef DEBUG_LOG_ZQ
						my_printf("dot %d is single det within 200m", i);
#endif // DEBUG_LOG_ZQ
						//continue;
					}
						
				}
				if (fabsf(measurement->array[0]- twsTracker->kalman_state.H_s.array[0]) < 40 && measurement->array[0] < 200)
				{
					predictFlag = 1;
					break;
				}
				/*else if (fabsf(measurement->array[0] - twsTracker->kalman_state.H_s.array[0]) < 40 && measurement->array[0]>=200 && fabsf(measurement->array[1] - twsTracker->kalman_state.H_s.array[1]) * measurement->array[0] < 50.f)
				{
					predictFlag = 1;
					break;
				}*/
				else if (fabsf(measurement->array[0] - twsTracker->kalman_state.H_s.array[0]) < 40 &&\
					measurement->array[0] >= 200 && measurement->array[0] < 400 && fabsf(measurement->array[1] - twsTracker->kalman_state.H_s.array[1]) * RAD2DEG < 20.f)
				{
					predictFlag = 1;
					break;
				}
				else if (fabsf(measurement->array[0] - twsTracker->kalman_state.H_s.array[0]) < 40 &&\
					measurement->array[0] > 400 && fabsf(measurement->array[1] - twsTracker->kalman_state.H_s.array[1]) * RAD2DEG < 10.f)
				{
					predictFlag = 1;
					break;
				}
			}
		}
		if (predictFlag != 1)
		{
			tElem = tracking_listGetNext(tElem);
			predictOmit++;
			omitThisFrame++;
			continue;
		}
		/*if (gVelunambigType == STEP_SCAN && twsTracker->objKinematic.velUnambigProp.velUnambigDone == 0)
		{
			tElem = tracking_listGetNext(tElem);
			continue;
		}*/
		tracking_unitCalDtTransMatrix(inst, twsTracker);
		tracking_unitPredict(inst->hTrack[uid], &inst->platformInfo);
		tElem = tracking_listGetNext(tElem);
	}
	/** we allow the tas tracker to update even though tws scan, so a predict step for each tas tracker comes */
	tElem = tracking_listGetFirst(&inst->tasTrackList);
	while (tElem != 0)
	{
		uid = tElem->data;
		tasTracker = (sTracking_objectUnit *)inst->hTrack[uid];
#ifdef DEBUG_TIMESTAMP
		//fprintf(fp1, "%u	%u	%.3f	%u	%.3f	%.3f	%.3f\n",g_scanType, g_detFrameId, g_detTimestamp, uid, g_curTimestamp, tasTracker->timestamp, g_curTimestamp - tasTracker->timestamp);
		//fprintf(fp1, "%u	%u	%u	%u	%u", g_detFrameId, g_scanType, predictAll, predictAll - predictOmit, predictOmit);
#endif // DEBUG_TIMESTAMP
		tracking_unitCalDtTransMatrix(inst, tasTracker);
		tracking_unitPredict(inst->hTrack[uid], &inst->platformInfo);
		tElem = tracking_listGetNext(tElem);
	}
#ifdef DEBUG_TIMESTAMP
	fprintf(fp1, "%u	%u	%u	%u	%u	%u	%u\n", g_detFrameId, g_scanType, inst->twsTrackList.count,omitThisFrame,predictAll, predictAll - predictOmit, predictOmit);
	fclose(fp1);
#endif // DEBUG_TIMESTAMP
}
/**
 * @b Description
 * @n
 *      tracking Module calls this function to perform an dt and transfer Matrix calculation for the tracking unit.
 *
 * @param[in] handle
 *		This is handle to tracking Module
 * @param[in] pTracker
 *
 *
 */
void tracking_unitCalDtTransMatrix(void *handle, sTracking_objectUnit* pTracker)
{
	float dt = 0.f;
	sTracking_moduleInstance *inst;
	inst = (sTracking_moduleInstance *)handle;
	inst->tracking_params->dt = g_curTimestamp - pTracker->timestamp;
	if (inst->tracking_params->dt<0.001f || inst->tracking_params->dt>5.f)
	{
		if (pTracker->objManagement.tasFlag == 1)
		{
			inst->tracking_params->dt = FRAME_TIME_TAS;
		}
		else
		{
			inst->tracking_params->dt = FRAME_TIME_TWS;
		}
	}
	pTracker->timestamp = g_curTimestamp;
	dt = inst->tracking_params->dt;
	tracking_cal_FQ(inst, dt, pTracker);
}

void calcZMeanVar(sTracking_objectUnit* inst)
{
	float lastMean = inst->objKinematic.z_mean;
	float lastVar = inst->objKinematic.z_var;
	uint16_t lastSampleNum = inst->objKinematic.sampleNum;
	float newMean = 0.f;
	float newVar = 0.f;
	uint16_t newSampleNum = lastSampleNum + 1;

	newMean = lastMean + (inst->kalman_state.S_hat[2] - lastMean) / newSampleNum;
	newVar = lastSampleNum * (inst->kalman_state.S_hat[2] - lastMean) * (inst->kalman_state.S_hat[2] - lastMean) / newSampleNum / newSampleNum \
		+ lastSampleNum * lastVar / newSampleNum;

	inst->objKinematic.z_mean = newMean;
	inst->objKinematic.z_var = newVar;
	inst->objKinematic.sampleNum = newSampleNum;
}