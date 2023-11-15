
#include "alg_init.h"
#include"../target_process/measurementProcess/include/measurementProcess.h"
#include "../target_process/tracking/include/tracking.h"
#include "../target_process/dispatch/include/dispatch.h"
#include "../target_process/targetThreatAssessment/include/targetThreatAssessment.h"
#include "../target_process/trackerExistingConfCoef/include/trackerExistingConfCoef.h"
#include "../target_process/radarPoseEstimation/include/radarPoseEstimation.h"

/******* Tracking algorithm version **********
Example: 01.11011
          |--------- main version number
		    |------- version number of tracking initialization
		     |------- version number of tracking association
		      |------- version number of tracking filter
		       |------- version number of tracking processer
		        |------- version number of post warning (including merge, planning, ego-motion ...)
*/
char g_version_str[TRACK_VERSION_STR_LEN] = TRACK_ALG_VERSION;

/*************** Allocate memory for data struct **************/
static sAlgObjData gAlgObj[1] = { 0 };

sDetectFrame detectsListOutput[1] = {0};

sMeasProcessInst measProcess_inst_malloc[1] = { 0 };
sMeasOutput measurement_scratchPadData_malloc[1] = { 0 };
sMeasInput measurement_detectionInputData_malloc[1] = { 0 };
sTwsDotCoheInst measurement_TwsDotCohesion_malloc[1] = { 0 };
sDotsCohesionFrame measurement_dotsCohesionOutput_malloc[1] = { 0 };


sTrackingFrame trajInfoOutput[1] = { 0 };
sTrackingInst tracking_inst_malloc[1] = { 0 };
sTracking_insideScratchPad tracking_scratchPadData_malloc[1] = { 0 };

sDispatchConfigParam dispatch_config_malloc[1] = { 0 };
sDispatchInst dispatch_inst_malloc[1] = { 0 };
sDispatchOutput dispatch_output[1] = { 0 };
sDispatchOutput dispatch_outputCurr[1] = { 0 };
sDispatchOutput dispatch_outputNext[1] = { 0 };
sDispatchTasTrackInfo dispatchTasTrackInfo_malloc[1] = { 0 };
sDispatchInitTrackInfo dispatchInitTrackInfo_malloc[1] = { 0 };
sPlatformInfo platformInfo_malloc[1] = { 0 };

sDebugAndCtrlParam debugAndCtrlInf_malloc[1] = { 0 };

sTargetThreatAssessmentInst targetThreatAssessment_malloc[1] = { 0 };

sTrackerExistingInst tracker_existing_malloc[1] = { 0 };
//sRadarPoseEstimation radarPostEstimation_malloc[1] = { 0 };
//sMagCaliParam magCaliParam_malloc[1] = { 0 };
//sMagCaliData magCaliData_malloc[1] = { 0 };
uint8_t trackerExisting_scratchPad[sizeof(sTrackerExisting_probClus) * TRACKEREXISTING_MAXPROBCLUSNUM + sizeof(sTrackerExisting_probLife) * TRACKEREXISTING_MAXPROBLIFENUM + \
sizeof(sTrackerExisting_probMotion) * (TRACKEREXISTING_MAXPROBMOTIONNUM + TRACKEREXISTING_OR_MAXPROBMOTIONNUM)] = { 0 };
int gAlgInitFlag = -1;
int gDummyData_flag = 0;
sScanType g_scanType = TWS_SCAN;
uint32_t g_detFrameId = 0;
float g_detTimestamp = 0.f;

#ifdef _WINDOWS
volatile int32_t g_byDummyTest = 0;
volatile int32_t g_byAbudhabiShowScenes = 0;
volatile int32_t g_byDevHwGen = 3;//generation of radar:2nd:2; 3rd:3;
volatile int32_t g_byMagClutRemove = 1;//whether to remove clutter points by mag:Yes:1; No:0;
#else
#include <sys/time.h>
#include <stdio.h>
#include <time.h>
struct timespec g_tUsTrackProcCur[32] = { 0 };
struct timespec g_tUsTrackProcEnd[32] = { 0 };
uint32_t g_tUsTrackProcUsed[32] = { 0 };
#endif

sVelUnambiType gVelunambigType = NO_UNAMBIGIOUS;
int gCfmBeamFlag = 0;
char* getTrackAlgVersion()
{
	return g_version_str;
}

void setDummyFlag(int flag)
{
	gDummyData_flag = flag;
}
sDotsCohesionFrame* GetDotResult()
{
	return measurement_dotsCohesionOutput_malloc;
}
// Allocation memory
void algBufferAlloc(sAlgObjData *algObj)
{
	//sTrackingInst* tracking_inst;
	algObj->detectsListOutput = detectsListOutput; // the output of signal process module

	algObj->measurementProcess_inst = measProcess_inst_malloc;

	algObj->tracking_inst = tracking_inst_malloc;
	algObj->dispatch_inst = dispatch_inst_malloc;
	algObj->platformInfo = platformInfo_malloc;
	algObj->debugAndCtrlParam = debugAndCtrlInf_malloc;
	algObj->targetThreatAssessment_inst = targetThreatAssessment_malloc;
	//algObj->radarPoseEstimation_inst = radarPostEstimation_malloc;
	//tracking_inst = (sTrackingInst*)algObj->tracking_inst;
	/*tracking_inst->scratchPadData->moduleInst.trackerExisting_inst = tracker_existing_malloc;
	tracking_inst->scratchPadData->moduleInst.trackerExisting_scratchPad = trackerExisting_scratchPad;*/

	
	if (DEFAULT_SCAN_TYPE == 0)
	{
		algObj->scanType = TWS_SCAN;
	}
	else
	{
		algObj->scanType = TAS_SCAN;
	}
	g_scanType = algObj->scanType;
}

/************* measurement process module init **************/
int measProcAlgInstance_init(sAlgObjData *algObj)
{
	sMeasProcessInst *measProc_inst = (sMeasProcessInst *)algObj->measurementProcess_inst;
	sMeasConfigParam *measConfigParam = &measProc_inst->measConfigParam;
	int init_ret = -1;
	measProc_inst->measOutput = measurement_scratchPadData_malloc;
	measProc_inst->measInput = measurement_detectionInputData_malloc;
	measProc_inst->TwsDotCoheStr = measurement_TwsDotCohesion_malloc;
	measProc_inst->dotsCoheOutput = measurement_dotsCohesionOutput_malloc;

	memset(measProc_inst->measOutput, 0, sizeof(sMeasOutput));
	measConfigParam->rCohGatingTws = 8.f;
	measConfigParam->dCohGatingTws = 2.f;
	measConfigParam->rCohGatingTas = 8.f;
	measConfigParam->dCohGatingTas = 2.f;
	measConfigParam->rDetTrackTas = 15.f;
	measConfigParam->dDetTrackTas = 5.f;
	return 0;
}


/************* Tracking module **************/
int trackingAlgInstance_init(sAlgObjData *algObj)
{
	sTrackingInst *tracking_inst = (sTrackingInst*)(algObj->tracking_inst);
	sTrackingParams *configParams = &tracking_inst->trackingConfig;

	int loc = 0;
	int init_ret = -1;
	tracking_inst->trajInfoOutput = trajInfoOutput; // the output of tracking module
	tracking_inst->trajInfoOutput->trackObjNum = 0;
	memset(tracking_inst->trajInfoOutput->trajList, 0, sizeof(tracking_inst->trajInfoOutput->trajList));
	//printf("trajList: 0X%p \n", algObj->trajInfoOutput->trajList);
	configParams->workMode = 0;
	configParams->condenceDone = 0;
	configParams->tasTargetId = -1;
	configParams->stateVectorType = TRACKING_STATE_VECTORS_3DA;
	configParams->verbose = TRACKING_VERBOSE_NONE;

	//configParams->maxNumPoints = TRACKING_NUM_POINTS_MAX;
	//configParams->maxNumTracks = TRACKING_NUM_TRACKS_MAX;
	configParams->initialRadialVelocity = 0;
	configParams->maxRadialVelocity = 100;
	configParams->radialVelocityResolution = 0.3f;
	configParams->maxAcceleration[0] = 10.f;
	configParams->maxAcceleration[1] = 10.f;
	configParams->maxAcceleration[2] = 10.f;
	configParams->stateVectorDimNum = SSIZE_X;
#ifndef MULTI_AZIMU
	configParams->numExDoppler = 0;
#else
	configParams->numExDoppler = NUM_EXTERN_DOPPLER;
#endif
#ifndef MMWAVE_3D
	configParams->stateVectorDimLength = 2;
#else
	configParams->stateVectorDimLength = 3;
#endif
	if (!g_byDummyTest )
		configParams->dt = 0.05f; // 0.05s for initialization. It will be modified when loading data
	else
		configParams->dt = CONST_DELTA_T; // 0.05s for initialization. It will be modified when loading data

	configParams->dbscan_epsilon = 4.0f; // neighbor distance threshold in meter
	configParams->dbscan_minNumPoints = 1;
	configParams->cluster_weight = 0.5f * 0.5f * configParams->dbscan_epsilon;
	configParams->cluster_vThres = 1.0f; // Neighbor speed threshold in m/s
	configParams->advParams.gatingParams.gain = 5000.f;
	configParams->advParams.gatingParams.limits.depth = 6.0f;
	configParams->advParams.gatingParams.limits.width = 6.0f;
	configParams->advParams.gatingParams.limits.height = 10.f;
	configParams->advParams.gatingParams.limits.vel = 4.f;


	configParams->advParams.outOfRoiGatingParams.gain = 5000.f;
	configParams->advParams.outOfRoiGatingParams.limits.depth = 7.0f;
	configParams->advParams.outOfRoiGatingParams.limits.width = 6.0f;
	configParams->advParams.outOfRoiGatingParams.limits.height = 10.f;
	configParams->advParams.outOfRoiGatingParams.limits.vel = 4.f;
	configParams->advParams.exGatingParams.gain = 5000.f;
	configParams->advParams.exGatingParams.limits.depth = 0.4f; //0.8f;
	configParams->advParams.exGatingParams.limits.width = 0.8f;
	configParams->advParams.exGatingParams.limits.height = 0.5f;
	configParams->advParams.exGatingParams.limits.vel = 0.4f;
	configParams->advParams.exOutOfRoiGatingParams.gain = 5000.f;
	configParams->advParams.exOutOfRoiGatingParams.limits.depth = 0.4f; //0.5f;
	configParams->advParams.exOutOfRoiGatingParams.limits.width = 0.8f;
	configParams->advParams.exOutOfRoiGatingParams.limits.height = 0.5f;
	configParams->advParams.exOutOfRoiGatingParams.limits.vel = 0.4f;
	configParams->advParams.allocationParams.snrThre = 0.f;
	configParams->advParams.allocationParams.snrThreObscured = 0.f;
	configParams->advParams.allocationParams.velocityThre = 0.0f;
	configParams->advParams.allocationParams.pointsThre = 1U;
	configParams->advParams.allocationParams.maxDistanceThre = 4.f;
	configParams->advParams.allocationParams.maxVelThre = 3.f;
	configParams->advParams.allocationParams.limits.depth = 4.5f;
	configParams->advParams.allocationParams.limits.width = 5.5f;
	configParams->advParams.allocationParams.limits.height = 8.f;
	configParams->advParams.allocationParams.limits.vel = 1.5f;
	configParams->advParams.outOfRoiAllocationParams.snrThre = 0.f;
	configParams->advParams.outOfRoiAllocationParams.snrThreObscured = 0.f;
	configParams->advParams.outOfRoiAllocationParams.velocityThre = 0.0f;
	configParams->advParams.outOfRoiAllocationParams.pointsThre = 1U;
	configParams->advParams.outOfRoiAllocationParams.maxDistanceThre = 4.f;
	configParams->advParams.outOfRoiAllocationParams.maxVelThre = 3.f;
	configParams->advParams.outOfRoiAllocationParams.limits.depth = 4.5f;
	configParams->advParams.outOfRoiAllocationParams.limits.width = 5.5f;
	configParams->advParams.outOfRoiAllocationParams.limits.height = 8.f;
	configParams->advParams.outOfRoiAllocationParams.limits.vel = 1.5f;
	configParams->advParams.unrollingParams.alpha = 0.5f;
	configParams->advParams.unrollingParams.confidence = 0.1f;

	configParams->advParams.tasStateParams.det2actThre = 5U; //8U;
	configParams->advParams.tasStateParams.det2freeThre = 2U; //1U;
	configParams->advParams.tasStateParams.active2freeThre = 10U;
	configParams->advParams.tasStateParams.exit2freeThre = 5U;
	configParams->advParams.tasStateParams.unseenTimeThre = 3.f;

	configParams->advParams.twsStateParams.det2actThre = 5U; //8U;
	configParams->advParams.twsStateParams.det2freeThre = 2U; //1U;
	configParams->advParams.twsStateParams.active2freeThre = 5U;
	configParams->advParams.twsStateParams.exit2freeThre = 5U;
	configParams->advParams.twsStateParams.unseenTimeThre = 4.f;

	configParams->advParams.outOfRoiStateParams.det2actThre = 5U; //3U;
	configParams->advParams.outOfRoiStateParams.det2freeThre = 2U;
	configParams->advParams.outOfRoiStateParams.active2freeThre = 10U;
	configParams->advParams.outOfRoiStateParams.exit2freeThre = 5U;
	configParams->advParams.variationParams.widthStd = 1.f/3.46f;
	configParams->advParams.variationParams.depthStd = 1.f/3.46f;
	configParams->advParams.variationParams.heightStd = 1.f/3.46f;
	configParams->advParams.variationParams.dopplerStd = 2.f;
	configParams->advParams.sceneryParams.numBoundaryBoxes = 0;
	configParams->advParams.sceneryParams.numStaticBoxes = 0;

	configParams->advParams.tasMdGatingParams.gating = 9.49f;
	configParams->advParams.twsMdGatingParams.gating = 9.49f;

	tracking_inst->errorCode = TRACKING_EOK;
	tracking_inst->curTimestamp = -1;
	tracking_inst->frame_cnt = 0;
	tracking_inst->runningStatus = MODULE_OK;
	tracking_inst->init_process_noise[0] = 4.f;
	tracking_inst->init_process_noise[1] = 4.f;//25.0f;
	tracking_inst->init_process_noise[2] = 4.f; // 0.04f;
	tracking_inst->init_process_noise[3] = 4.f; // 0.04f;
	tracking_inst->init_process_noise[4] = 4.f; // 0.01f;
	tracking_inst->init_process_noise[5] = 4.f; // 0.01f;
	tracking_inst->init_process_noise[6] = 4.f; // 0.04f;
	tracking_inst->init_process_noise[7] = 4.f; // 0.01f;
	tracking_inst->init_process_noise[8] = 4.f; // 0.01f;
	tracking_inst->init_max_acceleration[0] = 0.01f;
	tracking_inst->init_max_acceleration[1] = 0.25f;
	tracking_inst->init_max_acceleration[2] = 0.25f;
	tracking_inst->init_tws_measurement_noise[0] = 4.f;
	tracking_inst->init_tws_measurement_noise[1] = 16.0f*DEG2RAD*DEG2RAD;
	tracking_inst->init_tws_measurement_noise[2] = 36.0f*DEG2RAD*DEG2RAD;
	tracking_inst->init_tws_measurement_noise[3] = 1.f;

	tracking_inst->init_tas_measurement_noise[0] = 4.f;
	tracking_inst->init_tas_measurement_noise[1] = 9.0f*DEG2RAD*DEG2RAD;
	tracking_inst->init_tas_measurement_noise[2] = 16.0f*DEG2RAD*DEG2RAD;
	tracking_inst->init_tas_measurement_noise[3] = 1.f;

	tracking_inst->init_roiArea.numBoundaryBoxes = 0U;
	if(1==gDummyData_flag){
		tracking_inst->init_roiArea.boundaryBox[0].x1 = -150.0f;
		tracking_inst->init_roiArea.boundaryBox[0].x2 = 150.0f;
	}
	else{
		tracking_inst->init_roiArea.boundaryBox[0].x1 = -3.5f;
		tracking_inst->init_roiArea.boundaryBox[0].x2 = 10.0f;
	}
	tracking_inst->init_roiArea.boundaryBox[0].y1 = -150.f; //-10.0f;
	tracking_inst->init_roiArea.boundaryBox[0].y2 = 150.f; //10.0f;
	tracking_inst->init_roiArea.boundaryBox[0].z1 = -10.0f;
	tracking_inst->init_roiArea.boundaryBox[0].z2 = 10.0f;

    /**  @brief Scene static boxes in radar frame*/
	tracking_inst->init_roiArea.numStaticBoxes = 0U;
	tracking_inst->init_roiArea.numROIBoxes = 1U;
	tracking_inst->init_roiArea.roiBox[0].x1 = 0.0f;
	tracking_inst->init_roiArea.roiBox[0].x2 = 30.0f;
	tracking_inst->init_roiArea.roiBox[0].y1 = -15.0f;
	tracking_inst->init_roiArea.roiBox[0].y2 = 15.0f;
	tracking_inst->init_roiArea.roiBox[0].z1 = -10.0f;
	tracking_inst->init_roiArea.roiBox[0].z2 = 10.0f;
	tracking_inst->init_roiArea.numNoiseBoxes = 1U;
	tracking_inst->init_roiArea.noiseBox[0].x1 = 0.0f;
	tracking_inst->init_roiArea.noiseBox[0].x2 = 40.0f;
	tracking_inst->init_roiArea.noiseBox[0].y1 = -6.0f;
	tracking_inst->init_roiArea.noiseBox[0].y2 = 6.0f;
	tracking_inst->init_roiArea.noiseBox[0].z1 = 0.0f;
	tracking_inst->init_roiArea.noiseBox[0].z2 = 2.0f;
	if(1==gDummyData_flag){
		tracking_inst->init_roiArea.mag_min_thres[0] = 0.f; //20.f; //14.0f; // for noiseBox
		tracking_inst->init_roiArea.snr_min_thres[0] = 0.f; //20.f; //14.0f; // for noiseBox
	}
	else{
		tracking_inst->init_roiArea.mag_min_thres[0] = 90.f; //20.f; //14.0f; // for noiseBox
		tracking_inst->init_roiArea.snr_min_thres[0] = 9.5f; //20.f; //14.0f; // for noiseBox
	}
	tracking_inst->init_roiArea.mag_asso_inROIThres[0] = 10000.0f; // for noiseBox
	tracking_inst->init_roiArea.mag_asso_outROIThres = 10000.0f; // for noiseBox
	tracking_inst->init_roiArea.snr_asso_inROIThres[0] = 10000.0f; // for noiseBox
	tracking_inst->init_roiArea.snr_asso_outROIThres = 10000.0f; // for noiseBox
	tracking_inst->init_roiArea.rcs_asso_inROIThres[0] = 10000.0f; // for noiseBox
	tracking_inst->init_roiArea.rcs_asso_outROIThres = 10000.0f; // for noiseBox

	tracking_inst->init_roiArea.scanBoundary.aziScanScope = 120.f;
	tracking_inst->init_roiArea.scanBoundary.eleScanScope = 40.f;
	tracking_inst->init_roiArea.scanBoundary.aziScanCenter = 0.f;
	tracking_inst->init_roiArea.scanBoundary.eleScanCenter = 0.f;

	tracking_inst->scratchPadData = tracking_scratchPadData_malloc;
	tracking_inst->scratchPadData->moduleInst.trackerExisting_inst = tracker_existing_malloc;
	tracking_inst->scratchPadData->moduleInst.trackerExisting_scratchPad = trackerExisting_scratchPad;
	init_ret = tracking_init(algObj);
	if (init_ret < 0) {
		return -1;
	}
	return 0;
}

/************* dispatch module **************/
int dispatchAlgInstance_init(sAlgObjData *algObj)
{
	sDispatchInst* dispatch_inst = algObj->dispatch_inst;
	dispatch_inst->dispatchConfigParam = dispatch_config_malloc;
	dispatch_inst->dispatchTasTrackInfo = dispatchTasTrackInfo_malloc;
	dispatch_inst->dispatchInitTrackInfo = dispatchInitTrackInfo_malloc;
	dispatch_inst->dispatchOutput = dispatch_output;
	dispatch_inst->dispatchOutputCurr = dispatch_outputCurr;
	dispatch_inst->dispatchOutputNext = dispatch_outputNext;
	return 0;
}
void moduleMemCalculate(sAlgObjData *algObj)
{
#ifdef DEBUG_LOG
	int sumSize = 0;
	int memSize = 0;
	
	//my_printf(" ***************** Detetion Struct *************** \n");
	//memSize = sizeof(detectsListOutput);
	//my_printf("Detection Struct: All the memSize: %d Bytes\n", memSize);
	//sumSize += memSize;

	//my_printf(" ***************** Tracking Module *************** \n");
	//algObj->trackingScratchPad_memSize = 0;
	//algObj->trackingScratchPad_memSize += sizeof(tracking_scratchPadData_malloc);
	//my_printf("Tracking Module: All the memSize for scratchPad: %d Bytes\n", algObj->trackingScratchPad_memSize);
	//memSize = algObj->trackingScratchPad_memSize;
	//memSize += sizeof(trajInfoOutput);
	////memSize += sizeof(trajInfoOutputtrajList);
	//memSize += sizeof(tracking_inst_malloc);
	//my_printf("Tracking Module: All the memSize: %d Bytes\n", memSize);
	//sumSize += memSize;

	//my_printf(" ***************** Measurement Process Module *************** \n");
	//algObj->measScratchPad_memSize = 0;
	//algObj->measScratchPad_memSize += sizeof(measurement_scratchPadData_malloc);
	//my_printf("Measurement Process Module: All the memSize for scratchPad: %d Bytes\n", algObj->measScratchPad_memSize);
	//memSize = algObj->measScratchPad_memSize;
	//memSize += sizeof(measProcess_inst_malloc);
	//memSize += sizeof(measurement_config_malloc);
	//my_printf("Measurement Process: All the memSize: %d Bytes\n", memSize);
	//sumSize += memSize;

	//my_printf(" ***************** Dispatch Module *************** \n");
	//memSize= sizeof(dispatch_output);
	//my_printf("Dispatch Module: All the memSize for Output: %d Bytes\n", memSize);
	//memSize += sizeof(dispatch_inst_malloc);
	//memSize += sizeof(dispatch_config_malloc);
	//my_printf("Dispatch Module: All the memSize: %d Bytes\n", memSize);
	//sumSize += memSize;

	//my_printf(" ***************** Others *************** \n");
	//memSize += sizeof(gAlgObj);
	//my_printf("Others: All the memSize: %d Bytes\n", memSize);
	//sumSize += memSize;
	//my_printf("*** Sum: All the memSize: %.3f K\n", ((float)(sumSize))/1024.f);

#endif
}

int targetThreatAssessmentAlgInstance_init(sAlgObjData* algObj)
{
	sTargetThreatAssessmentInst* targetThreatAssessment = (sTargetThreatAssessmentInst*)(algObj->targetThreatAssessment_inst);
	targetThreatAssessment->position_weight = 1; 
	targetThreatAssessment->direction_weight = 1;
	targetThreatAssessment->velocity_weight = 1;
	//targetThreatAssessment->target_threat[MAX_NUM_TRAJS] = { 0 };
	//targetThreatAssessment->protect_spots[MAXNUMOFPROTECTSPOTS] = {0};
	memset(targetThreatAssessment->target_threat, 0, MAX_NUM_TRAJS * sizeof(sTargetThreat));
	memset(targetThreatAssessment->protect_spots, 0, MAXNUMOFPROTECTSPOTS * sizeof(sProtectSpot));
	targetThreatAssessment->numOfProtectSpot = 1;
	targetThreatAssessment->protect_spots[0].x = 0.f;
	targetThreatAssessment->protect_spots[0].y = 0.f;
	targetThreatAssessment->protect_spots[0].z = 0.f;
	targetThreatAssessment->protect_spots[0].alert_radius = 200.f;
	targetThreatAssessment->protect_spots[0].id = 0;
	targetThreatAssessment->protect_spots[0].classification = 0;
	targetThreatAssessment->protect_spots[0].weight = 1;

	/*targetThreatAssessment->numOfProtectSpot = 2;
	targetThreatAssessment->protect_spots[0].x = 0.f;
	targetThreatAssessment->protect_spots[0].y = 0.f;
	targetThreatAssessment->protect_spots[0].z = 0.f;
	targetThreatAssessment->protect_spots[0].alert_radius = 200.f;
	targetThreatAssessment->protect_spots[0].id = 0;
	targetThreatAssessment->protect_spots[0].classification = 0;
	targetThreatAssessment->protect_spots[0].weight = 1;

	targetThreatAssessment->protect_spots[1].x = 0.f;
	targetThreatAssessment->protect_spots[1].y = 100.f;
	targetThreatAssessment->protect_spots[1].z = 0.f;
	targetThreatAssessment->protect_spots[1].alert_radius = 200.f;
	targetThreatAssessment->protect_spots[1].id = 1;
	targetThreatAssessment->protect_spots[1].classification = 1;
	targetThreatAssessment->protect_spots[1].weight = 1;*/

	return 0;
}

int trackerExistingAlgInstance_init(sAlgObjData* algObj)
{
	sTrackingInst* tracking_inst = (sTrackingInst*)algObj->tracking_inst;
	//sTrackerExistingInst* inst = (sTrackerExistingInst*)(algObj->trackerExisting_inst);
	sTrackerExistingInst* inst = tracking_inst->scratchPadData->moduleInst.trackerExisting_inst;
	uint8_t* trackerExisting_scratchPad = tracking_inst->scratchPadData->moduleInst.trackerExisting_scratchPad;
	int loc = 0;
	int init_ret = -1;
	inst->clusLamda = 1;
	inst->lifeLamda = 12;		// 20, delta t is around 80ms  2021-11-26
	loc = 0;
	inst->probClus_v = (sTrackerExisting_probClus*)(&trackerExisting_scratchPad[0]);
	loc += sizeof(sTrackerExisting_probClus) * TRACKEREXISTING_MAXPROBCLUSNUM;
	inst->probLife_v = (sTrackerExisting_probLife*)(&trackerExisting_scratchPad[loc]);
	loc += sizeof(sTrackerExisting_probLife) * TRACKEREXISTING_MAXPROBLIFENUM;
	inst->probMotion_v = (sTrackerExisting_probMotion*)(&trackerExisting_scratchPad[loc]);
	loc += sizeof(sTrackerExisting_probMotion) * TRACKEREXISTING_MAXPROBMOTIONNUM;
	inst->probMotion_or_v = (sTrackerExisting_probMotion*)(&trackerExisting_scratchPad[loc]);
	loc += sizeof(sTrackerExisting_probMotion) * TRACKEREXISTING_OR_MAXPROBMOTIONNUM;
	init_ret = trackerExisting_init(algObj);
	if (init_ret < 0) {
		return -1;
	}
	return 0;
}

//int radarPoseEstimationAlgInstance_init(sAlgObjData* algObj)
//{
//	sRadarPoseEstimation* radarPostEstimation_inst = (sRadarPoseEstimation*)algObj->radarPoseEstimation_inst;
//	radarPostEstimation_inst->magCaliParam = magCaliParam_malloc;
//	radarPostEstimation_inst->magCaliData = magCaliData_malloc;
//	memset(radarPostEstimation_inst->magCaliParam, 0, sizeof(sMagCaliParam));
//	memset(radarPostEstimation_inst->a, 0, 3 * sizeof(float));
//	memset(radarPostEstimation_inst->mag, 0, 3 * sizeof(float));
//	radarPostEstimation_inst->roll = 0.f;
//	radarPostEstimation_inst->pitch = 0.f;
//	radarPostEstimation_inst->yaw = 0.f;
//	radarPostEstimation_inst->caliDone = 0;
//	return 0;
//}

#ifndef _WINDOWS
extern sAlgObjData *gAlgData;
int alg_init_init( void )
{
	gAlgData = alg_init();
	
	return 0;
}
#endif

sAlgObjData* alg_init()
{
	sAlgObjData* algObj = gAlgObj;
	moduleMemCalculate(algObj);
	algBufferAlloc(algObj);

	// Initialize measurement process algorithm module
	if (measProcAlgInstance_init(algObj) < 0) {
		//printf("measurement process module initialization fails!!!");
		gAlgInitFlag = -1;
		return algObj;
	}
	// Initialize tracking algorithm module
	if (trackingAlgInstance_init(algObj) < 0) {
		//printf("Tracking initialization fails!!!");
		gAlgInitFlag = -1;
		return algObj;
	}
	// Initialize dispatch algorithm module
	if (dispatchAlgInstance_init(algObj) < 0) {
		//printf("dispatch initialization fails!!!");
		gAlgInitFlag = -1;
		return algObj;
	}
	
	if (targetThreatAssessmentAlgInstance_init(algObj) < 0) {
		//printf("target threat assessment initialization fails!!!");
		gAlgInitFlag = -1;
		return algObj;
	}

	if (trackerExistingAlgInstance_init(algObj) < 0) {
		//printf("TrackerExisting initialization fails!!!");
		gAlgInitFlag = -1;
		return algObj;
	}
	//if (radarPoseEstimationAlgInstance_init(algObj) < 0) {
	//	//printf("TrackerExisting initialization fails!!!");
	//	gAlgInitFlag = -1;
	//	return algObj;
	//}
	
	gAlgInitFlag = 0;
	return algObj;
}
/**
 * @b Description
 * @n
 *      .
 * 
 * @param[in] 
 * @param[in] 
 *       
 *       
 */
void measurementConfigLoad(sMeasConfigParam* measConfigParam, sDebugAndCtrlParam* ctrlInfo)
{
	;
}
void trackingConfigParamLoad(sTrackingParams* trackingConfigParam, sPlatformInfo* platformInfo, sDebugAndCtrlParam* ctrlInfo)
{
	trackingConfigParam->workMode = ctrlInfo->debugAndCtrlData.workMode;
#ifdef DEBUG_LOG_ZQ
	my_printf("workmode %d", trackingConfigParam->workMode);
#endif // DEBUG_LOG_ZQ

	if (isUserConfigParamValid(ctrlInfo))
	{
		trackingConfigParam->advParams.sceneryParams.scanBoundary.aziScanScope = (float)(ctrlInfo->debugAndCtrlData.aziScanScope + 4);
		trackingConfigParam->advParams.sceneryParams.scanBoundary.eleScanScope = (float)(ctrlInfo->debugAndCtrlData.eleScanScope + 10);
		trackingConfigParam->advParams.sceneryParams.scanBoundary.aziScanCenter = (float)ctrlInfo->debugAndCtrlData.aziScanCenter;
		trackingConfigParam->advParams.sceneryParams.scanBoundary.eleScanCenter = (float)ctrlInfo->debugAndCtrlData.aziScanCenter;
	}
}

void dispatchConfigLoad(sDispatchConfigParam* dispatchConfigParams, sDebugAndCtrlParam* ctrlInfo)
{
	dispatchConfigParams->workMode = ctrlInfo->debugAndCtrlData.workMode;
	if (isUserConfigParamValid(ctrlInfo))
	{
		dispatchConfigParams->valid = true;
		dispatchConfigParams->aziScanCenter = ctrlInfo->debugAndCtrlData.aziScanCenter;
		dispatchConfigParams->aziScanScope = ctrlInfo->debugAndCtrlData.aziScanScope;
		dispatchConfigParams->eleScanCenter = ctrlInfo->debugAndCtrlData.eleScanCenter;
		dispatchConfigParams->eleScanScope = ctrlInfo->debugAndCtrlData.eleScanScope;
	}
	
	//dispatchConfigParams->valid
}
void setDebugAndCtrlParam(sAlgObjData* algObj, sPlatformInfo* platformInfo, sDebugAndCtrlParam* ctrlInfo)
{
	sMeasProcessInst* measurementProcess_inst;
	sMeasConfigParam* measConfigParam;
	sDispatchInst* dispatch_inst;
	sDispatchConfigParam* dispatchConfigParams;
	sTrackingInst* tracking_inst = (sTrackingInst*)algObj->tracking_inst;
	sTrackingParams* trackingConfigParam = &tracking_inst->trackingConfig;
	if (NULL == ctrlInfo || NULL == platformInfo)
		return;
	measurementProcess_inst = (sMeasProcessInst*)algObj->measurementProcess_inst;
	measConfigParam = &measurementProcess_inst->measConfigParam;
	dispatch_inst = (sDispatchInst*)algObj->dispatch_inst;
	dispatchConfigParams = dispatch_inst->dispatchConfigParam;
	// load debug and contrl parameters from exterior data
	gVelunambigType = ctrlInfo->debugAndCtrlData.velUnambigiousMethod;

	measurementConfigLoad(measConfigParam, ctrlInfo);

	trackingConfigParamLoad(trackingConfigParam, platformInfo, ctrlInfo);

	dispatchConfigLoad(dispatchConfigParams, ctrlInfo);

}
uint8_t isUserConfigParamValid(sDebugAndCtrlParam* ctrlInfo)
{
	if (((ctrlInfo->debugAndCtrlData.aziScanScope > 0.1f) && (ctrlInfo->debugAndCtrlData.eleScanScope > 0.1f)) || \
		(ctrlInfo->debugAndCtrlData.aziScanCenter != 0) || (ctrlInfo->debugAndCtrlData.eleScanCenter != 0))
	{
		return 1;
	}
	return 0;
}
//void detectsListInput(sAlgObjData* algObj, sDetectFrame* radar_det)
//{
//	sDetectFrame *detectsListOutput = algObj->detectsListOutput;
//	memcpy(detectsListOutput, radar_det, sizeof(sDetectFrame));
//}
void loadDispatchParam(sAlgObjData* algObj)
{
	sTrackingInst* tracking_inst = (sTrackingInst*)algObj->tracking_inst;
	sTrackingParams* trackingConfig = &tracking_inst->trackingConfig;
	sDispatchInst *dispatch_inst = (sDispatchInst *)algObj->dispatch_inst;

	if (trackingConfig->workMode == 0)
	{
		algObj->scanType = TWS_SCAN;
	}
	else
	{
		algObj->scanType = dispatch_inst->dispatchOutput->trackTwsTasFlag;
}
	g_scanType = algObj->scanType;
	if (dispatch_inst->dispatchOutput->tasBeamTotal == 1 && algObj->scanType == TAS_SCAN)
	{
		gCfmBeamFlag = 1;
	}
	else
	{
		gCfmBeamFlag = 0;
	}
}

void module_process(sAlgObjData* algObj)
{
#if defined (DEBUG_LOG_GYRO)
	testRadarPoseEstimation(algObj);
#endif
	g_detFrameId = algObj->detectsListOutput->frameID;
	g_detTimestamp = algObj->detectsListOutput->timestamp;

#ifndef _WINDOWS
	clock_gettime(CLOCK_MONOTONIC, &g_tUsTrackProcCur[0]);
#endif
	measurement_process(algObj);
#ifndef _WINDOWS
	clock_gettime(CLOCK_MONOTONIC, &g_tUsTrackProcEnd[0]);
	g_tUsTrackProcUsed[0] = ((g_tUsTrackProcEnd[0].tv_sec - g_tUsTrackProcCur[0].tv_sec) * 1000000000LL +
					   (g_tUsTrackProcEnd[0].tv_nsec - g_tUsTrackProcCur[0].tv_nsec))/1000;
	clock_gettime(CLOCK_MONOTONIC, &g_tUsTrackProcCur[1]);
#endif

	tracking_process(algObj);    // Runs a single step of the given algorithm instance with input point cloud data
#ifndef _WINDOWS
	clock_gettime(CLOCK_MONOTONIC, &g_tUsTrackProcEnd[1]);
	g_tUsTrackProcUsed[1] = ((g_tUsTrackProcEnd[1].tv_sec - g_tUsTrackProcCur[1].tv_sec) * 1000000000LL +
					   (g_tUsTrackProcEnd[1].tv_nsec - g_tUsTrackProcCur[1].tv_nsec))/1000;
	clock_gettime(CLOCK_MONOTONIC, &g_tUsTrackProcCur[2]);
#endif
	targetThreatAssessment_process(algObj);

#ifndef _WINDOWS
	clock_gettime(CLOCK_MONOTONIC, &g_tUsTrackProcEnd[2]);
	g_tUsTrackProcUsed[2] = ((g_tUsTrackProcEnd[2].tv_sec - g_tUsTrackProcCur[2].tv_sec) * 1000000000LL +
					   (g_tUsTrackProcEnd[2].tv_nsec - g_tUsTrackProcCur[2].tv_nsec))/1000;
	clock_gettime(CLOCK_MONOTONIC, &g_tUsTrackProcCur[3]);
#endif

	DispatchProcess(algObj);
#ifndef _WINDOWS
	clock_gettime(CLOCK_MONOTONIC, &g_tUsTrackProcEnd[3]);
	g_tUsTrackProcUsed[3] = ((g_tUsTrackProcEnd[3].tv_sec - g_tUsTrackProcCur[3].tv_sec) * 1000000000LL +
					   (g_tUsTrackProcEnd[3].tv_nsec - g_tUsTrackProcCur[3].tv_nsec))/1000;
#endif
	//trackerExisting_process(algObj);
}

void alg_process(sAlgObjData* algObj)
{
	loadDispatchParam(algObj);

	module_process(algObj);
}

int32_t Get_g_byDummyTest(void)
{
	return g_byDummyTest;
}

int32_t Get_g_byAbudhabiShowScenes(void)
{
	return g_byAbudhabiShowScenes;
}



