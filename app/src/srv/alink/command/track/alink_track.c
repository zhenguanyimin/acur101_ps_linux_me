
#include "alink_track.h"
#include "../../../protocol/protocol_object.h"
#include "../../../protocol/protocol_parameter.h"
#include "../../../protocol/protocol_system.h"

static void track_set_upload_mode( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	alink_track_set_upload_mode_t *psUploadMode;
	alink_track_set_upload_mode_ack_t *psUploadModeAck;

	psUploadMode = (alink_track_set_upload_mode_t*)(psPayload->pbyData);
	psUploadModeAck = (alink_track_set_upload_mode_ack_t*)(psResp->pbyBuffer);

	psUploadModeAck->status = 0;
	psResp->wCount = sizeof(alink_track_set_upload_mode_ack_t);
}

extern protocol_cfg_param_t gConfigParmInfo[1];
static void track_get_config_info( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	alink_track_get_config_info_t *psGetInfo;
	alink_track_get_config_info_ack_t *psGetInfoAck;

	psGetInfo = (alink_track_get_config_info_t*)(psPayload->pbyData);
	psGetInfoAck = (alink_track_get_config_info_ack_t*)(psResp->pbyBuffer);

	psGetInfoAck->trSwitchCtrl			= gConfigParmInfo[0].trSwitchCtrl;
	psGetInfoAck->workMode				= gConfigParmInfo[0].workMode;
	psGetInfoAck->workWaveCode			= gConfigParmInfo[0].workWaveCode;
	psGetInfoAck->workFreqCode			= gConfigParmInfo[0].workFrqCode;
	psGetInfoAck->prfPeriod				= gConfigParmInfo[0].prfPeriod;
	psGetInfoAck->accuNum				= gConfigParmInfo[0].accuNum;
	psGetInfoAck->cohesionVelThre		= gConfigParmInfo[0].cohesionVelThre;
	psGetInfoAck->cohesionRgnThre		= gConfigParmInfo[0].cohesionRgnThre;
	psGetInfoAck->clutterMapSwitch		= gConfigParmInfo[0].clutterMapSwitch;
	psGetInfoAck->clutterMapUpdateCoef	= gConfigParmInfo[0].clutterMapUpdateCoef;
	psGetInfoAck->aziCalcSlope			= gConfigParmInfo[0].aziCalcSlope;
	psGetInfoAck->aziCalcPhase			= gConfigParmInfo[0].aziCalcPhase;
	psGetInfoAck->eleCalcSlope			= gConfigParmInfo[0].eleCalcSlope;
	psGetInfoAck->eleCalcPhase			= gConfigParmInfo[0].eleCalcPhase;
	psGetInfoAck->aziScanCenter			= gConfigParmInfo[0].aziScanCenter;
	psGetInfoAck->aziScanScope			= gConfigParmInfo[0].aziScanScope;
	psGetInfoAck->eleScanCenter			= gConfigParmInfo[0].eleScanCenter;
	psGetInfoAck->eleScanScope			= gConfigParmInfo[0].eleScanScope;
	psGetInfoAck->coherentDetectSwitch	= gConfigParmInfo[0].coherentDetectSwitch;
	psGetInfoAck->noiseCoef				= gConfigParmInfo[0].noiseCoef;
	psGetInfoAck->clutterCoef			= gConfigParmInfo[0].clutterCoef;
	psGetInfoAck->cfarCoef				= gConfigParmInfo[0].cfarCoef;
	psGetInfoAck->focusRangeMin			= gConfigParmInfo[0].focusRangeMin;
	psGetInfoAck->focusRangeMax			= gConfigParmInfo[0].focusRangeMax;
	psGetInfoAck->clutterCurveNum		= gConfigParmInfo[0].clutterCurveNum;
	psGetInfoAck->lobeCompCoef			= gConfigParmInfo[0].lobeCompCoef;
	
	psResp->wCount = sizeof(alink_track_get_config_info_ack_t);
}

static uint16_t track_pkg_detected( uint8_t *pbyBuffer, uint16_t wSize, void *pvObjDetect )
{
	uint32_t i = 0;
	alink_track_detected_t *psDetected;
	protocol_object_list_detected_t *psObjDetect;

	psObjDetect = (protocol_object_list_detected_t*)pvObjDetect;
	psDetected = (alink_track_detected_t*)pbyBuffer;
	

	psDetected->detectObjNum	= psObjDetect->detectObjNum;
	psDetected->trackTwsTasFlag	= psObjDetect->trackTwsTasFlag;
	psDetected->detectObjByte	= sizeof(alink_track_detected_item_t);
	psDetected->aziBeamID		= psObjDetect->detectPoint[0].aziBeamID;
	psDetected->eleBeamID		= psObjDetect->detectPoint[0].eleBeamID;

	for( ; psDetected->detectObjNum > i; ++i )
	{
		psDetected->detectPoint[i].id 				= psObjDetect->detectPoint[i].id;
		psDetected->detectPoint[i].reserved 		= 0;
		psDetected->detectPoint[i].ambiguous 		= psObjDetect->detectPoint[i].ambiguous;
		psDetected->detectPoint[i].classification	= psObjDetect->detectPoint[i].classification;
		psDetected->detectPoint[i].classfyProb 		= psObjDetect->detectPoint[i].classifyProb;
		psDetected->detectPoint[i].cohesionOkFlag 	= psObjDetect->detectPoint[i].cohesionOkFlag;
		psDetected->detectPoint[i].cohesionPntNum 	= psObjDetect->detectPoint[i].cohesionPntNum;
		psDetected->detectPoint[i].cohesionBeamNum 	= psObjDetect->detectPoint[i].cohesionBeamNum;
		psDetected->detectPoint[i].azimuth 			= psObjDetect->detectPoint[i].azimuth;
		psDetected->detectPoint[i].elevation 		= psObjDetect->detectPoint[i].elevation;
		psDetected->detectPoint[i].velocity 		= psObjDetect->detectPoint[i].velocity;
		psDetected->detectPoint[i].dopplerChn 		= psObjDetect->detectPoint[i].dopplerChn;
		psDetected->detectPoint[i].mag 				= psObjDetect->detectPoint[i].mag;
		psDetected->detectPoint[i].objConfidence 	= psObjDetect->detectPoint[i].objConfidence;
		psDetected->detectPoint[i].range 			= psObjDetect->detectPoint[i].range;
	}

	return 5+i*24;
}

static uint16_t track_pkg_tracked( uint8_t *pbyBuffer, uint16_t wSize, void *pvObjTrack )
{
	uint32_t i = 0;
	alink_track_tracked_t *psTracked;
	protocol_object_list_tracked_t *psObjTrack;

	psObjTrack = (protocol_object_list_tracked_t*)pvObjTrack;
	psTracked = (alink_track_tracked_t*)pbyBuffer;

	psTracked->reserved		= 0;
	psTracked->trackObjNum	= psObjTrack->trackObjNum;
	psTracked->trackTwsNum	= psObjTrack->trackTwsNum;
	psTracked->trackTasNum	= psObjTrack->trackTasNum;
	psTracked->trackObjByte	= sizeof(alink_track_tracked_item_t);

	for( ; psTracked->trackObjNum > i; ++i )
	{
		psTracked->trackPoint[i].id 				= psObjTrack->trackPoint[i].id;
		psTracked->trackPoint[i].classification 	= psObjTrack->trackPoint[i].classification;
		psTracked->trackPoint[i].classfyProb 		= psObjTrack->trackPoint[i].classifyProb;
		psTracked->trackPoint[i].existingProb 		= psObjTrack->trackPoint[i].existingProb;
		psTracked->trackPoint[i].ambiguous 			= psObjTrack->trackPoint[i].ambiguous;
		psTracked->trackPoint[i].twsTasFlag 		= psObjTrack->trackPoint[i].twsTasFlag;
		psTracked->trackPoint[i].stateType 			= psObjTrack->trackPoint[i].stateType;
		psTracked->trackPoint[i].motionType 		= psObjTrack->trackPoint[i].motionType;
		psTracked->trackPoint[i].azimuth 			= psObjTrack->trackPoint[i].azimuth;
		psTracked->trackPoint[i].range 				= psObjTrack->trackPoint[i].range;
		psTracked->trackPoint[i].elevation 			= psObjTrack->trackPoint[i].elevation;
		psTracked->trackPoint[i].velocity 			= psObjTrack->trackPoint[i].velocity;
		psTracked->trackPoint[i].dopplerChn 		= psObjTrack->trackPoint[i].dopplerChn;
		psTracked->trackPoint[i].mag 				= psObjTrack->trackPoint[i].mag;
		psTracked->trackPoint[i].absVel 			= psObjTrack->trackPoint[i].absVel;
		psTracked->trackPoint[i].orientationAngle 	= psObjTrack->trackPoint[i].orientationAngle;
		psTracked->trackPoint[i].alive 				= psObjTrack->trackPoint[i].alive;
		psTracked->trackPoint[i].x 					= psObjTrack->trackPoint[i].x;
		psTracked->trackPoint[i].y 					= psObjTrack->trackPoint[i].y;
		psTracked->trackPoint[i].z 					= psObjTrack->trackPoint[i].z;
		psTracked->trackPoint[i].vx 				= psObjTrack->trackPoint[i].vx;
		psTracked->trackPoint[i].vy 				= psObjTrack->trackPoint[i].vy;
		psTracked->trackPoint[i].vz 				= psObjTrack->trackPoint[i].vz;
		psTracked->trackPoint[i].ax 				= psObjTrack->trackPoint[i].ax;
		psTracked->trackPoint[i].ay 				= psObjTrack->trackPoint[i].ay;
		psTracked->trackPoint[i].az 				= psObjTrack->trackPoint[i].az;
		psTracked->trackPoint[i].x_variance 		= psObjTrack->trackPoint[i].x_variance;
		psTracked->trackPoint[i].y_variance 		= psObjTrack->trackPoint[i].y_variance;
		psTracked->trackPoint[i].z_variance 		= psObjTrack->trackPoint[i].z_variance;
		psTracked->trackPoint[i].vx_variance 		= psObjTrack->trackPoint[i].vx_variance;
		psTracked->trackPoint[i].vy_variance 		= psObjTrack->trackPoint[i].vy_variance;
		psTracked->trackPoint[i].vz_variance 		= psObjTrack->trackPoint[i].vz_variance;
		psTracked->trackPoint[i].ax_variance 		= psObjTrack->trackPoint[i].ax_variance;
		psTracked->trackPoint[i].ay_variance 		= psObjTrack->trackPoint[i].ay_variance;
		psTracked->trackPoint[i].az_variance 		= psObjTrack->trackPoint[i].az_variance;
		psTracked->trackPoint[i].forcastFrameNum 	= psObjTrack->trackPoint[i].forcastFrameNum;
		psTracked->trackPoint[i].associationNum 	= psObjTrack->trackPoint[i].associationNum;
		psTracked->trackPoint[i].assocBit0 			= psObjTrack->trackPoint[i].assocBit0;
		psTracked->trackPoint[i].assocBit1 			= psObjTrack->trackPoint[i].assocBit1;
	}

	return 5+i*82;
}

#include <sys/time.h>
extern uint8_t bFlagPlathfrom;
static uint16_t track_pkg_tracked_plus( uint8_t *pbyBuffer, uint16_t wSize, void *pvObjTrack )
{
	uint32_t i = 0;
	uint32_t j = 0;
	alink_track_tracked_plus_t *psTracked;
	protocol_object_list_tracked_t *psObjTrack;

	psObjTrack = (protocol_object_list_tracked_t*)pvObjTrack;
	psTracked = (alink_track_tracked_plus_t*)pbyBuffer;

	psTracked->status		= bFlagPlathfrom;
	psTracked->trackObjNum	= psObjTrack->trackObjNum;
	psTracked->trackTwsNum	= psObjTrack->trackTwsNum;
	psTracked->trackTasNum	= psObjTrack->trackTasNum;
	psTracked->trackObjByte	= sizeof(alink_track_tracked_item_plus_t);

	struct timeval tv;
	gettimeofday(&tv, NULL);
	psTracked->timestamp = 10000000 * tv.tv_sec + tv.tv_usec;

	for( ; psTracked->trackObjNum > i; ++i )
	{
		if( 1 == psObjTrack->trackPoint[i].stateType )
		{
			psTracked->trackPoint[j].id 			= psObjTrack->trackPoint[i].id;
			psTracked->trackPoint[j].classification 	= psObjTrack->trackPoint[i].classification;
			psTracked->trackPoint[j].classfyProb 		= psObjTrack->trackPoint[i].classifyProb;
			psTracked->trackPoint[j].existingProb 		= psObjTrack->trackPoint[i].existingProb;
			psTracked->trackPoint[j].ambiguous 		= psObjTrack->trackPoint[i].ambiguous;
			psTracked->trackPoint[j].twsTasFlag 		= psObjTrack->trackPoint[i].twsTasFlag;
			psTracked->trackPoint[j].stateType 		= psObjTrack->trackPoint[i].stateType;
			psTracked->trackPoint[j].motionType 		= psObjTrack->trackPoint[i].motionType;
			psTracked->trackPoint[j].azimuth 		= psObjTrack->trackPoint[i].azimuth;
			psTracked->trackPoint[j].range 			= psObjTrack->trackPoint[i].range;
			psTracked->trackPoint[j].elevation 		= psObjTrack->trackPoint[i].elevation;
			psTracked->trackPoint[j].velocity 		= psObjTrack->trackPoint[i].velocity;
			psTracked->trackPoint[j].dopplerChn 		= psObjTrack->trackPoint[i].dopplerChn;
			psTracked->trackPoint[j].mag 			= psObjTrack->trackPoint[i].mag;
			psTracked->trackPoint[j].absVel 		= psObjTrack->trackPoint[i].absVel;
			psTracked->trackPoint[j].orientationAngle 	= psObjTrack->trackPoint[i].orientationAngle;
			psTracked->trackPoint[j].alive 			= psObjTrack->trackPoint[i].alive;
			psTracked->trackPoint[j].x 			= psObjTrack->trackPoint[i].x;
			psTracked->trackPoint[j].y 			= psObjTrack->trackPoint[i].y;
			psTracked->trackPoint[j].z 			= psObjTrack->trackPoint[i].z;
			psTracked->trackPoint[j].vx 			= psObjTrack->trackPoint[i].vx;
			psTracked->trackPoint[j].vy 			= psObjTrack->trackPoint[i].vy;
			psTracked->trackPoint[j].vz 			= psObjTrack->trackPoint[i].vz;
			psTracked->trackPoint[j].ax 			= psObjTrack->trackPoint[i].ax;
			psTracked->trackPoint[j].ay 			= psObjTrack->trackPoint[i].ay;
			psTracked->trackPoint[j].az 			= psObjTrack->trackPoint[i].az;
			psTracked->trackPoint[j].x_variance 		= psObjTrack->trackPoint[i].x_variance;
			psTracked->trackPoint[j].y_variance 		= psObjTrack->trackPoint[i].y_variance;
			psTracked->trackPoint[j].z_variance 		= psObjTrack->trackPoint[i].z_variance;
			psTracked->trackPoint[j].vx_variance 		= psObjTrack->trackPoint[i].vx_variance;
			psTracked->trackPoint[j].vy_variance 		= psObjTrack->trackPoint[i].vy_variance;
			psTracked->trackPoint[j].vz_variance 		= psObjTrack->trackPoint[i].vz_variance;
			psTracked->trackPoint[j].ax_variance 		= psObjTrack->trackPoint[i].ax_variance;
			psTracked->trackPoint[j].ay_variance 		= psObjTrack->trackPoint[i].ay_variance;
			psTracked->trackPoint[j].az_variance 		= psObjTrack->trackPoint[i].az_variance;
			psTracked->trackPoint[j].forcastFrameNum 	= psObjTrack->trackPoint[i].forcastFrameNum;
			psTracked->trackPoint[j].associationNum 	= psObjTrack->trackPoint[i].associationNum;
			psTracked->trackPoint[j].assocBit0 		= psObjTrack->trackPoint[i].assocBit0;
			psTracked->trackPoint[j].assocBit1 		= psObjTrack->trackPoint[i].assocBit1;
			++j;
		}
	}

	psTracked->trackObjNum	= j;
	psTracked->trackTwsNum	= psTracked->trackObjNum - psTracked->trackTasNum;

	return 13+j*83;
}

static uint16_t track_pkg_beam_scheduling( uint8_t *pbyBuffer, uint16_t wSize, void *pvObjBeamScheduling )
{
	alink_track_beam_scheduling_t *psBeamScheduling;
	protocol_beam_scheduling_t *psObjBeamScheduling;

	psObjBeamScheduling = (protocol_beam_scheduling_t*)pvObjBeamScheduling;
	psBeamScheduling = (alink_track_beam_scheduling_t*)pbyBuffer;

	psBeamScheduling->aziBeamID 				= psObjBeamScheduling->aziBeamID;
	psBeamScheduling->eleBeamID 				= psObjBeamScheduling->eleBeamID;
	psBeamScheduling->trackTwsTasFlag 			= psObjBeamScheduling->trackTwsTasFlag;
	psBeamScheduling->tasObjId 					= psObjBeamScheduling->tasObjId;
	psBeamScheduling->tasBeamTotal 				= psObjBeamScheduling->tasBeamTotal;
	psBeamScheduling->tasBeamCntCur 			= psObjBeamScheduling->tasBeamCntCur;
	psBeamScheduling->aziBeamSin 				= psObjBeamScheduling->aziBeamSin;
	psBeamScheduling->eleBeamSin 				= psObjBeamScheduling->eleBeamSin;
	psBeamScheduling->tasObjFilterNum 			= psObjBeamScheduling->tasObjFilterNum;
	psBeamScheduling->tasObjRange 				= psObjBeamScheduling->tasObjRange;
	psBeamScheduling->samplePntStart 			= psObjBeamScheduling->samplePntStart;
	psBeamScheduling->samplePntDepth 			= psObjBeamScheduling->samplePntDepth;
	psBeamScheduling->beamSwitchTime 			= psObjBeamScheduling->beamSwitchTime;
	psBeamScheduling->wholeSpaceScanCycleCnt 	= psObjBeamScheduling->wholeSpaceScanCycleCnt;

	return 25;
}

static uint16_t track_pkg_attitude( uint8_t *pbyBuffer, uint16_t wSize, void *pvObjPlatfrom )
{
	uint32_t i;
	alink_track_attitude_t *psAttitude;
	protocol_radar_platfrom_info_t *psObjPlatfrom;

	psObjPlatfrom = (protocol_radar_platfrom_info_t*)pvObjPlatfrom;
	psAttitude = (alink_track_attitude_t*)pbyBuffer;

	psAttitude->heading 		= psObjPlatfrom->heading;
	psAttitude->pitching 		= psObjPlatfrom->pitching;
	psAttitude->rolling 		= psObjPlatfrom->rolling;
	psAttitude->longitude 		= psObjPlatfrom->longitude;
	psAttitude->latitude 		= psObjPlatfrom->latitude;
	psAttitude->altitude 		= psObjPlatfrom->altitude;
	psAttitude->velocityNavi 	= psObjPlatfrom->velocityNavi;
	for( i = 0; sizeof(psAttitude->targetTimeMark)/sizeof(psAttitude->targetTimeMark[0]) > i; ++i )
	{
		psAttitude->targetTimeMark[i]	= psObjPlatfrom->targetTimeMark[i];
	}
	psAttitude->sigProcRelativeTime 	= psObjPlatfrom->sigProcRelativeTime;

	return 45;
}

static alink_cmd_list_t sAlinkCmd_TrackSetUploadMode = { 0 };
static alink_cmd_list_t sAlinkCmd_TrackGetConfigInfo = { 0 };

static alink_package_list_t sAlinkPackage_Detected = { 0 };
static alink_package_list_t sAlinkPackage_Tracked = { 0 };
static alink_package_list_t sAlinkPackage_TrackedPlus = { 0 };
static alink_package_list_t sAlinkPackage_BeamScheduling = { 0 };
static alink_package_list_t sAlinkPackage_Attitude = { 0 };

void alink_track_init( void )
{
	alink_register_cmd( &sAlinkCmd_TrackSetUploadMode, 		0x11,	sizeof(alink_track_set_upload_mode_t),	track_set_upload_mode );
	alink_register_cmd( &sAlinkCmd_TrackGetConfigInfo, 		0x15,	sizeof(alink_track_get_config_info_t),	track_get_config_info );

	alink_register_package( &sAlinkPackage_Detected,		0x10,	0, 										track_pkg_detected );
	alink_register_package( &sAlinkPackage_Tracked,			0x12,	0, 										track_pkg_tracked );
	alink_register_package( &sAlinkPackage_TrackedPlus,		0x1F,	0, 										track_pkg_tracked_plus );
	alink_register_package( &sAlinkPackage_BeamScheduling,	0x14,	0, 										track_pkg_beam_scheduling );
	alink_register_package( &sAlinkPackage_Attitude,		0x13,	0, 										track_pkg_attitude );
}

int32_t alink_upload_detected( void *psObjDetect )
{
	return alink_upload_package( &sAlinkPackage_Detected, psObjDetect );
}

int32_t alink_upload_tracked( void *psObjTrack )
{
	alink_upload_package( &sAlinkPackage_TrackedPlus, psObjTrack );
	return alink_upload_package( &sAlinkPackage_Tracked, psObjTrack );
}

int32_t alink_upload_beam_scheduling( void *psObjBeamScheduling )
{
	return alink_upload_package( &sAlinkPackage_BeamScheduling, psObjBeamScheduling );
}

int32_t alink_upload_attitude( void *psObjPlatfrom )
{
	return alink_upload_package( &sAlinkPackage_Attitude, psObjPlatfrom );
}

