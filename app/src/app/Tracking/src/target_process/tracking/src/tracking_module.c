
#include <math.h>
#include <float.h>
#include "../include/tracking_common.h"
#include "../include/tracking_int.h"

#ifdef MULTI_AZIMU
#define NUM_DOPPLERS (NUM_EXTERN_DOPPLER+1)
#else
#define NUM_DOPPLERS (1)
#endif

/**
*  @b Description
*  @n
*      This is a MODULE level predict function. The function is called by external step function to perform unit level kalman filter predictions
*
*  @param[in]  inst
*      Pointer to tracking module instance
*
*  \ingroup TRACKING_ALG_MODULE_FUNCTION
*
*  @retval
*      None
*/

void tracking_modulePredict(sTracking_moduleInstance *inst, sMeasOutput *measInfo)
{
	//tracking_cal_dt_transMatrix(inst);

	if (g_scanType == TAS_SCAN)
	{
		tracking_tasPredict(inst, measInfo);
	}
	else
	{
		tracking_twsPredict(inst, measInfo);
	}
}





/**
*  @b Description
*  @n
*      This is a MODULE level associatiation function. The function is called by external step function to associate measurement points with known targets
*
*  @param[in]  inst
*      Pointer to TRACKING module instance
*  @param[in]  point
*      Pointer to an array of input measurments. Each measurement has range/angle/radial velocity information
*  @param[in]  num
*      Number of input measurements
*
*  \ingroup TRACKING_ALG_MODULE_FUNCTION
*
*  @retval
*      None
*/
void tracking_moduleAssociate(sTracking_moduleInstance *inst, sMeasOutput *measInfo)
{
	if (g_scanType == TAS_SCAN)
	{
		tracking_tasAssociate(inst, measInfo);
	}
	else
	{
		tracking_twsAssociate(inst, measInfo);
	}
}

/**
*  @b Description
*  @n
*      This is a MODULE level update function. The function is called by external step function to perform unit level kalman filter updates
*
*  @param[in]  inst
*      Pointer to tracking module instance
*  @param[in]  point
*      Pointer to an array of input measurments. Each measurement has range/angle/radial velocity information
*  @param[in]  var
*      Pointer to an array of input measurment variances. Set to NULL if variances are unknown
*  @param[in]  num
*      Number of input measurements
*
*  \ingroup TRACKING_ALG_MODULE_FUNCTION
*
*  @retval
*      None
*/

void tracking_moduleUpdate(sTracking_moduleInstance *inst, sMeasOutput *measInfo)
{
	if (g_scanType == TAS_SCAN)
	{
		tracking_tasUpdate(inst, measInfo);
	}
	else
	{
		tracking_twsUpdate(inst, measInfo);
	}
}
void tracking_targetMerge(sTracking_moduleInstance *inst, sTracking_ListObj *activeList)
{
	int i, j, k, gateMerge, gateAlive;
	float gateX, gateY, gateZ, gateVx, gateVy, gateVz;
	float Xi, Xj, Yi, Yj, Zi, Zj, Vxi, Vxj, Vyi, Vyj, Vzi, Vzj,iRange,jRange,iDopper,jDopper,\
		iAzimuth,jAzimuth,iPitch,jPitch;
	sTracking_ListElem *iElem, *jElem;
	sTracking_objectUnit *iUnit, *jUnit;
	uint16_t iData, jData;
	sTracking_ListElem *tElem = NULL;
	sTracking_ListElem *tElemToRemove = NULL;
	sTracking_enum_state state = TRACK_STATE_INIT;
	uint16_t uid = 0;
	int iAlive, jAlive;
	gateX = 10.0f;
	gateY = 10.0f;
	gateZ = 10.f;
	gateVx = 3.0f;
	gateVy = 3.0f;
	gateVz = 3.0f;
	gateMerge = 4;
	gateAlive = 20;

	if (inst->wholeSpaceScanCycleFinalBeamFlag == 1)
	{
		for (iElem = tracking_listGetFirst(activeList), i = 0; iElem != 0; iElem = tracking_listGetNext(iElem), i++)
		{
			iData = iElem->data;
			iUnit = (sTracking_objectUnit*)inst->hTrack[iData];
			if (iUnit->objMerge.isCurrentScanMerge)
			{
				iUnit->objMerge.mergeScanCycleCnt++;
			}
			else
			{
				iUnit->objMerge.mergeScanCycleCnt = iUnit->objMerge.mergeScanCycleCnt ? iUnit->objMerge.mergeScanCycleCnt - 1 : 0;
			}
			iUnit->objMerge.isCurrentScanMerge = 0;
		}
	}

	for (iElem = tracking_listGetFirst(activeList), i = 0; iElem != 0; iElem = tracking_listGetNext(iElem), i++)
	{
		iData = iElem->data;
		iUnit = (sTracking_objectUnit*)inst->hTrack[iData];
		Xi = iUnit->kalman_state.S_hat[0];
		Yi = iUnit->kalman_state.S_hat[1];
		Zi = iUnit->kalman_state.S_hat[2];
		Vxi = iUnit->kalman_state.S_hat[3];
		Vyi = iUnit->kalman_state.S_hat[4];
		Vzi = iUnit->kalman_state.S_hat[5]; 
		iRange = iUnit->kalman_state.H_s.vector.range;
		iDopper = iUnit->kalman_state.H_s.vector.doppler;
		iAzimuth = iUnit->kalman_state.H_s.vector.azimuthRad;
		iPitch = iUnit->kalman_state.H_s.vector.pitchRad;
		iAlive = iUnit->objManagement.lifetime;
		/*if (iUnit->objManagement.state == TRACK_STATE_DETECTION)
			continue;*/
		if (iAlive <= 3) continue;

		for (jElem = tracking_listGetFirst(activeList), j = 0; jElem != 0; jElem = tracking_listGetNext(jElem), j++)
		{
			jData = jElem->data;
			jUnit = (sTracking_objectUnit*)inst->hTrack[jData];
			if (j <= i)	continue;				
			/*if (jUnit->objManagement.state == TRACK_STATE_DETECTION)
				continue;*/
			jAlive = jUnit->objManagement.lifetime;
			if (jAlive <= 3) continue;
			Xj = jUnit->kalman_state.S_hat[0];
			Yj = jUnit->kalman_state.S_hat[1];
			Zj = jUnit->kalman_state.S_hat[2];
			Vxj = jUnit->kalman_state.S_hat[3];
			Vyj = jUnit->kalman_state.S_hat[4];
			Vzj = jUnit->kalman_state.S_hat[5];
			jRange = jUnit->kalman_state.H_s.vector.range;
			jDopper = jUnit->kalman_state.H_s.vector.doppler;
			jAzimuth = jUnit->kalman_state.H_s.vector.azimuthRad;
			jPitch = jUnit->kalman_state.H_s.vector.pitchRad;
			
			if (((fabsf(Xi - Xj) < gateX) && (fabsf(Yi - Yj) < gateY) && (fabsf(Zi - Zj) < gateZ)\
				&& (fabsf(Vxi - Vxj) < gateVx) && (fabsf(Vyi - Vyj) < gateVy) && (fabsf(Vzi - Vzj) < gateVz)) || \
				(iUnit->assoProperty.assoCondenceMeasId == jUnit->assoProperty.assoCondenceMeasId&&\
				(fabsf(iRange - jRange) < gateX) && (fabsf(iDopper - jDopper) < gateVx / 2.f) &&\
					(fabsf(iAzimuth - jAzimuth) < 4.f*DEG2RAD) && (fabsf(iPitch - jPitch) < 3.f*DEG2RAD))) 
			{
				iUnit->objMerge.isCurrentScanMerge = 1;
				jUnit->objMerge.isCurrentScanMerge = 1;
				if (iAlive >= jAlive) 
				{
					if (jUnit->objMerge.mergeScanCycleCnt >= gateMerge && jAlive < gateAlive && isTrackerCanDelete(jUnit))
					{
						jUnit->objManagement.state = TRACK_STATE_FREE;
#ifdef DEBUG_LOG_ZQ
						my_printf("debug %d id %d", 6, jUnit->tid);
#endif // DEBUG_LOG

					}
				}
				else
				{
					if (iUnit->objMerge.mergeScanCycleCnt >= 5 && iAlive < 20 && isTrackerCanDelete(iUnit))
					{
						iUnit->objManagement.state = TRACK_STATE_FREE;
#ifdef DEBUG_LOG_ZQ
						my_printf("debug %d id %d", 7, iUnit->tid);
#endif // DEBUG_LOG

					}
				}
				
//				if (iAlive >= jAlive)
//				{
//					if (isTrackerCanDelete(jUnit)) {
//						jUnit->objManagement.state = TRACK_STATE_FREE;
//#ifdef DEBUG_LOG_ZQ
//						my_printf("debug %d id %d", 6, jUnit->uid);
//#endif // DEBUG_LOG
//
//					}
//				}
//				else
//				{
//					if (isTrackerCanDelete(iUnit)) {
//						iUnit->objManagement.state = TRACK_STATE_FREE;
//#ifdef DEBUG_LOG_ZQ
//						my_printf("debug %d id %d", 7, iUnit->uid);
//#endif // DEBUG_LOG
//
//					}
//				}
			}
		}
	}
}

void tracking_targetMergeTAS(sTracking_moduleInstance* inst, sTracking_ListObj* activeList, sTracking_ListObj* TWSList)
{
	int i, j, k;
	float gateX, gateY, gateZ, gateVx, gateVy, gateVz;
	float Xi, Xj, Yi, Yj, Zi, Zj, Vxi, Vxj, Vyi, Vyj, Vzi, Vzj, iRange, jRange, iDopper, jDopper, \
		iAzimuth, jAzimuth, iPitch, jPitch;
	sTracking_ListElem* iElem, * jElem;
	sTracking_objectUnit* iUnit, * jUnit;
	uint16_t iData, jData;
	sTracking_ListElem* tElem = NULL;
	sTracking_ListElem* tElemToRemove = NULL;
	sTracking_enum_state state = TRACK_STATE_INIT;
	uint16_t uid = 0;
	int iAlive, jAlive;
	gateX = 10.0f;
	gateY = 10.0f;
	gateZ = 10.f;
	gateVx = 3.0f;
	gateVy = 3.0f;
	gateVz = 3.0f;
	for (iElem = tracking_listGetFirst(activeList), i = 0; iElem != 0; iElem = tracking_listGetNext(iElem), i++)
	{
		iData = iElem->data;
		iUnit = (sTracking_objectUnit*)inst->hTrack[iData];
		Xi = iUnit->kalman_state.S_hat[0];
		Yi = iUnit->kalman_state.S_hat[1];
		Zi = iUnit->kalman_state.S_hat[2];
		Vxi = iUnit->kalman_state.S_hat[3];
		Vyi = iUnit->kalman_state.S_hat[4];
		Vzi = iUnit->kalman_state.S_hat[5];
		iRange = iUnit->kalman_state.H_s.vector.range;
		iDopper = iUnit->kalman_state.H_s.vector.doppler;
		iAzimuth = iUnit->kalman_state.H_s.vector.azimuthRad;
		iPitch = iUnit->kalman_state.H_s.vector.pitchRad;
		iAlive = iUnit->objManagement.lifetime;
		/*if (iUnit->objManagement.state == TRACK_STATE_DETECTION)
			continue;*/
		if (iAlive <= 3) continue;

		for (jElem = tracking_listGetFirst(activeList), j = 0; jElem != 0; jElem = tracking_listGetNext(jElem), j++)
		{
			jData = jElem->data;
			jUnit = (sTracking_objectUnit*)inst->hTrack[jData];
			if (j <= i)	continue;
			/*if (jUnit->objManagement.state == TRACK_STATE_DETECTION)
				continue;*/
			jAlive = jUnit->objManagement.lifetime;
			if (jAlive <= 3) continue;
			Xj = jUnit->kalman_state.S_hat[0];
			Yj = jUnit->kalman_state.S_hat[1];
			Zj = jUnit->kalman_state.S_hat[2];
			Vxj = jUnit->kalman_state.S_hat[3];
			Vyj = jUnit->kalman_state.S_hat[4];
			Vzj = jUnit->kalman_state.S_hat[5];
			jRange = jUnit->kalman_state.H_s.vector.range;
			jDopper = jUnit->kalman_state.H_s.vector.doppler;
			jAzimuth = jUnit->kalman_state.H_s.vector.azimuthRad;
			jPitch = jUnit->kalman_state.H_s.vector.pitchRad;

			if (((fabsf(Xi - Xj) < gateX) && (fabsf(Yi - Yj) < gateY) && (fabsf(Zi - Zj) < gateZ)\
				&& (fabsf(Vxi - Vxj) < gateVx) && (fabsf(Vyi - Vyj) < gateVy) && (fabsf(Vzi - Vzj) < gateVz)) || \
				(iUnit->assoProperty.assoCondenceMeasId == jUnit->assoProperty.assoCondenceMeasId && \
					(fabsf(iRange - jRange) < gateX) && (fabsf(iDopper - jDopper) < gateVx / 2.f) && \
					(fabsf(iAzimuth - jAzimuth) < 4.f * DEG2RAD) && (fabsf(iPitch - jPitch) < 3.f * DEG2RAD)))
			{
				if (iAlive >= jAlive)
				{
					if (isTrackerCanDelete(jUnit)) {
						jUnit->objManagement.state = TRACK_STATE_FREE;
#ifdef DEBUG_LOG_ZQ
						my_printf("debug %d id %d", 6, jUnit->tid);
#endif // DEBUG_LOG

					}
				}
				else
				{
					if (isTrackerCanDelete(iUnit)) {
						iUnit->objManagement.state = TRACK_STATE_FREE;
#ifdef DEBUG_LOG_ZQ
						my_printf("debug %d id %d", 7, iUnit->tid);
#endif // DEBUG_LOG

					}
				}
			}
		}

		for (jElem = tracking_listGetFirst(TWSList), j = 0; jElem != 0; jElem = tracking_listGetNext(jElem), j++)
		{
			jData = jElem->data;
			jUnit = (sTracking_objectUnit*)inst->hTrack[jData];
			//if (j <= i)	continue;
			/*if (jUnit->objManagement.state == TRACK_STATE_DETECTION)
				continue;*/
			jAlive = jUnit->objManagement.lifetime;
			if (jAlive <= 3) continue;
			Xj = jUnit->kalman_state.S_hat[0];
			Yj = jUnit->kalman_state.S_hat[1];
			Zj = jUnit->kalman_state.S_hat[2];
			Vxj = jUnit->kalman_state.S_hat[3];
			Vyj = jUnit->kalman_state.S_hat[4];
			Vzj = jUnit->kalman_state.S_hat[5];
			jRange = jUnit->kalman_state.H_s.vector.range;
			jDopper = jUnit->kalman_state.H_s.vector.doppler;
			jAzimuth = jUnit->kalman_state.H_s.vector.azimuthRad;
			jPitch = jUnit->kalman_state.H_s.vector.pitchRad;

			if (((fabsf(Xi - Xj) < gateX) && (fabsf(Yi - Yj) < gateY) && (fabsf(Zi - Zj) < gateZ)\
				&& (fabsf(Vxi - Vxj) < gateVx) && (fabsf(Vyi - Vyj) < gateVy) && (fabsf(Vzi - Vzj) < gateVz)) || \
				(iUnit->assoProperty.assoCondenceMeasId == jUnit->assoProperty.assoCondenceMeasId && \
					(fabsf(iRange - jRange) < gateX) && (fabsf(iDopper - jDopper) < gateVx / 2.f) && \
					(fabsf(iAzimuth - jAzimuth) < 4.f * DEG2RAD) && (fabsf(iPitch - jPitch) < 3.f * DEG2RAD)))
			{
				if (iAlive >= jAlive)
				{
					if (isTrackerCanDelete(jUnit)) {
						jUnit->objManagement.state = TRACK_STATE_FREE;
#ifdef DEBUG_LOG_ZQ
						my_printf("debug %d id %d", 6, jUnit->tid);
#endif // DEBUG_LOG

					}
				}
				else
				{
					if (isTrackerCanDelete(iUnit)) {
						iUnit->objManagement.state = TRACK_STATE_FREE;
#ifdef DEBUG_LOG_ZQ
						my_printf("debug %d id %d", 7, iUnit->tid);
#endif // DEBUG_LOG

					}
				}
			}
		}
	}
}

void tracking_moduleMerge(sTracking_moduleInstance *inst)
{
	if (g_scanType == TAS_SCAN)
	{
		tracking_targetMergeTAS(inst, &inst->tasTrackList, &inst->twsTrackList);
	}
	else
	{
		tracking_targetMerge(inst, &inst->twsTrackList);
	}
}

/**
*  @b Description
*  @n
*      This is a MODULE level tracking manager function. The function is called by external step function to perform unit level of deleting tracker
*
*  @param[in]  inst
*      Pointer to tracking module instance

*  \ingroup TRACKING_ALG_MODULE_FUNCTION
*
*  @retval
*      None
*/
void tracking_moduleManager(sTracking_moduleInstance *inst)
{
	if (g_scanType == TAS_SCAN)
	{
		tracking_tasManager(inst);
	}
	else
	{
		tracking_twsManager(inst);
	}
	//tracking_TasTwsTypeManagement(inst);
}
// TRACK_STATE_INIT tracker associate and unambigious velocity
/**
 * @b Description
 * @n
 *      .
 * 
 * @param[in] inst
 * @param[in] measInfo
 *       
 *       
 */
void tracking_moduleInitAssociate(sTracking_moduleInstance *inst, sMeasOutput *measInfo)
{
	if (inst->tracking_params->workMode == 0)
	{
		tracking_twsModeInitAssociate(inst, measInfo);
	}
	else
	{
		tracking_tasModeInitAssociate(inst, measInfo);
	}
}
/**
 * @b Description
 * @n
 *      .
 * 
 * @param[in] inst
 * @param[in] pTracker
 * @param[in] measInfo
 *       
 *       
 */
void tracking_unitInitStateAssociate(sTracking_moduleInstance *inst, sTracking_objectUnit *pTracker, sMeasOutput *measInfo)
{
	int n;
	sMeasurement_SphPoint* measurement;
	uint8_t unambigiousFlag = 0;
	for (n = 0; n < measInfo->num; n++)
	{
		if (inst->bestIndex[n] == TRACKING_ID_POINT_NOT_ASSOCIATED) {
			measurement = &measInfo->measurement[n];
			if (tracking_initVelocityUnambigious(inst,pTracker, measurement))
			{
				unambigiousFlag = 1;
				inst->bestIndex[n] = pTracker->uid;
				pTracker->objManagement.state = TRACK_STATE_DETECTION;
				break;
			}
		}
	}
	if (unambigiousFlag == 0)
	{
		if (inst->tracking_params->workMode == 1)
		{
			pTracker->objManagement.state = TRACK_STATE_FREE;
#ifdef DEBUG_LOG_ZQ
			my_printf("init2free %d", pTracker->tid);
#endif // DEBUG_LOG_ZQ

		}
		else //inst->tracking_params->workMode = 0
		{
			// init detections in k-1 frame, init associate in k frame, when k frame ends(wholeSpaceScanCycleCnt=k+1), 
			// free the init state detections 
			if (inst->wholeSpaceScanCycleCnt - 1 - pTracker->assoProperty.lastSeenWholeScanId >= 1)
			{
				pTracker->objManagement.state = TRACK_STATE_FREE;
#ifdef DEBUG_LOG_ZQ
				my_printf("init2free %d", pTracker->tid);
#endif // DEBUG_LOG_ZQ
			}
		}
	}
}
/**
 * @b Description
 * @n
 *      .
 * 
 * @param[in] pTracker
 * @param[in] measurement
 * @param[in] waveType
 *       
 * @return 
 *       
 */
uint8_t tracking_initVelocityUnambigious(sTracking_moduleInstance *inst,sTracking_objectUnit *pTracker, sMeasurement_SphPoint* measurement)
{
	uint8_t matchedFlag = 0;
	uint8_t matchNum = 0;
	int N = 0, M = 0;
	float trackerVel = 0.f, measVel = 0.f, velScopeTrack = 0.f, velScopeMeas = 0.f;
	float initVel = pTracker->kalman_state.H_s.vector.doppler;
	int i, j, n, m;
	float measArray[MSIZE_SPH] = { 0.f };
	float tempDoppler = 0.f;
	sTracking_ListElem *tElemFree = NULL;
	sTracking_ListElem *tElemActive = NULL;
	sTracking_ListElem *tElemMinScore = NULL;
	int unitStart_ret = -1;
	float maxRange;
	uint8_t uid;
	sTracking_objectUnit *sameRootTracker = NULL;
	sTracking_objectUnit *minScoreTracker = NULL;
	sMeasurement_enum_wavetype waveType = measurement->waveType;
	sTracking_objectUnit *printTracker = NULL;
	float rangeTh = 8.f;
	if (g_scanType == TWS_SCAN)
	{
		rangeTh = 15.f;
	}
	if (pTracker->objManagement.allocationWavetype == waveType)
	{
#ifdef DEBUG_LOG_ZQ
		//my_printf("")
#endif // DEBUG_LOG_ZQ
		return 0;
	}
	if (fabsf(pTracker->kalman_state.H_s.vector.range - measurement->vector.range) > rangeTh)
	{
#ifdef DEBUG_LOG_ZQ
		//my_printf("delta range is out of threthld init meas [%d %d] dr %.2f",pTracker->uid, measurement->detId,\
			pTracker->kalman_state.H_s.vector.range - measurement->vector.range);
#endif // DEBUG_LOG_ZQ
		return 0;
	}
	if (waveType == WAVE_TYPE_0)
	{
		velScopeMeas = DOPPLER_SCOPE_WAVETYPE_0;
		velScopeTrack = DOPPLER_SCOPE_WAVETYPE_1;
		M = AMBI_NUM_T0;
		N = AMBI_NUM_T1;
	}
	else
	{
		velScopeMeas = DOPPLER_SCOPE_WAVETYPE_1;
		velScopeTrack = DOPPLER_SCOPE_WAVETYPE_0;
		M = AMBI_NUM_T1;
		N = AMBI_NUM_T0;
	}
	for (i = -N; i <= N; i++)
	{
		trackerVel = initVel + i * velScopeTrack;
		if (fabsf(trackerVel) > MAXVEL)
			continue;
		for (j = -M; j <= M; j++) {
			measVel = measurement->vector.doppler + j * velScopeMeas;
			if (fabsf(measVel) > MAXVEL)
				continue;
			if (fabsf(trackerVel - measVel) < 2.f)
			{
				matchedFlag = 1;
				matchNum++;
				memcpy(measArray, measurement->array, sizeof(measurement->array));
				measArray[MSIZE_SPH - 1] = measVel;
				if (matchNum == 1)
				{
					pTracker->kalman_state.H_s.vector.doppler = trackerVel;
					pTracker->assoProperty.assoUnambiVel = measVel;
					tracking_spherical2cartesian(pTracker->tracking_params->stateVectorType, measArray,
						pTracker->kalman_state.S_hat);
					memcpy(pTracker->kalman_state.H_s.array, measArray, sizeof(measArray));
					pTracker->timestamp = g_curTimestamp;
					pTracker->assoProperty.lastSeenTime = g_curTimestamp;
					pTracker->assoProperty.lastSeenWholeScanId = inst->wholeSpaceScanCycleCnt;
					pTracker->objManagement.forcastFrameNum = 0;
					memset(pTracker->assoProperty.bitIndicatorsOfDetAsso, 0, sizeof(pTracker->assoProperty.bitIndicatorsOfDetAsso));
					m = measurement->detId;
					pTracker->assoProperty.bitIndicatorsOfDetAsso[m >> 3] |= (1 << (m & 0x7));
					/*for (n = 0; n < measurement->detectionNum; n++)
					{
						m = measurement->detectionId[n];
						pTracker->assoProperty.bitIndicatorsOfDetAsso[m >> 3] |= (1 << (m & 0x7));
					}*/
#ifdef DEBUG_LOG_ZQ
					my_printf("init2det: id %d y %.2f doppler %.2f", pTracker->tid, \
						pTracker->kalman_state.S_hat[10], pTracker->kalman_state.H_s.vector.doppler);
#endif // DEBUG_LOG_ZQ

					break;
				}
				else if (matchNum == 2)
				{
					measurement->vector.doppler = measVel;
					tElemFree = tracking_listGetFirst(&inst->freeList);
					if (tElemFree == 0)
					{
						// TODO: 
						maxRange = 0.1f;
						tElemActive = tracking_listGetFirst(&inst->twsTrackList);
						while (tElemActive != 0)
						{
							uid = tElemActive->data;
							minScoreTracker = (sTracking_objectUnit *)inst->hTrack[uid];
							if (minScoreTracker->kalman_state.H_s.vector.range > maxRange && minScoreTracker->objManagement.state != TRACK_STATE_ACTIVE) {
								maxRange = minScoreTracker->kalman_state.H_s.vector.range;
								tElemMinScore = tElemActive;
							}
							tElemActive = tracking_listGetNext(tElemActive);
						}

						if (measurement->vector.range < maxRange)
						{
#ifdef DEBUG_LOG_ZQ
							minScoreTracker = (sTracking_objectUnit *)inst->hTrack[tElemMinScore->data];
							my_printf("Delete, id[%d]\n", (int)minScoreTracker->tid);
#endif
							tracking_listRemoveElement(&inst->twsTrackList, tElemMinScore);
							inst->targetNumCurrent--;
							tracking_listEnqueue(&inst->freeList, tElemMinScore);
							tElemFree = tracking_listGetFirst(&inst->freeList);
						}
					}
					// 3.allocate new tracker
					tElemFree = tracking_listDequeue(&inst->freeList);
					if (tElemFree != 0) 
					{
						inst->targetNumTotal++;
						tempDoppler = measurement->vector.doppler;
						measurement->vector.doppler = measArray[MSIZE_SPH - 1];
						unitStart_ret = tracking_unitStart(inst->hTrack[tElemFree->data], &inst->platformInfo, \
							measurement, inst->bestIndex, inst->wholeSpaceScanCycleCnt, waveType, inst->targetNumTotal);
						measurement->vector.doppler = tempDoppler;
#ifdef DEBUG_LOG_ZQ
						
						printTracker = (sTracking_objectUnit *)inst->hTrack[tElemFree->data];
						my_printf("same root id origin id [%d %d] doppler %.2f", printTracker->tid, pTracker->tid, \
							measArray[MSIZE_SPH - 1]);
#endif // DEBUG_LOG_ZQ
						if (unitStart_ret == 0)
						{
							sameRootTracker = (sTracking_objectUnit *)inst->hTrack[tElemFree->data];
							tracking_listEnqueue(&inst->twsTrackList, tElemFree);
							pTracker->assoProperty.sameRootTracker.sameRootTrackerId[0] = sameRootTracker->uid;
							sameRootTracker->assoProperty.sameRootTracker.sameRootTrackerId[0] = pTracker->uid;
							sameRootTracker->assoProperty.assoUnambiVel = measArray[MSIZE_SPH - 1];
							sameRootTracker->objManagement.state = TRACK_STATE_DETECTION;
						}
						else
						{
							tracking_listEnqueue(&inst->freeList, tElemFree);
						}
					}
				}
			}
		}
		if (matchNum >= 2)
		{
			break;
		}
	}
#ifdef DEBUG_LOG_ZQ
	my_printf("init meas [%d %d]match %d", pTracker->tid, measurement->detId, matchedFlag);
#endif // DEBUG_LOG_ZQ

	return matchedFlag;
}
/**
 * @b Description
 * @n
 *      velocity unambigious for tws workmode.
 * 
 * @param[in] inst
 *		This is tracking instance module
 * @param[in] measInfo
 *      measurement array 
 *       
 */
void tracking_twsModeInitAssociate(sTracking_moduleInstance *inst, sMeasOutput *measInfo)
{
	sTracking_objectUnit *pTracker;
	sTrackingParams *tracking_params = inst->tracking_params;
	sTracking_ListElem *tElem = NULL;
	sTracking_ListElem *tElemToRemove = NULL;
	sTracking_ListElem *tElemTemp = NULL;
	uint16_t uid;
	int i = 0;
	sTracking_objectUnit *initTracker = NULL;
	tElem = tracking_listGetFirst(&inst->initTrackList);
#ifdef DEBUG_LOG_ZQ
	if(tElem != 0)
		my_printf("1st init target %d", tElem->data);
#endif // DEBUG_LOG_ZQ

	while (tElem != 0)
	{
		uid = tElem->data;
		initTracker = inst->hTrack[uid];
		if (inst->wholeSpaceScanCycleCnt - initTracker->assoProperty.lastSeenWholeScanId == 0)
			break;
		tracking_unitInitStateAssociate(inst, initTracker, measInfo);
		if (initTracker->objManagement.state != TRACK_STATE_INIT)
		{
			tElemToRemove = tElem;
			tElemTemp = tracking_listGetNext(tElem);
			tracking_listRemoveElement(&inst->initTrackList, tElemToRemove);
			if (initTracker->objManagement.state == TRACK_STATE_FREE)
			{
				inst->targetNumCurrent--;
				tracking_listEnqueue(&inst->freeList, tElemToRemove);
			}
			else if (initTracker->objManagement.state == TRACK_STATE_DETECTION)
			{
				tracking_listEnqueue(&inst->twsTrackList, tElemToRemove);
			}
			else
			{
				// do nothing
			}
			tElem = tElemTemp;
		}
		else
		{
			tElem = tracking_listGetNext(tElem);
		}
	}
}

/**
 * @b Description
 * @n
 *      Velocity unambigious function for tas workmode, This function is called by tracking_moduleInitAssociate.
 * 
 * @param[in] inst
 *		This is tracking instance module
 * @param[in] measInfo
 *      measurement 
 *       
 */
void tracking_tasModeInitAssociate(sTracking_moduleInstance *inst, sMeasOutput *measInfo)
{
	sTracking_objectUnit *pTracker;
	sTrackingParams *tracking_params = inst->tracking_params;
	sTracking_ListElem *tElem = NULL;
	sTracking_ListElem *tElemToRemove = NULL;
	sTracking_ListElem *tElemTemp = NULL;
	uint16_t uid;
	int i = 0;
	sTracking_objectUnit *initTracker = NULL;
	if (g_scanType == TWS_SCAN)
		return;
	//1. velocity unambigious of init tracker in tas scan
	tElem = tracking_listGetFirst(&inst->initTrackList);
	while (tElem != 0)
	{
		uid = tElem->data;
		if (uid == inst->tasTargetId)
		{
			initTracker = inst->hTrack[uid];
			tracking_unitInitStateAssociate(inst, initTracker, measInfo);
			tElemToRemove = tElem;
			tracking_listRemoveElement(&inst->initTrackList, tElemToRemove);
			if (initTracker->objManagement.state == TRACK_STATE_FREE)
			{
				inst->targetNumCurrent--;
				tracking_listEnqueue(&inst->freeList, tElemToRemove);
			}
			else if (initTracker->objManagement.state == TRACK_STATE_DETECTION)
			{
				tracking_listEnqueue(&inst->twsTrackList, tElemToRemove);
			}
			break;
		}
		tElem = tracking_listGetNext(tElem);
	}
	//2. free the unscaned init trackers for finite beam resources
	if (inst->endOfInitTrackScan == 1)
	{
		tElem = tracking_listGetFirst(&inst->initTrackList);
		while (tElem != 0)
		{
			tElemTemp = tracking_listGetNext(tElem);
			tracking_listRemoveElement(&inst->initTrackList, tElem);
			inst->targetNumCurrent--;
			tracking_listEnqueue(&inst->freeList, tElem);
			tElem = tElemTemp;
		}
	}
	// TODO: whether abort or create new tracker using unassociated measurements for TAS_SCAN
	for (i = 0; i < measInfo->num; i++)
	{
		if (inst->bestIndex[i] != inst->tasTargetId)
		{
			inst->bestIndex[i] = TRACKING_ID_POINT_BEHIND_THE_WALL;
		}
	}

}

void tracking_moduleAllocate(sTracking_moduleInstance *inst, sMeasOutput* measInfo)
{
	sMeasurement_SphPoint* measurement;
	sTracking_allocationParams allocThres;
	sTrackingParams *tracking_params = inst->tracking_params;
	int n;
	int i;
	float minMag_thre_array[MAX_NUM_DETECTS] = { 0.f };
	float minSNR_thre_array[MAX_NUM_DETECTS] = { 0.f };
	float maxVelThre_array[MAX_NUM_DETECTS] = { 0.f };
	int allcId = -1;
	int allcSameRootId = -1;
	int allcSameRootId2 = -1;
	float measDoppler = 0.f;
	sTracking_objectUnit *iTracker = NULL;
	sTracking_objectUnit *jTracker = NULL;
	sTracking_objectUnit *kTracker = NULL;
	// 1.get new tracker allocate params
	memcpy(&allocThres, &inst->tracking_params->advParams.allocationParams, sizeof(sTracking_allocationParams));
	for (n = 0; n < measInfo->num; n++)
	{
		measurement = &measInfo->measurement[n];
		if (inst->bestIndex[n] == TRACKING_ID_POINT_NOT_ASSOCIATED) {
			getAllocationParams(measurement->array, &allocThres, \
				&inst->tracking_params->advParams, 1, &inst->platformInfo);
			maxVelThre_array[n] = allocThres.maxVelThre;
			getMagMinParams(measurement->array, &minMag_thre_array[n]);
			getSNRMinParams(measurement->array, &minSNR_thre_array[n]);
		}
	}
	for (i = 0; i < measInfo->num; i++)
	{
		measurement = &measInfo->measurement[i];
		measDoppler = measurement->vector.doppler;
		/*if (measurement->vector.range < 400 && measInfo->pointsIdInfo[i].detectionNum == 1)
		{
#ifdef DEBUG_LOG_ZQ
			my_printf("dot %d is a singal det within 400m", i);
#endif // DEBUG_LOG_ZQ
			continue;
		}*/
		if (inst->bestIndex[i] == TRACKING_ID_POINT_NOT_ASSOCIATED)
		{
			// allocate a new tracker
			allcId = tracking_allocate_unitTracker(inst, measurement);
			if (gVelunambigType == STEP_SCAN && measurement->unambigVelDone == 0 && measurement->vector.range > 400.f)
			{
				float velScope = DOPPLER_SCOPE_WAVETYPE_0;
				if (measurement->waveType == 1)
				{
					velScope = DOPPLER_SCOPE_WAVETYPE_1;
				}
				if (NUM_EXTERN_DOPPLER == 1)
				{
					if (measurement->vector.doppler < 0.f)
					{
						measurement->vector.doppler += velScope;
					}
					else
					{
						measurement->vector.doppler -= velScope;
					}
					allcSameRootId = tracking_allocate_unitTracker(inst, measurement);
					if (allcSameRootId != -1 && allcId != -1)
					{
						iTracker = (sTracking_objectUnit *)inst->hTrack[allcId];
						jTracker = (sTracking_objectUnit *)inst->hTrack[allcSameRootId];
						iTracker->assoProperty.sameRootTracker.sameRootTrackerId[0] = allcSameRootId;
						iTracker->assoProperty.sameRootTracker.sameRootNum = 1;
						jTracker->assoProperty.sameRootTracker.sameRootTrackerId[0] = allcId;
						jTracker->assoProperty.sameRootTracker.sameRootNum = 1;
#ifdef DEBUG_SAMEROOT
						my_printf("meas waveType %d, itker id doppler [%d %.2f], jtker id doppler [%d %.2f] sameRootId [%d %d]",
							measurement->waveType, iTracker->tid, iTracker->kalman_state.H_s.vector.doppler,
							jTracker->tid, jTracker->kalman_state.H_s.vector.doppler,
							allcId, allcSameRootId);
#endif // DEBUG_SAMEROOT

					}
				}
				else if(NUM_EXTERN_DOPPLER == 2)
				{
					measurement->vector.doppler += velScope;
					allcSameRootId = tracking_allocate_unitTracker(inst, measurement);
					measurement->vector.doppler -= 2.f*velScope;
					allcSameRootId2 = tracking_allocate_unitTracker(inst, measurement);
					if (allcSameRootId2 != -1 && allcSameRootId != -1 && allcId != -1)
					{
						iTracker = (sTracking_objectUnit *)inst->hTrack[allcId];
						jTracker = (sTracking_objectUnit *)inst->hTrack[allcSameRootId];
						kTracker = (sTracking_objectUnit *)inst->hTrack[allcSameRootId2];
						iTracker->assoProperty.sameRootTracker.sameRootTrackerId[0] = allcSameRootId;
						iTracker->assoProperty.sameRootTracker.sameRootTrackerId[1] = allcSameRootId2;
						iTracker->assoProperty.sameRootTracker.sameRootNum = 2;
						jTracker->assoProperty.sameRootTracker.sameRootTrackerId[0] = allcId;
						jTracker->assoProperty.sameRootTracker.sameRootTrackerId[1] = allcSameRootId2;
						jTracker->assoProperty.sameRootTracker.sameRootNum = 2;
						kTracker->assoProperty.sameRootTracker.sameRootTrackerId[0] = allcId;
						kTracker->assoProperty.sameRootTracker.sameRootTrackerId[1] = allcSameRootId;
						kTracker->assoProperty.sameRootTracker.sameRootNum = 2;
					}
					else if (allcSameRootId != -1 && allcId != -1)
					{
						iTracker = (sTracking_objectUnit *)inst->hTrack[allcId];
						jTracker = (sTracking_objectUnit *)inst->hTrack[allcSameRootId];
						iTracker->assoProperty.sameRootTracker.sameRootTrackerId[0] = allcSameRootId;
						iTracker->assoProperty.sameRootTracker.sameRootNum = 1;
						jTracker->assoProperty.sameRootTracker.sameRootTrackerId[0] = allcId;
						jTracker->assoProperty.sameRootTracker.sameRootNum = 1;
					}
				}
				// recover measurement doppler
				measurement->vector.doppler = measDoppler;
			}
		}
	}
}
int tracking_allocate_unitTracker(sTracking_moduleInstance *inst, sMeasurement_SphPoint* measurement)
{
	sTracking_ListElem *tElemFree = NULL;
	sTracking_ListElem *tElemActive = NULL;
	sTracking_ListElem *tElemMinScore = NULL;
	int unitStart_ret = -1;
	sTracking_objectUnit *pTracker;
	sTrackingParams *tracking_params = inst->tracking_params;
	int n;
	int i;
	uint16_t uid;
	int ret = -1;
	float maxRange = 0.1f;
	// 1. check whether trackList is full or not,if so, delete the worest one
	tElemFree = tracking_listGetFirst(&inst->freeList);
	if (tElemFree == 0)
	{
		// TODO: 
		maxRange = 0.1f;
		tElemActive = tracking_listGetFirst(&inst->twsTrackList);
		while (tElemActive != 0)
		{
			uid = tElemActive->data;
			pTracker = (sTracking_objectUnit *)inst->hTrack[uid];
			if (pTracker->kalman_state.H_s.vector.range > maxRange && pTracker->objManagement.state != TRACK_STATE_ACTIVE &&
				pTracker->assoProperty.cumDetNum < 2) {
				maxRange = pTracker->kalman_state.H_s.vector.range;
				tElemMinScore = tElemActive;
			}
			tElemActive = tracking_listGetNext(tElemActive);
		}

		if (measurement->vector.range < maxRange)
		{
#ifdef DEBUG_LOG_ZQ
			pTracker = (sTracking_objectUnit *)inst->hTrack[tElemMinScore->data];
			my_printf("Delete, id[%d]\n", pTracker->tid);
#endif
			tracking_listRemoveElement(&inst->twsTrackList, tElemMinScore);
			inst->targetNumCurrent--;
			tracking_listEnqueue(&inst->freeList, tElemMinScore);
			tElemFree = tracking_listGetFirst(&inst->freeList);
		}
	}
	// 2.allocate new tracker
	tElemFree = tracking_listDequeue(&inst->freeList);
	if (tElemFree != 0) {
		inst->targetNumTotal++;
		unitStart_ret = tracking_unitStart(inst->hTrack[tElemFree->data], &inst->platformInfo, \
			measurement, inst->bestIndex, inst->wholeSpaceScanCycleCnt, measurement->waveType, inst->targetNumTotal);
		if (unitStart_ret == 0)
		{
			pTracker = (sTracking_objectUnit *)inst->hTrack[tElemFree->data];
			ret = tElemFree->data;
#ifdef DEBUG_LOG_ZQ
			my_printf("new tracker id %d, x %.2f,y %.2f,z %.2f, vx %.2f, vy %.2f,vz %.2f,", pTracker->tid, \
				pTracker->kalman_state.S_hat[0], pTracker->kalman_state.S_hat[1], pTracker->kalman_state.S_hat[2], \
				pTracker->kalman_state.S_hat[3], pTracker->kalman_state.S_hat[4], pTracker->kalman_state.S_hat[5]);
#endif // DEBUG_LOG

					// if tws workmode was setted , new trackers were created in twsTrackList, else created in tasTrackList
			if (tracking_params->workMode == 0)
			{
				if (gVelunambigType == WHOLE_SCAN)
				{
					tracking_listEnqueue(&inst->initTrackList, tElemFree);
				}
				else
				{
					tracking_listEnqueue(&inst->twsTrackList, tElemFree);
				}				
			}
			else
			{
				if (gVelunambigType == WHOLE_SCAN)
				{
					tracking_listEnqueue(&inst->initTrackList, tElemFree);
				}
				else
				{
					tracking_listEnqueue(&inst->twsTrackList, tElemFree);
				}
			}
#ifdef DEBUG_LOG_CFMSCAN
			fprintf(fp1, "NEW_TRAJ	uid:%u	assoMId:%d	waveInfo:%u	confirmBeam:%u	velUnambigDone:%u	forcastFrameNum:%u\n", \
				pTracker->uid, pTracker->assoProperty.assoCondenceMeasId, pTracker->assoProperty.waveInfo, pTracker->assoProperty.confirmBeam, pTracker->objKinematic.velUnambigProp.velUnambigDone, pTracker->objManagement.forcastFrameNum);
#endif
		}
		else
		{
			tracking_listEnqueue(&inst->freeList, tElemFree);
		}

	}
	return ret;
#ifdef DEBUG_LOG_CFMSCAN
	fclose(fp1);
#endif
}

void tracking_confirmSelect(sTracking_moduleInstance* inst, sAlgObjData* algObj)
{
	if (g_scanType == TWS_SCAN)
	{
		tracking_confirmList(inst, algObj);
	}
}

void tracking_confirmList(sTracking_moduleInstance* inst, sAlgObjData* algObj)
{
	sTracking_ListElem* tElem = NULL;
	uint16_t uid = 0, uid_TAS = 0;
	int16_t uid_sameRoot = -1;
	sTracking_objectUnit* twsTracker = NULL;
	sTrackingParams* tracking_params = inst->tracking_params;
	uint16_t cumDetNumArray[MAX_NUM_TRAJS] = { 0 };
	uint16_t trajIdArray[MAX_NUM_TRAJS] = { 0 };
	sTracking_objectUnit* traj = NULL, *iTracker = NULL;
	uint16_t ii = 0, jj = 0, nn = 0, kk = 0, i = 0;
	uint16_t sortBoarder = 0;
	uint16_t sortHappen = 0;
	uint16_t lastExchangeIdx = 0;
	uint16_t temp_cumDet = 0;
	uint16_t confirmTargetNum = 0;
	sTracking_ListElem* tElem_TAS = NULL;
	sTracking_objectUnit* traj_TAS = NULL;
	uint16_t CFMSKIP_thres = 4;

#ifdef DEBUG_LOG_CFMSCAN
	FILE* fp1 = NULL;
	fopen_s(&fp1, "E:/record/20230628/record2.txt", "a+");
#endif
#ifdef PINGSHIZADIAN
	FILE* fp1;
	sTracking_objectUnit* traj = NULL;
	fopen_s(&fp1, "E:/record/20221122/genzong.txt", "a+");
#endif // PINGSHIZADIAN

	for (kk = 0; kk < 3; kk++)
	{
		inst->confirmBeamId[kk] = MAX_NUM_TRAJS;
	}

	tElem = tracking_listGetFirst(&inst->twsTrackList);
	while (tElem != 0)
	{
		uid = tElem->data;
		traj = (sTracking_objectUnit*)inst->hTrack[uid];
		if (traj->objManagement.state == TRACK_STATE_DETECTION && traj->assoProperty.cfm_times < CFMSKIP_thres && traj->assoProperty.confirmBeam == CFM_NEAR_TAS)
		{
			traj->assoProperty.confirmBeam = CFM_INITIAL_STATE;
			traj->assoProperty.cfm_times = 0;
		}
		tElem = tracking_listGetNext(tElem);
	}

	tElem_TAS = tracking_listGetFirst(&inst->tasTrackList);
	while (tElem_TAS != 0)
	{
		uid_TAS = tElem_TAS->data;
		traj_TAS = (sTracking_objectUnit*)inst->hTrack[uid_TAS];

		tElem = tracking_listGetFirst(&inst->twsTrackList);
		while (tElem != 0)
		{
			uid = tElem->data;
			traj = (sTracking_objectUnit*)inst->hTrack[uid];
			if (traj->assoProperty.confirmBeam != CFM_SKIP && traj->assoProperty.confirmBeam != CFM_NEAR_TAS && traj->objManagement.state == TRACK_STATE_DETECTION)
			{
				if (traj_TAS->kalman_state.H_s.array[0] < 300.f \
					&& fabsf(traj_TAS->kalman_state.H_s.array[0] - traj->kalman_state.H_s.array[0]) < 10.f \
					&& fabsf(traj_TAS->kalman_state.H_s.array[1] - traj->kalman_state.H_s.array[1]) * RAD2DEG<20.f \
					&& fabsf(traj_TAS->kalman_state.H_s.array[3] - traj->kalman_state.H_s.array[3]) < 2.f)
				{
					traj->assoProperty.confirmBeam = CFM_NEAR_TAS;
				}
				else if (traj_TAS->kalman_state.H_s.array[0] >= 300.f \
					&& fabsf(traj_TAS->kalman_state.H_s.array[0] - traj->kalman_state.H_s.array[0]) < 10.f \
					&& fabsf(traj_TAS->kalman_state.H_s.array[1] - traj->kalman_state.H_s.array[1]) * RAD2DEG < 11.f \
					&& fabsf(traj_TAS->kalman_state.H_s.array[3] - traj->kalman_state.H_s.array[3]) < 2.f)
				{
					traj->assoProperty.confirmBeam = CFM_NEAR_TAS;
				}

			}
			tElem = tracking_listGetNext(tElem);
		}
		tElem_TAS = tracking_listGetNext(tElem_TAS);
	}

	tElem = tracking_listGetFirst(&inst->twsTrackList);
	while (tElem != 0)
	{
		uid = tElem->data;
		traj = (sTracking_objectUnit*)inst->hTrack[uid];

		if (traj->assoProperty.cfm_times >= CFMSKIP_thres && traj->objManagement.state == TRACK_STATE_DETECTION)
		{
			traj->assoProperty.confirmBeam = CFM_SKIP;
		}
		if (traj->assoProperty.cfm_times >= 1 && traj->assoProperty.cumDetNum == 1)
		{
			traj->assoProperty.confirmBeam = CFM_SKIP;
		}
		if (gVelunambigType == STEP_SCAN && traj->assoProperty.confirmBeam != CFM_SKIP && traj->assoProperty.confirmBeam != CFM_NEAR_TAS  && traj->assoProperty.sameRootTracker.sameRootNum != 0)
		{
			for (i = 0; i < NUM_EXTERN_DOPPLER; i++)
			{
				uid_sameRoot = traj->assoProperty.sameRootTracker.sameRootTrackerId[i];
				if (uid_sameRoot != -1)
				{
					iTracker = (sTracking_objectUnit*)inst->hTrack[uid_sameRoot];
					iTracker->assoProperty.confirmBeam = CFM_SKIP;
				}
			}
			//my_printf("sameRootNum: %d root: %d sameRootId: %d %d\n", traj->assoProperty.sameRootTracker.sameRootNum, uid, traj->assoProperty.sameRootTracker.sameRootTrackerId[0], traj->assoProperty.sameRootTracker.sameRootTrackerId[1]);
		}

		if (traj->assoProperty.confirmBeam != CFM_SKIP && traj->assoProperty.confirmBeam != CFM_NEAR_TAS && traj->objManagement.state == TRACK_STATE_DETECTION && traj->kalman_state.H_s.vector.range <= 500)
		{
			cumDetNumArray[nn] = traj->assoProperty.cumDetNum;
			trajIdArray[nn] = traj->uid;
			nn++;
		}
#ifdef PINGSHIZADIAN
		traj = (sTracking_objectUnit*)inst->hTrack[uid];
		fprintf(fp1, "%u	%u	%.3f	%.3f	%.3f\n", detectsListOutput.frameID, traj->uid, traj->kalman_state.S_hat[0], traj->kalman_state.S_hat[1], traj->kalman_state.S_hat[2]);
#endif // PINGSHIZADIAN
		tElem = tracking_listGetNext(tElem);
	}

	if (nn == 0)
	{
#ifdef DEBUG_LOG_CFMSCAN
		fprintf(fp1,"no new traj-----------------\n");
		fclose(fp1);
#endif
		return;
	}

	sortBoarder = nn - 1;
	for (ii = 0; ii < nn - 1; ii++)
	{
		sortHappen = 0;
		for (jj = 0; jj < sortBoarder; jj++)
		{
			if (cumDetNumArray[jj] < cumDetNumArray[jj + 1])
			{
				// swap cumDet
				temp_cumDet = cumDetNumArray[jj];
				cumDetNumArray[jj] = cumDetNumArray[jj + 1];
				cumDetNumArray[jj + 1] = temp_cumDet;

				// swap id
				trajIdArray[jj] = trajIdArray[jj] + trajIdArray[jj + 1];
				trajIdArray[jj + 1] = trajIdArray[jj] - trajIdArray[jj + 1];
				trajIdArray[jj] = trajIdArray[jj] - trajIdArray[jj + 1];

				sortHappen = 1;
				lastExchangeIdx = jj;
			}
		}
		sortBoarder = jj;
		if (sortHappen == 0)
			break;
	}

	confirmTargetNum = nn > 3 ? 3: nn;

	for (kk = 0; kk < confirmTargetNum; kk++)
	{
		inst->confirmBeamId[kk] = trajIdArray[kk];
	}
	// my_printf("confirmTargetNum: %d %d %d", inst->confirmBeamId[0], inst->confirmBeamId[1], inst->confirmBeamId[2]);

#ifdef DEBUG_LOG_CFMSCAN
	fprintf(fp1, "traj_Id:	");
	for (ii = 0; ii < nn; ii++)
	{
		fprintf(fp1, "%u	", trajIdArray[ii]);
	}
	fprintf(fp1, "\n");

	fprintf(fp1, "cum_det:	");
	for (ii = 0; ii < nn; ii++)
	{
		fprintf(fp1, "%u	", cumDetNumArray[ii]);
	}
	fprintf(fp1, "\n");


	fprintf(fp1, "cfm traj Id:	");
	for (ii = 0; ii < confirmTargetNum; ii++)
	{
		fprintf(fp1, "%u	", inst->confirmBeamId[ii]);
	}
	fprintf(fp1, "\n");


	fclose(fp1);
#endif
}



//uint8_t tracking_unit_cluster(sTracking_moduleInstance* inst, \
//	sMeasurement_SphPoint* point, \
//	uint16_t num, uint16_t n, sTracking_measurementSphUnion* mCenter, \
//	sTracking_measurementSphUnion* mCurrent, \
//	uint8_t* vIsClustered, uint16_t* vIndex, int* clu_num, uint8_t n_ex, \
//	float* minMag_thre_array, float* minSNR_thre_array, float* maxVelThre_array)
//{
//	int k = 0;
//	int ex_k = 0;
//	int m = 0;
//	uint8_t isWithinLimits = true;
//	sTracking_measurementSphUnion H_limits;
//	sTracking_measurementSphUnion u_tilda;
//	uint8_t getNeighbor = false;
//
//	tracking_calcMeasurementLimits(mCenter->vector.range, &inst->tracking_params->advParams.allocationParams.limits, &H_limits.vector);
//	if (point[n].mag < minMag_thre_array[n]) {
//		return 0;
//	}
//	if (point[n].snr < minSNR_thre_array[n]) {
//		return 0;
//	}
//	for(k=n+1; k<num; k++) {
//		isWithinLimits = true;
//		if(inst->bestIndex[k] == TRACKING_ID_POINT_NOT_ASSOCIATED) {
//			if (vIsClustered[k*NUM_DOPPLERS] != 0 && vIsClustered[n*NUM_DOPPLERS+n_ex] != 0) {
//				continue;
//			}
//			if (point[k].mag < minMag_thre_array[k]) {
//				continue;
//			}
//			if (point[k].snr < minSNR_thre_array[k]) {
//				continue;
//			}
//			memcpy(&(mCurrent->vector), &(point[k].vector), sizeof(mCurrent->vector));
//			if(fabsf(mCurrent->vector.doppler - mCenter->vector.doppler) < maxVelThre_array[n]) {
//				tracking_vectorSub(MEASUREMENT_VECTOR_SIZE, mCurrent->array, mCenter->array, u_tilda.array); 
//				for (m = 0; m < MSIZE_SPH-1; m++)
//				{
//					if (fabsf(u_tilda.array[m]) > H_limits.array[m])
//					{
//						isWithinLimits = false;
//						break;
//					}
//				}
//				if(point[n].flag_moving != point[k].flag_moving)
//				{
//					if (fabsf(u_tilda.array[MSIZE_SPH-1]) > TRACKING_MIN_STATIC_VELOCITY_RAD)
//					{
//						isWithinLimits = false;
//					}
//				}
//				else
//				{
//					if (fabsf(u_tilda.array[MSIZE_SPH-1]) > H_limits.array[MSIZE_SPH-1])
//					{
//						isWithinLimits = false;
//					}
//				}
//				
//				if(isWithinLimits) {
//					if (vIsClustered[k*NUM_DOPPLERS] == 0 && vIsClustered[n*NUM_DOPPLERS+n_ex]==0)
//					{
//						vIndex[k*NUM_DOPPLERS] = (uint16_t)(*clu_num);
//						vIsClustered[k*NUM_DOPPLERS] = 1;
//						vIndex[n*NUM_DOPPLERS+n_ex] = (uint16_t)(*clu_num);
//						vIsClustered[n*NUM_DOPPLERS+n_ex] = 1;
//						(*clu_num)++;
//					}
//					else if(vIsClustered[k*NUM_DOPPLERS]==0 && vIsClustered[n*NUM_DOPPLERS*n_ex]!=0)
//					{
//						vIndex[k*NUM_DOPPLERS] = vIndex[n*NUM_DOPPLERS+n_ex];
//						vIsClustered[k*NUM_DOPPLERS] = 1;
//					}
//					else if (vIsClustered[k*NUM_DOPPLERS] != 0 && vIsClustered[n*NUM_DOPPLERS+n_ex] == 0) {
//						vIndex[n*NUM_DOPPLERS+n_ex] = vIndex[k*NUM_DOPPLERS];
//						vIsClustered[n*NUM_DOPPLERS+n_ex] = 1;
//					}
//					getNeighbor = true;
//				}
//			}
//
//		}
//	}
//	return getNeighbor;
//}
/**
 * @b Description
 * @n
 *      This function is used to judge whether the current trakcer is sameroot with 
 * the confirmed trakcer in tas confirm beam mode.
 * 
 *       
 * @return 
 *       
 */
int isCfmBeamSameRootTracker()
{
	int ret = -1;


	return ret;
}

