
#include <math.h>
#include <string.h>

#include "../include/tracking_common.h"
#include "../include/tracking_int.h"

extern sDetectFrame detectsListOutput;

void tracking_tasAssociate(sTracking_moduleInstance *inst, sMeasOutput *measInfo)
{
	sTracking_ListElem *tElem = NULL;
	sTracking_ListElem *initElem = NULL;
	uint16_t uid;
	sTrackingParams *tracking_params = inst->tracking_params;
	int i = 0;
	float mdGating = tracking_params->advParams.tasMdGatingParams.gating;
	sTracking_objectUnit* traj = NULL;
	int sameRootuid = 0;
	sTracking_objectUnit *cfmSameRootTracker = NULL;
	int iSameRoot = 0;
	uint16_t cumDetNumGating = 4;
#ifdef DEBUG_LOG_CFMSCAN
	FILE* fp1 = NULL;
	int ii = 0;
	fopen_s(&fp1, "E:/record/20230628/record2.txt", "a+");
	for (ii = 0; ii < measInfo->num; ii++)
	{
		fprintf(fp1, "condence id:%u	unambigVelDone:%u	wavetype:%u	unambigVel:%.3f\n", measInfo->measurement[ii].detId, measInfo->measurement[ii].unambigVelDone, measInfo->measurement[ii].waveType, measInfo->measurement[ii].unambigVel);
	}
	if (measInfo->num == 0)
	{
		fprintf(fp1, "No Condence\n");
	}
#endif

	if (inst->tasTargetId != -1)
	{
		traj = (sTracking_objectUnit*)(inst->hTrack[inst->tasTargetId]);
	}
	if (traj && traj->objManagement.state == TRACK_STATE_DETECTION && gCfmBeamFlag == 1 && gVelunambigType == STEP_SCAN) //ÅÐ¶ÏÊÇ·ñÎªÈ·ÈÏ²¨Êø
	{
		tElem = tracking_listGetFirst(&inst->twsTrackList);
		while (tElem != 0)
		{
			uid = tElem->data;
			traj = (sTracking_objectUnit*)(inst->hTrack[uid]);
			traj->assoProperty.assoCondenceMeasId = -1;
			if (uid == inst->tasTargetId)
			{
#ifdef DEBUG_LOG_CFMSCAN
				fprintf(fp1, "TASf-CFMt before asso	uid:%u	assoMId:%d	waveInfo:%u	confirmBeam:%u	velUnambigDone:%u	forcastFrameNum:%u	state:%u\n", \
					traj->uid, traj->assoProperty.assoCondenceMeasId, traj->assoProperty.waveInfo, traj->assoProperty.confirmBeam,traj->objManagement.forcastFrameNum, traj->objKinematic.velUnambigProp.velUnambigDone,traj->objManagement.forcastFrameNum, traj->objManagement.state);
#endif
				traj->assoProperty.cfm_times += 1;
				tracking_unitAssociate(inst->hTrack[uid], &inst->platformInfo, measInfo, \
					mdGating, &inst->bestIndex[0], inst->wholeSpaceScanCycleCnt, inst->wholeSpaceScanCycleFinalBeamFlag);
				if (traj->assoProperty.assoCondenceMeasId != -1)
				{
					if (traj->assoProperty.confirmBeam == CFM_INITIAL_STATE)
					{
						if (traj->assoProperty.cumDetNum < cumDetNumGating)
						{
							traj->assoProperty.confirmBeam = CFM_ASSO_ONCE;
						}
						else
						{
							traj->assoProperty.confirmBeam = CFM_ASSO_TWICE;
						}
					}
					else if (traj->assoProperty.confirmBeam == CFM_ASSO_ONCE)
					{
						traj->assoProperty.confirmBeam = CFM_ASSO_TWICE;
					}
#ifdef DEBUG_LOG_CFMSCAN
					fprintf(fp1, "TASf-CFMt after asso	uid:%u	assoMId:%d	waveInfo:%u	confirmBeam:%u	velUnambigDone:%u	forcastFrameNum:%u	state:%u\n", \
						traj->uid, traj->assoProperty.assoCondenceMeasId, traj->assoProperty.waveInfo, traj->assoProperty.confirmBeam, traj->objManagement.forcastFrameNum, traj->objKinematic.velUnambigProp.velUnambigDone,traj->objManagement.forcastFrameNum, traj->objManagement.state);
#endif
				}
				if (traj->assoProperty.sameRootTracker.sameRootNum > 0)
				{
					for (iSameRoot = 0; iSameRoot < NUM_EXTERN_DOPPLER; iSameRoot++)
					{
						sameRootuid = traj->assoProperty.sameRootTracker.sameRootTrackerId[iSameRoot];
						if (sameRootuid != -1)
						{
							cfmSameRootTracker = (sTracking_objectUnit *)inst->hTrack[sameRootuid];
							cfmSameRootTracker->assoProperty.cfm_times += 1;
							tracking_unitAssociate(inst->hTrack[sameRootuid], &inst->platformInfo, measInfo, \
								mdGating, &inst->bestIndex[0], inst->wholeSpaceScanCycleCnt, inst->wholeSpaceScanCycleFinalBeamFlag);
							if (cfmSameRootTracker->assoProperty.assoCondenceMeasId != -1)
							{
								if (cfmSameRootTracker->assoProperty.confirmBeam == CFM_INITIAL_STATE)
								{
									if (cfmSameRootTracker->assoProperty.cumDetNum < cumDetNumGating)
									{
										cfmSameRootTracker->assoProperty.confirmBeam = CFM_ASSO_ONCE;
									}
									else
									{
										cfmSameRootTracker->assoProperty.confirmBeam = CFM_ASSO_TWICE;
									}
								}
								else if (cfmSameRootTracker->assoProperty.confirmBeam == CFM_ASSO_ONCE)
								{
									cfmSameRootTracker->assoProperty.confirmBeam = CFM_ASSO_TWICE;
								}
							}
						}

					}
				}
				//break;
#ifdef DEBUG_LOG_CFMSCAN
				fprintf(fp1, "TASf-CFMt after asso	uid:%u	assoMId:%d	waveInfo:%u	confirmBeam:%u	velUnambi%u	forcastFrameNum:%u	state:%u\n", \
					traj->uid, traj->assoProperty.assoCondenceMeasId, traj->assoProperty.waveInfo, traj->assoProperty.confirmBeam, traj->objManagement.forcastFrameNum, traj->objKinematic.velUnambigProp.velUnambigDone, traj->objManagement.forcastFrameNum, traj->objManagement.state);
#endif
			}
			tElem = tracking_listGetNext(tElem);
		}
	}
	else
	{
		tElem = tracking_listGetFirst(&inst->tasTrackList);
		while (tElem != 0)
		{
			uid = tElem->data;
			traj = (sTracking_objectUnit*)(inst->hTrack[uid]);
			traj->assoProperty.assoCondenceMeasId = -1;
			if (uid == inst->tasTargetId)
			{
				tracking_unitAssociate(inst->hTrack[uid], &inst->platformInfo, measInfo, \
					mdGating, &inst->bestIndex[0], inst->wholeSpaceScanCycleCnt, inst->wholeSpaceScanCycleFinalBeamFlag);
				//break;
			}
			tElem = tracking_listGetNext(tElem);
		}
		tElem = tracking_listGetFirst(&inst->twsTrackList);
		while (tElem != 0)
		{
			uid = tElem->data;
			traj = (sTracking_objectUnit*)(inst->hTrack[uid]);
			traj->assoProperty.assoCondenceMeasId = -1;
			tElem = tracking_listGetNext(tElem);
		}
	}

	if (gVelunambigType == WHOLE_SCAN)
	{
		initElem = tracking_listGetFirst(&inst->initTrackList);
		while (initElem != 0)
		{
			uid = initElem->data;
			if (uid == inst->tasTargetId)
			{
				break;
			}
			initElem = tracking_listGetNext(initElem);
		}
	}
	// TODO: whether abort or create new tracker using unassociated measurements for TAS_SCAN
	for (i = 0; i < measInfo->num; i++)
	{
		if (gVelunambigType == WHOLE_SCAN)
		{
			if (initElem != NULL && initElem->data == inst->tasTargetId)
				break;
		}
		if (inst->bestIndex[i] != inst->tasTargetId)
		{
			inst->bestIndex[i] = TRACKING_ID_POINT_BEHIND_THE_WALL;
		}
	}
}
void tracking_twsAssociate(sTracking_moduleInstance *inst, sMeasOutput *measInfo)
{
	sTracking_ListElem *tElem = NULL;
	uint16_t uid;
	sTracking_objectUnit *twsTracker = NULL; 
	sTracking_objectUnit *tasTracker = NULL;
	sTrackingParams *tracking_params = inst->tracking_params;
	sMeasurement_SphPoint *measurement;
	sMeasurement_SphPoint u_tilda;
	float md = 0.f;
	float mdGating = tracking_params->advParams.twsMdGatingParams.gating;
	int i = 0;
	float dt = 0.f;
	float measArray[MSIZE_SPH] = { 0.f };

	//1.using NN to associate tws targets and measurements
	tElem = tracking_listGetFirst(&inst->twsTrackList);
	while (tElem != 0)
	{
		uid = tElem->data;

		tracking_unitAssociate(inst->hTrack[uid], &inst->platformInfo, measInfo, \
			mdGating, &inst->bestIndex[0],inst->wholeSpaceScanCycleCnt,inst->wholeSpaceScanCycleFinalBeamFlag);
		tElem = tracking_listGetNext(tElem);
	}
	// 2. we allow the tas tracker to update even though tws scan, so an assocation step for each tas tracker comes 
	tElem = tracking_listGetFirst(&inst->tasTrackList);
	while (tElem != 0)
	{
		uid = tElem->data;
		tracking_unitAssociate(inst->hTrack[uid], &inst->platformInfo, measInfo, \
			mdGating, &inst->bestIndex[0], inst->wholeSpaceScanCycleCnt, inst->wholeSpaceScanCycleFinalBeamFlag);
		tElem = tracking_listGetNext(tElem);
	}
	
	//2.confirm the unassociated measurements whether origined from tas targets or not, if so, abort the measurement
//	for (i = 0; i < measInfo->num; i++)
//	{
//		measurement = &measInfo->measurement[i];
//		memcpy(measArray, measurement->array, sizeof(measurement->array));
//		if (inst->bestIndex[i] == TRACKING_ID_POINT_NOT_ASSOCIATED)
//		{
//			tElem = tracking_listGetFirst(&inst->tasTrackList);
//			while (tElem != 0)
//			{
//				uid = tElem->data;
//				tasTracker = (sTracking_objectUnit *)inst->hTrack[uid];
//				// predict tas targets  
//				dt = g_curTimestamp - tasTracker->timestamp;
//				tracking_cal_FQ(inst, dt);
//				tracking_unitPredict(inst->hTrack[uid], &inst->platformInfo);
//				tracking_measurementCovCal(tasTracker);
//#ifdef VELOCITY_UNAMBIGIOUS
//				measurement->unambigVel = tracking_velocityUnambigious(measInfo->waveType, tasTracker->kalman_state.H_s.vector.doppler,
//					measurement->vector.doppler);
//				measArray[MSIZE_SPH - 1] = measurement->unambigVel;
//#endif // VELOCITY_UNAMBIGIOUS
//				tracking_vectorSub(MEASUREMENT_VECTOR_SIZE, measArray, tasTracker->kalman_state.H_s.array, u_tilda.array);
//				tracking_computeMahalanobis(u_tilda.array, tasTracker->assoProperty.gC_inv, &md);
//				if (md < mdGating)
//				{
//					inst->bestIndex[i] = TRACKING_ID_POINT_BEHIND_THE_WALL;
//				}
//				tElem = tracking_listGetNext(tElem);
//			}
//		}
//	}
#ifdef DEBUG_LOG_CFMSCAN
	fclose(fp1);
#endif
}


void tracking_unitAssociate(void* handle, sTracking_platformInfo* platformInfo, sMeasOutput *measInfo,\
	float gating,uint16_t* bestIndex,uint32_t wholeSpaceScanCycleCnt, uint32_t wholeSpaceScanCycleFinalBeamFlag)
{
	sTracking_objectUnit* inst = (sTracking_objectUnit*)handle;
	uint8_t num = measInfo->num;
	sMeasurement_SphPoint u_tilda;
	sMeasurement_SphPoint* measurement;
	sTracking_measurementSphUnion meaSph;
	float measArray[MSIZE_SPH] = { 0.f };
	float md; // mah distance
	int i;
	float minMd = 1000.f;
	int8_t assocId = -1;
	int m = 0, n = 0;
	uint8_t acclerTargetFlag = 0;
	uint8_t unambigCondition = 0;
	float velScope = 0.f;
	float mdGating = gating;
	float range = sqrtf(inst->kalman_state.S_hat[0] * inst->kalman_state.S_hat[0] + \
		inst->kalman_state.S_hat[1] * inst->kalman_state.S_hat[1] + \
		inst->kalman_state.S_hat[2] * inst->kalman_state.S_hat[2]);
	float originDoppler = inst->kalman_state.H_s.vector.doppler;
	float unambiTrackDoppler = inst->kalman_state.H_s.vector.doppler;
#ifdef DEBUG_COHE_TRACK
	FILE* fp1 = NULL;
	//fopen_s(&fp1, "E:/record/20221202/var.txt", "a+");
	fopen_s(&fp1, "E:/record/20230419/asso.txt", "a+");
#endif // DEBUG_COHE_TRACK
	memset(&u_tilda, 0, sizeof(sMeasurement_SphPoint));
	inst->assoProperty.assoCondenceMeasId = -1;
	tracking_measurementCovCal(inst);
	for (i = 0; i < num; i++)
	{
		measurement = &measInfo->measurement[i];
		measurement->unambigVel = measurement->vector.doppler;
		memcpy(measArray, measurement->array, sizeof(measurement->array));
		tracking_vectorSub(MEASUREMENT_VECTOR_SIZE, measArray, inst->kalman_state.H_s.array, u_tilda.array);
		if (gVelunambigType == WHOLE_SCAN)
		{
			if ((fabsf(inst->kalman_state.H_s.vector.doppler) > 5.0f && fabsf(inst->kalman_state.S_hat[6]) > 2.0f) ||
				fabsf(inst->kalman_state.H_s.vector.doppler) > 8.0f)
			{
				acclerTargetFlag = 1;
			}
			measurement->unambigVel = tracking_velocityUnambigious(measurement->waveType, inst->kalman_state.H_s.vector.doppler,
				measurement->vector.doppler, acclerTargetFlag);
#ifdef DEBUG_LOG_ZQ
			my_printf("vTrk vMeas after unambig %.2f %.2f", inst->kalman_state.H_s.vector.doppler, measurement->unambigVel);
#endif // DEBUG_LOG_ZQ
			measArray[MSIZE_SPH - 1] = measurement->unambigVel;
			u_tilda.array[MSIZE_SPH - 1] = measArray[MSIZE_SPH - 1] - inst->kalman_state.H_s.array[MSIZE_SPH - 1];
		}
		else if (gVelunambigType == STEP_SCAN)
		{
			if (fabsf(u_tilda.vector.range) > 30.f || fabsf(u_tilda.vector.azimuthRad) > 20.f*DEG2RAD ||
				fabsf(u_tilda.vector.pitchRad) > 20.f*DEG2RAD)
			{
				continue;
			}
			unambigCondition = 0;
			if (inst->objKinematic.velUnambigProp.velUnambigDone == 1)
			{
				if (measurement->unambigVelDone == 1)
				{
					// do nothing
					unambigCondition = 1;
				}
				else
				{
					measurement->unambigVel = tracking_velocityUnambigious(measurement->waveType, inst->kalman_state.H_s.vector.doppler,
						measurement->vector.doppler, 0);
					measArray[MSIZE_SPH - 1] = measurement->unambigVel;
#ifdef DEBUG_UNAMBIVEL
					my_printf("asso measWaveType %d trkvel meavel [%.2f %.2f] unambiel %.2f", measurement->waveType, \
						inst->kalman_state.H_s.vector.doppler, measurement->vector.doppler, measurement->unambigVel);
#endif // DEBUG_UNAMBIVEL

					unambigCondition = 2;
				}
			}
			else
			{
				if (measurement->unambigVelDone == 1)
				{
					unambiTrackDoppler = tracking_velocityUnambigious(inst->objKinematic.velUnambigProp.waveType,
						measurement->vector.doppler, unambiTrackDoppler, 0);
					unambigCondition = 3;
				}
				else
				{
					if (measurement->waveType != inst->objKinematic.velUnambigProp.waveType)
					{
						tracking_velocityUnambigiousInit(&unambiTrackDoppler,
							&measurement->unambigVel, measurement->waveType);
						measArray[MSIZE_SPH - 1] = measurement->unambigVel;
					}
					else
					{
						// associate for sameRoot trackers
						if (inst->assoProperty.sameRootTracker.sameRootTrackerId[0] != -1 ||
							inst->assoProperty.sameRootTracker.sameRootTrackerId[NUM_EXTERN_DOPPLER - 1] != -1) 
						{
							if (measurement->waveType == 0)
							{
								velScope = DOPPLER_SCOPE_WAVETYPE_0;
							}
							else
							{
								velScope = DOPPLER_SCOPE_WAVETYPE_1;
							}
							if (NUM_EXTERN_DOPPLER == 1)
							{
								if (fabsf(inst->kalman_state.H_s.vector.doppler - measurement->unambigVel) < 1.f)
								{
									measArray[MSIZE_SPH - 1] = measurement->unambigVel;
								}
								else
								{
									int direction = 1;
									if (measurement->unambigVel > 0)
									{
										direction = -1;
									}
									measurement->unambigVel += direction * velScope;
									if (fabsf(inst->kalman_state.H_s.vector.doppler - measurement->unambigVel) < 1.f)
									{
										measArray[MSIZE_SPH - 1] = measurement->unambigVel;
									}
								}

							}
							else if (NUM_EXTERN_DOPPLER == 2)
							{
								int iExtern = 0;
								float unambigVel = measurement->unambigVel;
								for (iExtern = -1; iExtern < NUM_EXTERN_DOPPLER - 1; iExtern++)
								{
									measurement->unambigVel = unambigVel + iExtern * velScope;
									if (fabsf(inst->kalman_state.H_s.vector.doppler - measurement->unambigVel) < 1.f)
									{
										measArray[MSIZE_SPH - 1] = measurement->unambigVel;
										break;
									}
								}
							}

						}
					}
					unambigCondition = 4;
				}
			}
			if (unambigCondition >= 2)
			{
				u_tilda.array[MSIZE_SPH - 1] = measArray[MSIZE_SPH - 1] - inst->kalman_state.H_s.array[MSIZE_SPH - 1];
			}
			measurement->unambigVel = measArray[MSIZE_SPH - 1];
		}
#ifdef DEBUG_UNAMBIVEL
		my_printf("trk meas %d %d condi %d", inst->tid, i, unambigCondition);
#endif // DEBUG_UNAMBIVEL
		
		if (!tracking_isMeasurementBesideTracker(u_tilda, inst))
			continue;
		//tracking_vectorSub(MEASUREMENT_VECTOR_SIZE, measArray, inst->kalman_state.H_s.array, u_tilda.array);
		//if (!g_byDummyTest)
		//{
		//	if (measArray[0] < 200 && measInfo->pointsIdInfo[i].detectionNum == 1 && measInfo->scanType == TWS_SCAN)	// TODO
		//		continue;
		//}

		tracking_computeMahalanobis(u_tilda.array, inst->assoProperty.gC_inv, &md);
#ifdef DEBUG_LOG_ZQ
		my_printf("trackId measId [%d %d] md %.2f dr da dp dv[%.2f %.2f %.2f %.2f]", inst->tid, i, md, \
			u_tilda.array[0], u_tilda.array[1] * RAD2DEG, u_tilda.array[2] * RAD2DEG, u_tilda.array[3]);
#endif // DEBUG_log

		if (md < minMd)
		{
			minMd = md;
			assocId = i;
			if (gVelunambigType == STEP_SCAN)
			{
				inst->kalman_state.H_s.vector.doppler = unambiTrackDoppler;
			}
		}
	}

	/*if (wholeSpaceScanCycleCnt - lastWholeSpaceScanCycleCnt >= 1)
	{
		inst->curTarget.mdScore = 100.f;
	}*/
	if (fabsf(inst->kalman_state.S_hat[3]) > 10.f || fabsf(inst->kalman_state.S_hat[4]) > 10.f \
		|| fabsf(inst->kalman_state.S_hat[5]) > 10.f)
	{
		mdGating = 12.f;
	}
	if (minMd < mdGating)
		//if (minMd < 20.f)
	{
		if (inst->assoProperty.lastSeenWholeScanId == wholeSpaceScanCycleCnt)//±¾ÂÖscanÖÐ²»ÊÇµÚÒ»´Î±»¹ØÁª£¬mdScoreÈ¡×îÐ¡Öµ
		{
			if (minMd < inst->curTarget.mdScore) {
				inst->curTarget.mdScore = minMd;
			}
		}
		else {
			inst->curTarget.mdScore = minMd; //±¾ÂÖscan³õ´Î¹ØÁª£¬Ö±½Ó¸üÐÂmdScore
		}
#ifdef DEBUG_COHE_TRACK
		/*if (inst->uid == 8)
		fprintf(fp1, "%u	%u	%.3f	%.3f	%.3f	%.3f	\n%.3f	%.3f	%.3f	%.3f	\n%.3f	%.3f	%.3f	%.3f	\n%.3f	%.3f	%.3f	%.3f	\n%.3f	%.3f	%.3f	%.3f\n", \
			detectsListOutput.frameID, inst->uid, u_tilda.array[0], u_tilda.array[1], u_tilda.array[2], u_tilda.array[3],\
			inst->assoProperty.gC_inv[0], inst->assoProperty.gC_inv[1], inst->assoProperty.gC_inv[2], inst->assoProperty.gC_inv[3],\
			inst->assoProperty.gC_inv[4], inst->assoProperty.gC_inv[5], inst->assoProperty.gC_inv[6], inst->assoProperty.gC_inv[7], \
			inst->assoProperty.gC_inv[8], inst->assoProperty.gC_inv[9], inst->assoProperty.gC_inv[10], inst->assoProperty.gC_inv[11], \
			inst->assoProperty.gC_inv[12], inst->assoProperty.gC_inv[13], inst->assoProperty.gC_inv[14], inst->assoProperty.gC_inv[15]);*/
		fprintf(fp1, "%u	%u	%.3f	%.3f	%.3f	%.3f\n", detectsListOutput.frameID, inst->uid, inst->kalman_state.H_s.array[0], inst->kalman_state.H_s.array[1] * RAD2DEG, \
			measInfo->measurement[assocId].array[0], measInfo->measurement[assocId].array[1] * RAD2DEG);
#endif // DEBUG_COHE_TRACK
		inst->assoProperty.assoCondenceMeasId = assocId;
		inst->assoProperty.lastSeenTime = g_curTimestamp;
		inst->assoProperty.lastSeenWholeScanId = wholeSpaceScanCycleCnt;
		inst->assoProperty.assMahDistance = minMd;
		inst->objManagement.forcastFrameNum = 0;
		inst->objManagement.classification = measurement->classification;
		measurement = &measInfo->measurement[assocId];
		if (gVelunambigType == WHOLE_SCAN)
		{
			inst->assoProperty.assoUnambiVel = measurement->unambigVel;
			inst->assoProperty.sameRootTracker.mdScore = minMd;
		}
		else if (gVelunambigType == STEP_SCAN)
		{
			inst->assoProperty.assoUnambiVel = measurement->unambigVel;
			inst->assoProperty.sameRootTracker.mdScore = minMd;
			if (inst->objKinematic.velUnambigProp.velUnambigDone == 0)
			{
				inst->objKinematic.velUnambigProp.velUnambigDone = 1;
				if (fabsf(originDoppler - inst->kalman_state.H_s.vector.doppler) > 1.f)
				{
					memcpy(meaSph.array, measurement->array, sizeof(measurement->array));
					meaSph.vector.doppler = measurement->unambigVel;
					tracking_spherical2cartesian(inst->tracking_params->stateVectorType, meaSph.array, inst->kalman_state.S_hat);
				}
				// TODO: no need to update
			}
		}
		m = measurement->detId;
		inst->assoProperty.bitIndicatorsOfDetAsso[m >> 3] |= (1 << (m & 0x7));
		/*for (n = 0; n < measurement->detectionNum; n++)
		{
			m = measurement->detectionId[n];
			inst->assoProperty.bitIndicatorsOfDetAsso[m >> 3] |= (1 << (m & 0x7));
		}*/
		bestIndex[assocId] = inst->uid;
#ifdef DEBUG_LOG_ZQ
		my_printf("assoPairs [%d %d] md %.2f", assocId, inst->tid, minMd);
#endif // DEBUGLOG
		if (measurement->unambigVelDone == 0)
		{
			if (inst->assoProperty.waveInfo == 0 && measurement->waveType == 0)
			{
				inst->assoProperty.waveInfo = 0;//do nothing
			}
			else if (inst->assoProperty.waveInfo == 1 && measurement->waveType == 1)
			{
				inst->assoProperty.waveInfo = 1;//do nothing
			}
		}
		else
		{
			inst->assoProperty.waveInfo = 2;
		}
	}
	else  // unassociated
	{
		inst->assoProperty.assMahDistance = -1.f;
		if (g_scanType == TAS_SCAN)
		{
#ifdef DEBUG_LOG_ZQ
			my_printf("condenceDone %d", measInfo->condenceDone);
#endif // DEBUG_LOG
			if (measInfo->condenceDone == 1)
			{
				inst->objManagement.forcastFrameNum++;
			}
		}
		else
		{
			if (wholeSpaceScanCycleFinalBeamFlag == 1 && inst->objManagement.tasFlag == 0)		// start of current scan
			{
				if (wholeSpaceScanCycleCnt - inst->assoProperty.lastSeenWholeScanId > inst->objManagement.forcastFrameNum)
				{
					inst->objManagement.forcastFrameNum = wholeSpaceScanCycleCnt - inst->assoProperty.lastSeenWholeScanId;
				}
			}
		}
#ifdef DEBUG_LOG
		//my_printf("id %d forcastNum %d", inst->uid, inst->objManagement.forcastFrameNum);
#endif // DEBUG_LOG

	}
	if (inst->objManagement.forcastFrameNum > 1U)
	{
		inst->kalman_state.S_hat[6] = 0.f;
		inst->kalman_state.S_hat[7] = 0.f;
		inst->kalman_state.S_hat[8] = 0.f;
	}
#ifdef DEBUG_COHE_TRACK
	fclose(fp1);
#endif // DEBUG_COHE_TRACK
}
/**
 * @b Description
 * @n
 *      .
 * 
 *       
 * @return 
 *       
 */
uint8_t tracking_velocityUnambigiousInit(float* dotsVel, float* measVel, sMeasurement_enum_wavetype waveType)
{
	uint8_t unambigFlag = 0;
	int N = 0, M = 0, i = 0, j = 0;
	float velScopeDots = 0.f, velScopeMeas = 0.f, dotsUnambiVel = 0.f, measUnambiVel = 0.f;
	if (waveType == WAVE_TYPE_0)
	{
		velScopeMeas = DOPPLER_SCOPE_WAVETYPE_0;
		velScopeDots = DOPPLER_SCOPE_WAVETYPE_1;
		M = AMBI_NUM_T0;
		N = AMBI_NUM_T1;
	}
	else
	{
		velScopeMeas = DOPPLER_SCOPE_WAVETYPE_1;
		velScopeDots = DOPPLER_SCOPE_WAVETYPE_0;
		M = AMBI_NUM_T1;
		N = AMBI_NUM_T0;
	}
	for (i = -N; i <= N; i++)
	{
		dotsUnambiVel = *dotsVel + i * velScopeDots;
		if (fabsf(dotsUnambiVel) > MAXVEL)
			continue;
		for (j = -M; j <= M; j++) {
			measUnambiVel = *measVel + j * velScopeMeas;
			if (fabsf(*measVel) > MAXVEL)
				continue;
			if (fabsf(dotsUnambiVel - measUnambiVel) < 1.f)
			{
				unambigFlag = 1;
				*dotsVel = dotsUnambiVel;
				*measVel = measUnambiVel;
			}
		}
	}
	return unambigFlag;
}

float tracking_velocityUnambigious(sMeasurement_enum_wavetype waveType, float trackDoppler, float measDoppler,uint8_t acclerTargetFlag)
{
	float dopplerScope = 0.f;
	float dopplerRet = measDoppler;
	int M = 0;
	int i = 0;
	float deltaV = 0.f;
	float dvTh = 1.f;
	if (acclerTargetFlag == 1)
	{
		dvTh += 0.5f;
	}
	if (waveType == WAVE_TYPE_0)
	{
		dopplerScope = DOPPLER_SCOPE_WAVETYPE_0;
		M = AMBI_NUM_T0;
	}
	else
	{
		dopplerScope = DOPPLER_SCOPE_WAVETYPE_1;
		M = AMBI_NUM_T1;
	}
	for (i = -M; i <= M; i++)
	{
		deltaV = trackDoppler - (measDoppler + i * dopplerScope);
#ifdef DEBUG_LOG_ZQ
		//my_printf("deltaV %.2f", deltaV);
#endif // DEBUG_LOG_TAS

		if (fabsf(deltaV) < dvTh)
		{
			dopplerRet = measDoppler + i * dopplerScope;
			break;
		}
	}
	return dopplerRet;
}

uint8_t tracking_isMeasurementBesideTracker(sMeasurement_SphPoint measDelta, sTracking_objectUnit *inst)
{
	uint8_t ret = 0;
	float range = inst->kalman_state.H_s.vector.range;
	float rangeBound = 250.f;
	float rangeBoundFar = 600.f;
	if (
		(range < rangeBound &&
			fabsf(measDelta.vector.range) < 20.f &&
			fabsf(measDelta.vector.azimuthRad) < 20.f * DEG2RAD &&
			fabsf(measDelta.vector.pitchRad) < 20.f * DEG2RAD &&
			fabsf(measDelta.vector.doppler) < 2.f) ||
		(range >= rangeBound && range < rangeBoundFar &&
			fabsf(measDelta.vector.range) < 30.f &&
			fabsf(measDelta.vector.azimuthRad) < 10.f * DEG2RAD &&
			fabsf(measDelta.vector.pitchRad) < 20.f * DEG2RAD &&
			fabsf(measDelta.vector.doppler) < 1.5f) ||
		(range >= rangeBoundFar &&
			fabsf(measDelta.vector.range) < 30.f &&
			fabsf(measDelta.vector.azimuthRad) < 6.f * DEG2RAD &&
			fabsf(measDelta.vector.pitchRad) < 20.f * DEG2RAD &&
			fabsf(measDelta.vector.doppler) < 1.5f)
		)
	{
			ret = 1;
	}
	return ret;
}
/**
*  @b Description
*  @n
*		tracking Module calls this function to obtain the measurement vector scoring from the tracking unit perspective
*
*  @param[in]  handle
*		This is handle to tracking unit
*  @param[in]  point
*		This is an array of measurement points
*  @param[inout]  bestScore
*		This is a pointer current scoresheet with best scores. Only better scores are updated
*  @param[inout]  bestInd
*		This is a pointer current scoresheet winners. Only better scores winners are updated
*  @param[out]  isUnique
*       This is an array indicating whether point belongs to a single target (1) or not (0)
*  @param[in]  num
*		Number of measurement points
*
*  \ingroup tracking_ALG_UNIT_FUNCTION
*
*  @retval
*      None
*/
// Consider the ambiguity azimuth, given the multi-azimuth
void tracking_unitScore_multiAzim(void *handle, sMeasurement_SphPoint *point, float *bestScore, \
					  uint16_t *bestInd, uint8_t *isUnique, uint16_t num, uint8_t *isAssociatedActive, \
					  float *minMag_thre_array, float *mag_thre_array, float *minSNR_thre_array, float *snr_thre_array, \
					  float *rcs_thre_array, float *statVel_thre_array, void** hTrack, sTracking_platformInfo *platformInfo)
{
	sTracking_objectUnit *inst = NULL;
	uint16_t n = 0;
	uint16_t m = 0;
	uint16_t k = 0;
	uint16_t i = 0;
	uint8_t isWithinLimits = 0;
	float mdp = 0.f;
	float md = 0.f;
	sTracking_measurementSphUnion u_tilda;
	float score = 0.f;
	float rvOut = 0.f;
	uint8_t *visited = gScratchPadData->visited;
	uint16_t *neighs = gScratchPadData->neighs;

	float logdet = 0.f;
	uint16_t neighCount=0;
	uint16_t *neighCurrent = NULL;
	uint16_t *neighLast = NULL;
	int16_t DebugFlag = 0;
	//sTracking_allocationParams allocThres;
	sTracking_cartesian_position cart_c;
	sTracking_boundaryBox box;
	float width = 0.f;
	float length = 0.f;
	float max_rcs = 0;
	float max_snr = 0;
	float max_mag = 0.0f;
	float max_angelSin = 0.0f;
	float max_magCoorX = 0.f;
	float max_magCoorY = 0.f;
	float rcs_thre = 10.0f;
	float mag_thre = 0.0f;
	float sinAngle = 0.f;
	float cosAngle = 0.f;
	uint16_t validDet_index[MAX_NUM_DETECTS] = { 0U };
	uint16_t validDet_cnt = 0U;
	float cart_tilda[6] = { 0.f };
	float doppler_tmp = 0.f;
	float dx = 0.f;
	float dy = 0.f;
	float dz = 0.f;
	float dvrad = 0.f;
	float dx_ex[NUM_EXTERN_DOPPLER] = { 0.f };
	float dy_ex[NUM_EXTERN_DOPPLER] = { 0.f };
	float dz_ex[NUM_EXTERN_DOPPLER] = { 0.f };
	float dvrad_ex[NUM_EXTERN_DOPPLER] = { 0.f };
	int ex_idx = -2; // -1: dx, 0: dx_ex[0], 1: dx_ex[1]
	int ex_idx_array[MAX_NUM_DETECTS] = { 0 };
	float x_ = 0.f;
	float y_ = 0.f;
	float vrad_ = 0.f;
	float angle_ = 0.f;
	sTracking_gateLimits gating_;
	sTracking_gateLimits gating1_;
	sTracking_gateLimits gating2_;
	int member = 0;
	float vel_thre = 0.f;
	sTracking_gatingParams exGating_scope_;
	sTracking_gatingParams gatingParams_;
	int alive0 = 0;
	int alive1 = 0;
	int alive2 = 0;
	sTracking_objectUnit *pTracker = NULL;
	memset((uint8_t *)&u_tilda, 0, sizeof(sTracking_measurementSphUnion));
	inst = (sTracking_objectUnit *)handle;
	memset((uint8_t *)visited, 0, sizeof(uint8_t)*TRACKING_NUM_POINTS_MAX);
	memset((uint8_t *)neighs, 0, sizeof(uint16_t)*TRACKING_NUM_POINTS_MAX);
	//memset((uint8_t *)&allocThres, 0, sizeof(sTracking_allocationParams));
	memset((uint8_t *)&cart_c, 0, sizeof(sTracking_cartesian_position));
	memset((uint8_t *)&box, 0, sizeof(sTracking_boundaryBox));
	neighCurrent = neighLast = neighs;
	memset((uint8_t *)&box, 0, sizeof(sTracking_boundaryBox));
	memset(&gating_, 0, sizeof(sTracking_gateLimits));
	memset(&gating1_, 0, sizeof(sTracking_gateLimits));
	memset(&gating2_, 0, sizeof(sTracking_gateLimits));
	memset(&exGating_scope_, 0, sizeof(sTracking_gatingParams));
	memset(ex_idx_array, -1, sizeof(int)*MAX_NUM_DETECTS);

	if(!getExGatingParams(inst->kalman_state.H_s.array, &exGating_scope_, &inst->tracking_params->advParams, platformInfo)){
		exGating_scope_.limits.depth = 0.2f;
		exGating_scope_.limits.width = 0.4f;
		exGating_scope_.limits.height = 0.5f;
		exGating_scope_.limits.vel = 0.4f;
	}

	getGatingParams(inst->kalman_state.H_s.array, &gatingParams_, &inst->tracking_params->advParams, platformInfo);
	inst->assoProperty.assocGating.depth = inst->assoProperty.assocDynamicGating.depth + 2.f*exGating_scope_.limits.depth;
	inst->assoProperty.assocGating.width = inst->assoProperty.assocDynamicGating.width + 2.f*exGating_scope_.limits.width;
	inst->assoProperty.assocGating.height = inst->assoProperty.assocDynamicGating.height + 2.f*exGating_scope_.limits.height;
	inst->assoProperty.assocGating.vel = inst->assoProperty.assocDynamicGating.vel + 2.f*exGating_scope_.limits.vel;

	gating_.depth = inst->assoProperty.assocGating.depth;
	gating_.width = inst->assoProperty.assocGating.width;
	gating_.height = inst->assoProperty.assocGating.height;
	gating_.vel = inst->assoProperty.assocGating.vel;

	/*if(inst->objClass.eObjClass==CLASS_VEHICLE){
		gating_.width *= 2.f;
	}*/
	if(    fabsf(inst->kalman_state.S_hat[0])<10.f \
		&& fabsf(inst->kalman_state.S_hat[1])<10.f \
		//&& inst->objClass.eObjClass==VEHICLE 
		&& (    fabsf(inst->kalman_state.fTrackerHeading-90.f*DEG2RAD)<80.f*DEG2RAD \
			 || fabsf(inst->kalman_state.fTrackerHeading+90.f*DEG2RAD)<80.f*DEG2RAD \
			 || inst->eDynamicProperty==TRACKING_DYNAMIC_STATIC  ) \
		){
		gating_.width *= 2.0f;
		//if(inst->kalman_state.S_hat[1]<0.f){
		//	gating_.depth *= 1.0f;
		//}
		//else{
		//	gating_.depth *= 0.5f;
		//}
		gating_.vel *= 2.0f;
		//gating_.depth *= 2.0f;
		if(    fabsf(inst->kalman_state.S_hat[1])<3.f \
			&& fabsf(inst->kalman_state.S_hat[0])<4.5f ){
			gating_.depth *= 0.7f;
		}
	}
	else if(fabsf(inst->kalman_state.S_hat[3])<5.f){
	}
	if (inst->eDynamicProperty != TRACKING_DYNAMIC_STATIC && fabsf(inst->kalman_state.S_hat[0]) < 3.f\
		&&fabsf(inst->kalman_state.S_hat[0]) < 3.f && (inst->objManagement.lifetime > 20 || \
		(inst->objManagement.heartBeatCount - inst->objManagement.allocationTime > 20 && fabsf(inst->kalman_state.fVabsY) > 5.f)) )
	{
		gating_.vel *= 1.5f;
		gatingParams_.limits.vel *= 1.5f;
	}
	inst->assoProperty.assocGating.depth = gating_.depth;
	inst->assoProperty.assocGating.width = gating_.width;
	inst->assoProperty.assocGating.vel = gating_.vel;

	if(inst->assoProperty.assocGating.depth>gatingParams_.limits.depth){
		inst->assoProperty.assocGating.depth = gatingParams_.limits.depth;
	}

	if (inst->assoProperty.assocGating.width > gatingParams_.limits.width) {
		inst->assoProperty.assocGating.width = gatingParams_.limits.width;
	}
	if(inst->assoProperty.assocGating.height>gatingParams_.limits.height){
		inst->assoProperty.assocGating.height = gatingParams_.limits.height;
	}
	if(inst->assoProperty.assocGating.vel>gatingParams_.limits.vel){
		inst->assoProperty.assocGating.vel = gatingParams_.limits.vel;
	}

	gating_.depth = inst->assoProperty.assocGating.depth;
	gating_.width = inst->assoProperty.assocGating.width;
	gating_.height = inst->assoProperty.assocGating.height;
	gating_.vel = inst->assoProperty.assocGating.vel;
	gating1_.depth = inst->assoProperty.assocGating.depth*0.5f;
	gating1_.width = inst->assoProperty.assocGating.width*0.5f;
	gating1_.height = inst->assoProperty.assocGating.height*0.5f;
	gating1_.vel = inst->assoProperty.assocGating.vel*0.5f;
	gating2_.depth = inst->assoProperty.assocGating.depth*3.f;
	gating2_.width = inst->assoProperty.assocGating.width*3.f;
	gating2_.height = inst->assoProperty.assocGating.height*3.f;
	gating2_.vel = inst->assoProperty.assocGating.vel*3.f;
#ifdef DEBUG_LOG
	if (inst->eDynamicProperty != TRACKING_DYNAMIC_STATIC)
	{
		//my_printf("id[%d], gating_: depth: %.4f, width: %.4f, height: %.4f, vel: %.4f,SmallObjFlag:%d,state:%d,atc2free:%d\n", \
			inst->uid, gating_.depth, gating_.width, gating_.height, gating_.vel,\
			inst->objGatingCtrl.nearRegionSmallObjFlag,inst->objManagement.state,inst->objManagement.active2freeCount);
	}
#endif

	memset((uint8_t *)(inst->assoProperty.bitIndicatorsOfDetAsso), 0, sizeof(inst->assoProperty.bitIndicatorsOfDetAsso));
	memset((uint8_t*)(&inst->curTarget.unionSph), 0, sizeof(inst->curTarget.unionSph));
	memset((uint8_t*)(&inst->curTarget.unionX), 0, sizeof(inst->curTarget.unionX));
	memset((uint8_t*)(&inst->curTarget.unionY), 0, sizeof(inst->curTarget.unionY));
	
	for (n = 0; n < num; n++) {
		if (bestInd[n] == TRACKING_ID_POINT_BEHIND_THE_WALL)
			continue;
		if (point[n].mag < minMag_thre_array[n]) {
			continue;
		}
		if (point[n].snr < minSNR_thre_array[n]) {
			continue;
		}
		ex_idx_array[n] = -1;
		ex_idx = -1;

		//tracking_vectorSub(MSIZE_SPH, point[n].array, inst->kalman_state.H_s.array, u_tilda.array);
		isWithinLimits = false;
		dx = point[n].x - inst->kalman_state.S_hat[0];
		dy = point[n].y - inst->kalman_state.S_hat[1];
		dvrad = point[n].vector.doppler - inst->kalman_state.H_s.vector.doppler;
		if(1U==tracking_isInsideWithGating(dx, dy, 0.f, 0.f, &gating2_)){
			isWithinLimits = true;
		}
		// pre-select the valid detections for the tracker association
		if(isWithinLimits){
			validDet_index[validDet_cnt] = n;
			validDet_cnt++;
		}
		/* Any point outside the limits is outside the gate */
		isWithinLimits = true;
		isWithinLimits = tracking_unitScore_association( \
			&point[n], inst, &gating_, &gating1_, &gating2_, \
			point[n].vector.azimuthRad, point[n].sinAzim, point[n].cosAzim, \
			point[n].vector.doppler, dx, dy, dvrad, platformInfo,0);
	   if(true==isWithinLimits){
			ex_idx = -1;
		}
		if (isWithinLimits == false)
			continue;
		ex_idx_array[n] = ex_idx;
		//if(inst->objManagement.state==TRACK_STATE_ACTIVE || inst->objManagement.state==TRACK_STATE_NEW_ACTIVE){
		//	point[n].unambigVel = 1.f;
		//}
		visited[n] = 1;
		neighs[neighCount] = (uint16_t)(n);
		neighCount++;
		neighLast++;
		if (max_rcs < point[n].rcs) {
			max_rcs = point[n].rcs;
		}
		if (max_snr < point[n].snr) {
			max_snr = point[n].snr;
		}
		if (max_mag < point[n].mag) {
			max_mag = point[n].mag;
			max_magCoorX = point[n].x;
			max_magCoorY = point[n].y;
		}
		if (fabsf(max_angelSin) < fabsf(point[n].vector.azimuthRad)) {
			max_angelSin = point[n].vector.azimuthRad;
		}
	}
	while (neighCurrent != neighLast)
	{
		member = (int)(*neighCurrent++);
		for (k = 0; k < validDet_cnt; k++) {
			n = validDet_index[k];
			if (bestInd[n] == TRACKING_ID_POINT_BEHIND_THE_WALL)
				continue;
			if (visited[n] == 1) {
				continue;
			}
			// Stationary targets can associate with moving object, 
			//  but stationary object cannot associate with moving target
			//if(    inst->eDynamicProperty  ==  TRACKING_DYNAMIC_STATIC \
			//	&& point[n].flag_moving    !=                       0U)
			//if(point[n].flag_moving!=point[member].flag_moving)
			//{
			//	continue;
			//}
			if (point[n].mag < minMag_thre_array[n]) {
				continue;
			}
			if (point[n].snr < minSNR_thre_array[n]) {
				continue;
			}
			ex_idx = -1;
			isWithinLimits = false;
			x_ = point[member].x;
			y_ = point[member].y;
			vrad_ = point[member].vector.doppler;
			dx = x_ - point[n].x;
			dy = y_ - point[n].y;
			dvrad = vrad_ - point[n].vector.doppler;
			
			/* Any point outside the limits is outside the gate */
			isWithinLimits = true;
			isWithinLimits = tracking_unitScore_pointsAssociation( \
				&point[n], inst, &point[member], ex_idx_array[member], &gating_, &gating1_, &gating2_, \
				point[n].vector.azimuthRad, point[n].sinAzim, point[n].cosAzim, \
				point[n].vector.doppler, dx, dy, dvrad);

			if(true==isWithinLimits){
				ex_idx = -1;
			}
			if (isWithinLimits == false)
				continue;
			ex_idx_array[n] = ex_idx;
			//if(inst->objManagement.state==TRACK_STATE_ACTIVE || inst->objManagement.state==TRACK_STATE_NEW_ACTIVE){
			//	point[n].unambigVel = 1.f;
			//}
			visited[n] = 1;
			neighs[neighCount] = (uint16_t)(n);
			neighCount++;
			neighLast++;
			if (max_rcs < point[n].rcs) {
				max_rcs = point[n].rcs;
			}
			if (max_snr < point[n].snr) {
				max_snr = point[n].snr;
			}
			if (max_mag < point[n].mag) {
				max_mag = point[n].mag;
				max_magCoorX = point[n].x;
				max_magCoorY = point[n].y;
			}
			if (fabsf(max_angelSin) < fabsf(point[n].vector.azimuthRad)) {
				max_angelSin = point[n].vector.azimuthRad;
			}
		}
	}
#ifdef DEBUG_LOG
	//my_printf("id[%d], preAssociated cnt: %d\n", inst->uid, neighCount);
#endif
	memset(&inst->assoProperty.u_tilda_sum, 0, sizeof(inst->assoProperty.u_tilda_sum));
	inst->curTarget.uNumOfDetections = 0;
	for(m=0; m<neighCount; m++)
	{
		n = (int)(neighs[m]);
		tracking_vectorSub(MSIZE_SPH, point[n].array, inst->kalman_state.H_s.array, u_tilda.array);
		/* Radial velocity estimation is known */
		// Consider the long object when crossing
		sinAngle = point[n].sinAzim;
		cosAngle = point[n].cosAzim;
		//myMath_sincosd(point[n].vector.azimuthRad*RAD2DEG, &sinAngle, &cosAngle);
		doppler_tmp = inst->kalman_state.S_hat[2]*cosAngle + inst->kalman_state.S_hat[3]*sinAngle;
		rvOut = point[n].vector.doppler;
		//point[n].unambigVel = 1.0f;
		//rvOut = tracking_unrollRadialVelocity(inst->tracking_params->maxRadialVelocity, inst->kalman_state.H_s.vector.doppler, point[n].vector.doppler);
		u_tilda.vector.azimuthRad = point[n].vector.azimuthRad - inst->kalman_state.H_s.vector.azimuthRad;
		u_tilda.vector.doppler = rvOut - doppler_tmp;
		/* For the gating purposes we compute partial Mahalanobis distance, ignoring doppler */		
		if(inst->kalman_state.uType==0U)
		{
			tracking_spherical2cartesian(inst->tracking_params->stateVectorType, u_tilda.array, \
				cart_tilda);
			cart_tilda[2] = sqrtf(cart_tilda[2]*cart_tilda[2]+cart_tilda[3]*cart_tilda[3]);
			// mdp = v*D*v'; v(x,y,vr), D(3x3) for x,y,vr
			tracking_computeMahalanobisPartial(cart_tilda, inst->assoProperty.gC_inv, &mdp);
		}
		else
		{
			tracking_computeMahalanobisPartial(u_tilda.array, inst->assoProperty.gC_inv, &mdp);
		}
		//printf("---- mdp: %.2f \n", mdp);
		/* Gating Step */
		//if(1 || mdp < inst->G) {
		//if (fabsf(inst->max_rcs - max_rcs) < 10 && fabsf(inst->max_mag-max_mag)<5 && mdp < inst->G) {
		if(1 || mdp < inst->assoProperty.G) 
		{
			if(    inst->objManagement.state==TRACK_STATE_ACTIVE 
				|| inst->objManagement.state==TRACK_STATE_NEW_ACTIVE 
				|| isAssociatedActive[n]==0)
			{
				//if(bestInd[n] < TRACKING_NUM_TRACKS_MAX)
				//    /* This point is not unique. Clear the indication */
				isUnique[n>>3] &= ~(1<<(n & 0x0007));
				inst->assoProperty.bitIndicatorsOfDetAsso[n>>3] |= (1<<(n&0x7));
				inst->curTarget.uNumOfDetections++;
				/* Scoring */
				tracking_computeMahalanobis(u_tilda.array, inst->assoProperty.gC_inv, &md);
				score = logdet + md;
				if(    (	inst->eDynamicProperty  ==  TRACKING_DYNAMIC_STATIC \
						 && point[n].flag_moving    !=                       0U) \
					|| (	inst->eDynamicProperty  !=  TRACKING_DYNAMIC_STATIC \
						 && point[n].flag_moving    ==                       0U)    ){
					score = score * 2.0f;
				}
				if(    (score < bestScore[n] && isAssociatedActive[n]==0 && inst->objManagement.state==TRACK_STATE_DETECTION) \
					|| (score < bestScore[n] && isAssociatedActive[n]==1 && inst->objManagement.state!=TRACK_STATE_DETECTION) \
					|| bestInd[n]==TRACKING_ID_POINT_NOT_ASSOCIATED) {
					/* If we are the best, register our score, and the index */
					bestScore[n] = score;
					bestInd[n] = (uint16_t)inst->uid;
				}
				if(inst->objManagement.state==TRACK_STATE_ACTIVE || inst->objManagement.state==TRACK_STATE_NEW_ACTIVE)
				{
					isAssociatedActive[n] = 1;
				}

			}
		}
	}
}


uint8_t tracking_unitScore_association( \
	sMeasurement_SphPoint* point, void* handle,\
	sTracking_gateLimits* gating_, sTracking_gateLimits* gating1_, sTracking_gateLimits* gating2_, \
	float azimuthRad, float sinAngle, float cosAngle, float doppler, float dx, \
	float dy, float dvrad, sTracking_platformInfo* platformInfo,uint8_t overtakeFlag)
{
	sTracking_objectUnit *inst = NULL;
	uint8_t isWithinLimits = true;
	float doppler_tmp = 100.f;
	float rvOut = 0.f;
	float vel_thre = gating_->vel;
	float egoVel = platformInfo->egoLineVel_lat;
	sTracking_gateLimits gating_tmp;
	sTracking_gateLimits gating_overtake;
	float absVel_thres = 2.f;
	float flag_asso = -1.f;
	int alive = 0;
	
	inst = (sTracking_objectUnit *)handle;
	memcpy(&gating_tmp, gating_, sizeof(gating_tmp));
	memcpy(&gating_overtake, gating_, sizeof(gating_overtake));
	gating_overtake.depth *= 1.5f;
	alive = (int)(inst->objManagement.heartBeatCount-inst->objManagement.allocationTime);
	alive = (alive>1000)? (1000) : (alive);

	if(inst->eDynamicProperty  ==  TRACKING_DYNAMIC_STATIC){
		if(point->flag_moving==0U){
			if(0U==tracking_isInsideWithGating(dx, dy, 0.f, \
				0.f, gating1_)){
				isWithinLimits = false;
				flag_asso = 1.f;
			}
		}
		else{
			isWithinLimits = false;
			flag_asso = 2.f;
		}
	}
	else{		// moving or stopped tracker
		if (fabsf(inst->kalman_state.fVabsX) > 2.f && fabsf(inst->kalman_state.S_hat[1]) > 5.f&&\
			fabsf(platformInfo->egoLineVel_lat)<1.f)
		{
			if (point->flag_moving == 0U)
			{
				isWithinLimits = false;
				flag_asso = 18.f;
			}
		}
		if(point->flag_moving==1U){
			if (overtakeFlag == 0||dx>0)
			{
				if (0U == tracking_isInsideWithGating(dx, dy, 0.f, \
					0.f, &gating_tmp)) {
					isWithinLimits = false;
					flag_asso = 3.f;
				}
			}
			else 
			{
				if (0U == tracking_isInsideWithGating(dx, dy, 0.f, \
					0.f, &gating_overtake)) {
					isWithinLimits = false;
					flag_asso = 3.1f;
				}
			}
			
		}
		else{
			// Enable assocation between moving object and static target, 
			//  when the object is stable and from long distance
		}
	}

	if (isWithinLimits == false)
		return false;
	/* Radial velocity estimation is not yet known, unroll based on velocity measured at allocation time */
	// Consider the long object when crossing
	doppler_tmp = inst->kalman_state.S_hat[2]*cosAngle + inst->kalman_state.S_hat[3]*sinAngle;
	//doppler_tmp = inst->kalman_state.H_s.vector.doppler;
	rvOut = doppler;
	dvrad = rvOut - doppler_tmp;

//	}
//	else
//	{
////#ifdef DEBUG_LOG
////		//my_printf("id[%d], azimuth: %.4f\n", inst->uid, point->vector.azimuthRad*RAD2DEG);
////#endif
//		if(inst->eDynamicProperty  ==  TRACKING_DYNAMIC_STATIC){
//			vel_thre = gating1_->vel*0.5f;
//		}
//		//else if(fabsf(point[n].vector.azimuthRad*RAD2DEG)<3.0f){
//		//	vel_thre = inst->assoProperty.H_limits.vector.doppler*2.f;
//		//}
//		else{
//			vel_thre = gating_->vel;
//		}
//		if (fabsf(dvrad) > vel_thre)
//		{
//			isWithinLimits = false;
//			flag_asso = 7.f;
//		}
//	}
#ifdef DEBUG_LOG
if (inst->eDynamicProperty != TRACKING_DYNAMIC_STATIC)
{
	/*my_printf("222 id[%d], flag_asso: %.1f\n", \
		inst->uid, flag_asso);*/
}
	
#endif
#ifdef DEBUG_LOG
	//my_printf("id[%d], point.r: %.3f, point.vel: %.3f, dvrad: %.4f, vel_thre: %.4f, isWithLimits: %d\n", \
		inst->uid, point->vector.range, point->vector.doppler, dvrad, vel_thre, (int)(isWithinLimits));
#endif
	if(false==isWithinLimits){
		return false;
	}
	return true;
}

uint8_t tracking_unitScore_pointsAssociation( \
	sMeasurement_SphPoint* point1, void* handle, sMeasurement_SphPoint* point0, \
	int ex_idx, sTracking_gateLimits* gating_, sTracking_gateLimits* gating1_, sTracking_gateLimits* gating2_, \
	float azimuthRad, float sinAngle, float cosAngle, float doppler, float dx, float dy, float dvrad)
{
	sTracking_objectUnit *inst = NULL;
	uint8_t isWithinLimits = true;
	float doppler_tmp = 100.f;
	float rvOut = 0.f;
	//float dvrad_temp = 100.f;
	float vel_thre = gating_->vel;
	sTracking_gateLimits gating_tmp;
	
	inst = (sTracking_objectUnit *)handle;
	memcpy(&gating_tmp, gating_, sizeof(gating_tmp));

	/* Any point outside the limits is outside the gate */
	isWithinLimits = true;
	return true;
}

