
#include <math.h>

#include "../include/tracking_common.h"
#include "../include/tracking_int.h"
#include "../../dispatch/include/dispatch.h"

/**
*  @b Description
*  @n
*		tracking Module calls this function to run tracking unit level state machine 
*
*  @param[in]  handle
*		This is handle to GTARCK unit
*  @param[in]  num
*		This is number of associated measurments
*
*  \ingroup tracking_ALG_UNIT_FUNCTION
*
*  @retval
*      None
*/

extern sSmoothTrackingPosition smoothTraj[MAX_NUM_TRAJS];
extern sSmoothTrackingPosition smoothTraj_previous_1[MAX_NUM_TRAJS];
extern sSmoothTrackingPosition smoothTraj_previous_2[MAX_NUM_TRAJS];
extern sSmoothTrackingPosition smoothTraj_previous_3[MAX_NUM_TRAJS];
extern sSmoothTrackingPosition smoothTraj_previous_4[MAX_NUM_TRAJS];
uint16_t cumClotNum[MAX_NUM_TRAJS] = {0};
extern const sMeasOutput* measInfo1;
extern sDetectFrame detectsListOutput;

void tracking_unitEvent(void *handle, sTracking_platformInfo* platformInfo, sTrackingParams *tracking_params)
{
    sTracking_objectUnit *inst;
	//float v_duidi = 0.f;

	inst = (sTracking_objectUnit *)handle;

	inst->objManagement.heartBeatCount++;
	if (g_scanType != TAS_SCAN)
	{
		if (fabsf(inst->kalman_state.S_hat[0]) < 0.001f && fabsf(inst->kalman_state.S_hat[1]) < 0.001f)
		{
			inst->objManagement.state = TRACK_STATE_FREE;
#ifdef DEBUG_LOG_ZQ
			my_printf("dead tracker pos0 %d id %d", 1, inst->tid);
#endif // DEBUG_LOG

			inst->objManagement.lifetime = 0;
			return;
		}
		if (inst->kalman_state.S_hat[0] < 0.001f)
		{
			inst->objManagement.state = TRACK_STATE_FREE;
#ifdef DEBUG_LOG_ZQ
			my_printf("dead tracker debug %d id %d", 1, inst->tid);
#endif // DEBUG_LOG
			inst->objManagement.lifetime = 0;
			return;
		}
		if (outOfAziOrEleBoundary(inst, tracking_params))
		{
			inst->objManagement.state = TRACK_STATE_FREE;
#ifdef DEBUG_LOG_ZQ
			my_printf("dead tracker debug %d id %d", 1, inst->tid);
#endif // DEBUG_LOG
			inst->objManagement.lifetime = 0;
			return;

		}

		//ÑÝÊ¾ÓÃ
		/*v_duidi = sqrtf(inst->kalman_state.S_hat[3] * inst->kalman_state.S_hat[3] + inst->kalman_state.S_hat[4] * inst->kalman_state.S_hat[4] + inst->kalman_state.S_hat[5] * inst->kalman_state.S_hat[5]);
		if (v_duidi > 10.f && inst->objManagement.state == TRACK_STATE_DETECTION)
		{
			inst->objManagement.state = TRACK_STATE_FREE;
		}*/
	}
	if (tracking_params->workMode == 0)
	{
		tracking_TwsTargetStateManagement(inst);
	}
	else
	{
		if (g_scanType == TAS_SCAN)
		{
			tracking_TasTargetStateManagement(inst);
		}
		else
		{
			tracking_TwsTargetStateManagement(inst);
		}
	}
}

void putFreeStateTracker2FreeList(sTracking_moduleInstance *inst, sTracking_ListObj* activeList)
{
	sTracking_ListElem *tElem = NULL;
	sTracking_ListElem *tElemToRemove;
	uint16_t uid;
	sTracking_objectUnit *pTracker;
	tElem = tracking_listGetFirst(activeList);
	while (tElem != 0)
	{
		uid = tElem->data;
		pTracker = (sTracking_objectUnit *)inst->hTrack[uid];
		if (pTracker->objManagement.state == TRACK_STATE_FREE) {
			tElemToRemove = tElem;
			tElem = tracking_listGetNext(tElem);
			tracking_listRemoveElement(activeList, tElemToRemove);
			inst->targetNumCurrent--;
			tracking_listEnqueue(&inst->freeList, tElemToRemove);
		}
		else {
			tElem = tracking_listGetNext(tElem);
		}
	}

}
void tracking_tasManager(sTracking_moduleInstance *inst)
{
	sTracking_ListElem *tElem = NULL;
	uint16_t uid;
	sTrackingParams *tracking_params = inst->tracking_params;
	tElem = tracking_listGetFirst(&inst->tasTrackList);
	while (tElem != 0)
	{
		uid = tElem->data;
		if (uid == inst->tasTargetId)
		{
			tracking_unitEvent(inst->hTrack[uid], &inst->platformInfo, tracking_params);
			//updatePast8frame(inst->hTrack[uid], inst, 1);
			break;
		}
		tElem = tracking_listGetNext(tElem);
	}
	tElem = tracking_listGetFirst(&inst->twsTrackList);
	while (tElem != 0)
	{
		uid = tElem->data;
		if (uid == inst->tasTargetId)
		{
			tracking_unitEvent(inst->hTrack[uid], &inst->platformInfo, tracking_params);
			//updatePast8frame(inst->hTrack[uid], inst, 1);
			break;
		}
		tElem = tracking_listGetNext(tElem);
	}
	putFreeStateTracker2FreeList(inst, &inst->tasTrackList);
	putFreeStateTracker2FreeList(inst, &inst->twsTrackList);
}

void tracking_twsManager(sTracking_moduleInstance *inst)
{
	sTracking_ListElem *tElem = NULL;
	uint16_t uid;
	sTrackingParams *tracking_params = inst->tracking_params;
	sTracking_objectUnit* traj;
//#ifdef DEBUG_LOG_TRACKER_EXISTING
//	FILE* fp1;
//	if (inst->wholeSpaceScanCycleCnt - inst->wholeSpaceProcessCycleCnt != 0)
//	{
//		fopen_s(&fp1, "E:/record/20220929_huizhou/record1.txt", "a+");
//		fprintf(fp1, "\nstart of cycle %u ------------------------------------------------------------------------\n\n", inst->wholeSpaceScanCycleCnt);
//		fclose(fp1);
//	}
//#endif
	//if (inst->wholeSpaceScanCycleCnt - inst->wholeSpaceProcessCycleCnt != 0) // é‡ç½®å½“å‰scanæ˜¯å¦å…³è”
	//{
	//	inst->wholeSpaceProcessCycleCnt = inst->wholeSpaceScanCycleCnt;
	//	tElem = tracking_listGetFirst(&inst->twsTrackList);
	//	while (tElem != 0)
	//	{
	//		uid = tElem->data;
	//		traj = (sTracking_objectUnit*)(inst->hTrack[uid]);
	//		/*(sTracking_objectUnit*)(tracking_module_inst->hTrack[uid]->assoProperty.assoThisScan) = 0;
	//		(sTracking_objectUnit*)(tracking_module_inst->hTrack[uid]->assoProperty.countThisScan) = 0;*/
	//		//æ­¤æ—¶assoThisScanè¡¨ç¤ºä¸Šä¸€è½®è¯¥èˆªè¿¹æ˜¯å¦è¢«å…³è?
	//		if (traj->assoProperty.assoThisScan == 0 && traj->assoProperty.assoCondenceMeasId == -1) // è‹¥è¯¥èˆªè¿¹ä¸Šä¸€scanå’Œæœ¬æ¬¡scanç¬¬ä¸€å¸§å‡æœªå…³è”ï¼Œåˆå§‹åŒ–è¯¥èˆªè¿¹çš„mdScoreï¼›è‹¥ä¸Šä¸€scanå…³è”äº†ï¼š1ï¼‰æ­¤è½®scanæœªè¢«å…³è”ï¼Œæ­¤è½®scanä¿ç•™ä¸Šä¸€è½®mdScore 2ï¼‰æ­¤è½®scanè¢«å…³è”ï¼Œç”¨æ­¤è½®çš„æ–°mdScore
	//		{
	//			traj->curTarget.mdScore = 100.f;
	//		}
	//		tElem = tracking_listGetNext(tElem);
	//	}

	//}


	tElem = tracking_listGetFirst(&inst->twsTrackList);
	while (tElem != 0)
	{
		uid = tElem->data;
		tracking_unitEvent(inst->hTrack[uid], &inst->platformInfo, tracking_params);
		//updatePast8frame(inst->hTrack[uid], inst, 0);
		tElem = tracking_listGetNext(tElem);
	}
	tElem = tracking_listGetFirst(&inst->tasTrackList);
	while (tElem != 0)
	{
		uid = tElem->data;
		tracking_unitEvent(inst->hTrack[uid], &inst->platformInfo, tracking_params);
		//updatePast8frame(inst->hTrack[uid], inst, 0);
		tElem = tracking_listGetNext(tElem);
	}

	if (gVelunambigType == STEP_SCAN || gVelunambigType == WHOLE_SCAN)
	{
		tracking_sameRootTrackerManagement(inst);
	}
	putFreeStateTracker2FreeList(inst, &inst->twsTrackList);
	putFreeStateTracker2FreeList(inst, &inst->tasTrackList);
}
void setStateParams(sTracking_stateParams* stateParams, sTracking_objectUnit *inst)
{
	uint8_t active2freeThre;
	float range = inst->kalman_state.H_s.vector.range;
	/*float maxRangeThInClutter = 0.f;
	float maxRangeTh = 0.f;*/
	float minRangeTh = (inst->timestamp - inst->objManagement.allocationTime)*1.0f;
	float azimuth = inst->kalman_state.H_s.vector.azimuthRad*RAD2DEG;
	/*maxRangeThInClutter = minRangeTh > 30.f ? minRangeTh : 30.f;
	maxRangeTh = minRangeTh > 15.f ? minRangeTh : 15.f;*/
	//TODO : for demo
	//if (fabsf(azimuth) > 5.0f && range < 100.f)
	//{
	//	stateParams->det2actThre = 8U;
	//	//stateParams->det2freeThre = 1;
	//	//stateParams->active2freeThre = 3;
	//}
	//else if (fabsf(azimuth) > 5.0f)
	//{
	//	stateParams->det2actThre = 3U;
	//}
	// TODO: [210-320] is the range scope of 500kHz/2*500kHz power supply
//	if ((range > 210.f&&range < 320.f)||(range > 590.f&&range < 630.f))
//	{
//		stateParams->active2freeThre = 10;
//		stateParams->det2actThre = 100;
//		stateParams->det2freeThre = 2;
//		if (inst->objKinematic.maxDistance > maxRangeThInClutter)
//		{
//			stateParams->det2actThre = 30;
//		}
//	}
	/*active2freeThre = inst->objManagement.lifetime / 10;
	stateParams->active2freeThre = active2freeThre > stateParams->active2freeThre ? \
		active2freeThre : stateParams->active2freeThre;
	stateParams->active2freeThre = stateParams->active2freeThre > 20 ? 20 : stateParams->active2freeThre;*/
	//if (fabsf(inst->kalman_state.H_s.vector.doppler) > VELOCITY_RANGE / 2.f)
	//{
	//	//stateParams->active2freeThre = 2;
	//	stateParams->det2freeThre = 1;
	//}
	//if (range < 20.f)		// TODO: near region(20m) false targets
	//{
	//	stateParams->unseenTimeThre = 1.f;
	//	stateParams->active2freeThre = 3;
	//	stateParams->det2actThre = 30;
	//	stateParams->det2freeThre = 1;
	//}
	//else if (range < 50.f)
	//{
	//	stateParams->det2actThre = 8;
	//	stateParams->unseenTimeThre = 1.f;
	//	stateParams->active2freeThre = 3;
	//	stateParams->det2freeThre = 1;
	//}
	if (range < 100)
	{
			stateParams->det2actThre = 5;
			stateParams->active2freeThre = 5;
			stateParams->det2freeThre = 2;
	}
	else if (range >= 100  && range < 200.f)
	{
		stateParams->det2actThre = 5;
		stateParams->active2freeThre = 5;
		stateParams->det2freeThre = 2;
	}
	else if (range >= 200.f && range < 400.f)
	{
		stateParams->det2actThre = 5U;
		stateParams->active2freeThre = 8;
		stateParams->det2freeThre = 2;
	}
	else if (range >= 400.f && range < 800.f)
	{
		stateParams->det2actThre = 4;
		stateParams->active2freeThre = 8;
		stateParams->det2freeThre = 2;
	}
	else if (range >= 800.f)
	{
		stateParams->det2actThre = 4;
		stateParams->active2freeThre = 10;
		stateParams->det2freeThre = 2;
	}

	stateParams->active2freeThre += inst->objManagement.lifetime / 10;
	stateParams->active2freeThre = stateParams->active2freeThre > 10 ? 10 : stateParams->active2freeThre;

	stateParams->unseenTimeThre += inst->objManagement.lifetime / 10.0f;
	stateParams->unseenTimeThre = stateParams->unseenTimeThre > 7 ? 7 : stateParams->unseenTimeThre;

	/*if (isInPowerSupplyClutterRange(range))
	{
		stateParams->det2actThre = 80;
		stateParams->unseenTimeThre = 3.f;
		stateParams->active2freeThre = 15;
		stateParams->det2freeThre = 1;
	}*/
	//if (range > 50 && range < 200)
	//{
	//	stateParams->unseenTimeThre = (g_curTimestamp - inst->objManagement.allocationTime) / 10.f;
	//	stateParams->unseenTimeThre = stateParams->unseenTimeThre > 0.2f ? stateParams->unseenTimeThre : 0.2f;
	//	stateParams->unseenTimeThre = stateParams->unseenTimeThre < 2.0f ? stateParams->unseenTimeThre : 2.0f;
	//	/*stateParams->det2actThre = 10000;
	//	stateParams->det2freeThre = 10;
	//	if (g_curTimestamp - inst->objManagement.allocationTime > 3.f)
	//	{
	//		stateParams->det2actThre = 10;
	//	}*/
	//}

//	if ((range > 38 && range < 90) || (range > 0 && range < 23)) //ÒÖÖÆ½ü¾àÀëµÄ¼ÙÄ¿±ê
//	{
//		stateParams->det2actThre = 1000;
//		if (g_curTimestamp - inst->objManagement.allocationTime > 5.f)
//		{
//			stateParams->det2actThre = 10;
//		}
//	}

	//if (g_curTimestamp - inst->objManagement.allocationTime > 5.f && range > 225 && inst->objKinematic.maxDistance < 7.5f)
	//{
	//	inst->objManagement.state = TRACK_STATE_FREE;
	//}
}

void tracking_TasTargetStateManagement(sTracking_objectUnit *inst)
{
	sTracking_stateParams *tasStateParams = &inst->tracking_params->advParams.tasStateParams;
	sTracking_stateParams stateParams;
	memcpy(&stateParams, tasStateParams, sizeof(sTracking_stateParams));
	setStateParams(&stateParams, inst);

	switch (inst->objManagement.state)
	{
	case TRACK_STATE_DETECTION:
	{
		if (inst->objManagement.lifetime < stateParams.det2actThre)
		{
			if (inst->objManagement.forcastFrameNum >= stateParams.det2freeThre&&
				isTrackerCanDelete(inst))
			{
				inst->objManagement.state = TRACK_STATE_FREE;
#ifdef DEBUG_LOG_ZQ
				my_printf("det 2 free id %d", inst->tid);
#endif // DEBUG_LOG

			}
		}
		else if(inst->objManagement.lifetime >= stateParams.det2actThre)
		{
			if (inst->objManagement.forcastFrameNum == 0)
			{
				inst->objManagement.state = TRACK_STATE_NEW_ACTIVE;
				//my_printf("*********det2actMode: TAS, id: %d, lifetime: %d, det2actThre: %d, cumDetNum: %d, CFM_FLAG: %d******", inst->uid, inst->objManagement.lifetime, stateParams.det2actThre, inst->assoProperty.cumDetNum, inst->assoProperty.confirmBeam);
			}
			else if (inst->objManagement.forcastFrameNum >= stateParams.det2freeThre && isTrackerCanDelete(inst))
			{
				inst->objManagement.state = TRACK_STATE_FREE;
			}
		}

		if (g_scanType == TAS_SCAN && inst->assoProperty.assoCondenceMeasId != -1 && inst->assoProperty.confirmBeam == CFM_ASSO_TWICE)
		{
			inst->objManagement.state = TRACK_STATE_NEW_ACTIVE;
			//my_printf("*********det2actMode: TAS_CFM, id: %d, lifetime: %d, det2actThre: %d, cumDetNum: %d, CFM_FLAG: %d******", inst->uid, inst->objManagement.lifetime, stateParams.det2actThre, inst->assoProperty.cumDetNum, inst->assoProperty.confirmBeam);
		}

		break;
	}
	case TRACK_STATE_NEW_ACTIVE:
	case TRACK_STATE_ACTIVE:
	{
		if (inst->objManagement.forcastFrameNum > stateParams.active2freeThre&&
			isTrackerCanDelete(inst) && inst->timestamp - inst->assoProperty.lastSeenTime > 4)
		{
			inst->objManagement.state = TRACK_STATE_FREE;
#ifdef DEBUG_LOG_ZQ
			my_printf("act 2 free id %d", inst->tid);
#endif // DEBUG_LOG
		}
		else
		{
			inst->objManagement.state = TRACK_STATE_ACTIVE;
		}

		break;
	}
	default:
		break;
	}
}
void tracking_TwsTargetStateManagement(sTracking_objectUnit *inst)
{
	sTracking_stateParams *twsStateParams = &inst->tracking_params->advParams.twsStateParams;
	sTracking_stateParams stateParams;
	float range = inst->kalman_state.H_s.vector.range;
	memcpy(&stateParams, twsStateParams, sizeof(sTracking_stateParams));
	setStateParams(&stateParams, inst);
	switch (inst->objManagement.state)
	{
	case TRACK_STATE_DETECTION:
	{
		if (inst->objManagement.lifetime < stateParams.det2actThre)
		{
			if (inst->objManagement.forcastFrameNum == stateParams.det2freeThre)
			{
				inst->objManagement.state = TRACK_STATE_FREE;
#ifdef DEBUG_LOG_ZQ
				my_printf("det2free id %d, forcNum %d", inst->tid, inst->objManagement.forcastFrameNum);
#endif // DEBUG_LOG
			}
		}
		else if (inst->objManagement.lifetime >= stateParams.det2actThre)
		{
			if (inst->objManagement.forcastFrameNum == 0)
			{
				inst->objManagement.state = TRACK_STATE_NEW_ACTIVE;
				//my_printf("*********det2actMode: TWS, id: %d, lifetime: %d, det2actThre: %d, cumDetNum: %d, CFM_FLAG: %d******", inst->uid, inst->objManagement.lifetime, stateParams.det2actThre, inst->assoProperty.cumDetNum, inst->assoProperty.confirmBeam);

			}
			else if (inst->objManagement.forcastFrameNum >= stateParams.det2freeThre)
			{
				inst->objManagement.state = TRACK_STATE_FREE;
			}
		}
		if (inst->assoProperty.assoCondenceMeasId != -1)
		{
			inst->assoProperty.cumDetNum += measInfo1->pointsIdInfo[inst->assoProperty.assoCondenceMeasId].detectionNum;
			if (inst->assoProperty.cumDetNum >= 60 && range < 75)
			{
				inst->objManagement.state = TRACK_STATE_NEW_ACTIVE;
			}
		}
		if (g_scanType == TWS_SCAN && inst->assoProperty.confirmBeam == CFM_ASSO_ONCE && inst->assoProperty.assoCondenceMeasId != -1)
		{
			inst->objManagement.state = TRACK_STATE_NEW_ACTIVE;
			//my_printf("*********det2actMode: TWS_CFM, id: %d, lifetime: %d, det2actThre: %d, cumDetNum: %d, CFM_FLAG: %d******", inst->uid, inst->objManagement.lifetime, stateParams.det2actThre, inst->assoProperty.cumDetNum, inst->assoProperty.confirmBeam);
		}

		break;
	}
	case TRACK_STATE_NEW_ACTIVE:
	case TRACK_STATE_ACTIVE:
	{
		if (inst->objManagement.forcastFrameNum > stateParams.active2freeThre ||\
			inst->timestamp - inst->assoProperty.lastSeenTime > stateParams.unseenTimeThre)
		{
			inst->objManagement.state = TRACK_STATE_FREE;
#ifdef DEBUG_LOG_ZQ
			my_printf("act2free id %d, forcNum %d", inst->tid, inst->objManagement.forcastFrameNum);
#endif // DEBUG_LOG

		}
		else
		{
			inst->objManagement.state = TRACK_STATE_ACTIVE;
		}
	}
	default:
		break;
	}
}
/**
 * @b Description
 * @n
 *      management the velocity unambigious same root trackers.
 * 
 * @param[in] inst
 *       tracking module instance.
 *       
 */
void tracking_sameRootTrackerManagement(sTracking_moduleInstance *inst)
{
	sTracking_ListElem *tElem = NULL;
	int16_t uid;
	int16_t uidSameRoot;
	float mdscore;
	float mdscoreSameRoot;
	int i, j;
	sTracking_objectUnit *pTracker;
	sTracking_objectUnit *pTrackerSameRoot;
	tElem = tracking_listGetFirst(&inst->twsTrackList);
	while (tElem != 0)
	{
		uid = tElem->data;
		pTracker = (sTracking_objectUnit *)inst->hTrack[uid];
		mdscore = pTracker->assoProperty.sameRootTracker.mdScore;
		if (fabsf(mdscore - 1000.f) > 1.f && pTracker->assoProperty.sameRootTracker.sameRootNum > 0 &&
			pTracker->objManagement.state != TRACK_STATE_FREE && pTracker->objManagement.lifetime > 0 )
		{
			for (i = 0; i < NUM_EXTERN_DOPPLER; i++)
			{
				uidSameRoot = pTracker->assoProperty.sameRootTracker.sameRootTrackerId[i];
				if (uidSameRoot != -1) 
				{
					pTrackerSameRoot= (sTracking_objectUnit *)inst->hTrack[uidSameRoot];
					mdscoreSameRoot = pTrackerSameRoot->assoProperty.sameRootTracker.mdScore;
					if (mdscore < mdscoreSameRoot)
					{
						if (isTrackerCanDelete(pTrackerSameRoot))
						{
							pTrackerSameRoot->objManagement.state = TRACK_STATE_FREE;
							pTrackerSameRoot->assoProperty.sameRootTracker.sameRootNum = 0;
							pTrackerSameRoot->assoProperty.sameRootTracker.mdScore = 1000.f;
							for (j = 0; j < NUM_EXTERN_DOPPLER; j++)
							{
								pTrackerSameRoot->assoProperty.sameRootTracker.sameRootTrackerId[j] = -1;
							}
							pTracker->assoProperty.sameRootTracker.sameRootNum--;
							pTracker->assoProperty.sameRootTracker.sameRootTrackerId[i] = -1;
						}
#ifdef DEBUG_SAMEROOT
						pTrackerSameRoot->objManagement.state = TRACK_STATE_FREE;
						my_printf("same root [%d %d],mdscore[%.2f %.2f], sameRootNum[%d %d]", pTracker->tid, pTrackerSameRoot->tid,
							mdscore, mdscoreSameRoot, pTracker->assoProperty.sameRootTracker.sameRootNum,
							pTrackerSameRoot->assoProperty.sameRootTracker.sameRootNum);
#endif // DEBUG_SAMEROOT
					}
				}
			}
		}
		tElem = tracking_listGetNext(tElem);
	}
}
//bubble ascending sort by range\doppler\distance\...
void sortTrackList(sTracking_moduleInstance *inst,sTracking_ListObj* trackList)
{
	sTracking_ListElem* p=trackList->begin;
	sTracking_ListElem* q=NULL;
	if (0==trackList->count || 0==trackList->begin)
	{
		return;
	}

	for (; p->next!=NULL; p=p->next)
	{
		for (q=p->next; q!=NULL; q = q->next)
		{				
			sTracking_objectUnit *instDataP = (sTracking_objectUnit *)(inst->hTrack[p->data]);
			sTracking_objectUnit *instDataQ = (sTracking_objectUnit *)(inst->hTrack[q->data]);
			float p_data=0, q_data=0;
			p_data = instDataP->kalman_state.H_s.vector.range*exp(-0.1*instDataP->kalman_state.H_s.vector.doppler+0);//weight coefficient to be adjusted,TODO add...
			q_data = instDataQ->kalman_state.H_s.vector.range*exp(-0.1*instDataQ->kalman_state.H_s.vector.doppler+0);

			if (q_data < p_data)//swap uid
			{
				int tmp = q->data;
				q->data = p->data;
				p->data = tmp;
			}
		}
	}
}

//by cdx //bubble sort TAS, ascending sort by fusion factors, and save the top TAS_ID_MAX_SIZE, others are moved to TWS
void tracking_TasTwsTypeManagement(sTracking_moduleInstance *inst)
{
	sTracking_ListElem *tTasElem = NULL;
	sTracking_ListElem *tTwsElem = NULL;
	sTracking_ListElem *tElemToMove;

	uint16_t uid;
	sTrackingParams *tracking_params = inst->tracking_params;
	if (0 == tracking_params->workMode)
	{
		tTasElem = tracking_listGetFirst(&inst->tasTrackList);
		if (NULL == tTasElem)
			return;
		while (tTasElem != 0)
		{
			tElemToMove = tTasElem;
			uid = tTasElem->data;
			tTasElem = tracking_listGetNext(tTasElem);
			tracking_listRemoveElement(&inst->tasTrackList, tElemToMove);
			tracking_listEnqueue(&inst->twsTrackList, tElemToMove);
		}
	}
	else
	{
		tracking_listMerge(&inst->twsTrackList, &inst->tasTrackList);

		//bubble sort, ascending sort by range\doppler\distance\...
		//sortTrackList(&inst->twsTrackList);
		sortTrackList(inst,&inst->twsTrackList);

		tracking_listSplit(&inst->twsTrackList,TAS_ID_MAX_SIZE, &inst->tasTrackList);

		//tracking_setTragetTasTwsProperty();
	}
}
//void updatePast8frame(void* handle, sTracking_moduleInstance* trackingModuleInst, uint16_t uid)
//{
//	sTracking_objectUnit* inst;
//	sTracking_stateParams stateThres;
//	int aa;
//#ifdef DEBUG_LOG_TRACKER_EXISTING
//	FILE* fp1;
//#endif
//
//	inst = (sTracking_objectUnit*)handle;
//	/*if (wholeSpaceScanCycleCnt - lastWholeSpaceScanCycleCnt == 1)*/ //lastSeenWholeScanId
//	/*if (trackingModuleInst->wholeSpaceScanCycleCnt - trackingModuleInst->lastWholeSpaceScanCycleCnt == 1)
//	{
//		if (inst->assoProperty.assoMeasId >= 0)
//		{
//			inst->objManagement.updatePast8frame = ((inst->objManagement.updatePast8frame << 1) | (0x00));
//		}
//		else
//		{
//			inst->objManagement.updatePast8frame = ((inst->objManagement.updatePast8frame << 1) | (0x01));
//		}
//	}*/
//	trackingModuleInst->tasTargetId = 30;
//	if (inst->objManagement.lifetime - inst->objManagement.lifetime_old != 0 && inst->uid == trackingModuleInst ->tasTargetId)
//	{
//		/*if (inst->objManagement.lifetime % 2 == 0)
//		{
//			inst->assoProperty.assoMeasId = 0;
//		}
//		else
//		{
//			inst->assoProperty.assoMeasId = -1;
//		}*/
//
//		//inst->assoProperty.assoMeasId = 0;
//
//		if (inst->assoProperty.assoCondenceMeasId != -1) {
//			inst->assoProperty.assoThisScan = 1;
//			inst->objManagement.updatePast8frame = ((inst->objManagement.updatePast8frame << 1) | (0x01));
//		}
//		else {
//			inst->assoProperty.assoThisScan = 0;
//			inst->objManagement.updatePast8frame = ((inst->objManagement.updatePast8frame << 1) | (0x00));
//		}
//		inst->objManagement.lifetime_old = inst->objManagement.lifetime;
//	}
//	else if (inst->uid != trackingModuleInst->tasTargetId)
//	{
//		if ((trackingModuleInst->wholeSpaceScanCycleCnt - trackingModuleInst->lastWholeSpaceScanCycleCnt == 1) && inst->assoProperty.assoThisScan == 0) // ÏÈÅÐ¶ÏÉÏÒ»scanÊÇ·ñÎ´¸üÐÂ£¬Î´¸üÐÂ8frame×·¼Ó0
//		{
//			inst->objManagement.updatePast8frame = ((inst->objManagement.updatePast8frame << 1) | (0x00));
//		}
//		if (trackingModuleInst->wholeSpaceScanCycleCnt - trackingModuleInst->lastWholeSpaceScanCycleCnt == 1) // initialize assoThisScan
//		{
//			/*if (inst->assoProperty.assoCondenceMeasId >= 0)
//			{
//				inst->assoProperty.assoThisScan = 1;
//			}
//			else {
//				inst->assoProperty.assoThisScan = 0;
//			}*/
//			inst->assoProperty.assoThisScan = 0;
//		}
//		if (inst->assoProperty.assoCondenceMeasId >= 0 && inst->assoProperty.assoThisScan == 0) // ±¾ÂÖscan8frame¸üÐÂ
//		{
//			inst->assoProperty.assoThisScan = 1;
//			inst->objManagement.updatePast8frame = ((inst->objManagement.updatePast8frame << 1) | (0x01));
//			aa = 1;
//		}
//	}
//#ifdef DEBUG_LOG_TRACKER_EXISTING
//	// æ¯æ¬¡è¿›å…¥èˆªè¿¹ç®¡ç†éƒ½ä¼šæ‰“å°ï¼Œå³æ¯å¸§éƒ½ä¼šæ‰“å°æ‰?æœ?**tasæˆ–tws** èˆªè¿¹
//	aa = 1;
//	fopen_s(&fp1, "E:/record/20220929_huizhou/record1.txt", "a+");
//	/*fprintf(fp1, "8frame: %x	assoid: %d	", inst->objManagement.updatePast8frame, inst->assoProperty.assoMeasId);*/
//	fprintf(fp1, "id:%u	8frame: %u	assoid: %d	assoThisScan:%u	life:%d	lifeold:%d	tas_id:%d	process:%u\n", \
//		inst->uid, inst->objManagement.updatePast8frame, inst->assoProperty.assoCondenceMeasId, inst->assoProperty.assoThisScan,\
//		inst->objManagement.lifetime, inst->objManagement.lifetime_old, trackingModuleInst->tasTargetId, uid);
//	fclose(fp1);
//#endif
//}


void tracking_trajSmooth(sTracking_moduleInstance* inst, sAlgObjData* algObj)
{
	sTracking_ListElem* tElem = NULL;
	uint16_t uid;
	sTrackingParams* tracking_params = inst->tracking_params;
	sTracking_objectUnit* traj = NULL;
	tElem = tracking_listGetFirst(&inst->tasTrackList);
	while (tElem != 0)
	{
		uid = tElem->data;
		tracking_unitTrajSmooth(inst->hTrack[uid], 1, algObj);
		tElem = tracking_listGetNext(tElem);
	}

	tElem = tracking_listGetFirst(&inst->twsTrackList);
	while (tElem != 0)
	{
		uid = tElem->data;
		traj = (sTracking_objectUnit*)inst->hTrack[uid];
		tracking_unitTrajSmooth(inst->hTrack[uid], 1, algObj);
		tElem = tracking_listGetNext(tElem);
	}

}

void tracking_unitTrajSmooth(void* handle, int a, sAlgObjData* algObj)
{
	sTracking_objectUnit* inst = NULL;
	sDispatchInst* dispatch_inst = (sDispatchInst*)algObj->dispatch_inst;
	sTrackingInst* tracking_inst = (sTrackingInst*)(algObj->tracking_inst);
	sTracking_moduleInstance* trackingModuleInst = (sTracking_moduleInstance*)tracking_inst->handle;

	inst = (sTracking_objectUnit*)handle;

	if (inst->assoProperty.assoCondenceMeasId != -1){
		;
	}
	else if (inst->assoProperty.assoThisScan == 0 && dispatch_inst->dispatchOutput->wholeSpaceScanCycleFinalBeamFlag == 1){
		;
	}
	else if (dispatch_inst->dispatchOutput->tasOrInitTrackFinalBeamFlag == 1 && inst->uid == trackingModuleInst->tasTargetId) {
		;
	}
	else{
		return;
	}

	//if (inst->objManagement.state == TRACK_STATE_FREE)
	//{
	//	smoothTraj[inst->uid].x = 0.f;
	//	smoothTraj[inst->uid].y = 0.f;
	//	smoothTraj[inst->uid].z = 0.f;
	//	smoothTraj[inst->uid].vx = 0.f;
	//	smoothTraj[inst->uid].vy = 0.f;
	//	smoothTraj[inst->uid].vz = 0.f;
	//	smoothTraj[inst->uid].range = 0.f;
	//	smoothTraj[inst->uid].azimuthRad = 0.f;
	//	smoothTraj[inst->uid].pitchRad = 0.f;
	//	smoothTraj[inst->uid].doppler = 0.f;
	//}
	//else if (inst->objManagement.lifetime > 0)
	//{
	//	/*smoothTraj[inst->uid].x = (1 - a) * inst->kalman_state.S_hat[0] + a * smoothTraj[inst->uid].x;
	//	smoothTraj[inst->uid].y = (1 - a) * inst->kalman_state.S_hat[1] + a * smoothTraj[inst->uid].y;
	//	smoothTraj[inst->uid].z = (1 - a) * inst->kalman_state.S_hat[2] + a * smoothTraj[inst->uid].z;
	//	smoothTraj[inst->uid].vx = (1 - a) * inst->kalman_state.S_hat[3] + a * smoothTraj[inst->uid].vx;
	//	smoothTraj[inst->uid].vy = (1 - a) * inst->kalman_state.S_hat[4] + a * smoothTraj[inst->uid].vy;
	//	smoothTraj[inst->uid].vz = (1 - a) * inst->kalman_state.S_hat[5] + a * smoothTraj[inst->uid].vz;
	//	smoothTraj[inst->uid].range = (1 - a) * inst->kalman_state.H_s.vector.range + a * smoothTraj[inst->uid].range;
	//	smoothTraj[inst->uid].azimuthRad = (1 - a) * inst->kalman_state.H_s.vector.azimuthRad + a * smoothTraj[inst->uid].azimuthRad;
	//	smoothTraj[inst->uid].pitchRad = (1 - a) * inst->kalman_state.H_s.vector.pitchRad + a * smoothTraj[inst->uid].pitchRad;
	//	smoothTraj[inst->uid].doppler = (1 - a) * inst->kalman_state.H_s.vector.doppler + a * smoothTraj[inst->uid].doppler;*/

	//	smoothTraj[inst->uid].y = (1 - a) * inst->kalman_state.S_hat[1] + a * smoothTraj[inst->uid].y;
	//	smoothTraj[inst->uid].z = (1 - a) * inst->kalman_state.S_hat[2] + a * smoothTraj[inst->uid].z;
	//	smoothTraj[inst->uid].x = inst->kalman_state.S_hat[0];
	//	smoothTraj[inst->uid].vx = inst->kalman_state.S_hat[3];
	//	smoothTraj[inst->uid].vy = inst->kalman_state.S_hat[4];
	//	smoothTraj[inst->uid].vz = inst->kalman_state.S_hat[5];
	//	smoothTraj[inst->uid].range = inst->kalman_state.H_s.vector.range;
	//	smoothTraj[inst->uid].azimuthRad = inst->kalman_state.H_s.vector.azimuthRad;
	//	smoothTraj[inst->uid].pitchRad = inst->kalman_state.H_s.vector.pitchRad;
	//	smoothTraj[inst->uid].doppler = inst->kalman_state.H_s.vector.doppler;
	//}
	//else if (inst->objManagement.lifetime == 0)
	//{
	//	smoothTraj[inst->uid].x = inst->kalman_state.S_hat[0];
	//	smoothTraj[inst->uid].y = inst->kalman_state.S_hat[1];
	//	smoothTraj[inst->uid].z = inst->kalman_state.S_hat[2];
	//	smoothTraj[inst->uid].vx = inst->kalman_state.S_hat[3];
	//	smoothTraj[inst->uid].vy = inst->kalman_state.S_hat[4];
	//	smoothTraj[inst->uid].vz = inst->kalman_state.S_hat[5];
	//	smoothTraj[inst->uid].range = inst->kalman_state.H_s.vector.range;
	//	smoothTraj[inst->uid].azimuthRad = inst->kalman_state.H_s.vector.azimuthRad;
	//	smoothTraj[inst->uid].pitchRad = inst->kalman_state.H_s.vector.pitchRad;
	//	smoothTraj[inst->uid].doppler = inst->kalman_state.H_s.vector.doppler;
	//}
	//smoothTraj[inst->uid].array[0] = smoothTraj[inst->uid].x;
	//smoothTraj[inst->uid].array[1] = smoothTraj[inst->uid].y;
	//smoothTraj[inst->uid].array[2] = smoothTraj[inst->uid].z;
	//smoothTraj[inst->uid].array[3] = smoothTraj[inst->uid].vx;
	//smoothTraj[inst->uid].array[4] = smoothTraj[inst->uid].vy;
	//smoothTraj[inst->uid].array[5] = smoothTraj[inst->uid].vz;
	//smoothTraj[inst->uid].array[6] = smoothTraj[inst->uid].range;
	//smoothTraj[inst->uid].array[7] = smoothTraj[inst->uid].azimuthRad;
	//smoothTraj[inst->uid].array[8] = smoothTraj[inst->uid].pitchRad;
	//smoothTraj[inst->uid].array[9] = smoothTraj[inst->uid].doppler;
	if (a == 1)
	{
		if (inst->objManagement.state == TRACK_STATE_FREE)
		{
			smoothTraj[inst->uid].x = 0.f;
			smoothTraj[inst->uid].y = 0.f;
			smoothTraj[inst->uid].z = 0.f;
			smoothTraj[inst->uid].vx = 0.f;
			smoothTraj[inst->uid].vy = 0.f;
			smoothTraj[inst->uid].vz = 0.f;
			smoothTraj[inst->uid].range = 0.f;
			smoothTraj[inst->uid].azimuthRad = 0.f;
			smoothTraj[inst->uid].pitchRad = 0.f;
			smoothTraj[inst->uid].doppler = 0.f;

			smoothTraj_previous_4[inst->uid].x = 0.f;
			smoothTraj_previous_4[inst->uid].y = 0.f;
			smoothTraj_previous_4[inst->uid].z = 0.f;
			smoothTraj_previous_4[inst->uid].vx = 0.f;
			smoothTraj_previous_4[inst->uid].vy = 0.f;
			smoothTraj_previous_4[inst->uid].vz = 0.f;
			smoothTraj_previous_4[inst->uid].range = 0.f;
			smoothTraj_previous_4[inst->uid].azimuthRad = 0.f;
			smoothTraj_previous_4[inst->uid].pitchRad = 0.f;
			smoothTraj_previous_4[inst->uid].doppler = 0.f;

			smoothTraj_previous_3[inst->uid].x = 0.f;
			smoothTraj_previous_3[inst->uid].y = 0.f;
			smoothTraj_previous_3[inst->uid].z = 0.f;
			smoothTraj_previous_3[inst->uid].vx = 0.f;
			smoothTraj_previous_3[inst->uid].vy = 0.f;
			smoothTraj_previous_3[inst->uid].vz = 0.f;
			smoothTraj_previous_3[inst->uid].range = 0.f;
			smoothTraj_previous_3[inst->uid].pitchRad = 0.f;
			smoothTraj_previous_3[inst->uid].doppler = 0.f;

			smoothTraj_previous_2[inst->uid].x = 0.f;
			smoothTraj_previous_2[inst->uid].y = 0.f;
			smoothTraj_previous_2[inst->uid].z = 0.f;
			smoothTraj_previous_2[inst->uid].vx = 0.f;
			smoothTraj_previous_2[inst->uid].vy = 0.f;
			smoothTraj_previous_2[inst->uid].vz = 0.f;
			smoothTraj_previous_2[inst->uid].range = 0.f;
			smoothTraj_previous_2[inst->uid].pitchRad = 0.f;
			smoothTraj_previous_2[inst->uid].doppler = 0.f;

			smoothTraj_previous_1[inst->uid].x = 0.f;
			smoothTraj_previous_1[inst->uid].y = 0.f;
			smoothTraj_previous_1[inst->uid].z = 0.f;
			smoothTraj_previous_1[inst->uid].vx = 0.f;
			smoothTraj_previous_1[inst->uid].vy = 0.f;
			smoothTraj_previous_1[inst->uid].vz = 0.f;
			smoothTraj_previous_1[inst->uid].range = 0.f;
			smoothTraj_previous_1[inst->uid].pitchRad = 0.f;
			smoothTraj_previous_1[inst->uid].doppler = 0.f;
		}
		else if (inst->objManagement.lifetime > 3)
		{

			smoothTraj[inst->uid].y = 0.5 * inst->kalman_state.S_hat[1] + 0.2 * smoothTraj_previous_1[inst->uid].y + 0.1 * smoothTraj_previous_2[inst->uid].y + 0.1 * smoothTraj_previous_3[inst->uid].y + 0.1 * smoothTraj_previous_4[inst->uid].y;
			smoothTraj[inst->uid].z = 0.5 * inst->kalman_state.S_hat[2] + 0.2 * smoothTraj_previous_1[inst->uid].z + 0.1 * smoothTraj_previous_2[inst->uid].z + 0.1 * smoothTraj_previous_3[inst->uid].z + 0.1 * smoothTraj_previous_4[inst->uid].z;
			smoothTraj[inst->uid].x = inst->kalman_state.S_hat[0];
			smoothTraj[inst->uid].vx = inst->kalman_state.S_hat[3];
			smoothTraj[inst->uid].vy = inst->kalman_state.S_hat[4];
			smoothTraj[inst->uid].vz = inst->kalman_state.S_hat[5];
			smoothTraj[inst->uid].range = inst->kalman_state.H_s.vector.range;
			smoothTraj[inst->uid].azimuthRad = inst->kalman_state.H_s.vector.azimuthRad;
			smoothTraj[inst->uid].pitchRad = inst->kalman_state.H_s.vector.pitchRad;
			smoothTraj[inst->uid].doppler = inst->kalman_state.H_s.vector.doppler;

			smoothTraj_previous_4[inst->uid].x = smoothTraj_previous_3[inst->uid].x;
			smoothTraj_previous_4[inst->uid].y = smoothTraj_previous_3[inst->uid].y;
			smoothTraj_previous_4[inst->uid].z = smoothTraj_previous_3[inst->uid].z;
			smoothTraj_previous_4[inst->uid].vx = smoothTraj_previous_3[inst->uid].vx;
			smoothTraj_previous_4[inst->uid].vy = smoothTraj_previous_3[inst->uid].vy;
			smoothTraj_previous_4[inst->uid].vz = smoothTraj_previous_3[inst->uid].vz;
			smoothTraj_previous_4[inst->uid].range = smoothTraj_previous_3[inst->uid].range;
			smoothTraj_previous_4[inst->uid].azimuthRad = smoothTraj_previous_3[inst->uid].azimuthRad;
			smoothTraj_previous_4[inst->uid].pitchRad = smoothTraj_previous_3[inst->uid].pitchRad;
			smoothTraj_previous_4[inst->uid].doppler = smoothTraj_previous_3[inst->uid].doppler;

			smoothTraj_previous_3[inst->uid].x = smoothTraj_previous_2[inst->uid].x;
			smoothTraj_previous_3[inst->uid].y = smoothTraj_previous_2[inst->uid].y;
			smoothTraj_previous_3[inst->uid].z = smoothTraj_previous_2[inst->uid].z;
			smoothTraj_previous_3[inst->uid].vx = smoothTraj_previous_2[inst->uid].vx;
			smoothTraj_previous_3[inst->uid].vy = smoothTraj_previous_2[inst->uid].vy;
			smoothTraj_previous_3[inst->uid].vz = smoothTraj_previous_2[inst->uid].vz;
			smoothTraj_previous_3[inst->uid].range = smoothTraj_previous_2[inst->uid].range;
			smoothTraj_previous_3[inst->uid].azimuthRad = smoothTraj_previous_2[inst->uid].azimuthRad;
			smoothTraj_previous_3[inst->uid].pitchRad = smoothTraj_previous_2[inst->uid].pitchRad;
			smoothTraj_previous_3[inst->uid].doppler = smoothTraj_previous_2[inst->uid].doppler;

			smoothTraj_previous_2[inst->uid].x = smoothTraj_previous_1[inst->uid].x;
			smoothTraj_previous_2[inst->uid].y = smoothTraj_previous_1[inst->uid].y;
			smoothTraj_previous_2[inst->uid].z = smoothTraj_previous_1[inst->uid].z;
			smoothTraj_previous_2[inst->uid].vx = smoothTraj_previous_1[inst->uid].vx;
			smoothTraj_previous_2[inst->uid].vy = smoothTraj_previous_1[inst->uid].vy;
			smoothTraj_previous_2[inst->uid].vz = smoothTraj_previous_1[inst->uid].vz;
			smoothTraj_previous_2[inst->uid].range = smoothTraj_previous_1[inst->uid].range;
			smoothTraj_previous_2[inst->uid].azimuthRad = smoothTraj_previous_1[inst->uid].azimuthRad;
			smoothTraj_previous_2[inst->uid].pitchRad = smoothTraj_previous_1[inst->uid].pitchRad;
			smoothTraj_previous_2[inst->uid].doppler = smoothTraj_previous_1[inst->uid].doppler;

			smoothTraj_previous_1[inst->uid].x = inst->kalman_state.S_hat[0];
			smoothTraj_previous_1[inst->uid].y = inst->kalman_state.S_hat[1];
			smoothTraj_previous_1[inst->uid].z = inst->kalman_state.S_hat[2];
			smoothTraj_previous_1[inst->uid].vx = inst->kalman_state.S_hat[3];
			smoothTraj_previous_1[inst->uid].vy = inst->kalman_state.S_hat[4];
			smoothTraj_previous_1[inst->uid].vz = inst->kalman_state.S_hat[5];
			smoothTraj_previous_1[inst->uid].range = inst->kalman_state.H_s.vector.range;
			smoothTraj_previous_1[inst->uid].azimuthRad = inst->kalman_state.H_s.vector.azimuthRad;
			smoothTraj_previous_1[inst->uid].pitchRad = inst->kalman_state.H_s.vector.pitchRad;
			smoothTraj_previous_1[inst->uid].doppler = inst->kalman_state.H_s.vector.doppler;
		}
		else if (inst->objManagement.lifetime == 0)
		{
			smoothTraj[inst->uid].x = inst->kalman_state.S_hat[0];
			smoothTraj[inst->uid].y = inst->kalman_state.S_hat[1];
			smoothTraj[inst->uid].z = inst->kalman_state.S_hat[2];
			smoothTraj[inst->uid].vx = inst->kalman_state.S_hat[3];
			smoothTraj[inst->uid].vy = inst->kalman_state.S_hat[4];
			smoothTraj[inst->uid].vz = inst->kalman_state.S_hat[5];
			smoothTraj[inst->uid].range = inst->kalman_state.H_s.vector.range;
			smoothTraj[inst->uid].azimuthRad = inst->kalman_state.H_s.vector.azimuthRad;
			smoothTraj[inst->uid].pitchRad = inst->kalman_state.H_s.vector.pitchRad;
			smoothTraj[inst->uid].doppler = inst->kalman_state.H_s.vector.doppler;

			smoothTraj_previous_4[inst->uid].x = inst->kalman_state.S_hat[0];
			smoothTraj_previous_4[inst->uid].y = inst->kalman_state.S_hat[1];
			smoothTraj_previous_4[inst->uid].z = inst->kalman_state.S_hat[2];
			smoothTraj_previous_4[inst->uid].vx = inst->kalman_state.S_hat[3];
			smoothTraj_previous_4[inst->uid].vy = inst->kalman_state.S_hat[4];
			smoothTraj_previous_4[inst->uid].vz = inst->kalman_state.S_hat[5];
			smoothTraj_previous_4[inst->uid].range = inst->kalman_state.H_s.vector.range;
			smoothTraj_previous_4[inst->uid].azimuthRad = inst->kalman_state.H_s.vector.azimuthRad;
			smoothTraj_previous_4[inst->uid].pitchRad = inst->kalman_state.H_s.vector.pitchRad;
			smoothTraj_previous_4[inst->uid].doppler = inst->kalman_state.H_s.vector.doppler;
		}

		else if (inst->objManagement.lifetime == 1)
		{
			smoothTraj[inst->uid].x = inst->kalman_state.S_hat[0];
			smoothTraj[inst->uid].y = inst->kalman_state.S_hat[1];
			smoothTraj[inst->uid].z = inst->kalman_state.S_hat[2];
			smoothTraj[inst->uid].vx = inst->kalman_state.S_hat[3];
			smoothTraj[inst->uid].vy = inst->kalman_state.S_hat[4];
			smoothTraj[inst->uid].vz = inst->kalman_state.S_hat[5];
			smoothTraj[inst->uid].range = inst->kalman_state.H_s.vector.range;
			smoothTraj[inst->uid].azimuthRad = inst->kalman_state.H_s.vector.azimuthRad;
			smoothTraj[inst->uid].pitchRad = inst->kalman_state.H_s.vector.pitchRad;
			smoothTraj[inst->uid].doppler = inst->kalman_state.H_s.vector.doppler;

			smoothTraj_previous_3[inst->uid].x = inst->kalman_state.S_hat[0];
			smoothTraj_previous_3[inst->uid].y = inst->kalman_state.S_hat[1];
			smoothTraj_previous_3[inst->uid].z = inst->kalman_state.S_hat[2];
			smoothTraj_previous_3[inst->uid].vx = inst->kalman_state.S_hat[3];
			smoothTraj_previous_3[inst->uid].vy = inst->kalman_state.S_hat[4];
			smoothTraj_previous_3[inst->uid].vz = inst->kalman_state.S_hat[5];
			smoothTraj_previous_3[inst->uid].range = inst->kalman_state.H_s.vector.range;
			smoothTraj_previous_3[inst->uid].azimuthRad = inst->kalman_state.H_s.vector.azimuthRad;
			smoothTraj_previous_3[inst->uid].pitchRad = inst->kalman_state.H_s.vector.pitchRad;
			smoothTraj_previous_3[inst->uid].doppler = inst->kalman_state.H_s.vector.doppler;
		}

		else if (inst->objManagement.lifetime == 2)
		{
			smoothTraj[inst->uid].x = inst->kalman_state.S_hat[0];
			smoothTraj[inst->uid].y = inst->kalman_state.S_hat[1];
			smoothTraj[inst->uid].z = inst->kalman_state.S_hat[2];
			smoothTraj[inst->uid].vx = inst->kalman_state.S_hat[3];
			smoothTraj[inst->uid].vy = inst->kalman_state.S_hat[4];
			smoothTraj[inst->uid].vz = inst->kalman_state.S_hat[5];
			smoothTraj[inst->uid].range = inst->kalman_state.H_s.vector.range;
			smoothTraj[inst->uid].azimuthRad = inst->kalman_state.H_s.vector.azimuthRad;
			smoothTraj[inst->uid].pitchRad = inst->kalman_state.H_s.vector.pitchRad;
			smoothTraj[inst->uid].doppler = inst->kalman_state.H_s.vector.doppler;

			smoothTraj_previous_2[inst->uid].x = inst->kalman_state.S_hat[0];
			smoothTraj_previous_2[inst->uid].y = inst->kalman_state.S_hat[1];
			smoothTraj_previous_2[inst->uid].z = inst->kalman_state.S_hat[2];
			smoothTraj_previous_2[inst->uid].vx = inst->kalman_state.S_hat[3];
			smoothTraj_previous_2[inst->uid].vy = inst->kalman_state.S_hat[4];
			smoothTraj_previous_2[inst->uid].vz = inst->kalman_state.S_hat[5];
			smoothTraj_previous_2[inst->uid].range = inst->kalman_state.H_s.vector.range;
			smoothTraj_previous_2[inst->uid].azimuthRad = inst->kalman_state.H_s.vector.azimuthRad;
			smoothTraj_previous_2[inst->uid].pitchRad = inst->kalman_state.H_s.vector.pitchRad;
			smoothTraj_previous_2[inst->uid].doppler = inst->kalman_state.H_s.vector.doppler;
		}

		else if (inst->objManagement.lifetime == 3)
		{
			smoothTraj[inst->uid].x = inst->kalman_state.S_hat[0];
			smoothTraj[inst->uid].y = inst->kalman_state.S_hat[1];
			smoothTraj[inst->uid].z = inst->kalman_state.S_hat[2];
			smoothTraj[inst->uid].vx = inst->kalman_state.S_hat[3];
			smoothTraj[inst->uid].vy = inst->kalman_state.S_hat[4];
			smoothTraj[inst->uid].vz = inst->kalman_state.S_hat[5];
			smoothTraj[inst->uid].range = inst->kalman_state.H_s.vector.range;
			smoothTraj[inst->uid].azimuthRad = inst->kalman_state.H_s.vector.azimuthRad;
			smoothTraj[inst->uid].pitchRad = inst->kalman_state.H_s.vector.pitchRad;
			smoothTraj[inst->uid].doppler = inst->kalman_state.H_s.vector.doppler;

			smoothTraj_previous_1[inst->uid].x = inst->kalman_state.S_hat[0];
			smoothTraj_previous_1[inst->uid].y = inst->kalman_state.S_hat[1];
			smoothTraj_previous_1[inst->uid].z = inst->kalman_state.S_hat[2];
			smoothTraj_previous_1[inst->uid].vx = inst->kalman_state.S_hat[3];
			smoothTraj_previous_1[inst->uid].vy = inst->kalman_state.S_hat[4];
			smoothTraj_previous_1[inst->uid].vz = inst->kalman_state.S_hat[5];
			smoothTraj_previous_1[inst->uid].range = inst->kalman_state.H_s.vector.range;
			smoothTraj_previous_1[inst->uid].azimuthRad = inst->kalman_state.H_s.vector.azimuthRad;
			smoothTraj_previous_1[inst->uid].pitchRad = inst->kalman_state.H_s.vector.pitchRad;
			smoothTraj_previous_1[inst->uid].doppler = inst->kalman_state.H_s.vector.doppler;
		}
	}
	else
	{
	smoothTraj[inst->uid].x = inst->kalman_state.S_hat[0];
	smoothTraj[inst->uid].y = inst->kalman_state.S_hat[1];
	smoothTraj[inst->uid].z = inst->kalman_state.S_hat[2];
	smoothTraj[inst->uid].vx = inst->kalman_state.S_hat[3];
	smoothTraj[inst->uid].vy = inst->kalman_state.S_hat[4];
	smoothTraj[inst->uid].vz = inst->kalman_state.S_hat[5];
	smoothTraj[inst->uid].range = inst->kalman_state.H_s.vector.range;
	smoothTraj[inst->uid].azimuthRad = inst->kalman_state.H_s.vector.azimuthRad;
	smoothTraj[inst->uid].pitchRad = inst->kalman_state.H_s.vector.pitchRad;
	smoothTraj[inst->uid].doppler = inst->kalman_state.H_s.vector.doppler;
	}
	smoothTraj[inst->uid].array[0] = smoothTraj[inst->uid].x;
	smoothTraj[inst->uid].array[1] = smoothTraj[inst->uid].y;
	smoothTraj[inst->uid].array[2] = smoothTraj[inst->uid].z;
	smoothTraj[inst->uid].array[3] = smoothTraj[inst->uid].vx;
	smoothTraj[inst->uid].array[4] = smoothTraj[inst->uid].vy;
	smoothTraj[inst->uid].array[5] = smoothTraj[inst->uid].vz;
	smoothTraj[inst->uid].array[6] = smoothTraj[inst->uid].range;
	smoothTraj[inst->uid].array[7] = smoothTraj[inst->uid].azimuthRad;
	smoothTraj[inst->uid].array[8] = smoothTraj[inst->uid].pitchRad;
	smoothTraj[inst->uid].array[9] = smoothTraj[inst->uid].doppler;
	//trajRestrict(handle, 0.8, algObj);
}

void trajRestrict(void* handle, float a, sAlgObjData* algObj)
{
	sTracking_objectUnit* inst = (sTracking_objectUnit*)handle;
	sDispatchInst* dispatch_inst = (sDispatchInst*)algObj->dispatch_inst;
	sTrackingInst* tracking_inst = (sTrackingInst*)(algObj->tracking_inst);

	float lastMean_x = inst->objKinematic.x_mean300;
	float lastMean_y = inst->objKinematic.y_mean300;
	float lastMean_z = inst->objKinematic.z_mean300;
	float newMean_x = 0.f;
	float newMean_y = 0.f;
	float newMean_z = 0.f;

	if (inst->objKinematic.restrict800 == 1)
	{
		if (inst->assoProperty.assoCondenceMeasId != -1 && inst->kalman_state.H_s.array[0] < 400)
		{
			inst->objKinematic.count300 += 1;
			newMean_x = lastMean_x + (inst->kalman_state.S_hat[0] - lastMean_x) / inst->objKinematic.count300;
			newMean_y = lastMean_y + (inst->kalman_state.S_hat[1] - lastMean_y) / inst->objKinematic.count300;
			newMean_z = lastMean_z + (inst->kalman_state.S_hat[2] - lastMean_z) / inst->objKinematic.count300;

			inst->objKinematic.x_mean300 = newMean_x;
			inst->objKinematic.y_mean300 = newMean_y;
			inst->objKinematic.z_mean300 = newMean_z;
		}


		if (inst->kalman_state.H_s.array[0] >= 400)
		{
			smoothTraj[inst->uid].y = (1 - a) * smoothTraj[inst->uid].y + a * (inst->objKinematic.y_mean300 / inst->objKinematic.x_mean300) * smoothTraj[inst->uid].x;
			smoothTraj[inst->uid].z = (1 - a) * smoothTraj[inst->uid].z + a * (inst->objKinematic.z_mean300 / inst->objKinematic.x_mean300) * smoothTraj[inst->uid].x;
			/*smoothTraj[inst->uid].x = inst->kalman_state.S_hat[0];
			smoothTraj[inst->uid].vx = inst->kalman_state.S_hat[3];
			smoothTraj[inst->uid].vy = inst->kalman_state.S_hat[4];
			smoothTraj[inst->uid].vz = inst->kalman_state.S_hat[5];
			smoothTraj[inst->uid].range = inst->kalman_state.H_s.vector.range;
			smoothTraj[inst->uid].azimuthRad = inst->kalman_state.H_s.vector.azimuthRad;
			smoothTraj[inst->uid].pitchRad = inst->kalman_state.H_s.vector.pitchRad;
			smoothTraj[inst->uid].doppler = inst->kalman_state.H_s.vector.doppler;*/

			smoothTraj[inst->uid].array[1] = smoothTraj[inst->uid].y;
			smoothTraj[inst->uid].array[2] = smoothTraj[inst->uid].z;
			smoothTraj[inst->uid].range = sqrtf(smoothTraj[inst->uid].x * smoothTraj[inst->uid].x + smoothTraj[inst->uid].y * smoothTraj[inst->uid].y + smoothTraj[inst->uid].z * smoothTraj[inst->uid].z);
		}
	}
}
