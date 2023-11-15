#include "../include/targetThreatAssessment.h"

uint16_t manualTasTraj[3] = { MAX_NUM_TRAJS,MAX_NUM_TRAJS,MAX_NUM_TRAJS };

void targetThreatAssessment_process(sAlgObjData* algObj)
{
	sTargetThreatAssessmentInst* inst = (sTargetThreatAssessmentInst*)algObj->targetThreatAssessment_inst;
	sTrackingInst* tracking_inst = (sTrackingInst*)(algObj->tracking_inst);
	sTrackingTarget* trajList = tracking_inst->trajInfoOutput->trajList;
	uint16_t numTrajs = tracking_inst->trajInfoOutput->trackObjNum;
	sTracking_moduleInstance* tracking_module_inst = (sTracking_moduleInstance*)(tracking_inst->handle);
	int ii,jj = 0;
	sTracking_ListElem* tElem = NULL;
	uint16_t uid;
	int numOfProtectSpot = inst->numOfProtectSpot;

	float horizontal_dis[MAXNUMOFPROTECTSPOTS] = { 0.f };
	float height[MAXNUMOFPROTECTSPOTS] = { 0.f };
	float dir_angle[MAXNUMOFPROTECTSPOTS] = { 0.f };
	//float threat[MAXNUMOFPROTECTSPOTS] = { 0.f };
	float threat_descend[MAX_NUM_TRAJS] = { 0.f };
	uint16_t id_sorted[MAX_NUM_TRAJS] = { 0.f };
	float threat_max = 0.f;
	uint16_t threat_max_spot = 0;
	float speed = 0.f;
	//float aa, bb = 0.f;

	//float horizontal_th
	
	uint16_t classification = 0;
	float class_weight = 1.f;
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float vx = 0.f;
	float vy = 0.f;
	float vz = 0.f;
	float x_prt = 0.f;
	float y_prt = 0.f;
	float z_prt = 0.f;
	float cos_dir = 0.f;

	float D = 150; // 200 位置威胁度参数
	float H = 100;

	uint16_t sortBoarder = 0;
	uint16_t sortHappen = 0;
	uint16_t lastExchangeIdx = 0;
	float temp_flt = 0.f;
	float R = 0.f;
	float azimuth = 0.f;
	sDispatchInst* dispatch_inst = (sDispatchInst*)algObj->dispatch_inst;
	sDispatchConfigParam* dispatchConfigParams = dispatch_inst->dispatchConfigParam;
	float traj_azi = 0.f;
	float weilanqu = 200.f;
	float elv_max = dispatchConfigParams->eleScanCenter + dispatchConfigParams->eleScanScope / 2.f + algObj->platformInfo->platformData.pitching * INV_ONE15FORMAT_SCALE;
	float elv_min = dispatchConfigParams->eleScanCenter - dispatchConfigParams->eleScanScope / 2.f + algObj->platformInfo->platformData.pitching * INV_ONE15FORMAT_SCALE;
	float T1, T2 = 100.f;
	float d_horizontal,d2weilan, d2blind = 1000.f;
	float radial_v = 0.f;
	float jueceshijian = 1000.f;
	uint16_t T1_level, T2_level, threatLevel = 0;

#if ((defined DEBUG_LOG_TTA)||(defined DEBUG_LOG_THREATLEVEL))
	FILE* fp1 = NULL;
#endif


	memset(inst->threat_descend, 0, MAX_NUM_TRAJS * sizeof(sIDandThreat));
	memset(inst->target_threat, 0, MAX_NUM_TRAJS * sizeof(sTargetThreat));

#if (defined DEBUG_LOG_TTA) || (defined DEBUG_LOG_THREATLEVEL)
	fopen_s(&fp1, "E:/record/20230209/threatlevel.txt", "a+");
	fprintf(fp1, "frame	%u ----------------------------------------\n", algObj->detectsListOutput->frameID);
	//fprintf(fp1, "id	spotID	hori_dis	height	speed	angle\n");
	fprintf(fp1, "id	x	y	z	vx	vy	d_hori	radial_v	d2weilan	T1	juece_T	T1_level	d2blind	T2	T2_level	threatLevel\n");
#endif


	if (numTrajs == 0)
	{
#if (defined DEBUG_LOG_TTA) || (defined DEBUG_LOG_THREATLEVEL)
		fclose(fp1);
#endif
		return;
	}
	for (ii = 0; ii < numTrajs; ii++)
	{
		if (trajList[ii].stateType == UNSTABLE_TRAJ)
		{
			threat_descend[ii] = 0;
			id_sorted[ii] = tracking_module_inst->uidTrack[ii];
			continue;
		}
		R = sqrtf(trajList[ii].traj.x * INV_ONE6FORMAT_SCALE * trajList[ii].traj.x * INV_ONE6FORMAT_SCALE + \
			trajList[ii].traj.y * INV_ONE6FORMAT_SCALE * trajList[ii].traj.y * INV_ONE6FORMAT_SCALE + \
			trajList[ii].traj.z * INV_ONE6FORMAT_SCALE * trajList[ii].traj.z * INV_ONE6FORMAT_SCALE);
		azimuth = trajList[ii].azimuth * INV_ONE6FORMAT_SCALE;
		/*if (R >= 370.f || fabsf(azimuth) >= 45.f)*/
		/*if (R >= 600.f)
		{
			threat_descend[ii] = 0;
			id_sorted[ii] = trajList[ii].id;
			continue;
		}*/
		//inst->target_threat->uid = trajList[ii].id; //所有uid都相同，为啥？？？
		inst->target_threat[ii].uid = tracking_module_inst->uidTrack[ii];
		classification = trajList[ii].classification;
		x = trajList[ii].traj.x * INV_ONE6FORMAT_SCALE;
		y = trajList[ii].traj.y * INV_ONE6FORMAT_SCALE;
		z = trajList[ii].traj.z * INV_ONE6FORMAT_SCALE;
		vx = trajList[ii].traj.vx * INV_ONE6FORMAT_SCALE;
		vy = trajList[ii].traj.vy * INV_ONE6FORMAT_SCALE;
		vz = trajList[ii].traj.vz * INV_ONE6FORMAT_SCALE;
		//speed = sqrt(vx * vx + vy * vy + vz * vz);
		speed = sqrtf(vx * vx + vy * vy); //水平速度
		//航迹威胁等级计算
		d_horizontal = sqrtf(x * x + y * y);
		traj_azi = atan2f(y, x);
		radial_v = vx * cosf(traj_azi) + vy * sinf(traj_azi);
		if (radial_v > 0 && radial_v < 0.01)
			radial_v = 0.01;
		else if (radial_v <= 0 && radial_v > -0.01)
			radial_v = -0.01;
		d2weilan = d_horizontal - weilanqu;
		d2weilan = d2weilan > 0 ? d2weilan : 0;
		T1 = -1 * d2weilan / radial_v;
		T1 = (T1 >= 0 && T1 <= 1000.f) ? T1 : 1000.f; //只有在围栏区外且向外飞或者向内飞但速度很小，T1才会等于1000


		jueceshijian = T1 - 15;
		if (jueceshijian < 70.f)
			T1_level = THREAT_LEVEL1;
		else if (jueceshijian >= 10.f && jueceshijian <= 90.f)
			T1_level = THREAT_LEVEL2;
		else
			T1_level = THREAT_LEVEL3;

		if(z >= 0)
			d2blind = d_horizontal - z / tanf(elv_max*DEG2RAD);
		else
			d2blind = d_horizontal - z / tanf(elv_min*DEG2RAD);
		d2blind = d2blind > 0 ? d2blind : 0;
		T2 = -1 * d2blind / radial_v;
		T2 = (T2 >= 0 && T2 <= 1000.f) ? T2 : 1000.f;

		if (T2 < 10.f)
			T2_level = THREAT_LEVEL1;
		else if (T2 >= 10.f && T2 <= 20.f)
			T2_level = THREAT_LEVEL2;
		else
			T2_level = THREAT_LEVEL3;

		trajList[ii].threatLevel = T1_level <= T2_level ? T1_level : T2_level;

#ifdef DEBUG_LOG_THREATLEVEL
		fprintf(fp1, "%u	%.3f	%.3f	%.3f	%.3f	%.3f	%.3f	%.3f	%.3f	%.3f	%.3f	%u	%.3f	%.3f	%u	%u\n", trajList[ii].id, \
			x,y,z,vx,vy,d_horizontal, radial_v, d2weilan, T1, jueceshijian, T1_level, d2blind, T2, T2_level, trajList[ii].threatLevel);
#endif // DEBUG_LOG_THREATLEVEL

		switch (classification)
		{
		case 0:
			class_weight = 1.f;
			break;
		case 1:
			class_weight = 1.f;
			break;
		case 2:
			class_weight = 0.8f;
			break;
		case 3:
			class_weight = 0.4f;
			break;
		default:
			class_weight = 1.f;
		}

		for (jj = 0; jj < inst->numOfProtectSpot; jj++)
		{
			x_prt = inst->protect_spots[jj].x;
			y_prt = inst->protect_spots[jj].y;
			z_prt = inst->protect_spots[jj].z;
			horizontal_dis[jj] = sqrt((x - x_prt) * (x - x_prt) + (y - y_prt) * (y - y_prt));
			height[jj] = z - z_prt;
			//cos_dir = ((x - x_prt) * vx + (y - y_prt) * vy) / (horizontal_dis[jj] * speed);//错了，方向应该是prt - x
			cos_dir = ((x_prt - x) * vx + (y_prt - y) * vy) / (horizontal_dis[jj] * speed);
			dir_angle[jj] = acosf(cos_dir); // 航向夹角[0, 180]°

			/*aa = 1 / (1 + exp((1.5 * horizontal_dis[jj] - 4 * D) / D));
			bb = 1 / (1 + exp((2 * height[jj] - 6 * H) / H));*/
			inst->target_threat[ii].position_threat[jj] = 1 / (1 + exp((1.5 * horizontal_dis[jj] - 4 * D) / D)) * 1 / (1 + exp((2 * height[jj] - 6 * H) / H));
			inst->target_threat[ii].velocity_threat[jj] = 1 / (1 + 0.3 / log(0.25 * speed + 1.5));
			inst->target_threat[ii].direction_threat[jj] = 1 / (1 + 0.4 * exp(-2 * cos(0.6 * dir_angle[jj])));
			inst->target_threat[ii].threat[jj] = class_weight * \
				inst->target_threat[ii].position_threat[jj] * \
				inst->target_threat[ii].velocity_threat[jj] * \
				inst->target_threat[ii].direction_threat[jj];
#ifdef DEBUG_LOG_TTA
			if (trajList[ii].traj.x * INV_ONE6FORMAT_SCALE < 2000.f)
			{
				fprintf(fp1, "%u	%d	%.3f	%.3f	%.3f	%.3f\n", inst->target_threat[ii].uid, jj, horizontal_dis[jj], height[jj], speed, dir_angle[jj]);
			}
#endif
		}

		threat_max = 0;
		for (jj = 0; jj < inst->numOfProtectSpot; jj++) // find max threat of current traj
		{
			if (inst->target_threat[ii].threat[jj] > threat_max)
			{
				threat_max = inst->target_threat[ii].threat[jj];
				inst->target_threat[ii].threat_max = threat_max;
				inst->target_threat[ii].threat_max_spot = jj;
			}
		}

		threat_descend[ii] = inst->target_threat[ii].threat_max;
		id_sorted[ii] = inst->target_threat[ii].uid;
	}

#if (defined DEBUG_LOG_TTA) || (defined DEBUG_LOG_THREATLEVEL)
	fclose(fp1);
#endif


		// descend sort of traj according to threat
	sortBoarder = numTrajs - 1;
	for (ii = 0; ii < numTrajs - 1; ii++)
	{
		sortHappen = 0;
		for (jj = 0; jj < sortBoarder; jj++)
		{
			if (threat_descend[jj] < threat_descend[jj + 1])
			{
				// swap threat
				temp_flt = threat_descend[jj];
				threat_descend[jj] = threat_descend[jj + 1];
				threat_descend[jj + 1] = temp_flt;

				// swap id
				id_sorted[jj] = id_sorted[jj] + id_sorted[jj + 1];
				id_sorted[jj + 1] = id_sorted[jj] - id_sorted[jj + 1];
				id_sorted[jj] = id_sorted[jj] - id_sorted[jj + 1];

				sortHappen = 1;
				lastExchangeIdx = jj;
			}
		}
		sortBoarder = jj;
		if (sortHappen == 0)
			break;
	}

	for (ii = 0; ii < numTrajs; ii++)
	{
		inst->threat_descend[ii].id = id_sorted[ii];
		inst->threat_descend[ii].threat = threat_descend[ii];
	}

#ifdef DEBUG_LOG_TTA
	fopen_s(&fp1, "E:/record/20220929_huizhou/record.txt", "a+");
	//fprintf(fp1, "frame	%u ----------------------------------------\n", algObj->detectsListOutput->frameID);
	fprintf(fp1, "threat info --------------------------------\n");
	fprintf(fp1, "id	spotID	pos_T	velo_T	dir_T	THREAT\n");
	for (ii = 0; ii < numTrajs; ii++)
	{
		if (trajList[ii].stateType == UNSTABLE_TRAJ)
		{
			continue;
		}
		if (trajList[ii].traj.x * INV_ONE6FORMAT_SCALE < 2000.f)
		{
			for (jj = 0; jj < inst->numOfProtectSpot; jj++)
			{
				fprintf(fp1, "%u	%d	%.3f	%.3f	%.3f	%.3f\n", inst->target_threat[ii].uid, jj, inst->target_threat[ii].position_threat[jj], \
					inst->target_threat[ii].velocity_threat[jj], inst->target_threat[ii].direction_threat[jj], inst->target_threat[ii].threat[jj]);
			}
		}
	}
	for (ii = 0; ii < numTrajs; ii++)
	{
		/*if (trajList[ii].traj.x * INV_ONE6FORMAT_SCALE < 200.f)
		{
			for (jj = 0; jj < numTrajs; jj++)
			{
				if (inst->threat_descend[jj].id == trajList[ii].id)
					fprintf(fp1, "%u	", inst->threat_descend[jj].id);
			}
		}*/
		for (jj = 0; jj < numTrajs; jj++)
		{
			if (inst->threat_descend[ii].id == trajList[jj].id && trajList[jj].traj.x * INV_ONE6FORMAT_SCALE < 2000.f)
				fprintf(fp1, "%u	", inst->threat_descend[ii].id);
		}
	}
	fprintf(fp1, "\n");
	for (ii = 0; ii < numTrajs; ii++)
	{
		/*if (trajList[ii].traj.x * INV_ONE6FORMAT_SCALE < 200.f)
		{
			for (jj = 0; jj < numTrajs; jj++)
			{
				if (inst->threat_descend[jj].id == trajList[ii].id)
					fprintf(fp1, "%.3f	", inst->threat_descend[jj].threat);
			}
		}*/
		for (jj = 0; jj < numTrajs; jj++)
		{
			if (inst->threat_descend[ii].id == trajList[jj].id && trajList[jj].traj.x * INV_ONE6FORMAT_SCALE < 2000.f)
				fprintf(fp1, "%.3f	", inst->threat_descend[ii].threat);
		}
	}
	fprintf(fp1, "\n");
	fclose(fp1);
#endif // DEBUG_LOG_TTA

	tastwsListConstruct(algObj);
	
}


void tastwsListConstruct(sAlgObjData* algObj)
{
	sTrackingInst* tracking_inst = (sTrackingInst*)(algObj->tracking_inst);
	sTracking_moduleInstance* trackingModule_inst = (sTracking_moduleInstance*)tracking_inst->handle;
	sTargetThreatAssessmentInst* TTA_inst = (sTargetThreatAssessmentInst*)algObj->targetThreatAssessment_inst;
	sDispatchInst* dispatchInst = (sDispatchInst*)algObj->dispatch_inst;
	sDispatchOutput* dispatchOutput = (sDispatchOutput*)dispatchInst->dispatchOutput;
	int tasTargetNum = MAX_NUM_TRAJS;
	uint16_t trajNum = tracking_inst->trajInfoOutput->trackObjNum;
	sTracking_ListElem* tElem = NULL;
	uint16_t uid;
	sTracking_objectUnit* pTracker;
	int ii = 0,jj = 0,kk = 0;
	uint16_t manualTasFlag = 0;

	float R = 0.f,azimuth = 0.f;
	sTrackingTarget* trajList = tracking_inst->trajInfoOutput->trajList;

#ifdef DEBUG_LOG_TTA
	FILE* fp1;
#endif
	/*manualTasTraj[1] = 9;
	manualTasTraj[2] = 8;*/
	for (ii = 0; ii < TAS_ID_MAX_SIZE; ii++)
	{
		if (manualTasTraj[ii] < MAX_NUM_TRAJS)
		{
			manualTasFlag = 1;
			break;
		}
	}

	if (manualTasFlag == 0) //自动选择tas目标
	{
		for (ii = 0; ii < trajNum; ii++)
		{
			R = sqrtf(trajList[ii].traj.x * INV_ONE6FORMAT_SCALE * trajList[ii].traj.x * INV_ONE6FORMAT_SCALE + \
				trajList[ii].traj.y * INV_ONE6FORMAT_SCALE * trajList[ii].traj.y * INV_ONE6FORMAT_SCALE + \
				trajList[ii].traj.z * INV_ONE6FORMAT_SCALE * trajList[ii].traj.z * INV_ONE6FORMAT_SCALE);
			azimuth = trajList[ii].azimuth * INV_ONE6FORMAT_SCALE;
			if (!g_byDummyTest )
			{
				//if (tracking_inst->trajInfoOutput->trajList[ii].stateType != UNSTABLE_TRAJ && R < 370.f && fabsf(azimuth) < 45.f)
				if (tracking_inst->trajInfoOutput->trajList[ii].stateType != UNSTABLE_TRAJ)
				{
					jj++;
				}
			}
			else
			{
				if (tracking_inst->trajInfoOutput->trajList[ii].stateType != UNSTABLE_TRAJ)
				{
					jj++;
				}
			}
		}
		tasTargetNum = TAS_ID_MAX_SIZE <= jj ? TAS_ID_MAX_SIZE : jj;//如果只有1个稳态航迹，那么就只选1个tas目标；该航迹威胁度非0，排在threat_descend[]最前面，其余暂态航迹威胁度为0
		if (trackingModule_inst->tracking_params->workMode == 1)
		{
			if (dispatchOutput->tasBeamCntCur == 0)
			{
				//tws波束期间每帧对链表重新排序
				tracking_listMerge(&trackingModule_inst->twsTrackList, &trackingModule_inst->tasTrackList);
#ifdef DEBUG_LOG_TTA
				fopen_s(&fp1, "E:/record/20220929_huizhou/record.txt", "a+");
				fprintf(fp1, "original TWS targets: ");
				tElem = tracking_listGetFirst(&trackingModule_inst->twsTrackList);
				while (tElem != 0)
				{
					fprintf(fp1, "%u	", tElem->data);
					tElem = tracking_listGetNext(tElem);
				}
				fclose(fp1);
#endif
				sortTrackListTTA(&trackingModule_inst->twsTrackList, TTA_inst, trajNum); //tas模式对整个链表进行排序，稳态航迹按威胁度排序，暂态航迹按alive排序
				tracking_listSplit(&trackingModule_inst->twsTrackList, tasTargetNum, &trackingModule_inst->tasTrackList);//拆分出tasTargetNum个TAS航迹
			}
			else
			{
				;//tas波束期间不对链表作处理
				//sortTrackListTTA(&trackingModule_inst->tasTrackList, TTA_inst, tasTargetNum);
			}
		}
		else
		{
			tracking_listMerge(&trackingModule_inst->twsTrackList, &trackingModule_inst->tasTrackList);
			sortTrackListTTA(&trackingModule_inst->twsTrackList, TTA_inst, trajNum); //tws模式对整个链表进行排序，不拆分，稳态航迹按威胁度排序，暂态航迹按alive排序
		}
	}
	else
	{
		//用户手动选择了tas目标
		for (ii = 0; ii < TAS_ID_MAX_SIZE; ii++) //用户选择的目标数
		{
			if (manualTasTraj[ii] < MAX_NUM_TRAJS)
			{
				//for (kk = 0; kk < trajNum; kk++)
				//{
				//	if (tracking_inst->trajInfoOutput->trajList[kk].id == manualTasTraj[ii])
				//	{
				//		if (tracking_inst->trajInfoOutput->trajList[kk].stateType == STABLE_TRAJ)
				//			jj++;//只有稳态航迹才作为tas目标，航迹存在，且稳态
				//		else
				//			break;
				//	}
				//}
				jj++;
			}
		}
		tasTargetNum = TAS_ID_MAX_SIZE <= jj ? TAS_ID_MAX_SIZE : jj;
		if (trackingModule_inst->tracking_params->workMode == 1)
		{
			if (dispatchOutput->tasBeamCntCur == 0)
			{
				tracking_listMerge(&trackingModule_inst->twsTrackList, &trackingModule_inst->tasTrackList);
#ifdef DEBUG_LOG_TTA
				fopen_s(&fp1, "E:/record/20220929_huizhou/record.txt", "a+");
				fprintf(fp1, "original TWS targets: ");
				tElem = tracking_listGetFirst(&trackingModule_inst->twsTrackList);
				while (tElem != 0)
				{
					fprintf(fp1, "%u	", tElem->data);
					tElem = tracking_listGetNext(tElem);
				}
				fclose(fp1);
#endif
				//sortTrackListTTA(&trackingModule_inst->twsTrackList, TTA_inst, tasTargetNum); //此处sort只是把tas目标提到链表的最前面，不是对整个链表进行排序
				sortTrackListManual(&trackingModule_inst->twsTrackList,manualTasTraj, (uint16_t*)&tasTargetNum, trackingModule_inst); //此处sort只是把tas目标提到链表的最前面，不是对整个链表进行排序
				tracking_listSplit(&trackingModule_inst->twsTrackList, tasTargetNum, &trackingModule_inst->tasTrackList);
			}
			else
			{
				;//tas波束期间不对链表作处理
				//sortTrackListTTA(&trackingModule_inst->tasTrackList, TTA_inst, tasTargetNum);
			}
		}
		else
		{
			tracking_listMerge(&trackingModule_inst->twsTrackList, &trackingModule_inst->tasTrackList);
			sortTrackListTTA(&trackingModule_inst->twsTrackList, TTA_inst, trajNum); //tws模式只排序，不拆分，稳态航迹按威胁度排序，暂态航迹按alive排序
		}
	}
	tracking_inst->trajInfoOutput->trackTwsNum = trackingModule_inst->twsTrackList.count;
	tracking_inst->trajInfoOutput->trackTasNum = trackingModule_inst->tasTrackList.count + trackingModule_inst->initTrackList.count;

	// 把航迹新的tws tas属性向tracking输出赋值
	tElem = tracking_listGetFirst(&trackingModule_inst->twsTrackList);
	while (tElem != 0)
	{
		uid = tElem->data;
		pTracker = (sTracking_objectUnit*)trackingModule_inst->hTrack[uid];
		pTracker->objManagement.tasFlag = 0;
		for (ii = 0; ii < trajNum; ii++)
		{
			if (tracking_inst->trajInfoOutput->trajList[ii].id == pTracker->tid)
			{
				tracking_inst->trajInfoOutput->trajList[ii].twsTasFlag = 0;
				break;
			}
		}
		tElem = tracking_listGetNext(tElem);
	}

	tElem = tracking_listGetFirst(&trackingModule_inst->tasTrackList);
	while (tElem != 0)
	{
		uid = tElem->data;
		pTracker = (sTracking_objectUnit*)trackingModule_inst->hTrack[uid];
		pTracker->objManagement.tasFlag = 1;
		for (ii = 0; ii < trajNum; ii++)
		{
			if (tracking_inst->trajInfoOutput->trajList[ii].id == pTracker->tid)
			{
				tracking_inst->trajInfoOutput->trajList[ii].twsTasFlag = 1;
#ifdef DEBUG_LOG_TAS
				my_printf("tas id %d", pTracker->tid);
#endif // DEBUG_LOG_TAS

				break;
			}
		}
		tElem = tracking_listGetNext(tElem);
	}
#ifdef DEBUG_LOG_TTA
	fopen_s(&fp1, "E:/record/20220929_huizhou/record.txt", "a+");
	fprintf(fp1, "\nTAS targets: ");
	tElem = tracking_listGetFirst(&trackingModule_inst->tasTrackList);
	while (tElem != 0)
	{
		uid = tElem->data;
		tElem = tracking_listGetNext(tElem);
		fprintf(fp1, "%u	", uid);
	}
	fprintf(fp1, "\n");

	fprintf(fp1, "TWS targets: ");
	tElem = tracking_listGetFirst(&trackingModule_inst->twsTrackList);
	while (tElem != 0)
	{
		uid = tElem->data;
		tElem = tracking_listGetNext(tElem);
		fprintf(fp1, "%u	", uid);
	}
	fprintf(fp1, "\n");
	fclose(fp1);
#endif
}

// 根据威胁度将航迹降序排列
void sortTrackListTTA(sTracking_ListObj* trackList, sTargetThreatAssessmentInst* inst_TTA, uint16_t trajNum)
{
	sTracking_ListElem* node_1 = trackList->begin;
	sTracking_ListElem* node_2 = trackList->begin;
	int ii, jj = 0;
	uint16_t tmp = MAX_NUM_TRAJS;
	int tasTargetNum, tasTargetID = MAX_NUM_TRAJS;

	//tasTargetNum = TAS_ID_MAX_SIZE <= trackList->count ? TAS_ID_MAX_SIZE : trackList->count;
	//tasTargetNum = TAS_ID_MAX_SIZE <= trajNum ? TAS_ID_MAX_SIZE : trajNum;
	tasTargetNum = trajNum;

	if (0 == trackList->count || 0 == trackList->begin)
	{
		return;
	}


	node_1 = trackList->begin;
	node_2 = trackList->begin;
	for (ii = 0; node_1->next != NULL && ii < tasTargetNum; node_1 = node_1->next, ii++)
	{
		tasTargetID = inst_TTA->threat_descend[ii].id;
		if (node_1->data == tasTargetID)
		{
			;
		}
		else
		{
			for (node_2 = trackList->begin; node_2 != NULL; node_2 = node_2->next)
			{
				if (node_2->data == tasTargetID)
				{
					tmp = node_2->data;
					node_2->data = node_1->data;
					node_1->data = tmp;
					break;
				}
			}
		}
	}
}

void sortTrackListManual(sTracking_ListObj* trackList, uint16_t* manualTasTraj, uint16_t* trajNum, sTracking_moduleInstance* trackingModule_inst)
{
	sTracking_ListElem* node_1 = trackList->begin;
	sTracking_ListElem* node_2 = trackList->begin;
	int ii = 0, jj = 0;
	uint16_t tmp = MAX_NUM_TRAJS;
	int tasTargetNum = TAS_ID_MAX_SIZE, tasTargetID = MAX_NUM_TRAJS;
	uint16_t existFlag = 0;

	//tasTargetNum = TAS_ID_MAX_SIZE <= trackList->count ? TAS_ID_MAX_SIZE : trackList->count;
	tasTargetNum = TAS_ID_MAX_SIZE <= *trajNum ? TAS_ID_MAX_SIZE : *trajNum;

	if (0 == trackList->count || 0 == trackList->begin)
	{
		return;
	}
	if (tasTargetNum == 0)
	{
		return;
	}

	node_1 = trackList->begin;
	node_2 = trackList->begin;
	for (ii = 0; node_1->next != NULL && ii < TAS_ID_MAX_SIZE; ii++)
	{
		if (manualTasTraj[ii] == MAX_NUM_TRAJS)
			continue;
		existFlag = 0;//初始化
		tasTargetID = manualTasTraj[ii];
		if (node_1->data == tasTargetID)
		{
			existFlag = 1;
			node_1 = node_1->next;
		}
		else
		{
			for (node_2 = trackList->begin; node_2 != NULL; node_2 = node_2->next)
			{
				if (node_2->data == tasTargetID)
				{
					tmp = node_2->data;
					node_2->data = node_1->data;
					node_1->data = tmp;
					existFlag = 1;
					node_1 = node_1->next;
					break;
				}
			}
		}
		if (existFlag == 0)
		{
			manualTasTraj[ii] = MAX_NUM_TRAJS;
			*trajNum -= 1;
		}
	}
}
