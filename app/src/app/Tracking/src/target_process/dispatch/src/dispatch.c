
/*========================================================================================
*                                     INCLUDE FILES
=========================================================================================*/
#include "../include/dispatch.h"
#include "../../tracking/include/tracking_int.h"

/*========================================================================================
*                                    GLOBAL CONSTANTS
=========================================================================================*/
uint32_t gBeamMode = SCAN_BEAM_MODE; //1 for fix beam mode ,2 for scan beam mode
uint32_t gTasScanBeamCnt = 0;	// beam counter of tas scan mode, each beam add 1
uint32_t gWholeSpaceScanCycleCnt = 0;	// cycle counter of whole space scan
uint32_t gTasScanCycleCnt = 0;	// cycle counter of tas scan
int32_t gBeamParaCalcInit = -1;

/* azimuth direction beam */
int16_t gAziFov = DEFAULT_AZI_FOV;
int16_t gAziFovCenter = DEFAULT_AZI_FOV_CENTER;
int16_t gAziBeamNum = DEFAULT_AZI_BEAM_NUM;
uint16_t gAziBeamIndexCur = 0;
uint16_t gAziBeamIndexLast = 0;
// gAziBeamIndex = 0:1:29
uint16_t gAziBeamIndex[DEFAULT_AZI_BEAM_NUM] = { 0 };
//{
//	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29
//};
// gAziBeamAngle = -58:4:58, in degree
int16_t gAziBeamAngle[DEFAULT_AZI_BEAM_NUM] = { 0 };
//{
//	-58, -54, -50, -46, -42, -38, -34, -30, -26, -22,
//	-18, -14, -10, -6, -2, 2, 6, 10, 14, 18,
//	22, 26, 30, 34, 38, 42, 46, 50, 54, 58
//};
// gAziBeamAngleSin = sinf(gAziBeamAngle * DEG2RAD), in radian
float gAziBeamAngleSin[DEFAULT_AZI_BEAM_NUM] = { 0 };
//{
//	-0.848f, -0.809f, -0.766f, -0.719f, -0.669f, -0.616f, -0.559f, -0.5f, -0.438f, -0.375f,
//	-0.309f, -0.242f, -0.174f, -0.105f, -0.035f, 0.035f, 0.105f, 0.174f, 0.242f, 0.309f,
//	0.375f, 0.438f, 0.5f, 0.559f, 0.616f, 0.669f, 0.719f, 0.766f, 0.809f, 0.848f
//};
// gAziBeamAngleSinInt16 = round(gAziBeamAngleSin * 32767)
int16_t gAziBeamAngleSinInt16[DEFAULT_AZI_BEAM_NUM] = { 0 };
//{
//	-27787, -26509, -25100, -23560, -21922, -20185, -18317, -16384, -14352, -12288,
//	-10125, -7930, -5702, -3441, -1147,	1147, 3441, 5702, 7930, 10125,
//	12288, 14352, 16384, 18317, 20185, 21922, 23560, 25100, 26509, 27787
//};
/* azimuth direction, search grid beam buffer */
int16_t gAziBeamNumGrid[GRID_SEARCH_ROUND_NUM] = { DEFAULT_AZI_BEAM_NUM, DEFAULT_AZI_BEAM_NUM,
			DEFAULT_AZI_BEAM_NUM, DEFAULT_AZI_BEAM_NUM, DEFAULT_AZI_BEAM_NUM, DEFAULT_AZI_BEAM_NUM };
uint16_t gAziBeamIndexGrid[GRID_SEARCH_ROUND_NUM][DEFAULT_AZI_BEAM_NUM] = { { 0 } };
int16_t gAziBeamAngleGrid[GRID_SEARCH_ROUND_NUM][DEFAULT_AZI_BEAM_NUM] = { { 0 } };
float gAziBeamAngleSinGrid[GRID_SEARCH_ROUND_NUM][DEFAULT_AZI_BEAM_NUM] = { { 0 } };
int16_t gAziBeamAngleSinInt16Grid[GRID_SEARCH_ROUND_NUM][DEFAULT_AZI_BEAM_NUM] = { { 0 } };

/* elevation direction beam */
int16_t gEleFov = DEFAULT_ELE_FOV;
int16_t gEleFovCenter = DEFAULT_ELE_FOV_CENTER;
int16_t gEleBeamNum = DEFAULT_ELE_BEAM_NUM;
uint16_t gEleBeamIndexCur = 0;
uint16_t gEleBeamIndexLast = 0;
// gEleBeamIndex = 0:1:3
uint16_t gEleBeamIndex[DEFAULT_ELE_BEAM_NUM] = { 0 };
//{ 0, 1, 2, 3 };
// gEleBeamAngle = -15:10:15, in degree
int16_t gEleBeamAngle[DEFAULT_ELE_BEAM_NUM] = { 0 };
//{ -15, -5, 5, 15 };
// gEleBeamAngleSin = sinf(gEleBeam * DEG2RAD), in radian
float gEleBeamAngleSin[DEFAULT_ELE_BEAM_NUM] = { 0 };
//{ -0.259f, -0.087f, 0.087f, 0.259f };
// gEleBeamAngleSinInt16 = round(gEleBeamAngleSin * 32767)
int16_t gEleBeamAngleSinInt16[DEFAULT_ELE_BEAM_NUM] = { 0 };
//{ -8487, -2851, 2851, 8487 };
/* elevation direction, search grid beam buffer */
int16_t gEleBeamNumGrid[GRID_SEARCH_ROUND_NUM] = { DEFAULT_ELE_BEAM_NUM, DEFAULT_ELE_BEAM_NUM,
		DEFAULT_ELE_BEAM_NUM, DEFAULT_ELE_BEAM_NUM, DEFAULT_ELE_BEAM_NUM, DEFAULT_ELE_BEAM_NUM };
uint16_t gEleBeamIndexGrid[GRID_SEARCH_ROUND_NUM][DEFAULT_ELE_BEAM_NUM] = { { 0 } };
int16_t gEleBeamAngleGrid[GRID_SEARCH_ROUND_NUM][DEFAULT_ELE_BEAM_NUM] = { { 0 } };
float gEleBeamAngleSinGrid[GRID_SEARCH_ROUND_NUM][DEFAULT_ELE_BEAM_NUM] = { { 0 } };
int16_t gEleBeamAngleSinInt16Grid[GRID_SEARCH_ROUND_NUM][DEFAULT_ELE_BEAM_NUM] = { { 0 } };

// default offset angle(down, up, left, right) of tas target azimuth angle, in degree
int16_t gTasAziOffsetAngle[TAS_CYCLE_TOTAL_BEAM_NUM] = { 0, 0, DEFAULT_TAS_AZI_OFFSET_LEFT, DEFAULT_TAS_AZI_OFFSET_RIGHT };
// default offset angle(down, up, left, right) of tas target elevation angle, in degree
int16_t gTasEleOffsetAngle[TAS_CYCLE_TOTAL_BEAM_NUM] = { DEFAULT_TAS_ELE_OFFSET_DOWN, DEFAULT_TAS_ELE_OFFSET_UP, 0, 0 };


/*========================================================================================
*                                    FUNCTIONS
=========================================================================================*/
void DispatchProcess(sAlgObjData *algObj)
{
	sDispatchInst *dispatch_inst = (sDispatchInst *)algObj->dispatch_inst;
	sTrackingInst *tracking_inst = (sTrackingInst *)algObj->tracking_inst;
	sTrackingFrame *tracksListOutput = tracking_inst->trajInfoOutput;
	sTracking_moduleInstance *instModule = (sTracking_moduleInstance *)tracking_inst->handle;

	if (-1 == gBeamParaCalcInit)
	{
		dispatch_inst->dispatchConfigParam->aziScanCenter = DEFAULT_AZI_FOV_CENTER;
		dispatch_inst->dispatchConfigParam->aziScanScope = DEFAULT_AZI_FOV;
		dispatch_inst->dispatchConfigParam->eleScanCenter = DEFAULT_ELE_FOV_CENTER;
		dispatch_inst->dispatchConfigParam->eleScanScope = DEFAULT_ELE_FOV;
		DispatchCfgParaUpdate(dispatch_inst->dispatchConfigParam);
		gBeamParaCalcInit = 0;
	}

	if (dispatch_inst->dispatchConfigParam->valid)
	{// valid
		if ((gAziFovCenter != dispatch_inst->dispatchConfigParam->aziScanCenter) ||
			(gAziFov != dispatch_inst->dispatchConfigParam->aziScanScope) ||
			(gEleFovCenter != dispatch_inst->dispatchConfigParam->eleScanCenter) ||
			(gEleFov != dispatch_inst->dispatchConfigParam->eleScanScope))
		{// if azimuth or elevation config params has changed
			DispatchCfgParaUpdate(dispatch_inst->dispatchConfigParam);
		}
		gAziFovCenter = dispatch_inst->dispatchConfigParam->aziScanCenter;
		gAziFov = dispatch_inst->dispatchConfigParam->aziScanScope;
		gEleFovCenter = dispatch_inst->dispatchConfigParam->eleScanCenter;
		gEleFov = dispatch_inst->dispatchConfigParam->eleScanScope;
	}

	// before diapatch process
	memcpy(dispatch_inst->dispatchOutputCurr, dispatch_inst->dispatchOutputNext, sizeof(sDispatchOutput));

	if (dispatch_inst->dispatchConfigParam->workMode == 0)
	{// TWS_SCAN, first elevation scan, then azimuth scan
		DispatchTwsScan(dispatch_inst, tracksListOutput);
	}
	else
	{// TAS_SCAN
		DispatchTasScan(dispatch_inst, tracksListOutput, instModule);
	}
	// after dispatch process
	memcpy(dispatch_inst->dispatchOutputNext, dispatch_inst->dispatchOutput, sizeof(sDispatchOutput));
	if (!g_byDummyTest)
	{
		memcpy(dispatch_inst->dispatchOutput, dispatch_inst->dispatchOutputCurr, sizeof(sDispatchOutput));
	}
}


void DispatchCfgParaUpdate(sDispatchConfigParam *param)
{
	uint16_t i = 0;
	int8_t aziScanCenter = 0;
	int8_t aziScanScope = 0;
	int8_t aziScanScopeLowerBound = 0;
	int8_t eleScanCenter = 0;
	int8_t eleScanScope = 0;
	int8_t eleScanScopeLowerBound = 0;

	if (SCAN_MODE)
	{
		DispatchCfgParaGridReset();
	}
	/* 1. update azimuth beam config params */
	aziScanCenter = param->aziScanCenter;
	if (((param->aziScanScope / 2) + (int8_t)(fabsf(aziScanCenter))) <= DEFAULT_AZI_FOV_UPPER)
	{
		aziScanScope = param->aziScanScope;
	}
	else
	{
		aziScanScope = (DEFAULT_AZI_FOV_UPPER - (int8_t)(fabsf(aziScanCenter))) * 2;
	}

	aziScanScopeLowerBound = aziScanCenter - (aziScanScope / 2);
	if (aziScanScope >= DEFAULT_AZI_BEAM_INTER)
	{
		gAziBeamAngle[0] = aziScanScopeLowerBound + DEFAULT_AZI_BEAM_INTER / 2;
	}
	else
	{
		gAziBeamAngle[0] = aziScanScopeLowerBound + aziScanScope / 2;
	}
	gAziBeamNum = (int8_t)(aziScanScope / DEFAULT_AZI_BEAM_INTER);
	if (gAziBeamNum < 1)
		gAziBeamNum = 1;
	for (i = 0; i < gAziBeamNum; i++)
	{
		gAziBeamIndex[i] = i;
		gAziBeamAngle[i] = gAziBeamAngle[0] + (i * DEFAULT_AZI_BEAM_INTER);
		gAziBeamAngleSin[i] = (float)(sinf(1.0 * gAziBeamAngle[i] * DEG2RAD));
		gAziBeamAngleSinInt16[i] = (int16_t)(gAziBeamAngleSin[i] * INT16_SCALE);
	}
	gAziBeamIndexCur = gAziBeamIndex[0];

	/* 2. update elevation beam config params */
	eleScanCenter = param->eleScanCenter;
	if (((param->eleScanScope / 2) + (int8_t)(fabsf(eleScanCenter))) <= DEFAULT_ELE_FOV_UPPER)
	{
		eleScanScope = param->eleScanScope;
	}
	else
	{
		eleScanScope = (DEFAULT_ELE_FOV_UPPER - (int8_t)(fabsf(eleScanCenter))) * 2;
	}

	eleScanScopeLowerBound = eleScanCenter - (eleScanScope / 2);
	if (eleScanScope >= DEFAULT_ELE_BEAM_INTER)
	{
		gEleBeamAngle[0] = eleScanScopeLowerBound + DEFAULT_ELE_BEAM_INTER / 2;
	}
	else
	{
		gEleBeamAngle[0] = eleScanScopeLowerBound + eleScanScope / 2;
	}
	gEleBeamNum = (int8_t)(eleScanScope / DEFAULT_ELE_BEAM_INTER);
	if (gEleBeamNum < 1)
		gEleBeamNum = 1;
	for (i = 0; i < gEleBeamNum; i++)
	{
		gEleBeamIndex[i] = i;
		gEleBeamAngle[i] = gEleBeamAngle[0] + (i * DEFAULT_ELE_BEAM_INTER);
		gEleBeamAngleSin[i] = (float)(sinf(1.0 * gEleBeamAngle[i] * DEG2RAD));
		gEleBeamAngleSinInt16[i] = (int16_t)(gEleBeamAngleSin[i] * INT16_SCALE);
	}
	gEleBeamIndexCur = gEleBeamIndex[0];
	if (SCAN_MODE)
	{
		DispatchCfgParaUpdateEachGrid();
	}
}


void DispatchTwsScan(sDispatchInst *dispatch_inst, sTrackingFrame *tracksListOutput)
{
	uint32_t gridRndIdx = 0;
	dispatch_inst->dispatchOutput->frameID = tracksListOutput->frameID;
	dispatch_inst->dispatchOutput->timestamp = tracksListOutput->timestamp;

	if (SCAN_BEAM_MODE == gBeamMode)
	{
		dispatch_inst->dispatchOutput->aziBeamID = gAziBeamIndexCur;
		dispatch_inst->dispatchOutput->eleBeamID = gEleBeamIndexCur;
		dispatch_inst->dispatchOutput->aziBeamSin = gAziBeamAngleSinInt16[gAziBeamIndexCur];
		dispatch_inst->dispatchOutput->eleBeamSin = gEleBeamAngleSinInt16[gEleBeamIndexCur];
		DispatchTwsOutputFinalBeamFlag(dispatch_inst);
	}
	else if (FIX_BEAM_MODE == gBeamMode)
	{
		dispatch_inst->dispatchOutput->aziBeamID = 15;
		dispatch_inst->dispatchOutput->eleBeamID = 2;
		dispatch_inst->dispatchOutput->aziBeamSin = \
			(int16_t)(sinf(1.0 * dispatch_inst->dispatchConfigParam->aziScanCenter * DEG2RAD) * INT16_SCALE);
		dispatch_inst->dispatchOutput->eleBeamSin = \
			(int16_t)(sinf(1.0 * dispatch_inst->dispatchConfigParam->eleScanCenter * DEG2RAD) * INT16_SCALE);
		dispatch_inst->dispatchOutput->wholeSpaceScanCycleFinalBeamFlag = 1;
	}
	else
	{
	}

	dispatch_inst->dispatchOutput->tasBeamTotal = 0;
	dispatch_inst->dispatchOutput->tasBeamCntCur = 0;
	dispatch_inst->dispatchOutput->tasObjId = INVALID_TARGET_ID;
	dispatch_inst->dispatchOutput->tasObjFilterNum = INVALID_FIELD_DATA;
	dispatch_inst->dispatchOutput->tasObjRange = INVALID_FIELD_DATA;
	dispatch_inst->dispatchOutput->samplePntStart = INVALID_FIELD_DATA;
	dispatch_inst->dispatchOutput->samplePntDepth = INVALID_FIELD_DATA;
	dispatch_inst->dispatchOutput->beamSwitchTime = 0;
	dispatch_inst->dispatchOutput->wholeSpaceScanCycleCnt = gWholeSpaceScanCycleCnt;
	dispatch_inst->dispatchOutput->trackTwsTasFlag = TWS_SCAN;

	DispatchTwsOutputWaveType(dispatch_inst);

	dispatch_inst->dispatchOutput->tasOrInitTrackFinalBeamFlag = false;
	dispatch_inst->dispatchOutput->tasAndInitTrackFinishBeamFlag = false;

	if (FIX_BEAM_MODE == gBeamMode)
		gWholeSpaceScanCycleCnt++;	// fix beam mode

#ifdef DEBUG_LOG_ZXL
	my_printf("********* DispatchTwsScan ********** azi, id:%d, a:%d aSin:%d,	ele, id:%d, a:%d aSin:%d,	azi:%.3f, ele:%.3f	waveType:%d\n", \
		dispatch_inst->dispatchOutput->aziBeamID, gAziBeamAngle[gAziBeamIndexCur], gAziBeamAngleSinInt16[gAziBeamIndexCur], \
		dispatch_inst->dispatchOutput->eleBeamID, gEleBeamAngle[gEleBeamIndexCur], gEleBeamAngleSinInt16[gEleBeamIndexCur], \
		asinf(1.0 * dispatch_inst->dispatchOutput->aziBeamSin / INT16_SCALE) * RAD2DEG, \
		asinf(1.0 * dispatch_inst->dispatchOutput->eleBeamSin / INT16_SCALE) * RAD2DEG, \
		dispatch_inst->dispatchOutput->waveType);
#endif // DEBUG_LOG_ZXL

	if (gEleBeamIndexCur < gEleBeamIndex[gEleBeamNum - 1])
	{// elevation beam scan first
		gEleBeamIndexCur++;
	}
	else
	{// elevation beam scan finished, reset gEleBeamIndexCur, and switch to next azimuth direction beam
		gEleBeamIndexCur = gEleBeamIndex[0];
		gAziBeamIndexCur++;
		if (gAziBeamIndexCur > gAziBeamIndex[gAziBeamNum - 1])
		{// azimuth beam scan finished, reset gAziBeamIndexCur, and switch to next azimuth direction beam
			gAziBeamIndexCur = gAziBeamIndex[0];
			if (SCAN_BEAM_MODE == gBeamMode)
			{
				gWholeSpaceScanCycleCnt++;	// scan beam mode
				if (SCAN_MODE)
				{
					gridRndIdx = gWholeSpaceScanCycleCnt % GRID_SEARCH_ROUND_NUM;
					gAziBeamNum = gAziBeamNumGrid[gridRndIdx];
					memcpy(&gAziBeamIndex[0], &gAziBeamIndexGrid[gridRndIdx][0], sizeof(gAziBeamIndex[0]) * gAziBeamNumGrid[gridRndIdx]);
					memcpy(&gAziBeamAngle[0], &gAziBeamAngleGrid[gridRndIdx][0], sizeof(gAziBeamAngle[0]) * gAziBeamNumGrid[gridRndIdx]);
					memcpy(&gAziBeamAngleSin[0], &gAziBeamAngleSinGrid[gridRndIdx][0], sizeof(gAziBeamAngleSin[0]) * gAziBeamNumGrid[gridRndIdx]);
					memcpy(&gAziBeamAngleSinInt16[0], &gAziBeamAngleSinInt16Grid[gridRndIdx][0], sizeof(gAziBeamAngleSinInt16[0]) * gAziBeamNumGrid[gridRndIdx]);
					gEleBeamNum = gEleBeamNumGrid[gridRndIdx];
					memcpy(&gEleBeamIndex[0], &gEleBeamIndexGrid[gridRndIdx][0], sizeof(gEleBeamIndex[0]) * gEleBeamNumGrid[gridRndIdx]);
					memcpy(&gEleBeamAngle[0], &gEleBeamAngleGrid[gridRndIdx][0], sizeof(gEleBeamAngle[0]) * gEleBeamNumGrid[gridRndIdx]);
					memcpy(&gEleBeamAngleSin[0], &gEleBeamAngleSinGrid[gridRndIdx][0], sizeof(gEleBeamAngleSin[0]) * gEleBeamNumGrid[gridRndIdx]);
					memcpy(&gEleBeamAngleSinInt16[0], &gEleBeamAngleSinInt16Grid[gridRndIdx][0], sizeof(gEleBeamAngleSinInt16[0]) * gEleBeamNumGrid[gridRndIdx]);
				}
			}
		}
	}
}


void DispatchTasScan(sDispatchInst *dispatch_inst, sTrackingFrame *tracksListOutput, sTracking_moduleInstance *inst)
{
	sDispatchOutput dispatchOutputTmp = { 0 };

	/* 1. start and get tasTrackCurrentCycle in current tas scan cycle */
	if ((gTasScanBeamCnt % TAS_CYCLE_BEAM_NUM) == 0)
	{	/* 1.1 GetTasTrackCurrentCycle  */
		if (tracking_listGetCount(&inst->tasTrackList))
		{
			dispatch_inst->dispatchTasTrackInfo->tasDispatchFlag = true;
			dispatch_inst->dispatchTasTrackInfo->trackTasProcCnt = 0;
			dispatch_inst->dispatchTasTrackInfo->tasScanBeamInCycleCnt = 0;
			memset(dispatch_inst->dispatchTasTrackInfo->tasTrackCurrentCycle, 0, sizeof(sDispatchTrackInfo) * TAS_REAL_TARGET_MAX_NUM);
			GetTasTrackCurrentCycle(dispatch_inst, inst);
		}
		else
		{
			dispatch_inst->dispatchTasTrackInfo->tasDispatchFlag = false;
		}
		
		/*if (gVelunambigType != NO_UNAMBIGIOUS)
		{*/
		/* 1.2 GetInitTrackCurrentCycle */
		if (GetTempTrackNum(inst))
		{
			dispatch_inst->dispatchInitTrackInfo->tasDispatchFlag = true;
			dispatch_inst->dispatchInitTrackInfo->trackTasProcCnt = 0;
			dispatch_inst->dispatchInitTrackInfo->tasScanBeamInCycleCnt = 0;
			memset(dispatch_inst->dispatchInitTrackInfo->tasTrackCurrentCycle, 0, sizeof(sDispatchTrackInfo) * INIT_REAL_TARGET_MAX_NUM);
			GetInitTrackCurrentCycle(dispatch_inst, inst);
		}
		else
		{
			dispatch_inst->dispatchInitTrackInfo->tasDispatchFlag = false;
		}
		/*}*/

		gTasScanCycleCnt++;
	}

	/* 2. beam scan for all the trackInitNum and trackTasNum tas target, then go on normal search */
	if ((dispatch_inst->dispatchInitTrackInfo->tasDispatchFlag == true) && \
		(dispatch_inst->dispatchInitTrackInfo->trackTasProcCnt < dispatch_inst->dispatchInitTrackInfo->trackInitNum))
	{// 2.1 confirm beam for all the trackInitNum tas target
		InitTrackBeamProcAndOut(dispatch_inst, tracksListOutput, inst);
	}
	else if ((dispatch_inst->dispatchInitTrackInfo->tasDispatchFlag == false) && \
		(dispatch_inst->dispatchTasTrackInfo->tasDispatchFlag == true) && \
		(dispatch_inst->dispatchTasTrackInfo->trackTasProcCnt < dispatch_inst->dispatchTasTrackInfo->trackTasNum))
	{// 2.2 continuous 4(down/up/left/right) beam scan for all the trackTasNum tas target
		TasTrackBeamProcAndOut(dispatch_inst, tracksListOutput, inst);
	}
	else
	{// 2.3 go on normal search
		TasScanNormalSearchBeamProcAndOut(dispatch_inst, tracksListOutput);
	}

	gTasScanBeamCnt++;

#ifdef DEBUG_LOG_ZXL
	my_printf("********* DispatchTasScan ********** azi, id:%d, a:%d aSin:%d,	ele, id:%d, e:%d eSin:%d,	azi:%.3f, ele:%.3f	waveType:%d\n", \
		dispatch_inst->dispatchOutput->aziBeamID, gAziBeamAngle[gAziBeamIndexCur], gAziBeamAngleSinInt16[gAziBeamIndexCur], \
		dispatch_inst->dispatchOutput->eleBeamID, gEleBeamAngle[gEleBeamIndexCur], gEleBeamAngleSinInt16[gEleBeamIndexCur], \
		asinf(1.0 * dispatch_inst->dispatchOutput->aziBeamSin / INT16_SCALE) * RAD2DEG, \
		asinf(1.0 * dispatch_inst->dispatchOutput->eleBeamSin / INT16_SCALE) * RAD2DEG, \
		dispatch_inst->dispatchOutput->waveType);

	my_printf("scanFlag:%d,  finalBeamFlag:%d  id:%d, total:%d, cur:%d,	range:%.3f, azi:%.3f, ele:%.3f\n", \
		dispatch_inst->dispatchOutput->trackTwsTasFlag, dispatch_inst->dispatchOutput->wholeSpaceScanCycleFinalBeamFlag, \
		dispatch_inst->dispatchOutput->tasObjId, \
		dispatch_inst->dispatchOutput->tasBeamTotal, dispatch_inst->dispatchOutput->tasBeamCntCur, \
		(float)(dispatch_inst->dispatchOutput->tasObjRange * DEFAULT_RANGE_UNIT_M), \
		asinf(1.0 * dispatch_inst->dispatchOutput->aziBeamSin / INT16_SCALE) * RAD2DEG, \
		asinf(1.0 * dispatch_inst->dispatchOutput->eleBeamSin / INT16_SCALE) * RAD2DEG);
#endif // DEBUG_LOG_ZXL
}


void GetTasTrackCurrentCycle(sDispatchInst *dispatch_inst, sTracking_moduleInstance *inst)
{
	uint16_t i = 0;
	uint16_t uid = 0;
	sTracking_ListElem *tElem = NULL;
	sTracking_objectUnit *pTracker;
	float statePredict[SSIZE_XY] = { 0.f };
	sTracking_measurementSphUnion trackerPolar;
	float aziOffset[TAS_CYCLE_TOTAL_BEAM_NUM] = { 0.f };
	float eleOffset[TAS_CYCLE_TOTAL_BEAM_NUM] = { 0.f };
	float P_hat[SSIZE_XY * SSIZE_XY];
	// float F[SSIZE_XY * SSIZE_XY];
	// float Q[SSIZE_XY * SSIZE_XY];
	// float temp1[SSIZE_XY * SSIZE_XY] = { 0.f };
	// float temp3[SSIZE_XY * SSIZE_XY] = { 0.f };

	/* get trackTasNum */
	if (TAS_REAL_TARGET_MAX_NUM <= tracking_listGetCount(&inst->tasTrackList))
	{
		dispatch_inst->dispatchTasTrackInfo->trackTasNum = TAS_REAL_TARGET_MAX_NUM;
	}
	else
	{
		dispatch_inst->dispatchTasTrackInfo->trackTasNum = tracking_listGetCount(&inst->tasTrackList);
	}

	/* get tasTrackCurrentCycle */
	if (dispatch_inst->dispatchTasTrackInfo->trackTasNum > 0)
	{
		tElem = tracking_listGetFirst(&inst->tasTrackList);
		while (tElem != 0)
		{
			uid = tElem->data;
			pTracker = (sTracking_objectUnit*)inst->hTrack[uid];
			tracking_cal_FQ(inst, g_curTimestamp + DEFAULT_CPI_MS / 1000.f - pTracker->timestamp, pTracker);
			tracking_matrixMultiply(SSIZE_XY, SSIZE_XY, 1, inst->tracking_params->F, \
				pTracker->kalman_state.S_hat, statePredict);
			tracking_cartesian2spherical(inst->tracking_params->stateVectorType, \
				statePredict, trackerPolar.array);

			// memcpy(F, pTracker->tracking_params->F, sizeof(pTracker->tracking_params->F));
			// memcpy(Q, pTracker->tracking_params->Q, sizeof(pTracker->tracking_params->Q));
			// tracking_matrixMultiply(SSIZE_XY, SSIZE_XY, SSIZE_XY, F, pTracker->kalman_state.P_hat, temp1);
			// tracking_matrixTransposeMultiply(SSIZE_XY, SSIZE_XY, SSIZE_XY, temp1, F, temp3);
			// tracking_matrixAdd(SSIZE_XY, SSIZE_XY, temp3, Q, temp1);
			// tracking_matrixMakeSymmetrical(SSIZE_XY, temp1, P_hat);

			memcpy(P_hat, pTracker->kalman_state.P_hat, sizeof(pTracker->kalman_state.P_hat));
			GetTasTrackOffsetAngle(statePredict, P_hat, trackerPolar, aziOffset, eleOffset, inst);

			dispatch_inst->dispatchTasTrackInfo->tasTrackCurrentCycle[i].timestamp = pTracker->timestamp;
			dispatch_inst->dispatchTasTrackInfo->tasTrackCurrentCycle[i].id = pTracker->uid;
			dispatch_inst->dispatchTasTrackInfo->tasTrackCurrentCycle[i].tid = pTracker->tid;
			dispatch_inst->dispatchTasTrackInfo->tasTrackCurrentCycle[i].waveType = pTracker->assoProperty.waveInfo;
			dispatch_inst->dispatchTasTrackInfo->tasTrackCurrentCycle[i].range = trackerPolar.vector.range;
			dispatch_inst->dispatchTasTrackInfo->tasTrackCurrentCycle[i].azimuth = trackerPolar.vector.azimuthRad;
			dispatch_inst->dispatchTasTrackInfo->tasTrackCurrentCycle[i].elevation = trackerPolar.vector.pitchRad;
			dispatch_inst->dispatchTasTrackInfo->tasTrackCurrentCycle[i].velocity = trackerPolar.vector.doppler;
			memcpy(dispatch_inst->dispatchTasTrackInfo->tasTrackCurrentCycle[i].aziOffset, aziOffset, sizeof(aziOffset));
			memcpy(dispatch_inst->dispatchTasTrackInfo->tasTrackCurrentCycle[i].eleOffset, eleOffset, sizeof(eleOffset));
			tElem = tracking_listGetNext(tElem);
			i++;
			if (TAS_REAL_TARGET_MAX_NUM == i)
			{
				break;
			}
		}
	}
}


void GetInitTrackCurrentCycle(sDispatchInst *dispatch_inst, sTracking_moduleInstance *inst)
{
	uint16_t i = 0;
	uint16_t uid = 0;
	sTracking_objectUnit *pTracker;
	float statePredict[SSIZE_XY] = { 0 };
	sTracking_measurementSphUnion trackerPolar;

	/* get trackInitNum */
	if (INIT_REAL_TARGET_MAX_NUM <= GetTempTrackNum(inst))
	{
		dispatch_inst->dispatchInitTrackInfo->trackInitNum = INIT_REAL_TARGET_MAX_NUM;
	}
	else
	{
		dispatch_inst->dispatchInitTrackInfo->trackInitNum = GetTempTrackNum(inst);
	}

	/* get initTrackCurrentCycle */
	if (dispatch_inst->dispatchInitTrackInfo->trackInitNum > 0)
	{
		for (i = 0; i < dispatch_inst->dispatchInitTrackInfo->trackInitNum; i++)
		{
			uid = inst->confirmBeamId[i];
			pTracker = (sTracking_objectUnit *)inst->hTrack[uid];
			//tracking_cal_FQ(inst, g_curTimestamp + DEFAULT_CPI_MS / 1000.f - pTracker->timestamp);
			//tracking_matrixMultiply(SSIZE_XY, SSIZE_XY, 1, inst->tracking_params->F, \
				pTracker->kalman_state.S_hat, statePredict);
			tracking_cartesian2spherical(inst->tracking_params->stateVectorType, \
				pTracker->kalman_state.S_hat, trackerPolar.array);
			dispatch_inst->dispatchInitTrackInfo->tasTrackCurrentCycle[i].timestamp = pTracker->timestamp;
			dispatch_inst->dispatchInitTrackInfo->tasTrackCurrentCycle[i].id = pTracker->uid;
			dispatch_inst->dispatchInitTrackInfo->tasTrackCurrentCycle[i].tid = pTracker->tid;
			dispatch_inst->dispatchInitTrackInfo->tasTrackCurrentCycle[i].waveType = pTracker->assoProperty.waveInfo;
			dispatch_inst->dispatchInitTrackInfo->tasTrackCurrentCycle[i].range = trackerPolar.vector.range;
			dispatch_inst->dispatchInitTrackInfo->tasTrackCurrentCycle[i].azimuth = trackerPolar.vector.azimuthRad;
			dispatch_inst->dispatchInitTrackInfo->tasTrackCurrentCycle[i].elevation = trackerPolar.vector.pitchRad;
			dispatch_inst->dispatchInitTrackInfo->tasTrackCurrentCycle[i].velocity = trackerPolar.vector.doppler;
		}
	}
}

void TasTrackBeamProcAndOut(sDispatchInst *dispatch_inst, sTrackingFrame *tracksListOutput, sTracking_moduleInstance *inst)
{
	sDispatchOutput dispatchOutputTmp = { 0 };

	if (dispatch_inst->dispatchTasTrackInfo->tasScanBeamInCycleCnt < TAS_CYCLE_TOTAL_BEAM_NUM)
	{
		dispatchOutputTmp.aziBeamID = INVALID_FIELD_DATA;
		dispatchOutputTmp.eleBeamID = INVALID_FIELD_DATA;
		dispatchOutputTmp.aziBeamSin = (int16_t)(sinf(dispatch_inst->dispatchTasTrackInfo->tasTrackCurrentCycle[dispatch_inst->dispatchTasTrackInfo->trackTasProcCnt].azimuth \
			+ dispatch_inst->dispatchTasTrackInfo->tasTrackCurrentCycle[dispatch_inst->dispatchTasTrackInfo->trackTasProcCnt].aziOffset[dispatch_inst->dispatchTasTrackInfo->tasScanBeamInCycleCnt] * DEG2RAD) * INT16_SCALE);
		dispatchOutputTmp.eleBeamSin = (int16_t)(sinf(dispatch_inst->dispatchTasTrackInfo->tasTrackCurrentCycle[dispatch_inst->dispatchTasTrackInfo->trackTasProcCnt].elevation \
			+ dispatch_inst->dispatchTasTrackInfo->tasTrackCurrentCycle[dispatch_inst->dispatchTasTrackInfo->trackTasProcCnt].eleOffset[dispatch_inst->dispatchTasTrackInfo->tasScanBeamInCycleCnt] * DEG2RAD) * INT16_SCALE);
		dispatchOutputTmp.tasBeamTotal = TAS_CYCLE_TOTAL_BEAM_NUM;
		dispatchOutputTmp.tasBeamCntCur = dispatch_inst->dispatchTasTrackInfo->tasScanBeamInCycleCnt + 1;
		dispatchOutputTmp.tasObjId = dispatch_inst->dispatchTasTrackInfo->tasTrackCurrentCycle[dispatch_inst->dispatchTasTrackInfo->trackTasProcCnt].id;
		dispatchOutputTmp.tasObjFilterNum = INVALID_FIELD_DATA;
		dispatchOutputTmp.tasObjRange = dispatch_inst->dispatchTasTrackInfo->tasTrackCurrentCycle[dispatch_inst->dispatchTasTrackInfo->trackTasProcCnt].tid;
		//dispatchOutputTmp.tasObjRange = (uint16_t)(dispatch_inst->dispatchTasTrackInfo->tasTrackCurrentCycle[dispatch_inst->dispatchTasTrackInfo->trackTasProcCnt].range / DEFAULT_RANGE_UNIT_M);
		dispatchOutputTmp.samplePntStart = DEFAULT_SAMPLE_PNT_START;
		dispatchOutputTmp.samplePntDepth = DEFAULT_SAMPLE_PNT_DEPTH;
		dispatchOutputTmp.beamSwitchTime = DEFAULT_BEAM_SWITCH_TIME;

		TrackBeamProcAndOut(dispatch_inst, tracksListOutput, dispatchOutputTmp);
		DispatchTwsOutputWaveType(dispatch_inst);

		dispatch_inst->dispatchOutput->wholeSpaceScanCycleFinalBeamFlag = false;
		dispatch_inst->dispatchOutput->tasAndInitTrackFinishBeamFlag = false;
		
		dispatch_inst->dispatchTasTrackInfo->tasScanBeamInCycleCnt++;
		if (dispatch_inst->dispatchTasTrackInfo->tasScanBeamInCycleCnt == TAS_CYCLE_TOTAL_BEAM_NUM)
		{
			dispatch_inst->dispatchTasTrackInfo->tasScanBeamInCycleCnt = 0;
			dispatch_inst->dispatchTasTrackInfo->trackTasProcCnt++;
			if (dispatch_inst->dispatchTasTrackInfo->trackTasProcCnt == dispatch_inst->dispatchTasTrackInfo->trackTasNum)
			{
				dispatch_inst->dispatchTasTrackInfo->trackTasProcCnt = 0;
				dispatch_inst->dispatchTasTrackInfo->tasDispatchFlag = false;
			}
			dispatch_inst->dispatchOutput->tasOrInitTrackFinalBeamFlag = true;
		}
		else
		{
			dispatch_inst->dispatchOutput->tasOrInitTrackFinalBeamFlag = false;
		}
	}
}


void InitTrackBeamProcAndOut(sDispatchInst *dispatch_inst, sTrackingFrame *tracksListOutput, sTracking_moduleInstance *inst)
{
	sDispatchOutput dispatchOutputTmp = { 0 };
	
	if (dispatch_inst->dispatchInitTrackInfo->tasScanBeamInCycleCnt < INIT_CYCLE_TOTAL_BEAM_NUM)
	{
		dispatchOutputTmp.aziBeamID = INVALID_FIELD_DATA;
		dispatchOutputTmp.eleBeamID = INVALID_FIELD_DATA;
		dispatchOutputTmp.aziBeamSin = (int16_t)(sinf(dispatch_inst->dispatchInitTrackInfo->tasTrackCurrentCycle[dispatch_inst->dispatchInitTrackInfo->trackTasProcCnt].azimuth) * INT16_SCALE);
		dispatchOutputTmp.eleBeamSin = (int16_t)(sinf(dispatch_inst->dispatchInitTrackInfo->tasTrackCurrentCycle[dispatch_inst->dispatchInitTrackInfo->trackTasProcCnt].elevation) * INT16_SCALE);
		dispatchOutputTmp.tasBeamTotal = INIT_CYCLE_TOTAL_BEAM_NUM;
		dispatchOutputTmp.tasBeamCntCur = dispatch_inst->dispatchInitTrackInfo->tasScanBeamInCycleCnt + 1;
		dispatchOutputTmp.tasObjId = dispatch_inst->dispatchInitTrackInfo->tasTrackCurrentCycle[dispatch_inst->dispatchInitTrackInfo->trackTasProcCnt].id;
		dispatchOutputTmp.tasObjFilterNum = INVALID_FIELD_DATA;
		dispatchOutputTmp.tasObjRange = dispatch_inst->dispatchInitTrackInfo->tasTrackCurrentCycle[dispatch_inst->dispatchInitTrackInfo->trackTasProcCnt].tid;
		//dispatchOutputTmp.tasObjRange = (uint16_t)(dispatch_inst->dispatchInitTrackInfo->tasTrackCurrentCycle[dispatch_inst->dispatchInitTrackInfo->trackTasProcCnt].range / DEFAULT_RANGE_UNIT_M);
		dispatchOutputTmp.samplePntStart = DEFAULT_SAMPLE_PNT_START;
		dispatchOutputTmp.samplePntDepth = DEFAULT_SAMPLE_PNT_DEPTH;
		dispatchOutputTmp.beamSwitchTime = DEFAULT_BEAM_SWITCH_TIME;

		TrackBeamProcAndOut(dispatch_inst, tracksListOutput, dispatchOutputTmp);
		dispatch_inst->dispatchOutput->waveType = ((dispatchOutputTmp.waveType + 1) % 2);
		dispatch_inst->dispatchOutput->wholeSpaceScanCycleFinalBeamFlag = false;
		dispatch_inst->dispatchOutput->tasAndInitTrackFinishBeamFlag = false;

		dispatch_inst->dispatchInitTrackInfo->tasScanBeamInCycleCnt++;
		if (dispatch_inst->dispatchInitTrackInfo->tasScanBeamInCycleCnt == INIT_CYCLE_TOTAL_BEAM_NUM)
		{
			dispatch_inst->dispatchInitTrackInfo->tasScanBeamInCycleCnt = 0;
			dispatch_inst->dispatchInitTrackInfo->trackTasProcCnt++;
			if (dispatch_inst->dispatchInitTrackInfo->trackTasProcCnt == dispatch_inst->dispatchInitTrackInfo->trackInitNum)
			{
				dispatch_inst->dispatchInitTrackInfo->trackTasProcCnt = 0;
				dispatch_inst->dispatchInitTrackInfo->tasDispatchFlag = false;
				dispatch_inst->dispatchOutput->tasAndInitTrackFinishBeamFlag = true;
			}
			dispatch_inst->dispatchOutput->tasOrInitTrackFinalBeamFlag = true;
		}
		else
		{
			dispatch_inst->dispatchOutput->tasOrInitTrackFinalBeamFlag = false;
		}
	}
}


void TasScanNormalSearchBeamProcAndOut(sDispatchInst *dispatch_inst, sTrackingFrame *tracksListOutput)
{
	uint32_t gridRndIdx = 0;
	if (gEleBeamIndexCur < gEleBeamIndex[gEleBeamNum - 1])
	{// elevation beam scan first
		gEleBeamIndexCur++;
	}
	else
	{// elevation beam scan finished, reset gEleBeamIndexCur, and switch to next azimuth direction beam
		gEleBeamIndexCur = gEleBeamIndex[0];
		gAziBeamIndexCur++;
		if (gAziBeamIndexCur > gAziBeamIndex[gAziBeamNum - 1])
		{// azimuth beam scan finished, reset gAziBeamIndexCur, and switch to next azimuth direction beam
			gAziBeamIndexCur = gAziBeamIndex[0];
			gWholeSpaceScanCycleCnt++;
			if (SCAN_MODE)
			{
				gridRndIdx = gWholeSpaceScanCycleCnt % GRID_SEARCH_ROUND_NUM;
				gAziBeamNum = gAziBeamNumGrid[gridRndIdx];
				memcpy(&gAziBeamIndex[0], &gAziBeamIndexGrid[gridRndIdx][0], sizeof(gAziBeamIndex[0]) * gAziBeamNumGrid[gridRndIdx]);
				memcpy(&gAziBeamAngle[0], &gAziBeamAngleGrid[gridRndIdx][0], sizeof(gAziBeamAngle[0]) * gAziBeamNumGrid[gridRndIdx]);
				memcpy(&gAziBeamAngleSin[0], &gAziBeamAngleSinGrid[gridRndIdx][0], sizeof(gAziBeamAngleSin[0]) * gAziBeamNumGrid[gridRndIdx]);
				memcpy(&gAziBeamAngleSinInt16[0], &gAziBeamAngleSinInt16Grid[gridRndIdx][0], sizeof(gAziBeamAngleSinInt16[0]) * gAziBeamNumGrid[gridRndIdx]);
				gEleBeamNum = gEleBeamNumGrid[gridRndIdx];
				memcpy(&gEleBeamIndex[0], &gEleBeamIndexGrid[gridRndIdx][0], sizeof(gEleBeamIndex[0]) * gEleBeamNumGrid[gridRndIdx]);
				memcpy(&gEleBeamAngle[0], &gEleBeamAngleGrid[gridRndIdx][0], sizeof(gEleBeamAngle[0]) * gEleBeamNumGrid[gridRndIdx]);
				memcpy(&gEleBeamAngleSin[0], &gEleBeamAngleSinGrid[gridRndIdx][0], sizeof(gEleBeamAngleSin[0]) * gEleBeamNumGrid[gridRndIdx]);
				memcpy(&gEleBeamAngleSinInt16[0], &gEleBeamAngleSinInt16Grid[gridRndIdx][0], sizeof(gEleBeamAngleSinInt16[0]) * gEleBeamNumGrid[gridRndIdx]);
			}
		}
	}

	dispatch_inst->dispatchOutput->frameID = tracksListOutput->frameID;
	dispatch_inst->dispatchOutput->timestamp = tracksListOutput->timestamp;
	dispatch_inst->dispatchOutput->aziBeamID = gAziBeamIndexCur;
	dispatch_inst->dispatchOutput->eleBeamID = gEleBeamIndexCur;
	dispatch_inst->dispatchOutput->aziBeamSin = gAziBeamAngleSinInt16[gAziBeamIndexCur];
	dispatch_inst->dispatchOutput->eleBeamSin = gEleBeamAngleSinInt16[gEleBeamIndexCur];
	dispatch_inst->dispatchOutput->tasBeamTotal = 0;
	dispatch_inst->dispatchOutput->tasBeamCntCur = 0;
	dispatch_inst->dispatchOutput->tasObjId = INVALID_TARGET_ID;
	dispatch_inst->dispatchOutput->tasObjFilterNum = INVALID_FIELD_DATA;
	dispatch_inst->dispatchOutput->tasObjRange = INVALID_FIELD_DATA;
	dispatch_inst->dispatchOutput->samplePntStart = INVALID_FIELD_DATA;
	dispatch_inst->dispatchOutput->samplePntDepth = INVALID_FIELD_DATA;
	dispatch_inst->dispatchOutput->beamSwitchTime = 0;
	dispatch_inst->dispatchOutput->wholeSpaceScanCycleCnt = gWholeSpaceScanCycleCnt;
	dispatch_inst->dispatchOutput->trackTwsTasFlag = TWS_SCAN;

	DispatchTwsOutputWaveType(dispatch_inst);
	DispatchTwsOutputFinalBeamFlag(dispatch_inst);

	dispatch_inst->dispatchOutput->tasOrInitTrackFinalBeamFlag = false;
	dispatch_inst->dispatchOutput->tasAndInitTrackFinishBeamFlag = false;
}


void DispatchCfgParaGridReset(void)
{
	memset(&gAziBeamNumGrid[0], 0, sizeof(gAziBeamNumGrid[0]) * GRID_SEARCH_ROUND_NUM);
	memset(&gEleBeamNumGrid[0], 0, sizeof(gEleBeamNumGrid[0]) * GRID_SEARCH_ROUND_NUM);
	memset(&gAziBeamIndexGrid[0][0], 0, sizeof(gAziBeamIndexGrid[0][0]) * GRID_SEARCH_ROUND_NUM * DEFAULT_AZI_BEAM_NUM);
	memset(&gAziBeamAngleGrid[0][0], 0, sizeof(gAziBeamAngleGrid[0][0]) * GRID_SEARCH_ROUND_NUM * DEFAULT_AZI_BEAM_NUM);
	memset(&gAziBeamAngleSinGrid[0][0], 0, sizeof(gAziBeamAngleSinGrid[0][0]) * GRID_SEARCH_ROUND_NUM * DEFAULT_AZI_BEAM_NUM);
	memset(&gAziBeamAngleSinInt16Grid[0][0], 0, sizeof(gAziBeamAngleSinInt16Grid[0][0]) * GRID_SEARCH_ROUND_NUM * DEFAULT_AZI_BEAM_NUM);
	memset(&gEleBeamIndexGrid[0][0], 0, sizeof(gEleBeamIndexGrid[0][0]) * GRID_SEARCH_ROUND_NUM * DEFAULT_ELE_BEAM_NUM);
	memset(&gEleBeamAngleGrid[0][0], 0, sizeof(gEleBeamAngleGrid[0][0]) * GRID_SEARCH_ROUND_NUM * DEFAULT_ELE_BEAM_NUM);
	memset(&gEleBeamAngleSinGrid[0][0], 0, sizeof(gEleBeamAngleSinGrid[0][0]) * GRID_SEARCH_ROUND_NUM * DEFAULT_ELE_BEAM_NUM);
	memset(&gEleBeamAngleSinInt16Grid[0][0], 0, sizeof(gEleBeamAngleSinInt16Grid[0][0]) * GRID_SEARCH_ROUND_NUM * DEFAULT_ELE_BEAM_NUM);
}

void DispatchCfgParaUpdateEachGrid(void)
{
	uint16_t i = 0;

	gAziBeamNumGrid[0] = gAziBeamNum;
	gAziBeamNumGrid[1] = gAziBeamNum - 1;
	gAziBeamNumGrid[2] = gAziBeamNum;
	gAziBeamNumGrid[3] = gAziBeamNum - 1;
	gAziBeamNumGrid[4] = gAziBeamNum;
	gAziBeamNumGrid[5] = gAziBeamNum - 1;
	gEleBeamNumGrid[0] = gEleBeamNum;
	gEleBeamNumGrid[1] = gEleBeamNum;
	gEleBeamNumGrid[2] = gEleBeamNum - 1;
	gEleBeamNumGrid[3] = gEleBeamNum - 1;
	gEleBeamNumGrid[4] = gEleBeamNum - 1;
	gEleBeamNumGrid[5] = gEleBeamNum - 1;

	// round 0
	memcpy(&gAziBeamIndexGrid[0][0], &gAziBeamIndex[0], sizeof(gAziBeamIndex[0]) * gAziBeamNumGrid[0]);
	memcpy(&gAziBeamAngleGrid[0][0], &gAziBeamAngle[0], sizeof(gAziBeamAngle[0]) * gAziBeamNumGrid[0]);
	memcpy(&gAziBeamAngleSinGrid[0][0], &gAziBeamAngleSin[0], sizeof(gAziBeamAngleSin[0]) * gAziBeamNumGrid[0]);
	memcpy(&gAziBeamAngleSinInt16Grid[0][0], &gAziBeamAngleSinInt16[0], sizeof(gAziBeamAngleSinInt16[0]) * gAziBeamNumGrid[0]);
	memcpy(&gEleBeamIndexGrid[0][0], &gEleBeamIndex[0], sizeof(gEleBeamIndex[0]) * gEleBeamNumGrid[0]);
	memcpy(&gEleBeamAngleGrid[0][0], &gEleBeamAngle[0], sizeof(gEleBeamAngle[0]) * gEleBeamNumGrid[0]);
	memcpy(&gEleBeamAngleSinGrid[0][0], &gEleBeamAngleSin[0], sizeof(gEleBeamAngleSin[0]) * gEleBeamNumGrid[0]);
	memcpy(&gEleBeamAngleSinInt16Grid[0][0], &gEleBeamAngleSinInt16[0], sizeof(gEleBeamAngleSinInt16[0]) * gEleBeamNumGrid[0]);

	// round 1
	for (i = 0; i < gAziBeamNumGrid[1]; i++)
	{
		gAziBeamIndexGrid[1][i] = gAziBeamIndex[i];
		gAziBeamAngleGrid[1][i] = gAziBeamAngle[i] + GRID_SEARCH_BEAM_INTER;
		gAziBeamAngleSinGrid[1][i] = (float)(sinf(1.0 * gAziBeamAngleGrid[1][i] * DEG2RAD));
		gAziBeamAngleSinInt16Grid[1][i] = (int16_t)(gAziBeamAngleSinGrid[1][i] * INT16_SCALE);
	}	
	memcpy(&gEleBeamIndexGrid[1][0], &gEleBeamIndex[0], sizeof(gEleBeamIndex[0]) * gEleBeamNumGrid[1]);
	memcpy(&gEleBeamAngleGrid[1][0], &gEleBeamAngle[0], sizeof(gEleBeamAngle[0]) * gEleBeamNumGrid[1]);
	memcpy(&gEleBeamAngleSinGrid[1][0], &gEleBeamAngleSin[0], sizeof(gEleBeamAngleSin[0]) * gEleBeamNumGrid[1]);
	memcpy(&gEleBeamAngleSinInt16Grid[1][0], &gEleBeamAngleSinInt16[0], sizeof(gEleBeamAngleSinInt16[0]) * gEleBeamNumGrid[1]);

	// round 2
	memcpy(&gAziBeamIndexGrid[2][0], &gAziBeamIndex[0], sizeof(gAziBeamIndex[0]) * gAziBeamNumGrid[2]);
	memcpy(&gAziBeamAngleGrid[2][0], &gAziBeamAngle[0], sizeof(gAziBeamAngle[0]) * gAziBeamNumGrid[2]);
	memcpy(&gAziBeamAngleSinGrid[2][0], &gAziBeamAngleSin[0], sizeof(gAziBeamAngleSin[0]) * gAziBeamNumGrid[2]);
	memcpy(&gAziBeamAngleSinInt16Grid[2][0], &gAziBeamAngleSinInt16[0], sizeof(gAziBeamAngleSinInt16[0]) * gAziBeamNumGrid[2]);
	for (i = 0; i < gEleBeamNumGrid[2]; i++)
	{
		gEleBeamIndexGrid[2][i] = gEleBeamIndex[i];
		gEleBeamAngleGrid[2][i] = gEleBeamAngle[i] + GRID_SEARCH_BEAM_INTER * 2;
		gEleBeamAngleSinGrid[2][i] = (float)(sinf(1.0 * gEleBeamAngleGrid[2][i] * DEG2RAD));
		gEleBeamAngleSinInt16Grid[2][i] = (int16_t)(gEleBeamAngleSinGrid[2][i] * INT16_SCALE);
	}

	// round 3
	for (i = 0; i < gAziBeamNumGrid[3]; i++)
	{
		gAziBeamIndexGrid[3][i] = gAziBeamIndex[i];
		gAziBeamAngleGrid[3][i] = gAziBeamAngle[i] + GRID_SEARCH_BEAM_INTER;
		gAziBeamAngleSinGrid[3][i] = (float)(sinf(1.0 * gAziBeamAngleGrid[3][i] * DEG2RAD));
		gAziBeamAngleSinInt16Grid[3][i] = (int16_t)(gAziBeamAngleSinGrid[3][i] * INT16_SCALE);
	}
	for (i = 0; i < gEleBeamNumGrid[3]; i++)
	{
		gEleBeamIndexGrid[3][i] = gEleBeamIndex[i];
		gEleBeamAngleGrid[3][i] = gEleBeamAngle[i] + GRID_SEARCH_BEAM_INTER * 2;
		gEleBeamAngleSinGrid[3][i] = (float)(sinf(1.0 * gEleBeamAngleGrid[3][i] * DEG2RAD));
		gEleBeamAngleSinInt16Grid[3][i] = (int16_t)(gEleBeamAngleSinGrid[3][i] * INT16_SCALE);
	}

	// round 4
	memcpy(&gAziBeamIndexGrid[4][0], &gAziBeamIndex[0], sizeof(gAziBeamIndex[0]) * gAziBeamNumGrid[4]);
	memcpy(&gAziBeamAngleGrid[4][0], &gAziBeamAngle[0], sizeof(gAziBeamAngle[0]) * gAziBeamNumGrid[4]);
	memcpy(&gAziBeamAngleSinGrid[4][0], &gAziBeamAngleSin[0], sizeof(gAziBeamAngleSin[0]) * gAziBeamNumGrid[4]);
	memcpy(&gAziBeamAngleSinInt16Grid[4][0], &gAziBeamAngleSinInt16[0], sizeof(gAziBeamAngleSinInt16[0]) * gAziBeamNumGrid[4]);
	for (i = 0; i < gEleBeamNumGrid[4]; i++)
	{
		gEleBeamIndexGrid[4][i] = gEleBeamIndex[i];
		gEleBeamAngleGrid[4][i] = gEleBeamAngle[i] + GRID_SEARCH_BEAM_INTER;
		gEleBeamAngleSinGrid[4][i] = (float)(sinf(1.0 * gEleBeamAngleGrid[4][i] * DEG2RAD));
		gEleBeamAngleSinInt16Grid[4][i] = (int16_t)(gEleBeamAngleSinGrid[4][i] * INT16_SCALE);
	}

	// round 5
	for (i = 0; i < gAziBeamNumGrid[5]; i++)
	{
		gAziBeamIndexGrid[5][i] = gAziBeamIndex[i];
		gAziBeamAngleGrid[5][i] = gAziBeamAngle[i] + GRID_SEARCH_BEAM_INTER;
		gAziBeamAngleSinGrid[5][i] = (float)(sinf(1.0 * gAziBeamAngleGrid[5][i] * DEG2RAD));
		gAziBeamAngleSinInt16Grid[5][i] = (int16_t)(gAziBeamAngleSinGrid[5][i] * INT16_SCALE);
	}
	for (i = 0; i < gEleBeamNumGrid[5]; i++)
	{
		gEleBeamIndexGrid[5][i] = gEleBeamIndex[i];
		gEleBeamAngleGrid[5][i] = gEleBeamAngle[i] + GRID_SEARCH_BEAM_INTER;
		gEleBeamAngleSinGrid[5][i] = (float)(sinf(1.0 * gEleBeamAngleGrid[5][i] * DEG2RAD));
		gEleBeamAngleSinInt16Grid[5][i] = (int16_t)(gEleBeamAngleSinGrid[5][i] * INT16_SCALE);
	}
}

int32_t GetTempTrackNum(sTracking_moduleInstance* inst)
{
	int32_t i = 0;
	int32_t tempTrackNum = 0;

	for (i = 0; i < 3; i++)
	{
		if (inst->confirmBeamId[i] != TRACKING_NUM_TRACKS_MAX)
		{
			tempTrackNum++;
		}
	}
	return tempTrackNum;
}

void DispatchTwsOutputWaveType(sDispatchInst* dispatch_inst)
{
	if (gVelunambigType == WHOLE_SCAN)
	{
		dispatch_inst->dispatchOutput->waveType = (gWholeSpaceScanCycleCnt % 2);
	}
	else if (gVelunambigType == STEP_SCAN)
	{
		if ((gAziBeamIndexCur % 2) == 0)
		{
			if ((gEleBeamIndexCur % 2) == 0)
			{
				dispatch_inst->dispatchOutput->waveType = WAVE_TYPE_0;
			}
			else
			{
				dispatch_inst->dispatchOutput->waveType = WAVE_TYPE_1;
			}
		}
		else
		{
			if ((gEleBeamIndexCur % 2) == 0)
			{
				dispatch_inst->dispatchOutput->waveType = WAVE_TYPE_1;
			}
			else
			{
				dispatch_inst->dispatchOutput->waveType = WAVE_TYPE_0;
			}
		}
	}
	else
	{
		dispatch_inst->dispatchOutput->waveType = WAVE_TYPE_0;
	}
}

void DispatchTwsOutputFinalBeamFlag(sDispatchInst* dispatch_inst)
{
	if ((gAziBeamIndexCur == (gAziBeamIndex[gAziBeamNum - 1])) && (gEleBeamIndexCur == (gEleBeamIndex[gEleBeamNum - 1])))
	{
		dispatch_inst->dispatchOutput->wholeSpaceScanCycleFinalBeamFlag = 1;
	}
	else
	{
		dispatch_inst->dispatchOutput->wholeSpaceScanCycleFinalBeamFlag = 0;
	}
}

void GetTasTrackOffsetAngle(float *statePredict, float* P_hat, sTracking_measurementSphUnion trackerPolar, float* aziOffset, float* eleOffset, sTracking_moduleInstance* inst)
{
	float alpha;
	float sigmaY, sigmaZ;
	float unitY, unitZ, unitLen;
	float tasBeamPos[SSIZE_XY] = { 0.f };
	int i, j;
	float minAziDiff = 2 * TAS_AZI_OFFSET_UPPER_BOUND + 1;
	float minEleDiff = 2 * TAS_ELE_OFFSET_UPPER_BOUND + 1;
	int sameAziId[2], sameEleId[2];
	float sameAziAngle, sameEleAngle;
	sTracking_measurementSphUnion tasBeamPolar;

	alpha = 1.5f;
	sigmaY = sqrtf(P_hat[40]);
	sigmaZ = sqrtf(P_hat[50]);
	if (fabsf(statePredict[4]) < 0.001f || fabsf(statePredict[5]) < 0.001f)
	{
		unitY = 0.f;
		unitZ = -1.f;
	}
	else if (fabsf(statePredict[4]) > fabs(statePredict[5]))
	{
		unitY = 1.f / statePredict[4];
		unitZ = -1.f / statePredict[5];
	}
	else
	{
		unitY = statePredict[4];
		unitZ = statePredict[5];
	}
	if (unitZ > 0)
	{
		unitY *= -1;
		unitZ *= -1;
	}
	unitLen = sqrtf(unitY * unitY + unitZ * unitZ);
	unitY /= unitLen;
	unitZ /= unitLen;
	tasBeamPos[0] = statePredict[0];
	// down
	tasBeamPos[1] = statePredict[1] + alpha * sigmaY * unitY;
	tasBeamPos[2] = statePredict[2] + alpha * sigmaZ * unitZ;
	tracking_cartesian2spherical(inst->tracking_params->stateVectorType, \
		tasBeamPos, tasBeamPolar.array);
	aziOffset[0] = (tasBeamPolar.vector.azimuthRad - trackerPolar.vector.azimuthRad) * RAD2DEG;
	eleOffset[0] = (tasBeamPolar.vector.pitchRad - trackerPolar.vector.pitchRad) * RAD2DEG;
	if (eleOffset[0] > -TAS_ELE_OFFSET_LOWER_BOUND)
	{
		eleOffset[0] = -TAS_ELE_OFFSET_LOWER_BOUND;
	}
	else if (eleOffset[0] < -TAS_ELE_OFFSET_UPPER_BOUND)
	{
		eleOffset[0] = -TAS_ELE_OFFSET_UPPER_BOUND;
	}
	if (aziOffset[0] < -TAS_AZI_OFFSET_UPPER_BOUND)
	{
		aziOffset[0] = -TAS_AZI_OFFSET_UPPER_BOUND;
	}
	else if (aziOffset[0] > TAS_AZI_OFFSET_UPPER_BOUND)
	{
		aziOffset[0] = TAS_AZI_OFFSET_UPPER_BOUND;
	}
	// up
	tasBeamPos[1] = statePredict[1] - alpha * sigmaY * unitY;
	tasBeamPos[2] = statePredict[2] - alpha * sigmaZ * unitZ;
	tracking_cartesian2spherical(inst->tracking_params->stateVectorType, \
		tasBeamPos, tasBeamPolar.array);
	aziOffset[1] = (tasBeamPolar.vector.azimuthRad - trackerPolar.vector.azimuthRad) * RAD2DEG;
	eleOffset[1] = (tasBeamPolar.vector.pitchRad - trackerPolar.vector.pitchRad) * RAD2DEG;
	if (eleOffset[1] < TAS_ELE_OFFSET_LOWER_BOUND)
	{
		eleOffset[1] = TAS_ELE_OFFSET_LOWER_BOUND;
	}
	else if (eleOffset[1] > TAS_ELE_OFFSET_UPPER_BOUND)
	{
		eleOffset[1] = TAS_ELE_OFFSET_UPPER_BOUND;
	}
	if (aziOffset[1] < -TAS_AZI_OFFSET_UPPER_BOUND)
	{
		aziOffset[1] = -TAS_AZI_OFFSET_UPPER_BOUND;
	}
	else if (aziOffset[1] > TAS_AZI_OFFSET_UPPER_BOUND)
	{
		aziOffset[1] = TAS_AZI_OFFSET_UPPER_BOUND;
	}

	if (fabsf(statePredict[4]) < 0.001f || fabsf(statePredict[5]) < 0.001f)
	{
		unitY = -1.f;
		unitZ = 0.f;
	}
	else if (fabsf(statePredict[4]) < fabs(statePredict[5]))
	{
		unitY = 1.f / statePredict[4];
		unitZ = -1.f / statePredict[5];
	}
	else
	{
		unitY = statePredict[4];
		unitZ = statePredict[5];
	}
	if (unitY > 0)
	{
		unitY *= -1;
		unitZ *= -1;
	}
	unitLen = sqrtf(unitY * unitY + unitZ * unitZ);
	unitY /= unitLen;
	unitZ /= unitLen;
	tasBeamPos[0] = statePredict[0];
	// left
	tasBeamPos[1] = statePredict[1] + alpha * sigmaY * unitY;
	tasBeamPos[2] = statePredict[2] + alpha * sigmaZ * unitZ;
	tracking_cartesian2spherical(inst->tracking_params->stateVectorType, \
		tasBeamPos, tasBeamPolar.array);
	aziOffset[2] = (tasBeamPolar.vector.azimuthRad - trackerPolar.vector.azimuthRad) * RAD2DEG;
	eleOffset[2] = (tasBeamPolar.vector.pitchRad - trackerPolar.vector.pitchRad) * RAD2DEG;
	if (aziOffset[2] > -TAS_AZI_OFFSET_LOWER_BOUND)
	{
		aziOffset[2] = -TAS_AZI_OFFSET_LOWER_BOUND;
	}
	else if (aziOffset[2] < -TAS_AZI_OFFSET_UPPER_BOUND)
	{
		aziOffset[2] = -TAS_AZI_OFFSET_UPPER_BOUND;
	}
	if (eleOffset[2] < -TAS_ELE_OFFSET_UPPER_BOUND)
	{
		eleOffset[2] = -TAS_ELE_OFFSET_UPPER_BOUND;
	}
	else if (eleOffset[2] > TAS_ELE_OFFSET_UPPER_BOUND)
	{
		eleOffset[2] = TAS_ELE_OFFSET_UPPER_BOUND;
	}
	// right
	tasBeamPos[1] = statePredict[1] - alpha * sigmaY * unitY;
	tasBeamPos[2] = statePredict[2] - alpha * sigmaZ * unitZ;
	tracking_cartesian2spherical(inst->tracking_params->stateVectorType, \
		tasBeamPos, tasBeamPolar.array);
	aziOffset[3] = (tasBeamPolar.vector.azimuthRad - trackerPolar.vector.azimuthRad) * RAD2DEG;
	eleOffset[3] = (tasBeamPolar.vector.pitchRad - trackerPolar.vector.pitchRad) * RAD2DEG;
	if (aziOffset[3] < TAS_AZI_OFFSET_LOWER_BOUND)
	{
		aziOffset[3] = TAS_AZI_OFFSET_LOWER_BOUND;
	}
	else if (aziOffset[3] > TAS_AZI_OFFSET_UPPER_BOUND)
	{
		aziOffset[3] = TAS_AZI_OFFSET_UPPER_BOUND;
	}
	if (eleOffset[3] < -TAS_ELE_OFFSET_UPPER_BOUND)
	{
		eleOffset[3] = -TAS_ELE_OFFSET_UPPER_BOUND;
	}
	else if (eleOffset[3] > TAS_ELE_OFFSET_UPPER_BOUND)
	{
		eleOffset[3] = TAS_ELE_OFFSET_UPPER_BOUND;
	}

	for (i = 0; i < TAS_CYCLE_TOTAL_BEAM_NUM; i++)
	{
		for (j = i + 1; j < TAS_CYCLE_TOTAL_BEAM_NUM; j++)
		{
			if (fabsf(aziOffset[i] - aziOffset[j]) < minAziDiff)
			{
				minAziDiff = fabsf(aziOffset[i] - aziOffset[j]);
				sameAziId[0] = i;
				sameAziId[1] = j;
			}
			if (fabsf(eleOffset[i] - eleOffset[j]) < minEleDiff)
			{
				minEleDiff = fabsf(eleOffset[i] - eleOffset[j]);
				sameEleId[0] = i;
				sameEleId[1] = j;
			}
		}
	}
	sameAziAngle = (aziOffset[sameAziId[0]] + aziOffset[sameAziId[1]]) / 2.f;
	sameEleAngle = (eleOffset[sameEleId[0]] + eleOffset[sameEleId[1]]) / 2.f;
	if (sameAziAngle < TAS_AZI_OFFSET_LOWER_BOUND && sameAziAngle > -TAS_AZI_OFFSET_LOWER_BOUND)
	{
		if ((aziOffset[sameAziId[0]] >= TAS_AZI_OFFSET_LOWER_BOUND && aziOffset[sameAziId[1]] >= 0) || \
			(aziOffset[sameAziId[1]] >= TAS_AZI_OFFSET_LOWER_BOUND && aziOffset[sameAziId[0]] >= 0))
		{
			sameAziAngle = TAS_AZI_OFFSET_LOWER_BOUND;
		}
		else if ((aziOffset[sameAziId[0]] <= -TAS_AZI_OFFSET_LOWER_BOUND && aziOffset[sameAziId[1]] <= 0) || \
			(aziOffset[sameAziId[1]] <= -TAS_AZI_OFFSET_LOWER_BOUND && aziOffset[sameAziId[0]] <= 0))
		{
			sameAziAngle = -TAS_AZI_OFFSET_LOWER_BOUND;
		}
	}
	if (sameEleAngle < TAS_ELE_OFFSET_LOWER_BOUND && sameEleAngle > -TAS_ELE_OFFSET_LOWER_BOUND)
	{
		if ((eleOffset[sameEleId[0]] >= TAS_ELE_OFFSET_LOWER_BOUND && eleOffset[sameEleId[1]] >= 0) || \
			(eleOffset[sameEleId[1]] >= TAS_ELE_OFFSET_LOWER_BOUND && eleOffset[sameEleId[0]] >= 0))
		{
			sameEleAngle = TAS_ELE_OFFSET_LOWER_BOUND;
		}
		else if ((eleOffset[sameEleId[0]] <= -TAS_ELE_OFFSET_LOWER_BOUND && eleOffset[sameEleId[1]] <= 0) || \
			(eleOffset[sameEleId[1]] <= -TAS_ELE_OFFSET_LOWER_BOUND && eleOffset[sameEleId[0]] <= 0))
		{
			sameEleAngle = -TAS_ELE_OFFSET_LOWER_BOUND;
		}
	}
	aziOffset[sameAziId[0]] = sameAziAngle;
	aziOffset[sameAziId[1]] = sameAziAngle;
	eleOffset[sameEleId[0]] = sameEleAngle;
	eleOffset[sameEleId[1]] = sameEleAngle;
}

void TrackBeamProcAndOut(sDispatchInst* dispatch_inst, sTrackingFrame* tracksListOutput, sDispatchOutput dispatchOutputTmp)
{
	dispatch_inst->dispatchOutput->frameID = tracksListOutput->frameID;
	dispatch_inst->dispatchOutput->timestamp = tracksListOutput->timestamp;
	dispatch_inst->dispatchOutput->aziBeamID = dispatchOutputTmp.aziBeamID;
	dispatch_inst->dispatchOutput->eleBeamID = dispatchOutputTmp.eleBeamID;
	dispatch_inst->dispatchOutput->aziBeamSin = dispatchOutputTmp.aziBeamSin;
	dispatch_inst->dispatchOutput->eleBeamSin = dispatchOutputTmp.eleBeamSin;
	dispatch_inst->dispatchOutput->tasBeamTotal = dispatchOutputTmp.tasBeamTotal;
	dispatch_inst->dispatchOutput->tasBeamCntCur = dispatchOutputTmp.tasBeamCntCur;
	dispatch_inst->dispatchOutput->tasObjId = dispatchOutputTmp.tasObjId;
	dispatch_inst->dispatchOutput->tasObjFilterNum = dispatchOutputTmp.tasObjFilterNum;
	dispatch_inst->dispatchOutput->tasObjRange = dispatchOutputTmp.tasObjRange;
	dispatch_inst->dispatchOutput->samplePntStart = dispatchOutputTmp.samplePntStart;
	dispatch_inst->dispatchOutput->samplePntDepth = dispatchOutputTmp.samplePntDepth;
	dispatch_inst->dispatchOutput->beamSwitchTime = dispatchOutputTmp.beamSwitchTime;
	dispatch_inst->dispatchOutput->wholeSpaceScanCycleCnt = gWholeSpaceScanCycleCnt;
	dispatch_inst->dispatchOutput->trackTwsTasFlag = TAS_SCAN;
}