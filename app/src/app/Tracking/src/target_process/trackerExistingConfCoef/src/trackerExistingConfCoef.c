
#include "../include/trackerExistingConfCoef.h"
#include "../../tracking/include/tracking.h"
#include "../../dispatch/include/dispatch.h"

/* Initialize the Possion distribution for the probability of cluster and
   trajectory stability of lifetime.
   Initialize the Gaussian distribution for the trajectory stability of motion.

   Input: detLamda: The parameter of Possion distribution for detection
   Input: lifeLamda: The parameter of Possion distribution for trajectory stability of lifetime
*/

float CalcProb_init(int clusNum, int ObjNumMax)
{
	float prob = 0.0f;
	//if (clusNum >= 0 && clusNum < 255)
	//	clusNum = 1;
	//else
	//	clusNum = 0;
	if (clusNum >= ObjNumMax)
	{
		prob = 1.0f;//clusNum>=ObjNumMax：1
	}
	else
	{
		//prob = ((float)clusNum) / ((float)ObjNumMax);//clusNum<ObjNumMax：clusNum/ObjNumMax
		prob = 0.f;
	}
	return prob;
}


int trackerExisting_init(sAlgObjData* algObj)
{
	sTrackingTarget* trajList = NULL;
	sTrackerExistingInst* trackerExisting_inst = NULL;
	int i = 0;
	/*float accSum = 0.0;
	sTrackerExisting_probMotion probMotion_v[TRACKEREXISTING_MAXPROBMOTIONNUM] = {
		{0.318f, 0.25f},
		{0.674f, 0.50f},
		{0.994f, 0.68f},
		{1.000f, 0.68f},
		{1.281f, 0.80f},
		{1.644f, 0.90f},
		{1.959f, 0.95f},
		{2.000f, 0.95f},
		{2.575f, 0.99f},
		{3.000f, 0.99f}
	};*/

	////by hxj
	//sTrackerExisting_probMotion probMotion_or_v[TRACKEREXISTING_OR_MAXPROBMOTIONNUM] = {
	//	{0.11f, 0.99f},
	//	{0.22f, 0.97f},
	//	{0.35f, 0.95f},
	//	{0.58f, 0.9f},
	//	{1.21f, 0.75f},
	//	{2.37f, 0.50f},
	//	{4.11f, 0.25f},
	//	{6.25f, 0.10f},
	//	{7.81f, 0.05f},
	//	{9.35f, 0.025f},
	//	{11.34f, 0.01f},
	//	{12.84f, 0.005f}
	//};

	//by zzc
	sTrackerExisting_probMotion probMotion_or_v[TRACKEREXISTING_OR_MAXPROBMOTIONNUM] = {
		{0.297f, 0.99f},
		{0.484f, 0.975f},
		{0.711f, 0.95f},
		{1.064f, 0.90f},
		{1.923f, 0.75f},
		{3.357f, 0.50f},
		{5.39f, 0.25f},
		{7.78f, 0.10f},
		{9.49f, 0.05f},
		{11.15f, 0.025f},
	};
	sTrackingInst* tracking_inst = (sTrackingInst*)(algObj->tracking_inst);

	//trackerExisting_inst = (sTrackerExistingInst*)(algObj->trackerExisting_inst);
	trackerExisting_inst = tracking_inst->scratchPadData->moduleInst.trackerExisting_inst;
	if (trackerExisting_inst == NULL) {
		return -1;
	}
	if (trackerExisting_inst->clusLamda == 0 || trackerExisting_inst->lifeLamda == 0) {
		return -1;
	}

	trajList = tracking_inst->trajInfoOutput->trajList;
	for (i = 0; i < MAX_NUM_TRAJS; i++)
	{
		trajList[i].probOfExisting = 0;
		//trajList[i].trajProperty.variance[7] = 0;//by hxj
	}
	/*trackerExisting_inst->probClus_v[0].n = 0;
	trackerExisting_inst->probClus_v[0].prob = 0.0;
	for (i = 0; i < TRACKEREXISTING_MAXPROBCLUSNUM; i++)
	{
		trackerExisting_inst->probClus_v[i].n = i;
		trackerExisting_inst->probClus_v[i].prob = myMath_poissonProb(trackerExisting_inst->clusLamda, i);

		accSum = accSum + trackerExisting_inst->probClus_v[i].prob;
		trackerExisting_inst->probClus_v[i].prob = accSum;
	}
	trackerExisting_inst->probLife_v[0].n = 0;
	trackerExisting_inst->probLife_v[0].prob = 0.0;
	accSum = 0.0;
	for (i = 0; i < TRACKEREXISTING_MAXPROBLIFENUM; i++)
	{
		trackerExisting_inst->probLife_v[i].n = i;
		trackerExisting_inst->probLife_v[i].prob = myMath_poissonProb(trackerExisting_inst->lifeLamda, i);
		accSum = accSum + trackerExisting_inst->probLife_v[i].prob;
		trackerExisting_inst->probLife_v[i].prob = accSum;
	}

	memcpy(trackerExisting_inst->probMotion_v, probMotion_v, sizeof(sTrackerExisting_probMotion)*TRACKEREXISTING_MAXPROBMOTIONNUM);*/
	memcpy(trackerExisting_inst->probMotion_or_v, probMotion_or_v, sizeof(sTrackerExisting_probMotion) * TRACKEREXISTING_OR_MAXPROBMOTIONNUM);
	return 0;
}

/* Calculate the confidence coefficient of current trajectory
	prob_trajExist = w1*prob_clus*prob_motion + w2*prob_lifeTime
*/
void trackerExisting_process(sAlgObjData* algObj)
{
	//sTrackerExistingInst *inst = (sTrackerExistingInst*)algObj->trackerExisting_inst;
	sTrackingInst* tracking_inst = (sTrackingInst*)(algObj->tracking_inst);
	sTrackerExistingInst* inst = tracking_inst->scratchPadData->moduleInst.trackerExisting_inst;
	sTrackingTarget* trajList = tracking_inst->trajInfoOutput->trajList;
	//uint16_t numTrajs = tracking_inst->trajInfoOutput->trackObjNum;
	sTracking_moduleInstance* tracking_module_inst = (sTracking_moduleInstance*)(tracking_inst->handle);
	uint16_t numTrajs = tracking_module_inst->twsTrackList.count + tracking_module_inst->tasTrackList.count;
	sDispatchInst* dispatchInst = (sDispatchInst*)algObj->dispatch_inst;
	sDispatchOutput* dispatchOutput = (sDispatchOutput*)dispatchInst->dispatchOutput;


	////float invXYZQFormatCoef = 0.f;
	////float oneQFormatCoef = 0.f;
	//float prob_clus = 0.0f;
	//float prob_lifeTime = 0.0f;
	//float prob_motion = 1.0f;
	//int clusNum = 0;
	//int lifeTime = 0;
	//float motion_devByVar[3] = { 0.f };
	//int loop = 2;
	//float prob_motion_v[3] = { 0 };
	//int j = 0;
	//int m = 1;


	int i = 0;
	int m = 1;
	int ii, jj, sortBoarder, sortHappen, lastExchangeIdx = 0;
	int tmp[8] = { 0 };
	int MissFrameNum = 0;
	int clusNum = 0;
	int objLife = 0;//tracker alive time
	int maxFrame = 0;//the max frames Number considered of update status
	int past8frame = 0;
	float weight = 0.0f;
	float mdScore = 0.0f;
	float mdScoreSQ = 0.0f;
	float prob_clus = 0.0f;
	float prob_lifeTime = 0.0f;
	float prob_motion_or_v = 0.0f;
	float prob_motion = 0.0f;
	float X = 0.0f;
	float Vx = 0.0f;
	float probOfExist = 0U;
	float probOfExistNew = 0U;
	sTracking_ListElem* tElem = NULL;
	uint16_t uid;
	sTracking_objectUnit* traj;
	float temp = 0.f;
	//int alive_last = 0;
#ifdef DEBUG_LOG_TRACKER_EXISTING
	FILE* fp1;
#endif


	//#ifdef MMWAVE_3D
	//	loop = 3;
	//#endif
#ifdef DEBUG_LOG_TRACKER_EXISTING
	if (tracking_module_inst->wholeSpaceScanCycleCnt - tracking_module_inst->lastWholeSpaceScanCycleCnt != 0)
	{
		fopen_s(&fp1, "E:/record/20230512/record1.txt", "a+");
		fprintf(fp1, "\nstart of cycle %u ------------------------------------------------------------------------\n\n", tracking_module_inst->wholeSpaceScanCycleCnt);
		fclose(fp1);
	}
#endif
	
	// tas目标mdScore和lifetime更新
	if (g_scanType == TAS_SCAN)
	{
		tElem = tracking_listGetFirst(&tracking_module_inst->tasTrackList);
		while (tElem != 0)
		{
			uid = tElem->data;
			traj = (sTracking_objectUnit*)(tracking_module_inst->hTrack[uid]);
			/*if (traj->objManagement.lifetime - traj->objManagement.lifetime_old != 0 && traj->uid == tracking_module_inst->tasTargetId && traj->assoProperty.assoCondenceMeasId != -1)
			{
				traj->curTarget.mdScore = 100.f;
			}*/
			traj->objManagement.updateProbExisting = 0;
			if (uid == tracking_module_inst->tasTargetId)
			{
				if (dispatchOutput->tasAndInitTrackFinishBeamFlag == 1 && traj->assoProperty.assoCondenceMeasId == -1)
				{
					traj->curTarget.mdScore = 100.f;
				}
				updatePast8frame(tracking_module_inst->hTrack[uid], tracking_module_inst, 1); // 更新past8frame
				traj->objManagement.lifetime_old = traj->objManagement.lifetime;
			}
			tElem = tracking_listGetNext(tElem);
		}
	}
	
	// tws目标mdScore和8frame更新
	else if (g_scanType == TWS_SCAN)
	{
		tElem = tracking_listGetFirst(&tracking_module_inst->twsTrackList);
		while (tElem != 0)
		{
			uid = tElem->data;
			traj = (sTracking_objectUnit*)(tracking_module_inst->hTrack[uid]);
			traj->objManagement.updateProbExisting = 0;
			if (tracking_module_inst->wholeSpaceScanCycleFinalBeamFlag != 0 && \
				traj->assoProperty.assoThisScan == 0 && traj->assoProperty.assoCondenceMeasId == -1) // 如果上一轮scan没有被关联，且当前scan第一帧也未被关联，mdscore重置为100
			{
				traj->curTarget.mdScore = 100.f;
			}
			updatePast8frame(tracking_module_inst->hTrack[uid], tracking_module_inst, 0); // 更新past8frame
			traj->objManagement.lifetime_old = traj->objManagement.lifetime;//准备转为tas目标时用
			tElem = tracking_listGetNext(tElem);
		}
	}

	//tElem = tracking_listGetFirst(&tracking_module_inst->tasTrackList);
	//while (tElem != 0)
	//{
	//	uid = tElem->data;
	//	updatePast8frame(tracking_module_inst->hTrack[uid], tracking_module_inst, 0); // 更新past8frame
	//	tElem = tracking_listGetNext(tElem);
	//}

	tElem = tracking_listGetFirst(&tracking_module_inst->tasTrackList); // tas目标置信度计算
	i = 0;
#ifdef DEBUG_LOG_TRACKER_EXISTING
	fopen_s(&fp1, "E:/record/20230512/record1.txt", "a+");
#endif

	while (tElem != 0)
	{
		uid = tElem->data;
		/*  @brief Another method to calculate probOfExisting ,by hxj */
		traj = (sTracking_objectUnit*)(tracking_module_inst->hTrack[uid]);
		mdScore = 0.0f;
		mdScoreSQ = 0.0f;
		prob_motion_or_v = 0.0f;
		prob_lifeTime = 0.0f;
		MissFrameNum = 0;
		weight = 0.0f;
		objLife = 0;

		clusNum = traj->assoProperty.assoThisScan;
		//alive_last = (int)(trajList[i].variance[2] * INV_ONE6FORMAT_SCALE);

		/* 1 part : target clustering reliability */
		//if (clusNum >= TRACKEREXISTING_OR_MAXPROBCLUSNUM)
		//{
		//	prob_clus = 1.0f;//clusNum>=10：1
		//}
		//else 
		//{
		//	prob_clus = ((float)clusNum) / ((float)TRACKEREXISTING_OR_MAXPROBCLUSNUM);//clusNum<10：clusNum/10
		//}	

		//if (X < -9.0f)//<-9
		//{
		//	if (fabsf(Vx) < 4.5f)//person or bicycle
		//	{
		//		prob_clus = CalcProb_init(clusNum, 1);
		//	}
		//	else//car
		//	{
		//		prob_clus = CalcProb_init(clusNum, 2);
		//	}
		//}
		//else if (X < -2.0f)//-9~-2
		//{
		//	if (fabsf(Vx) < 4.5f)//person or bicycle
		//	{
		//		prob_clus = CalcProb_init(clusNum, 2);
		//	}
		//	else//car
		//	{
		//		prob_clus = CalcProb_init(clusNum, 3);
		//	}
		//}
		//else if (X <= 7.0f)//-2~7
		//{
		//	if (fabsf(Vx) < 4.5f)//person or bicycle
		//	{
		//		prob_clus = CalcProb_init(clusNum, 3);
		//	}
		//	else//car
		//	{
		//		prob_clus = CalcProb_init(clusNum, 4);
		//	}
		//}
		//else //>7
		//{
		//	if (fabsf(Vx) < 4.5f)//person or bicycle
		//	{
		//		prob_clus = CalcProb_init(clusNum, 2);
		//	}
		//	else //car
		//	{
		//		prob_clus = CalcProb_init(clusNum, 3);
		//	}
		//}
		prob_clus = CalcProb_init(clusNum, 1);

		//tracking_inst->trajInfoOutput->trackTasNum != 0

		/* 2 part : tracker existing stability */
		//updatePast8frame =(int)(trajList[i].updatePast8frame); //TODO: Get update past 8 frames
		//past8frame = (int)(trajList[i].variance[0] * INV_ONE6FORMAT_SCALE);
		//objLife = (int)(trajList[i].alive);//* INV_ONE6FORMAT_SCALE
		past8frame = traj->objManagement.updatePast8frame;
		objLife = traj->objManagement.past8FrameBitNum;
		maxFrame = (objLife >= 8) ? 8 : objLife;
	/*	if (maxFrame < 8)
		{
			for (ii = 0; ii < 8; ii++)
			{
				tmp[ii] = 0;
			}
		}*/
		for (ii = 0; ii < 8; ii++)
		{
			tmp[ii] = 0;
		}
		for (ii = 0; ii < maxFrame; ii++)
		{
			tmp[ii] = (past8frame >> ii) & (0x01);
			if (0 == tmp[ii])
			{
				MissFrameNum++;
			}
		}
		for (ii = 0; ii < 8; ii++) //合理吗？如果一个航迹life为1，前7帧都按关联计算，635行初始化为0比较合理
		{
			if (0 == ii) //current frame
			{
				prob_lifeTime = prob_lifeTime + tmp[ii] * 0.2f;
			}
			else if ((1 == ii) || (2 == ii))//2nd or 3rd to last frame
			{
				prob_lifeTime = prob_lifeTime + tmp[ii] * 0.15f;
			}
			else
			{
				prob_lifeTime = prob_lifeTime + tmp[ii] * 0.1f;
			}
		}

		/* 3 part : tracker space motion stability */
		//mdScore = (float)(trajList[i].variance[1] * INV_ONE6FORMAT_SCALE); // TODO: Get md
		mdScore = traj->curTarget.mdScore;
		//mdScoreSQ = mdScore * mdScore;
		//mdScoreSQ = mdScore;//TODO:

		////compute the value of prob_motion
		//for (m = 1; m < TRACKEREXISTING_OR_MAXPROBMOTIONNUM; m++) 
		//{
		//	//if (mdScoreSQ < inst->probMotion_or_v[m].dev) 
		//	if (mdScore < inst->probMotion_or_v[m].dev)
		//	{
		//		prob_motion_or_v = inst->probMotion_or_v[m - 1].prob;
		//		break;
		//	}
		//}
		//if (TRACKEREXISTING_OR_MAXPROBMOTIONNUM == m)
		//{
		//	prob_motion_or_v = inst->probMotion_or_v[m - 1].prob;
		//}

		//compute the value of prob_motion
		for (m = 0; m < TRACKEREXISTING_OR_MAXPROBMOTIONNUM; m++)
		{
			//if (mdScoreSQ < inst->probMotion_or_v[m].dev) 
			if (mdScore < inst->probMotion_or_v[m].dev)
			{
				prob_motion_or_v = inst->probMotion_or_v[m].prob;
				break;
			}
		}
		if (TRACKEREXISTING_OR_MAXPROBMOTIONNUM == m)
		{
			prob_motion_or_v = 0;
		}

		//compute the weight of prob_motion
		if (clusNum != 0 || MissFrameNum == 0)
		{
			weight = 1.0f;
		}
		else //if (clusNum == 0) xxxx xxx0
		{
			if (MissFrameNum != 8)
			{
				if (tmp[1] != 0)//xxxx xx10
				{
					if (1 == MissFrameNum)//updatePast8frame=1111 1110
					{
						weight = 0.9f;
					}
					else if (2 == MissFrameNum)
					{
						weight = 0.85f;
					}
					else if (3 == MissFrameNum)
					{
						weight = 0.8f;
					}
					else if (4 == MissFrameNum)
					{
						weight = 0.75f;
					}
					else if (5 == MissFrameNum)
					{
						weight = 0.7f;
					}
					else if (6 == MissFrameNum)
					{
						weight = 0.65f;
					}
					else if (7 == MissFrameNum)//0000 0010
					{
						weight = 0.6f;
					}
				}
				if ((0 == tmp[1]) && (tmp[2] != 0)) //xxxx x100
				{
					if (2 == MissFrameNum)//1111 1100
					{
						weight = 0.7f;
					}
					else if (3 == MissFrameNum)
					{
						weight = 0.65f;
					}
					else if (4 == MissFrameNum)
					{
						weight = 0.6f;
					}
					else if (5 == MissFrameNum)
					{
						weight = 0.55f;
					}
					else if (6 == MissFrameNum)
					{
						weight = 0.5f;
					}
					else if (7 == MissFrameNum)//0000 0100
					{
						weight = 0.45f;
					}
				}
				if ((0 == tmp[1]) && (0 == tmp[2]) && (tmp[3] != 0)) //xxxx 1000
				{
					if (3 == MissFrameNum)//1111 1000
					{
						weight = 0.6f;
					}
					else if (4 == MissFrameNum)
					{
						weight = 0.55f;
					}
					else if (5 == MissFrameNum)
					{
						weight = 0.5f;
					}
					else if (6 == MissFrameNum)
					{
						weight = 0.45f;
					}
					else if (7 == MissFrameNum)//0000 1000
					{
						weight = 0.4f;
					}
				}
				if ((0 == tmp[1]) && (0 == tmp[2]) && (0 == tmp[3]) && (tmp[4] != 0)) //xxx1 0000
				{
					if (4 == MissFrameNum)//1111 0000
					{
						weight = 0.5f;
					}
					else if (5 == MissFrameNum)
					{
						weight = 0.4f;
					}
					else if (6 == MissFrameNum)
					{
						weight = 0.3f;
					}
					else if (7 == MissFrameNum)//0001 0000
					{
						weight = 0.2f;
					}
				}
				if ((0 == tmp[1]) && (0 == tmp[2]) && (0 == tmp[3]) && (0 == tmp[4]) && (MissFrameNum >= 5)) //xx10 0000 / x1x0 0000 / 1xx0 0000
				{
					weight = 0.1f;
				}
			}
			else //0000 0000
			{
				weight = 0.0f;
			}
		}
		prob_motion = weight * prob_motion_or_v;


		/* total : prob_clus*w1 + prob_motion*w2 + prob_lifeTime*w3 */
		//if (trajList[jj].stateType == STABLE_TRAJ)
		//{
		//	/*probOfExistLast = (float)(trajList[jj].probOfExisting * INV_ONE7FORMAT_SCALE);
		//	probOfExist = (TRACKEREXISTING_OR_W1 * prob_clus + TRACKEREXISTING_OR_W2 * prob_lifeTime + TRACKEREXISTING_OR_W3 * prob_motion);
		//	myMath_lowPassFilter(&probOfExist, probOfExistLast, 0.3f);
		//	trajList[jj].probOfExisting = (uint8_t)(probOfExist * ONE7FORMAT_SCALE);*/

		//	probOfExistLast = (float)(trajList[jj].probOfExisting * INV_ONE7FORMAT_SCALE);
		//	probOfExist = (TRACKEREXISTING_OR_W1 * prob_clus + TRACKEREXISTING_OR_W2 * prob_lifeTime + TRACKEREXISTING_OR_W3 * prob_motion);
		//	myMath_lowPassFilter(&probOfExist, probOfExistLast, 0.3f);
		//	trajList[jj].probOfExisting = (uint8_t)(probOfExist * ONE7FORMAT_SCALE);
		//}
		//else
		//{
		//	//trajList[jj].probOfExisting = (uint8_t)((TRACKEREXISTING_OR_W1 * prob_clus + TRACKEREXISTING_OR_W2 * prob_lifeTime + TRACKEREXISTING_OR_W3 * prob_motion) * ONE7FORMAT_SCALE);
		//	traj->objManagement.probOfExisting = (TRACKEREXISTING_OR_W1 * prob_clus + TRACKEREXISTING_OR_W2 * prob_lifeTime + TRACKEREXISTING_OR_W3 * prob_motion);
		//}
		probOfExistNew = (TRACKEREXISTING_OR_W1 * prob_clus + TRACKEREXISTING_OR_W2 * prob_lifeTime + TRACKEREXISTING_OR_W3 * prob_motion);
		if (traj->objManagement.updateProbExisting == 1)
		{
			myMath_lowPassFilter(&traj->objManagement.probOfExisting, probOfExistNew, 0.5f);
		}

		//inst->id_sorted[i] = trajList[i].id;
		inst->id_sorted[i] = traj->uid;
		inst->existingProb_ascent[i] = (TRACKEREXISTING_OR_W1 * prob_clus + TRACKEREXISTING_OR_W2 * prob_lifeTime + TRACKEREXISTING_OR_W3 * prob_motion);

		tElem = tracking_listGetNext(tElem);
		//jj++;
		i++;

		/*if (trajList[i].stateType == ACTIVE_TRAJ)
		{
			probOfExistLast = (float)(trajList[i].trajProperty.variance[7]);
			probOfExist = (uint16_t)((TRACKEREXISTING_OR_W1 * prob_clus + TRACKEREXISTING_OR_W2 * prob_lifeTime + TRACKEREXISTING_OR_W3 * prob_motion) * ONE6FORMAT_SCALE);
			myMath_lowPassFilter(&probOfExist, probOfExistLast, 0.3f);
			trajList[i].trajProperty.variance[7] = (uint16_t)(probOfExist);
		}
		else
		{
			trajList[i].trajProperty.variance[7] = (uint16_t)((TRACKEREXISTING_OR_W1 * prob_clus + TRACKEREXISTING_OR_W2 * prob_lifeTime + TRACKEREXISTING_OR_W3 * prob_motion) * ONE6FORMAT_SCALE);
		}*/

#ifdef DEBUG_LOG
		/*my_printf("3333333333333  ID: %d, prob_clus: %.3f,objLife: %d, maxFrame: %d, prob_lifeTime: %.3f, prob_motion: %.3f, mdScoreSQ: %.3f,  weight: %.3f, updatePast8frame: %d, probOfExistLast: %.3f, probOfExisting2: %.3f\n", \
			trajList[i].id, prob_clus, objLife, maxFrame, prob_lifeTime, prob_motion, mdScoreSQ, weight, updatePast8frame, probOfExistLast, trajList[i].probOfExisting* INV_ONE7FORMAT_SCALE);*/

			/*my_printf("3333333333333  ID: %d, prob_clus: %.3f,objLife: %d, maxFrame: %d, prob_lifeTime: %.3f, prob_motion: %.3f, mdScoreSQ: %.3f,  weight: %.3f, updatePast8frame: %d, probOfExistLast: %.3f, probOfExisting2: %.3f\n", \
				trajList[i].id, prob_clus, objLife, maxFrame, prob_lifeTime, prob_motion, mdScoreSQ, weight, updatePast8frame, probOfExistLast* INV_ONE6FORMAT_SCALE, trajList[i].trajProperty.variance[7] * INV_ONE6FORMAT_SCALE);*/
#endif

#ifdef DEBUG_LOG_TRACKER_EXISTING
				// 每一帧过后，为所有航迹计算置信度，并打印
		/*fprintf(fp1, "TWS frame:%d	id:%u	md:%.2f	prob_clus:%.2f	prob_lifeTime:%.2f	prob_motion:%.2f	probOfExist:%.2f	assonum:%d\n", \
			tracking_inst->frame_cnt, trajList[i].id, (float)(trajList[i].variance[1] * INV_ONE6FORMAT_SCALE), prob_clus, \
			prob_lifeTime, prob_motion, (float)(trajList[i].probOfExisting * INV_ONE7FORMAT_SCALE), clusNum);*/
		fprintf(fp1, "TAS frame:%d	id:%u	md:%.2f	prob_clus:%.2f	prob_lifeTime:%.2f	prob_motion:%.2f	probOfExist:%.2f	assonum:%d\n", \
			tracking_inst->frame_cnt, traj->uid, traj->curTarget.mdScore, prob_clus, \
			prob_lifeTime, prob_motion, traj->objManagement.probOfExisting, clusNum);
#endif

	}
	//#ifdef DEBUG_LOG_TRACKER_EXISTING
	//	fclose(fp1);
	//#endif

	tElem = tracking_listGetFirst(&tracking_module_inst->twsTrackList);
	//i = 0;

//#ifdef DEBUG_LOG_TRACKER_EXISTING
//	fopen_s(&fp1, "E:/record/20220929_huizhou/record1.txt", "a+");
//#endif

	while (tElem != 0)
	{
		uid = tElem->data;
		/*  @brief Another method to calculate probOfExisting ,by hxj */
		traj = (sTracking_objectUnit*)(tracking_module_inst->hTrack[uid]);
		mdScore = 0.0f;
		mdScoreSQ = 0.0f;
		prob_motion_or_v = 0.0f;
		prob_lifeTime = 0.0f;
		MissFrameNum = 0;
		weight = 0.0f;
		objLife = 0;

		clusNum = traj->assoProperty.assoThisScan;
		//alive_last = (int)(trajList[i].variance[2] * INV_ONE6FORMAT_SCALE);

		/* 1 part : target clustering reliability */
		//if (clusNum >= TRACKEREXISTING_OR_MAXPROBCLUSNUM)
		//{
		//	prob_clus = 1.0f;//clusNum>=10：1
		//}
		//else 
		//{
		//	prob_clus = ((float)clusNum) / ((float)TRACKEREXISTING_OR_MAXPROBCLUSNUM);//clusNum<10：clusNum/10
		//}	

		//if (X < -9.0f)//<-9
		//{
		//	if (fabsf(Vx) < 4.5f)//person or bicycle
		//	{
		//		prob_clus = CalcProb_init(clusNum, 1);
		//	}
		//	else//car
		//	{
		//		prob_clus = CalcProb_init(clusNum, 2);
		//	}
		//}
		//else if (X < -2.0f)//-9~-2
		//{
		//	if (fabsf(Vx) < 4.5f)//person or bicycle
		//	{
		//		prob_clus = CalcProb_init(clusNum, 2);
		//	}
		//	else//car
		//	{
		//		prob_clus = CalcProb_init(clusNum, 3);
		//	}
		//}
		//else if (X <= 7.0f)//-2~7
		//{
		//	if (fabsf(Vx) < 4.5f)//person or bicycle
		//	{
		//		prob_clus = CalcProb_init(clusNum, 3);
		//	}
		//	else//car
		//	{
		//		prob_clus = CalcProb_init(clusNum, 4);
		//	}
		//}
		//else //>7
		//{
		//	if (fabsf(Vx) < 4.5f)//person or bicycle
		//	{
		//		prob_clus = CalcProb_init(clusNum, 2);
		//	}
		//	else //car
		//	{
		//		prob_clus = CalcProb_init(clusNum, 3);
		//	}
		//}
		prob_clus = CalcProb_init(clusNum, 1);

		//tracking_inst->trajInfoOutput->trackTasNum != 0

		/* 2 part : tracker existing stability */
		//updatePast8frame =(int)(trajList[i].updatePast8frame); //TODO: Get update past 8 frames
		//past8frame = (int)(trajList[i].variance[0] * INV_ONE6FORMAT_SCALE);
		//objLife = (int)(trajList[i].alive);//* INV_ONE6FORMAT_SCALE
		past8frame = traj->objManagement.updatePast8frame;
		objLife = traj->objManagement.past8FrameBitNum;
		maxFrame = (objLife >= 8) ? 8 : objLife;
		for (ii = 0; ii < 8; ii++)
		{
			tmp[ii] = 0;
		}
		for (ii = 0; ii < maxFrame; ii++)
		{
			tmp[ii] = (past8frame >> ii) & (0x01);
			if (0 == tmp[ii])
			{
				MissFrameNum++;
			}
		}
		for (ii = 0; ii < 8; ii++) //合理吗？如果一个航迹life为1，前7帧都按关联计算，635行初始化为0比较合理
		{
			if (0 == ii) //current frame
			{
				prob_lifeTime = prob_lifeTime + tmp[ii] * 0.2f;
			}
			else if ((1 == ii) || (2 == ii))//2nd or 3rd to last frame
			{
				prob_lifeTime = prob_lifeTime + tmp[ii] * 0.15f;
			}
			else
			{
				prob_lifeTime = prob_lifeTime + tmp[ii] * 0.1f;
			}
		}

		/* 3 part : tracker space motion stability */
		//mdScore = (float)(trajList[i].variance[1] * INV_ONE6FORMAT_SCALE); // TODO: Get md
		mdScore = traj->curTarget.mdScore;
		//mdScoreSQ = mdScore * mdScore;
		//mdScoreSQ = mdScore;//TODO:

		////compute the value of prob_motion
		//for (m = 1; m < TRACKEREXISTING_OR_MAXPROBMOTIONNUM; m++) 
		//{
		//	//if (mdScoreSQ < inst->probMotion_or_v[m].dev) 
		//	if (mdScore < inst->probMotion_or_v[m].dev)
		//	{
		//		prob_motion_or_v = inst->probMotion_or_v[m - 1].prob;
		//		break;
		//	}
		//}
		//if (TRACKEREXISTING_OR_MAXPROBMOTIONNUM == m)
		//{
		//	prob_motion_or_v = inst->probMotion_or_v[m - 1].prob;
		//}

		//compute the value of prob_motion
		for (m = 0; m < TRACKEREXISTING_OR_MAXPROBMOTIONNUM; m++)
		{
			//if (mdScoreSQ < inst->probMotion_or_v[m].dev) 
			if (mdScore < inst->probMotion_or_v[m].dev)
			{
				prob_motion_or_v = inst->probMotion_or_v[m].prob;
				break;
			}
		}
		if (TRACKEREXISTING_OR_MAXPROBMOTIONNUM == m)
		{
			prob_motion_or_v = 0;
		}

		//compute the weight of prob_motion
		if (clusNum != 0 || MissFrameNum == 0)
		{
			weight = 1.0f;
		}
		else //if (clusNum == 0) xxxx xxx0
		{
			if (MissFrameNum != 8)
			{
				if (tmp[1] != 0)//xxxx xx10
				{
					if (1 == MissFrameNum)//updatePast8frame=1111 1110
					{
						weight = 0.9f;
					}
					else if (2 == MissFrameNum)
					{
						weight = 0.85f;
					}
					else if (3 == MissFrameNum)
					{
						weight = 0.8f;
					}
					else if (4 == MissFrameNum)
					{
						weight = 0.75f;
					}
					else if (5 == MissFrameNum)
					{
						weight = 0.7f;
					}
					else if (6 == MissFrameNum)
					{
						weight = 0.65f;
					}
					else if (7 == MissFrameNum)//0000 0010
					{
						weight = 0.6f;
					}
				}
				if ((0 == tmp[1]) && (tmp[2] != 0)) //xxxx x100
				{
					if (2 == MissFrameNum)//1111 1100
					{
						weight = 0.7f;
					}
					else if (3 == MissFrameNum)
					{
						weight = 0.65f;
					}
					else if (4 == MissFrameNum)
					{
						weight = 0.6f;
					}
					else if (5 == MissFrameNum)
					{
						weight = 0.55f;
					}
					else if (6 == MissFrameNum)
					{
						weight = 0.5f;
					}
					else if (7 == MissFrameNum)//0000 0100
					{
						weight = 0.45f;
					}
				}
				if ((0 == tmp[1]) && (0 == tmp[2]) && (tmp[3] != 0)) //xxxx 1000
				{
					if (3 == MissFrameNum)//1111 1000
					{
						weight = 0.6f;
					}
					else if (4 == MissFrameNum)
					{
						weight = 0.55f;
					}
					else if (5 == MissFrameNum)
					{
						weight = 0.5f;
					}
					else if (6 == MissFrameNum)
					{
						weight = 0.45f;
					}
					else if (7 == MissFrameNum)//0000 1000
					{
						weight = 0.4f;
					}
				}
				if ((0 == tmp[1]) && (0 == tmp[2]) && (0 == tmp[3]) && (tmp[4] != 0)) //xxx1 0000
				{
					if (4 == MissFrameNum)//1111 0000
					{
						weight = 0.5f;
					}
					else if (5 == MissFrameNum)
					{
						weight = 0.4f;
					}
					else if (6 == MissFrameNum)
					{
						weight = 0.3f;
					}
					else if (7 == MissFrameNum)//0001 0000
					{
						weight = 0.2f;
					}
				}
				if ((0 == tmp[1]) && (0 == tmp[2]) && (0 == tmp[3]) && (0 == tmp[4]) && (MissFrameNum >= 5)) //xx10 0000 / x1x0 0000 / 1xx0 0000
				{
					weight = 0.1f;
				}
			}
			else //0000 0000
			{
				weight = 0.0f;
			}
		}
		prob_motion = weight * prob_motion_or_v;

		//compute the weight of prob_motion
		if (clusNum != 0 || MissFrameNum == 0)
		{
			weight = 1.0f;
		}
		else //if (clusNum == 0) xxxx xxx0
		{
			if (MissFrameNum != 8)
			{
				if (tmp[1] != 0)//xxxx xx10
				{
					if (1 == MissFrameNum)//updatePast8frame=1111 1110
					{
						weight = 0.9f;
					}
					else if (2 == MissFrameNum)
					{
						weight = 0.85f;
					}
					else if (3 == MissFrameNum)
					{
						weight = 0.8f;
					}
					else if (4 == MissFrameNum)
					{
						weight = 0.75f;
					}
					else if (5 == MissFrameNum)
					{
						weight = 0.7f;
					}
					else if (6 == MissFrameNum)
					{
						weight = 0.65f;
					}
					else if (7 == MissFrameNum)//0000 0010
					{
						weight = 0.6f;
					}
				}
				if ((0 == tmp[1]) && (tmp[2] != 0)) //xxxx x100
				{
					if (2 == MissFrameNum)//1111 1100
					{
						weight = 0.7f;
					}
					else if (3 == MissFrameNum)
					{
						weight = 0.65f;
					}
					else if (4 == MissFrameNum)
					{
						weight = 0.6f;
					}
					else if (5 == MissFrameNum)
					{
						weight = 0.55f;
					}
					else if (6 == MissFrameNum)
					{
						weight = 0.5f;
					}
					else if (7 == MissFrameNum)//0000 0100
					{
						weight = 0.45f;
					}
				}
				if ((0 == tmp[1]) && (0 == tmp[2]) && (tmp[3] != 0)) //xxxx 1000
				{
					if (3 == MissFrameNum)//1111 1000
					{
						weight = 0.6f;
					}
					else if (4 == MissFrameNum)
					{
						weight = 0.55f;
					}
					else if (5 == MissFrameNum)
					{
						weight = 0.5f;
					}
					else if (6 == MissFrameNum)
					{
						weight = 0.45f;
					}
					else if (7 == MissFrameNum)//0000 1000
					{
						weight = 0.4f;
					}
				}
				if ((0 == tmp[1]) && (0 == tmp[2]) && (0 == tmp[3]) && (tmp[4] != 0)) //xxx1 0000
				{
					if (4 == MissFrameNum)//1111 0000
					{
						weight = 0.5f;
					}
					else if (5 == MissFrameNum)
					{
						weight = 0.4f;
					}
					else if (6 == MissFrameNum)
					{
						weight = 0.3f;
					}
					else if (7 == MissFrameNum)//0001 0000
					{
						weight = 0.2f;
					}
				}
				if ((0 == tmp[1]) && (0 == tmp[2]) && (0 == tmp[3]) && (0 == tmp[4]) && (MissFrameNum >= 5)) //xx10 0000 / x1x0 0000 / 1xx0 0000
				{
					weight = 0.1f;
				}
			}
			else //0000 0000
			{
				weight = 0.0f;
			}
		}
		prob_motion = weight * prob_motion_or_v;


		/* total : prob_clus*w1 + prob_motion*w2 + prob_lifeTime*w3 */
		//if (trajList[jj].stateType == STABLE_TRAJ)
		//{
		//	/*probOfExistLast = (float)(trajList[jj].probOfExisting * INV_ONE7FORMAT_SCALE);
		//	probOfExist = (TRACKEREXISTING_OR_W1 * prob_clus + TRACKEREXISTING_OR_W2 * prob_lifeTime + TRACKEREXISTING_OR_W3 * prob_motion);
		//	myMath_lowPassFilter(&probOfExist, probOfExistLast, 0.3f);
		//	trajList[jj].probOfExisting = (uint8_t)(probOfExist * ONE7FORMAT_SCALE);*/

		/* total : prob_clus*w1 + prob_motion*w2 + prob_lifeTime*w3 */
		//if (trajList[jj].stateType == STABLE_TRAJ)
		//{
		//	/*probOfExistLast = (float)(trajList[jj].probOfExisting * INV_ONE7FORMAT_SCALE);
		//	probOfExist = (TRACKEREXISTING_OR_W1 * prob_clus + TRACKEREXISTING_OR_W2 * prob_lifeTime + TRACKEREXISTING_OR_W3 * prob_motion);
		//	myMath_lowPassFilter(&probOfExist, probOfExistLast, 0.3f);
		//	trajList[jj].probOfExisting = (uint8_t)(probOfExist * ONE7FORMAT_SCALE);*/

		//	probOfExistLast = (float)(trajList[jj].probOfExisting * INV_ONE7FORMAT_SCALE);
		//	probOfExist = (TRACKEREXISTING_OR_W1 * prob_clus + TRACKEREXISTING_OR_W2 * prob_lifeTime + TRACKEREXISTING_OR_W3 * prob_motion);
		//	myMath_lowPassFilter(&probOfExist, probOfExistLast, 0.3f);
		//	trajList[jj].probOfExisting = (uint8_t)(probOfExist * ONE7FORMAT_SCALE);
		//}
		//else
		//{
		//	//trajList[jj].probOfExisting = (uint8_t)((TRACKEREXISTING_OR_W1 * prob_clus + TRACKEREXISTING_OR_W2 * prob_lifeTime + TRACKEREXISTING_OR_W3 * prob_motion) * ONE7FORMAT_SCALE);
		//	traj->objManagement.probOfExisting = (TRACKEREXISTING_OR_W1 * prob_clus + TRACKEREXISTING_OR_W2 * prob_lifeTime + TRACKEREXISTING_OR_W3 * prob_motion);
		//}
		probOfExistNew = (TRACKEREXISTING_OR_W1 * prob_clus + TRACKEREXISTING_OR_W2 * prob_lifeTime + TRACKEREXISTING_OR_W3 * prob_motion);
		if (traj->objManagement.updateProbExisting == 1)
		{
			myMath_lowPassFilter(&traj->objManagement.probOfExisting, probOfExistNew, 0.5f);
		}

		//inst->id_sorted[i] = trajList[i].id;
		inst->id_sorted[i] = traj->uid;
		inst->existingProb_ascent[i] = (TRACKEREXISTING_OR_W1 * prob_clus + TRACKEREXISTING_OR_W2 * prob_lifeTime + TRACKEREXISTING_OR_W3 * prob_motion);

		tElem = tracking_listGetNext(tElem);
		//jj++;
		i++;

		/*if (trajList[i].stateType == ACTIVE_TRAJ)
		{
			probOfExistLast = (float)(trajList[i].trajProperty.variance[7]);
			probOfExist = (uint16_t)((TRACKEREXISTING_OR_W1 * prob_clus + TRACKEREXISTING_OR_W2 * prob_lifeTime + TRACKEREXISTING_OR_W3 * prob_motion) * ONE6FORMAT_SCALE);
			myMath_lowPassFilter(&probOfExist, probOfExistLast, 0.3f);
			trajList[i].trajProperty.variance[7] = (uint16_t)(probOfExist);
		}
		else
		{
			trajList[i].trajProperty.variance[7] = (uint16_t)((TRACKEREXISTING_OR_W1 * prob_clus + TRACKEREXISTING_OR_W2 * prob_lifeTime + TRACKEREXISTING_OR_W3 * prob_motion) * ONE6FORMAT_SCALE);
		}*/

#ifdef DEBUG_LOG
		/*my_printf("3333333333333  ID: %d, prob_clus: %.3f,objLife: %d, maxFrame: %d, prob_lifeTime: %.3f, prob_motion: %.3f, mdScoreSQ: %.3f,  weight: %.3f, updatePast8frame: %d, probOfExistLast: %.3f, probOfExisting2: %.3f\n", \
			trajList[i].id, prob_clus, objLife, maxFrame, prob_lifeTime, prob_motion, mdScoreSQ, weight, updatePast8frame, probOfExistLast, trajList[i].probOfExisting* INV_ONE7FORMAT_SCALE);*/

			/*my_printf("3333333333333  ID: %d, prob_clus: %.3f,objLife: %d, maxFrame: %d, prob_lifeTime: %.3f, prob_motion: %.3f, mdScoreSQ: %.3f,  weight: %.3f, updatePast8frame: %d, probOfExistLast: %.3f, probOfExisting2: %.3f\n", \
				trajList[i].id, prob_clus, objLife, maxFrame, prob_lifeTime, prob_motion, mdScoreSQ, weight, updatePast8frame, probOfExistLast* INV_ONE6FORMAT_SCALE, trajList[i].trajProperty.variance[7] * INV_ONE6FORMAT_SCALE);*/
#endif

#ifdef DEBUG_LOG_TRACKER_EXISTING
				// 每一帧过后，为所有航迹计算置信度，并打印
		/*fprintf(fp1, "TWS frame:%d	id:%u	md:%.2f	prob_clus:%.2f	prob_lifeTime:%.2f	prob_motion:%.2f	probOfExist:%.2f	assonum:%d\n", \
			tracking_inst->frame_cnt, trajList[i].id, (float)(trajList[i].variance[1] * INV_ONE6FORMAT_SCALE), prob_clus, \
			prob_lifeTime, prob_motion, (float)(trajList[i].probOfExisting * INV_ONE7FORMAT_SCALE), clusNum);*/
		fprintf(fp1, "TWS frame:%d	id:%u	md:%.2f	prob_clus:%.2f	prob_lifeTime:%.2f	prob_motion:%.2f	probOfExist:%.2f	assonum:%d\n", \
			tracking_inst->frame_cnt, traj->uid, traj->curTarget.mdScore, prob_clus, \
			prob_lifeTime, prob_motion, traj->objManagement.probOfExisting, clusNum);
#endif

	}
#ifdef DEBUG_LOG_TRACKER_EXISTING
	fclose(fp1);
#endif

	// 按照置信度升序排序
	sortBoarder = numTrajs - 1;
	for (ii = 0; ii < numTrajs - 1; ii++)
	{
		sortHappen = 0;
		for (jj = 0; jj < sortBoarder; jj++)
		{
			if (inst->existingProb_ascent[jj] > inst->existingProb_ascent[jj + 1])
			{
				// swap threat
				temp = inst->existingProb_ascent[jj];
				inst->existingProb_ascent[jj] = inst->existingProb_ascent[jj + 1];
				inst->existingProb_ascent[jj + 1] = temp;

				// swap id
				inst->id_sorted[jj] = inst->id_sorted[jj] + inst->id_sorted[jj + 1];
				inst->id_sorted[jj + 1] = inst->id_sorted[jj] - inst->id_sorted[jj + 1];
				inst->id_sorted[jj] = inst->id_sorted[jj] - inst->id_sorted[jj + 1];

				sortHappen = 1;
				lastExchangeIdx = jj;
			}
		}
		sortBoarder = jj;
		if (sortHappen == 0)
			break;
	}

#ifdef DEBUG_LOG_TRACKER_EXISTING
	fopen_s(&fp1, "E:/record/20230512/record1.txt", "a+");
	for (ii = 0; ii < numTrajs; ii++) {
		fprintf(fp1, "%d	", inst->id_sorted[ii]);
	}
	fprintf(fp1, "\n");
	for (ii = 0; ii < numTrajs; ii++) {
		fprintf(fp1, "%.3f	", inst->existingProb_ascent[ii]);
	}
	fprintf(fp1, "\n");
	fclose(fp1);
#endif
	//	if (tracking_module_inst->wholeSpaceScanCycleCnt - tracking_module_inst->wholeSpaceProcessCycleCnt != 0) // 重置当前scan是否关联
	//	{
	//		tracking_module_inst->wholeSpaceProcessCycleCnt = tracking_module_inst->wholeSpaceScanCycleCnt;
	//		tElem = tracking_listGetFirst(&tracking_module_inst->twsTrackList);
	//		while (tElem != 0)
	//		{
	//			uid = tElem->data;
	//			traj = (sTracking_objectUnit*)(tracking_module_inst->hTrack[uid]);
	//			/*(sTracking_objectUnit*)(tracking_module_inst->hTrack[uid]->assoProperty.assoThisScan) = 0;
	//			(sTracking_objectUnit*)(tracking_module_inst->hTrack[uid]->assoProperty.countThisScan) = 0;*/
	//			if (traj->assoProperty.assoThisScan == 0) // 若该航迹上一scan和本次scan第一帧均未关联，初始化该航迹的mdScore；若上一scan关联了：1）此轮scan未被关联，此轮scan保留上一轮mdScore 2）此轮scan被关联，用此轮的新mdScore
	//			{
	//				traj->curTarget.mdScore = 100.f;
	//			}
	//			if (traj->assoProperty.assoCondenceMeasId >= 0 || traj->objManagement.lifetime == 0) {
	//				;
	//			}
	//			else{
	//				traj->assoProperty.assoThisScan = 0;
	//			}
	//			tElem = tracking_listGetNext(tElem);
	//		}
	////#ifdef DEBUG_LOG_TRACKER_EXISTING
	////		fopen_s(&fp1, "E:/record/20220929_huizhou/record1.txt", "a+");
	////		fprintf(fp1, "\nstart of cycle %u ------------------------------------------------------------------------\n\n", tracking_module_inst->wholeSpaceScanCycleCnt);
	////		fclose(fp1);
	////#endif
	//	}
}

void updatePast8frame(void* handle, sTracking_moduleInstance* trackingModuleInst, uint16_t trajType)
{
	sTracking_objectUnit* inst;
	sTracking_stateParams stateThres;
	int aa;
#ifdef DEBUG_LOG_TRACKER_EXISTING
	FILE* fp1;
#endif

	inst = (sTracking_objectUnit*)handle;
	/*if (wholeSpaceScanCycleCnt - lastWholeSpaceScanCycleCnt == 1)*/ //lastSeenWholeScanId
	/*if (trackingModuleInst->wholeSpaceScanCycleCnt - trackingModuleInst->lastWholeSpaceScanCycleCnt == 1)
	{
		if (inst->assoProperty.assoMeasId >= 0)
		{
			inst->objManagement.updatePast8frame = ((inst->objManagement.updatePast8frame << 1) | (0x00));
		}
		else
		{
			inst->objManagement.updatePast8frame = ((inst->objManagement.updatePast8frame << 1) | (0x01));
		}
	}*/
	//trackingModuleInst->tasTargetId = 30;
	if (inst->objManagement.lifetime - inst->objManagement.lifetime_old != 0 && inst->uid == trackingModuleInst->tasTargetId && trajType == 1)
	{
		/*if (inst->objManagement.lifetime % 2 == 0)
		{
			inst->assoProperty.assoMeasId = 0;
		}
		else
		{
			inst->assoProperty.assoMeasId = -1;
		}*/

		//inst->assoProperty.assoMeasId = 0;

		if (inst->assoProperty.assoCondenceMeasId != -1) {
			inst->assoProperty.assoThisScan = 1;
			inst->objManagement.updatePast8frame = ((inst->objManagement.updatePast8frame << 1) | (0x01));
		}
		else {
			inst->assoProperty.assoThisScan = 0;
			inst->objManagement.updatePast8frame = ((inst->objManagement.updatePast8frame << 1) | (0x00));
		}
		inst->objManagement.past8FrameBitNum++;
		inst->objManagement.past8FrameBitNum = inst->objManagement.past8FrameBitNum > 8 ? 8 : inst->objManagement.past8FrameBitNum;
		inst->objManagement.updateProbExisting = 1;
		//inst->objManagement.lifetime_old = inst->objManagement.lifetime;
	}
	//else if (inst->uid != trackingModuleInst->tasTargetId)
	else if(trajType == 0)
	{
		if ((trackingModuleInst->wholeSpaceScanCycleFinalBeamFlag == 1) && inst->assoProperty.assoThisScan == 0) // 先判断上一scan是否未更新，未更新8frame追加0
		{
			inst->objManagement.updatePast8frame = ((inst->objManagement.updatePast8frame << 1) | (0x00));
			inst->objManagement.past8FrameBitNum++;
			inst->objManagement.past8FrameBitNum = inst->objManagement.past8FrameBitNum > 8 ? 8 : inst->objManagement.past8FrameBitNum;
			inst->objManagement.updateProbExisting = 1;
		}
		if (trackingModuleInst->wholeSpaceScanCycleFinalBeamFlag == 1) // initialize assoThisScan
		{
			/*if (inst->assoProperty.assoCondenceMeasId >= 0)
			{
				inst->assoProperty.assoThisScan = 1;
			}
			else {
				inst->assoProperty.assoThisScan = 0;
			}*/
			inst->assoProperty.assoThisScan = 0;
		}
		if (inst->assoProperty.assoCondenceMeasId >= 0 && inst->assoProperty.assoThisScan == 0) // 本轮scan8frame更新
		{
			inst->assoProperty.assoThisScan = 1;
			inst->objManagement.updatePast8frame = ((inst->objManagement.updatePast8frame << 1) | (0x01));
			inst->objManagement.past8FrameBitNum++;
			inst->objManagement.past8FrameBitNum = inst->objManagement.past8FrameBitNum > 8 ? 8 : inst->objManagement.past8FrameBitNum;
			inst->objManagement.updateProbExisting = 1;
		}
	}
#ifdef DEBUG_LOG_TRACKER_EXISTING
	// 姣忔杩涘叆鑸抗绠＄悊閮戒細鎵撳嵃锛屽嵆姣忓抚閮戒細鎵撳嵃鎵�鏈?**tas鎴杢ws** 鑸抗
	aa = 1;
	fopen_s(&fp1, "E:/record/20230512/record1.txt", "a+");
	/*fprintf(fp1, "8frame: %x	assoid: %d	", inst->objManagement.updatePast8frame, inst->assoProperty.assoMeasId);*/
	fprintf(fp1, "id:%u	8frame: %u	assoid: %d	assoThisScan:%u		8framebit:%u	life:%d	lifeold:%d	tas_id:%d	process:%u\n", \
		inst->uid, inst->objManagement.updatePast8frame, inst->assoProperty.assoCondenceMeasId, inst->assoProperty.assoThisScan, \
		inst->objManagement.past8FrameBitNum,inst->objManagement.lifetime, inst->objManagement.lifetime_old, trackingModuleInst->tasTargetId, trajType);
	fclose(fp1);
#endif
}