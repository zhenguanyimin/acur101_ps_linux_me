#include <arpa/inet.h>
#include "tracking_interface.h"
#include "src/target_process/dispatch/include/dispatch.h"
#include "../../inc/radar_common.h"
#include "../../srv/protocol/protocol_if.h"
#include "../DataPath/data_path.h"
#include "../../app/beam_calculate/bk_cal.h"
#include "../../app/temp_table/temp_table.h"
#include "../../srv/alink/command/track/alink_track.h"
#include "../../srv/flash_nv/flash_raw.h"

volatile int32_t g_byDevHwGen = 3;
volatile int32_t g_byMagClutRemove = 0;
volatile int32_t g_byDummyTest = 0;
volatile int32_t g_byAbudhabiShowScenes = 0;

protocol_object_list_detected_t g_obj_list_detected_send;
protocol_object_list_tracked_t g_obj_list_tracked_send;
protocol_object_list_dot_cohe_t g_obj_list_dot_cohe_send;
protocol_beam_scheduling_t g_beam_scheduling_send;
protocol_radar_status_t g_radar_status_send;
protocol_beam_scheduling_t gBeamInfoLast[1] = { 0 };

uint32_t gPrcoTimeTracking = 0;
uint32_t gPrcoTimeReport = 0;

sAlgObjData *gAlgData = NULL;
int init_flag;

sDummyRadarMeasUnit dummyData[MAX_NUM_DETECTS];
float curTime_dummy = 0.f;
float varyVx = 3.f;
float varyVy = 0.f;
int signDummyAccX = 1;
int signDummyAccY = 1;
uint32_t frameID = 0;

extern uint32_t gBeamMode ;

sTimingCtrlPara_t gTimingCtrlPara[WAVE_TYPE_NUM] =
{
	{
		.cpiDelay		= WAVE_TYPE_0_CPI_DELAY,
		.priPeriod		= WAVE_TYPE_0_PRI_PERIOD,
		.priNum			= WAVE_TYPE_0_PRI_NUM,
		.priPulseWidth	= WAVE_TYPE_0_PRI_PULSE_WIDTH,
		.startSample	= WAVE_TYPE_0_START_SAMPLE,
		.sampleLength	= WAVE_TYPE_0_SAMPLE_LENGTH,
		.waveCode 		= WAVE_TYPE_0_WAVE_CODE
	},
	{
		.cpiDelay 		= WAVE_TYPE_1_CPI_DELAY,
		.priPeriod 		= WAVE_TYPE_1_PRI_PERIOD,
		.priNum 		= WAVE_TYPE_1_PRI_NUM,
		.priPulseWidth	= WAVE_TYPE_1_PRI_PULSE_WIDTH,
		.startSample	= WAVE_TYPE_1_START_SAMPLE,
		.sampleLength	= WAVE_TYPE_1_SAMPLE_LENGTH,
		.waveCode		= WAVE_TYPE_1_WAVE_CODE
	}
};

#include "../../bsp/reg/pl_reg.h"

extern int aaaaaaamode;
static bool aaaaaaaflag = 0;

extern void monitor_temp_updata_txrx( void* pvData );
extern void monitor_power_updata_txrx( void* pvData );

#define TRACK_WRITE_REG(addr,data)		PL_REG_WRITE(0xFFFF&addr,data)
#define TRACK_READ_REG(addr)			PL_REG_READ(0xFFFF&addr)
#include <sys/time.h>
#include <stdio.h>
#include <time.h>

static int temp_compensation_set_table(float *temperature, int temp_len, char name_off)
{
	int i =0, ret = -1;
	uint32_t awmf_name = 0, reg_addr = 0, reg_val_high = 0, reg_val_low = 0, awmf_id = 0;
	unsigned long reg_val = 0;

	for(i=0; i<temp_len; i++) {
		awmf_name = name_off + i;
		ret = awmf_get_chip_id_by_name(awmf_name, &awmf_id);
		if(RET_OK == ret) {
			reg_val = temp_table_lookup(temperature[i]);
			reg_val_high = (reg_val & 0xffffffff00000000) >> 32;	
			reg_val_low = reg_val & 0xffffffff;
			ret = awmf_0165_drv_reg_set(awmf_id, 0x05, reg_val_high, reg_val_low);
			if( 0 != ret) {
				APP_LOG_ERROR("name %x, id %d, reg 0x05, reg_val %lu fail, ret = %d\n", awmf_name, awmf_id, reg_val, ret);
			}
		}
	}

	return 0;
}

void runTrackingAlg()
{
	struct timespec tCur1, tCur2;
	struct timespec tEnd1, tEnd2;
	uint32_t tUsed1, tUsed2;
	uint8_t fre_num = 0;
	int16_t az_deg = 0, el_deg = 0;
	sDispatchInst *dispatch_inst = (sDispatchInst *)gAlgData->dispatch_inst;

	clock_gettime(CLOCK_MONOTONIC, &tCur1);

	/* has been init at radar_alg_task */
//	if (gAlgInitFlag != 0)
//	{
//		gAlgData = alg_init();
//		init_flag = gAlgInitFlag;
//	}
	init_flag = gAlgInitFlag;
	//printf( "[%s %d]\r\n", __func__, __LINE__ );

	memcpy(gBeamInfoLast, gBeamInfo, sizeof(gBeamInfoLast));
	//printf( "[%s %d]\r\n", __func__, __LINE__ );
	gConfigParmInfo[0].velUnambigiousMethod = aaaaaaamode;

	if( g_byDummyTest )
	{
		SetDummyConfigParmInfo(gPlatformInfo, gConfigParmInfo);
		process_dummy3D(gDetectList, gTrackList, gDotCoheList, gBeamInfo, gPlatformInfo, gConfigParmInfo, gRadarStatus);
	}
	else
	{
		trackingAlgProcess(gDetectList, gTrackList, gDotCoheList, gBeamInfo, gPlatformInfo, gConfigParmInfo, gRadarStatus);
	}
	//printf( "[%s %d]\r\n", __func__, __LINE__ );

	static int uTempTimeCount = 0;
	/*
	* we don't need this anymore, block it. 2023.8.25
	*/
	if( 0 )
	{
		uTempTimeCount = 0;

		float power[17*8];
		float temperature[17];
		awmf0165_get_temperature_u8( &temperature[16] );
		awmf0165_get_temperature( &temperature[0] );

		awmf0165_get_power_u8(&power[16*8]);
		awmf0165_get_power(&power[0*8]);

		#if 0
		printf(
		"\r\nawmf0165 power is :\r\n"
		"	U16:%f %f %f %f %f %f %f %f\r\n	U14:%f %f %f %f %f %f %f %f\r\n	U13:%f %f %f %f %f %f %f %f\r\n	U15:%f %f %f %f %f %f %f %f\r\n"
		"	U12:%f %f %f %f %f %f %f %f\r\n	U10:%f %f %f %f %f %f %f %f\r\n	U09:%f %f %f %f %f %f %f %f\r\n	U11:%f %f %f %f %f %f %f %f\r\n"
		"	U23:%f %f %f %f %f %f %f %f\r\n	U21:%f %f %f %f %f %f %f %f\r\n	U22:%f %f %f %f %f %f %f %f\r\n	U24:%f %f %f %f %f %f %f %f\r\n"
		"	U19:%f %f %f %f %f %f %f %f\r\n	U17:%f %f %f %f %f %f %f %f\r\n	U18:%f %f %f %f %f %f %f %f\r\n	U20:%f %f %f %f %f %f %f %f\r\n",
		power[0], power[1], power[2], power[3], power[4], power[5], power[6], power[7],		
		power[8], power[9], power[10], power[11], power[12], power[13], power[14], power[15],
		power[16], power[17], power[18], power[19], power[20], power[21], power[22], power[23],
		power[24], power[25], power[26], power[27], power[28], power[29], power[30], power[31],
		power[32], power[33], power[34], power[5], power[36], power[37], power[38], power[39],
		power[40], power[41], power[42], power[43], power[44], power[45], power[46], power[47],
		power[48], power[49], power[50], power[51], power[52], power[53], power[54], power[55],
		power[56], power[57], power[58], power[59], power[60], power[61], power[62], power[63],
		power[64], power[65], power[66], power[67], power[68], power[69], power[70], power[71],
		power[72], power[73], power[74], power[75], power[76], power[77], power[78], power[79],
		power[80], power[81], power[82], power[83], power[84], power[85], power[86], power[87],
		power[88], power[89], power[90], power[91], power[92], power[93], power[94], power[95],
		power[96], power[97], power[98], power[99], power[100], power[101], power[102], power[103],
		power[104], power[105], power[106], power[107], power[108], power[109], power[110], power[111],
		power[112], power[113], power[114], power[115], power[116], power[117], power[118], power[119],
		power[120], power[121], power[122], power[123], power[124], power[125], power[126], power[127]
		);
		
		#endif
		#if 0
		printf(
		"\r\nawmf0165 temperature is :\r\n"
		"	U16:%0d.%03d\r\n	U14:%0d.%03d\r\n	U13:%0d.%03d\r\n	U15:%0d.%03d \r\n"
		"	U12:%0d.%03d\r\n	U10:%0d.%03d\r\n	U09:%0d.%03d\r\n	U11:%0d.%03d \r\n"
		"	U23:%0d.%03d\r\n	U21:%0d.%03d\r\n	U22:%0d.%03d\r\n	U24:%0d.%03d \r\n"
		"	U19:%0d.%03d\r\n	U17:%0d.%03d\r\n	U18:%0d.%03d\r\n	U20:%0d.%03d \r\n",
		(int)(temperature[0]), FractionToInt(temperature[0]),
		(int)(temperature[1]), FractionToInt(temperature[1]),
		(int)(temperature[2]), FractionToInt(temperature[2]),
		(int)(temperature[3]), FractionToInt(temperature[3]),
		(int)(temperature[4]), FractionToInt(temperature[4]),
		(int)(temperature[5]), FractionToInt(temperature[5]),
		(int)(temperature[6]), FractionToInt(temperature[6]),
		(int)(temperature[7]), FractionToInt(temperature[7]),
		(int)(temperature[8]), FractionToInt(temperature[8]),
		(int)(temperature[9]), FractionToInt(temperature[9]),
		(int)(temperature[10]), FractionToInt(temperature[10]),
		(int)(temperature[11]), FractionToInt(temperature[11]),
		(int)(temperature[12]), FractionToInt(temperature[12]),
		(int)(temperature[13]), FractionToInt(temperature[13]),
		(int)(temperature[14]), FractionToInt(temperature[14]),
		(int)(temperature[15]), FractionToInt(temperature[15])
		);
		#endif

		//temp_compensation_set_table(temperature, sizeof(temperature)/sizeof(float), 0x08);
		float temperature_send[17];
		temperature_send[0] = temperature[6];
		temperature_send[1] = temperature[5];
		temperature_send[2] = temperature[7];
		temperature_send[3] = temperature[4];
		temperature_send[4] = temperature[2];
		temperature_send[5] = temperature[1];
		temperature_send[6] = temperature[3];
		temperature_send[7] = temperature[0];
		temperature_send[8] = temperature[13];
		temperature_send[9] = temperature[14];
		temperature_send[10] = temperature[12];
		temperature_send[11] = temperature[15];
		temperature_send[12] = temperature[9];
		temperature_send[13] = temperature[10];
		temperature_send[14] = temperature[8];
		temperature_send[15] = temperature[11];
		temperature_send[16] = temperature[16];
		monitor_temp_updata_txrx( temperature_send );

		float power_send[17*8];
		for( int8_t i = 0; 8 > i; ++i )
			power_send[0*8 + i] = power[6*8 + i];
		for( int8_t i = 0; 8 > i; ++i )
			power_send[1*8 + i] = power[5*8 + i];
		for( int8_t i = 0; 8 > i; ++i )
			power_send[2*8 + i] = power[7*8 + i];
		for( int8_t i = 0; 8 > i; ++i )
			power_send[3*8 + i] = power[4*8 + i];
		for( int8_t i = 0; 8 > i; ++i )
			power_send[4*8 + i] = power[2*8 + i];
		for( int8_t i = 0; 8 > i; ++i )
			power_send[5*8 + i] = power[1*8 + i];
		for( int8_t i = 0; 8 > i; ++i )
			power_send[6*8 + i] = power[3*8 + i];
		for( int8_t i = 0; 8 > i; ++i )
			power_send[7*8 + i] = power[0*8 + i];
		for( int8_t i = 0; 8 > i; ++i )
			power_send[8*8 + i] = power[13*8 + i];
		for( int8_t i = 0; 8 > i; ++i )
			power_send[9*8 + i] = power[14*8 + i];
		for( int8_t i = 0; 8 > i; ++i )
			power_send[10*8 + i] = power[12*8 + i];
		for( int8_t i = 0; 8 > i; ++i )
			power_send[11*8 + i] = power[15*8 + i];
		for( int8_t i = 0; 8 > i; ++i )
			power_send[12*8 + i] = power[9*8 + i];
		for( int8_t i = 0; 8 > i; ++i )
			power_send[13*8 + i] = power[10*8 + i];
		for( int8_t i = 0; 8 > i; ++i )
			power_send[14*8 + i] = power[8*8 + i];
		for( int8_t i = 0; 8 > i; ++i )
			power_send[15*8 + i] = power[11*8 + i];
		for( int8_t i = 0; 8 > i; ++i )
			power_send[16*8 + i] = power[16*8 + i];

		monitor_power_updata_txrx( power_send );
	}

	az_deg = (int16_t)(round(asinf(dispatch_inst->dispatchOutputNext->aziBeamSin * INV_ONE15FORMAT_SCALE) * RAD2DEG));
	el_deg = (int16_t)(round(asinf(dispatch_inst->dispatchOutputNext->eleBeamSin * INV_ONE15FORMAT_SCALE) * RAD2DEG));
	/*az_deg = ((az_deg >= 0) ? (az_deg) : (az_deg - 1));
	el_deg = ((el_deg >= 0) ? (el_deg) : (el_deg - 1));*/
	//printf( "[%s %d] %x\r\n", __func__, __LINE__, el_deg );

//	rtos_printf( "track %d %d %d %d %d %d\r\n", gConfigParmInfo[0].eleScanCenter, gConfigParmInfo[0].eleScanScope, gConfigParmInfo[0].aziScanCenter, gConfigParmInfo[0].aziScanScope, az_deg, el_deg );

	bk_cak_func(fre_num, -az_deg, -el_deg);
//	printf( "[%s %d]\r\n", __func__, __LINE__ );
	bk_tran_func(TX_CH|RX_CH);
//	printf( "[%s %d]\r\n", __func__, __LINE__ );


#ifdef VELOCITY_UNAMBIGIOUS
	WaveTypeTimingCtrlPara_write_register(gBeamInfo[0].waveType);
#endif

	if( 0 == aaaaaaamode )
	{
		if ( WAVE_TYPE_0 == dispatch_inst->dispatchOutputNext->waveType )
		{
			TRACK_WRITE_REG( 0x800203a0, 0x189c07d0 );
			TRACK_WRITE_REG( 0x800203a8, 0x10010898 );
			TRACK_WRITE_REG( 0x8002042c, 0x01 );
			TRACK_WRITE_REG( 0x8002042c, 0x00 );
		}
		else if ( WAVE_TYPE_1 == dispatch_inst->dispatchOutputNext->waveType )
		{
			TRACK_WRITE_REG( 0x800203a0, 0x14B407d0 );
			TRACK_WRITE_REG( 0x800203a8, 0x100104B0 );
			TRACK_WRITE_REG( 0x8002042c, 0x01 );
                        TRACK_WRITE_REG( 0x8002042c, 0x00 );
		}
	}
	else
	{
		
	}

	struct timespec tTime;
	clock_gettime(CLOCK_MONOTONIC, &tTime);
	uint64_t ulTimesp = (uint64_t)(tTime.tv_sec * 1000 + tTime.tv_nsec/1000000);

	TRACK_WRITE_REG( 0x80021058, dispatch_inst->dispatchOutputNext->waveType );
//	TRACK_WRITE_REG( 0x8002105c, (ulTimesp>>0) & 0xFFFFFFFF );
//	TRACK_WRITE_REG( 0x80021060, (ulTimesp>>32) & 0xFFFFFFFF );
	TRACK_WRITE_REG( 0x80021064, az_deg + 180);
	TRACK_WRITE_REG( 0x80021068, el_deg + 180);
	TRACK_WRITE_REG( 0x8002106c, gConfigParmInfo[0].aziScanCenter );
	TRACK_WRITE_REG( 0x80021070, gConfigParmInfo[0].aziScanScope );
	TRACK_WRITE_REG( 0x80021074, gConfigParmInfo[0].eleScanCenter );
	TRACK_WRITE_REG( 0x80021078, gConfigParmInfo[0].eleScanScope );
	TRACK_WRITE_REG(0x8002107c, dispatch_inst->dispatchOutputNext->trackTwsTasFlag);
	TRACK_WRITE_REG( 0x80021080, dispatch_inst->dispatchOutputNext->wholeSpaceScanCycleFinalBeamFlag );
	clock_gettime(CLOCK_MONOTONIC, &tEnd1);
	tUsed1 = (tEnd1.tv_sec - tCur1.tv_sec) * 1000000 + (tEnd1.tv_nsec - tCur1.tv_nsec)/1000;
//	xil_printf("runTrackingAlg time elapsed is %d us\r\n\n", tUsed1);

	/* process time of tracking */
	gDetectList[0].detectPoint[0].reserve[1] = (uint16_t)tUsed1;
	gPrcoTimeTracking = tUsed1;

	clock_gettime(CLOCK_MONOTONIC, &tCur2);
	trackingReport(gDetectList, gTrackList, gDotCoheList, gBeamInfo, gRadarStatus);
	clock_gettime(CLOCK_MONOTONIC, &tEnd2);
	tUsed2 = (tEnd2.tv_sec - tCur2.tv_sec) * 1000000 + (tEnd2.tv_nsec - tCur2.tv_nsec)/1000;

	/* process time of report */
	gDetectList[0].detectPoint[0].reserve[2] = (uint16_t)tUsed2;
	gPrcoTimeReport = tUsed2;

//	printf( "track %d %d\r\n", tUsed1, tUsed2 );
	//printf( "[%s %d]\r\n", __func__, __LINE__ );
}


void trackingAlgProcess(protocol_object_list_detected_t *detect_list, \
		protocol_object_list_tracked_t *track_list, \
		protocol_object_list_dot_cohe_t *dotCohe_list, \
		protocol_beam_scheduling_t *beam_scheduling, \
		protocol_radar_platfrom_info_t *platformInfo, \
		protocol_cfg_param_t *configParmInfo, \
		protocol_radar_status_t *radar_status)
{
	sDetectFrame *detectsListOutput = gAlgData->detectsListOutput;

	gAlgData->platformInfo->frameID = platformInfo->stInfoHeader.frameID;
	gAlgData->platformInfo->timestamp = platformInfo->stInfoHeader.timestamp;
//	memcpy(&gAlgData->platformInfo->platformData, &platformInfo->heading, sizeof(sPlatformData));

	// ctrl info input
	gAlgData->debugAndCtrlParam->frameID = configParmInfo->stInfoHeader.frameID;
	gAlgData->debugAndCtrlParam->timestamp = configParmInfo->stInfoHeader.timestamp;
	memcpy(&gAlgData->debugAndCtrlParam->debugAndCtrlData, &configParmInfo->trSwitchCtrl, sizeof(sDebugAndCtrlData));

	if (init_flag != -1)
	{
		setDebugAndCtrlParam(gAlgData, gAlgData->platformInfo, gAlgData->debugAndCtrlParam);
	}

	detectsListOutput->detectObjNum = detect_list->detectObjNum;
	detectsListOutput->frameID = detect_list->stInfoHeader.frameID;
	detectsListOutput->timestamp = detect_list->stInfoHeader.timestamp;
	detectsListOutput->trackTwsTasFlag = detect_list->trackTwsTasFlag;
	detectsListOutput->waveType = (sMeasurement_enum_wavetype)detect_list->waveType;
	memcpy(detectsListOutput->detPointsList, detect_list->detectPoint, sizeof(sDetectPoint)*detect_list->detectObjNum);

	alg_process(gAlgData);
}


void process_dummy3D(protocol_object_list_detected_t *detect_list, \
		protocol_object_list_tracked_t *track_list, \
		protocol_object_list_dot_cohe_t *dotCohe_list, \
		protocol_beam_scheduling_t *beam_scheduling, \
		protocol_radar_platfrom_info_t *platformInfo, \
		protocol_cfg_param_t *configParmInfo, \
		protocol_radar_status_t *radar_status)
{
	if( g_byDummyTest )
	{
		sDetectFrame *detectsListOutput = gAlgData->detectsListOutput;
		sDetectPoint *detPointsList = gAlgData->detectsListOutput->detPointsList;
		sDispatchInst *dispatch_inst = (sDispatchInst *)gAlgData->dispatch_inst;
		sDispatchOutput *dispatchOutput = dispatch_inst->dispatchOutput;
		sDebugAndCtrlParam *debugAndCtrlParam = gAlgData->debugAndCtrlParam;

		const int const_nd = (int)(MAX_NUM_DETECTS);//2
		float F[SSIZE_XY*SSIZE_XY] = { 0 };
		int num_dets = 1;//const_nd;  //cdx
		int nd = 0;//[0,num_dets)
		int num_masterPoints = 0;
		int num_slavePoints = 0;
		float cosDelta = 0.f;
		float dt = CONST_DELTA_T;
		float ambiVelScope = 0.f;
		float vx = -5.f, vy = 0.f, vz = 0.f;
	#ifdef DEBUG_LOG
		/** reading dispatchInfo of current frame */
		memcpy(&dispatchInfo->aziBeamID, &dispatch_inst->dispatchOutput->aziBeamID, sizeof(sDispatchOutput) - 2 * sizeof(uint32_t));
	#endif // DEBUG_LOG
		detectsListOutput->timestamp = (uint32_t)(curTime_dummy * 1000.f);
		curTime_dummy += dt;
		detectsListOutput->trackTwsTasFlag = dispatchOutput->trackTwsTasFlag;
		detectsListOutput->waveType = (sMeasurement_enum_wavetype)dispatchOutput->waveType;
		detectsListOutput->frameID = (uint32_t)(++frameID);

		gAlgData->platformInfo->frameID = platformInfo->stInfoHeader.frameID;
		gAlgData->platformInfo->timestamp = platformInfo->stInfoHeader.timestamp;
	//	memcpy(&gAlgData->platformInfo->platformData, &platformInfo->heading, sizeof(sPlatformData));

		// ctrl info input
		gAlgData->debugAndCtrlParam->frameID = configParmInfo->stInfoHeader.frameID;
		gAlgData->debugAndCtrlParam->timestamp = configParmInfo->stInfoHeader.timestamp;
		memcpy(&gAlgData->debugAndCtrlParam->debugAndCtrlData, &configParmInfo->trSwitchCtrl, sizeof(sDebugAndCtrlData));


	#ifdef DEBUG_LOG
		my_printf("\n----------frameId %d  time %.2f  scanType %d waveType %d wholeFinal %d tasFinal %d tasFinish %d---------------", \
			frameID, curTime_dummy, dispatchOutput->trackTwsTasFlag, detectsListOutput->waveType, \
			dispatchOutput->wholeSpaceScanCycleFinalBeamFlag, dispatchOutput->tasOrInitTrackFinalBeamFlag, dispatchOutput->tasAndInitTrackFinishBeamFlag);
	#endif // DEBUG_LOG

		//debugAndCtrlParam->debugAndCtrlData.eleScanScope = 1;

		if (init_flag != -1)
		{
			setDebugAndCtrlParam(gAlgData, gAlgData->platformInfo, gAlgData->debugAndCtrlParam);
		}
		if (g_byAbudhabiShowScenes == 1)
		{
			vx = -3.f;
			myMath_generateDummyVaryingVx(&varyVx, &signDummyAccX, -6.f, 6.f, 0.2f);
			vy = varyVx;
			num_dets = 1;
		}
		else if (g_byAbudhabiShowScenes == 2)
		{
			vy = -5.f;
			num_dets = 1;
		}
		else if (g_byAbudhabiShowScenes >= 3)
		{
			myMath_generateDummyVaryingVx(&varyVx, &signDummyAccX, 3.f, 7.f, 0.2f);
			if (g_byAbudhabiShowScenes == 3)
			{
				myMath_generateDummyVaryingVx(&varyVy, &signDummyAccY, -7.f, 7.f, 0.2f);
			}
			vx = varyVx;
			vy = varyVy;
			num_dets = 3;
		}
		myMath_generateDummyRadarData(dummyData, &num_dets, vx, vy, vz, 0U, curTime_dummy); // disable stationary target

		//consider the first (seq 0) per 120(1 s)
		float aziBeam = asinf((float)(dispatchOutput->aziBeamSin) / 32767.f) * RAD2DEG;
		float eleBeam = asinf((float)(dispatchOutput->eleBeamSin) / 32767.f) * RAD2DEG;
	#ifdef DEBUG_LOG
		my_printf("dispatchInfo: wholeScanNum %d, azi pitch [%.2f %.2f],tasId %d  %d/%d\n", \
			dispatchOutput->wholeSpaceScanCycleCnt, aziBeam, eleBeam, dispatchOutput->tasObjId, \
			dispatchOutput->tasBeamCntCur, dispatchOutput->tasBeamTotal);
		//my_printf("dummy det azi pitch [%.2f %.2f]\n", dummyData[0].azimuth, fabsf(dummyData[0].pitch));
	#endif // DEBUG_LOG

		for (int detId = 0; detId < (int)(num_dets); detId++)//num_dets:1
		{
			if (detId == MAX_NUM_DETECTS) {
				break;
			}
			float eleDetScale = 1.f;
			if (g_byAbudhabiShowScenes == 1)
			{
				eleDetScale = 2.f;
			}
			if (fabsf(dummyData[detId].azimuth - aziBeam) > 2.f || \
				fabsf(dummyData[detId].pitch - eleBeam) > 6.f/ eleDetScale)
				continue;

			detPointsList[nd].id = (uint16_t)(nd);
			detPointsList[nd].range = (uint32_t)(dummyData[detId].range * ONE6FORMAT_SCALE);
			detPointsList[nd].azimuth = (int16_t)(aziBeam * ONE6FORMAT_SCALE);
			detPointsList[nd].elevation = (int16_t)(eleBeam * ONE6FORMAT_SCALE);
			//detPointsList[nd].azimuth = (int16_t)(dummyData[detId].azimuth * ONE6FORMAT_SCALE);
			//detPointsList[nd].elevation = (int16_t)(dummyData[detId].pitch * ONE6FORMAT_SCALE);
	#ifdef VELOCITY_UNAMBIGIOUS
			if (dispatchOutput->waveType == 0)
			{
				ambiVelScope = DOPPLER_SCOPE_WAVETYPE_0;
			}
			else
			{
				ambiVelScope = DOPPLER_SCOPE_WAVETYPE_1;
			}
			myMath_unrolling(&dummyData[detId].doppler, -ambiVelScope / 2.f, ambiVelScope / 2.f);
	#endif // VELOCITY_UNAMBIGIOUS
			detPointsList[nd].velocity = (int16_t)(dummyData[detId].doppler * ONE6FORMAT_SCALE);
			detPointsList[nd].mag = (uint16_t)((rand() % 21 + 6) * ONE6FORMAT_SCALE);//(uint16_t)(20.f*ONE6FORMAT_SCALE)
			detPointsList[nd].ambiguous = (uint16_t)(0);
			detPointsList[nd].detProperty.classification = (uint8_t)(1);
			detPointsList[nd].detProperty.cohesionOkFlag = (uint8_t)(1);
			detPointsList[nd].detProperty.cohesionPntNum = (uint8_t)(1);
			detPointsList[nd].detProperty.cohesionBeamNum = (uint8_t)(1);
			detPointsList[nd].detProperty.classifyProb = (uint16_t)(1);
			detPointsList[nd].aziBeamID = (uint16_t)(0.f*ONE6FORMAT_SCALE);
			detPointsList[nd].eleBeamID = (uint16_t)(0.f*ONE6FORMAT_SCALE);		//
	#ifdef DEBUG_LOG
			my_printf(" frameId %d  dummy detId %d, r a p v [%.2f,%.2f,%.2f,%.2f],vy %.2f", frameID, \
				nd, dummyData[detId].range, aziBeam, eleBeam, dummyData[detId].doppler, dummyData[detId].vy);
	#endif // DEBUG_LOG
			nd++;
	#ifdef  DUMMY_TEST_PRINT_TXT
			FILE *fp;
			fopen_s(&fp, "D:/work/acur100/dummyData/dummyDetData.txt", "a+");
			/*if (algObj->detectsListOutput->frameID == 1)
			{
				fprintf(fp, "frameId   x   y   z   vx   vy   vz  doppler \n");
			}*/
			fprintf(fp, "%d %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f \n", algObj->detectsListOutput->frameID,
				dummyData[detId].x, dummyData[detId].y, dummyData[detId].z,
				dummyData[detId].vx, dummyData[detId].vy, dummyData[detId].vz,
				dummyData[detId].range, dummyData[detId].doppler, dummyData[detId].azimuth, dummyData[detId].pitch);
			fclose(fp);
	#endif
		}
		if (g_byAbudhabiShowScenes >= 3)
		{
			float dr_10s = 40.f;
			for (int iAddTime = 1; iAddTime < 4; iAddTime++)
			{
				if (curTime_dummy > 10.f * iAddTime)
				{
					for (int iAddDummy = 0; iAddDummy < 3; iAddDummy++)
					{
						detPointsList[nd].id = (uint16_t)(nd);
						detPointsList[nd].range = detPointsList[nd - num_dets].range - (uint32_t)(dr_10s * ONE6FORMAT_SCALE);
						detPointsList[nd].azimuth = detPointsList[nd - num_dets].azimuth;
						detPointsList[nd].elevation = detPointsList[nd - num_dets].elevation;
						detPointsList[nd].velocity = detPointsList[nd - num_dets].velocity;
						detPointsList[nd].mag = (uint16_t)((rand() % 21 + 6) * ONE6FORMAT_SCALE);//(uint16_t)(20.f*ONE6FORMAT_SCALE)
						detPointsList[nd].ambiguous = (uint16_t)(0);
						detPointsList[nd].detProperty.classification = (uint8_t)(1);
						detPointsList[nd].detProperty.cohesionOkFlag = (uint8_t)(1);
						detPointsList[nd].detProperty.cohesionPntNum = (uint8_t)(1);
						detPointsList[nd].detProperty.cohesionBeamNum = (uint8_t)(1);
						detPointsList[nd].detProperty.classifyProb = (uint16_t)(1);
						detPointsList[nd].aziBeamID = (uint16_t)(0.f*ONE6FORMAT_SCALE);
						detPointsList[nd].eleBeamID = (uint16_t)(0.f*ONE6FORMAT_SCALE);
						nd++;
					}
				}
			}
		}

		gAlgData->detectsListOutput->detectObjNum = nd;//(uint16_t)(num_masterPoints);
		gAlgData->detectsListOutput->trackTwsTasFlag = 0;
		detect_list->detectObjNum = nd;
		detect_list->stInfoHeader.frameID = frameID;
		detect_list->stInfoHeader.timestamp = (uint32_t)(curTime_dummy*1000.f);

		alg_process(gAlgData);
	}
}


void trackingReport(protocol_object_list_detected_t *detect_list, \
		protocol_object_list_tracked_t *track_list, \
		protocol_object_list_dot_cohe_t *dotCohe_list, \
		protocol_beam_scheduling_t *beam_scheduling, \
		protocol_radar_status_t *radar_status)
{
	sDetectFrame *detectsListOutput = gAlgData->detectsListOutput;
	sTrackingInst *tracking_inst = (sTrackingInst *)gAlgData->tracking_inst;
	sTrackingFrame *retTrajs = tracking_inst->trajInfoOutput;
	sMeasProcessInst *measurementProcess_inst = (sMeasProcessInst *)gAlgData->measurementProcess_inst;
	sDotsCohesionFrame *dotsCoheOutput = (sDotsCohesionFrame*)measurementProcess_inst->dotsCoheOutput;
	sDispatchInst *dispatch_inst = (sDispatchInst *)gAlgData->dispatch_inst;
	sDispatchOutput *dispatchOutput = dispatch_inst->dispatchOutput;

	/* 1. detect_list */
	detect_list->stInfoHeader.frameID = detectsListOutput->frameID;
	detect_list->stInfoHeader.timestamp = detectsListOutput->timestamp;
	detect_list->stInfoHeader.infoType = PIT_DET_OBJ;
	/*detect_list->detectObjNum = detectsListOutput->detectObjNum;
	detect_list->detectObjByte = sizeof(protocol_object_item_detected_t);
	detect_list->trackTwsTasFlag = detectsListOutput->trackTwsTasFlag;
	detect_list->waveType = detectsListOutput->waveType;
	memcpy(detect_list->detectPoint, detectsListOutput->detPointsList, sizeof(sDetectPoint)*detect_list->detectObjNum);*/

	/* 2. track_list */
	track_list->stInfoHeader.frameID = retTrajs->frameID;
	track_list->stInfoHeader.timestamp = retTrajs->timestamp;
	track_list->stInfoHeader.infoType = PIT_TRK_OBJ;
	track_list->trackObjNum = retTrajs->trackObjNum;
	track_list->trackTwsNum = retTrajs->trackTwsNum;
	track_list->trackTasNum = retTrajs->trackTasNum;
	track_list->trackObjByte = sizeof(protocol_object_item_tracked_t);
	track_list->trackTwsTasFlag = retTrajs->trackTwsTasFlag;
	memcpy(track_list->trackPoint, retTrajs->trajList, sizeof(sTrackingTarget)*track_list->trackObjNum);

	/* 3. dotCohe_list */
	dotCohe_list->stInfoHeader.frameID = detectsListOutput->frameID;
	dotCohe_list->stInfoHeader.timestamp = detectsListOutput->timestamp;
	dotCohe_list->stInfoHeader.infoType = PIT_DOT_COHE_OBJ;
	dotCohe_list->dotCoheObjNum = dotsCoheOutput->dotCoheObjNum;
	dotCohe_list->dotCoheObjByte = sizeof(protocol_object_item_dot_cohe_t);
	dotCohe_list->tasTargetId = dotsCoheOutput->tasTargetId;
	dotCohe_list->scanType = dotsCoheOutput->scanType;
	dotCohe_list->condceDone = dotsCoheOutput->condenceDone;
	memcpy(dotCohe_list->dotCohePoint, dotsCoheOutput->coheDotsList, sizeof(sDotsCohesionInfo)*dotCohe_list->dotCoheObjNum);

	/* 4. beam_scheduling */
	beam_scheduling->stInfoHeader.frameID = detectsListOutput->frameID;//dispatchOutput->frameID;
	beam_scheduling->stInfoHeader.timestamp = detectsListOutput->timestamp;//dispatchOutput->timestamp;
	beam_scheduling->stInfoHeader.infoType = PIT_BEAM_SCHEDULING;
	beam_scheduling->aziBeamID = dispatchOutput->aziBeamID;
	beam_scheduling->eleBeamID = dispatchOutput->eleBeamID;
	beam_scheduling->aziBeamSin = dispatchOutput->aziBeamSin;
	beam_scheduling->eleBeamSin = dispatchOutput->eleBeamSin;
	beam_scheduling->tasBeamTotal = dispatchOutput->tasBeamTotal;
	beam_scheduling->tasBeamCntCur = dispatchOutput->tasBeamCntCur;
	beam_scheduling->tasObjId = dispatchOutput->tasObjId;
	beam_scheduling->tasObjFilterNum = dispatchOutput->tasObjFilterNum;
	beam_scheduling->tasObjRange = dispatchOutput->tasObjRange;
	beam_scheduling->samplePntStart = dispatchOutput->samplePntStart;
	beam_scheduling->samplePntDepth = dispatchOutput->samplePntDepth;
	beam_scheduling->beamSwitchTime = dispatchOutput->beamSwitchTime;
	beam_scheduling->wholeSpaceScanCycleCnt = dispatchOutput->wholeSpaceScanCycleCnt;
	beam_scheduling->trackTwsTasFlag = dispatchOutput->trackTwsTasFlag;
	beam_scheduling->waveType = dispatchOutput->waveType;
	beam_scheduling->wholeSpaceScanCycleFinalBeamFlag = dispatchOutput->wholeSpaceScanCycleFinalBeamFlag;
	beam_scheduling->tasOrInitTrackFinalBeamFlag = dispatchOutput->tasOrInitTrackFinalBeamFlag;
	beam_scheduling->tasAndInitTrackFinishBeamFlag = dispatchOutput->tasAndInitTrackFinishBeamFlag;

	/* 5. radar_status */
	radar_status->stInfoHeader.frameID = detectsListOutput->frameID;//dispatchOutput->frameID;
	radar_status->stInfoHeader.timestamp = detectsListOutput->timestamp;//dispatchOutput->timestamp;
	radar_status->stInfoHeader.infoType = PIT_RADAR_STATUS;
	radar_status->isFailFlag = 0;
	radar_status->failBitData1 = 0;
	radar_status->failBitData2 = 0;
	radar_status->batteryPower = (uint16_t)((90 + 10 * (rand()*1.0f / RAND_MAX)) * ONE7FORMAT_SCALE);

	gBeamInfoLast[0].stInfoHeader.frameID = detectsListOutput->frameID;
	gBeamInfoLast[0].stInfoHeader.timestamp = detectsListOutput->timestamp;

	trans_byte_order_obj_list_detected(detect_list);
	trans_byte_order_obj_list_tracked(track_list);
	trans_byte_order_obj_list_dot_cohe(dotCohe_list);
//	trans_byte_order_beam_scheduling(beam_scheduling);
	trans_byte_order_beam_scheduling(gBeamInfoLast);	//use gBeamInfoLast
	trans_byte_order_radar_status(radar_status);

	static uint8_t count_upload = 0;
	if( 12 == ++count_upload )
	{
		count_upload = 0;
//	sys_data_push_detected_rpc_m( detect_list, track_list, dotCohe_list, gBeamInfoLast, radar_status );
//	alink_upload_detected( detect_list );
	alink_upload_tracked( track_list );
//	alink_upload_beam_scheduling( beam_scheduling );
	alink_upload_beam_scheduling( gBeamInfoLast );
	}

	protocol_send_object_list_detected(&g_obj_list_detected_send);
	protocol_send_object_list_tracked(&g_obj_list_tracked_send);
	protocol_send_object_list_dot_cohe(&g_obj_list_dot_cohe_send);
	protocol_send_beam_scheduling(&g_beam_scheduling_send);
	protocol_send_radar_status(&g_radar_status_send);
}


void trans_byte_order_obj_list_detected(protocol_object_list_detected_t *detect_list)
{
	uint16_t crc = 0;
	uint32_t i = 0, j = 0;

	memcpy(&g_obj_list_detected_send, detect_list, sizeof(g_obj_list_detected_send));
	g_obj_list_detected_send.stInfoHeader.infoSync = htonl(INFO_HEAD_FLAG);
	g_obj_list_detected_send.stInfoHeader.infoLength = htonl(sizeof(g_obj_list_detected_send));
	g_obj_list_detected_send.stInfoHeader.frameID = htonl(detect_list->stInfoHeader.frameID);
	g_obj_list_detected_send.stInfoHeader.timestamp = htonl(detect_list->stInfoHeader.timestamp);
	g_obj_list_detected_send.stInfoHeader.infoType = htons(detect_list->stInfoHeader.infoType);
	g_obj_list_detected_send.stInfoHeader.terminalID = htons(detect_list->stInfoHeader.terminalID);
	g_obj_list_detected_send.detectObjNum = htons(detect_list->detectObjNum);
	g_obj_list_detected_send.detectObjByte = htons(detect_list->detectObjByte);
	g_obj_list_detected_send.trackTwsTasFlag = htons(detect_list->trackTwsTasFlag);
	g_obj_list_detected_send.waveType = htons(detect_list->waveType);
	for (i = 0; i < detect_list->detectObjNum; i++)
	{
		g_obj_list_detected_send.detectPoint[i].id = htons(detect_list->detectPoint[i].id);
		g_obj_list_detected_send.detectPoint[i].azimuth = htons(detect_list->detectPoint[i].azimuth);
		g_obj_list_detected_send.detectPoint[i].range = htonl(detect_list->detectPoint[i].range);
		g_obj_list_detected_send.detectPoint[i].elevation = htons(detect_list->detectPoint[i].elevation);
		g_obj_list_detected_send.detectPoint[i].velocity = htons(detect_list->detectPoint[i].velocity);
		g_obj_list_detected_send.detectPoint[i].dopplerChn = htons(detect_list->detectPoint[i].dopplerChn);
		g_obj_list_detected_send.detectPoint[i].mag = htons(detect_list->detectPoint[i].mag);
		g_obj_list_detected_send.detectPoint[i].objConfidence = htons(detect_list->detectPoint[i].objConfidence);
		g_obj_list_detected_send.detectPoint[i].aziBeamID = htons(detect_list->detectPoint[i].aziBeamID);
		g_obj_list_detected_send.detectPoint[i].eleBeamID = htons(detect_list->detectPoint[i].eleBeamID);
		for(j = 0; j < sizeof(g_obj_list_tracked_send.trackPoint[0].reserve)/sizeof(g_obj_list_tracked_send.trackPoint[0].reserve[0]); j++)
			g_obj_list_detected_send.detectPoint[i].reserve[j] = htons(detect_list->detectPoint[i].reserve[j]);
	}
	crc = crc_16bits_compute((uint8_t *)&g_obj_list_detected_send, sizeof(g_obj_list_detected_send) - sizeof(g_obj_list_detected_send.stInfoTail));
	g_obj_list_detected_send.stInfoTail.crc = htons(crc);
}


void trans_byte_order_obj_list_tracked(protocol_object_list_tracked_t *track_list)
{
	uint16_t crc = 0;
	uint32_t i = 0, j = 0;

	memcpy(&g_obj_list_tracked_send, track_list, sizeof(g_obj_list_tracked_send));
	g_obj_list_tracked_send.stInfoHeader.infoSync = htonl(INFO_HEAD_FLAG);
	g_obj_list_tracked_send.stInfoHeader.infoLength = htonl(sizeof(g_obj_list_tracked_send));
	g_obj_list_tracked_send.stInfoHeader.frameID = htonl(track_list->stInfoHeader.frameID);
	g_obj_list_tracked_send.stInfoHeader.timestamp = htonl(track_list->stInfoHeader.timestamp);
	g_obj_list_tracked_send.stInfoHeader.infoType = htons(track_list->stInfoHeader.infoType);
	g_obj_list_tracked_send.stInfoHeader.terminalID = htons(track_list->stInfoHeader.terminalID);
	g_obj_list_tracked_send.trackObjNum = htons(track_list->trackObjNum);
	g_obj_list_tracked_send.trackTwsNum = htons(track_list->trackTwsNum);
	g_obj_list_tracked_send.trackTasNum = htons(track_list->trackTasNum);
	g_obj_list_tracked_send.trackObjByte = htons(track_list->trackObjByte);
	g_obj_list_tracked_send.trackTwsTasFlag = htons(track_list->trackTwsTasFlag);
	for (i = 0; i < track_list->trackObjNum; i++)
	{
		g_obj_list_tracked_send.trackPoint[i].id = htons(track_list->trackPoint[i].id);
		g_obj_list_tracked_send.trackPoint[i].azimuth = htons(track_list->trackPoint[i].azimuth);
		g_obj_list_tracked_send.trackPoint[i].range = htonl(track_list->trackPoint[i].range);
		g_obj_list_tracked_send.trackPoint[i].elevation = htons(track_list->trackPoint[i].elevation);
		g_obj_list_tracked_send.trackPoint[i].velocity = htons(track_list->trackPoint[i].velocity);
		g_obj_list_tracked_send.trackPoint[i].dopplerChn = htons(track_list->trackPoint[i].dopplerChn);
		g_obj_list_tracked_send.trackPoint[i].mag = htons(track_list->trackPoint[i].mag);
		g_obj_list_tracked_send.trackPoint[i].absVel = htons(track_list->trackPoint[i].absVel);
		g_obj_list_tracked_send.trackPoint[i].orientationAngle = htons(track_list->trackPoint[i].orientationAngle);
		g_obj_list_tracked_send.trackPoint[i].alive = htons(track_list->trackPoint[i].alive);
		g_obj_list_tracked_send.trackPoint[i].twsTasFlag = htons(track_list->trackPoint[i].twsTasFlag);
		g_obj_list_tracked_send.trackPoint[i].x = htonl(track_list->trackPoint[i].x);
		g_obj_list_tracked_send.trackPoint[i].y = htonl(track_list->trackPoint[i].y);
		g_obj_list_tracked_send.trackPoint[i].z = htonl(track_list->trackPoint[i].z);
		g_obj_list_tracked_send.trackPoint[i].vx = htons(track_list->trackPoint[i].vx);
		g_obj_list_tracked_send.trackPoint[i].vy = htons(track_list->trackPoint[i].vy);
		g_obj_list_tracked_send.trackPoint[i].vz = htons(track_list->trackPoint[i].vz);
		g_obj_list_tracked_send.trackPoint[i].ax = htons(track_list->trackPoint[i].ax);
		g_obj_list_tracked_send.trackPoint[i].ay = htons(track_list->trackPoint[i].ay);
		g_obj_list_tracked_send.trackPoint[i].az = htons(track_list->trackPoint[i].az);
		g_obj_list_tracked_send.trackPoint[i].x_variance = htons(track_list->trackPoint[i].x_variance);
		g_obj_list_tracked_send.trackPoint[i].y_variance = htons(track_list->trackPoint[i].y_variance);
		g_obj_list_tracked_send.trackPoint[i].z_variance = htons(track_list->trackPoint[i].z_variance);
		g_obj_list_tracked_send.trackPoint[i].vx_variance = htons(track_list->trackPoint[i].vx_variance);
		g_obj_list_tracked_send.trackPoint[i].vy_variance = htons(track_list->trackPoint[i].vy_variance);
		g_obj_list_tracked_send.trackPoint[i].vz_variance = htons(track_list->trackPoint[i].vz_variance);
		g_obj_list_tracked_send.trackPoint[i].ax_variance = htons(track_list->trackPoint[i].ax_variance);
		g_obj_list_tracked_send.trackPoint[i].ay_variance = htons(track_list->trackPoint[i].ay_variance);
		g_obj_list_tracked_send.trackPoint[i].az_variance = htons(track_list->trackPoint[i].az_variance);
		g_obj_list_tracked_send.trackPoint[i].forcastFrameNum = htons(track_list->trackPoint[i].forcastFrameNum);
		g_obj_list_tracked_send.trackPoint[i].associationNum = htons(track_list->trackPoint[i].associationNum);
		g_obj_list_tracked_send.trackPoint[i].assocBit0 = htonl(track_list->trackPoint[i].assocBit0);
		g_obj_list_tracked_send.trackPoint[i].assocBit1 = htonl(track_list->trackPoint[i].assocBit1);
		for (j = 0; j < sizeof(g_obj_list_tracked_send.trackPoint[0].reserve)/sizeof(g_obj_list_tracked_send.trackPoint[0].reserve[0]); j++)
			g_obj_list_tracked_send.trackPoint[i].reserve[j] = htons(track_list->trackPoint[i].reserve[j]);
	}
	crc = crc_16bits_compute((uint8_t *)&g_obj_list_tracked_send, sizeof(g_obj_list_tracked_send) - sizeof(g_obj_list_tracked_send.stInfoTail));
	g_obj_list_tracked_send.stInfoTail.crc = htons(crc);
}

void trans_byte_order_obj_list_dot_cohe(protocol_object_list_dot_cohe_t *dotCohe_list)
{
	uint16_t crc = 0;
	uint32_t i = 0, j = 0;

	memcpy(&g_obj_list_dot_cohe_send, dotCohe_list, sizeof(g_obj_list_dot_cohe_send));
	g_obj_list_dot_cohe_send.stInfoHeader.infoSync = htonl(INFO_HEAD_FLAG);
	g_obj_list_dot_cohe_send.stInfoHeader.infoLength = htonl(sizeof(g_obj_list_dot_cohe_send));
	g_obj_list_dot_cohe_send.stInfoHeader.frameID = htonl(dotCohe_list->stInfoHeader.frameID);
	g_obj_list_dot_cohe_send.stInfoHeader.timestamp = htonl(dotCohe_list->stInfoHeader.timestamp);
	g_obj_list_dot_cohe_send.stInfoHeader.infoType = htons(dotCohe_list->stInfoHeader.infoType);
	g_obj_list_dot_cohe_send.stInfoHeader.terminalID = htons(dotCohe_list->stInfoHeader.terminalID);
	g_obj_list_dot_cohe_send.stInfoHeader.terminalType = dotCohe_list->stInfoHeader.terminalType;
	g_obj_list_dot_cohe_send.stInfoHeader.subTerminalType = dotCohe_list->stInfoHeader.subTerminalType;
	g_obj_list_dot_cohe_send.stInfoHeader.infoVersion = dotCohe_list->stInfoHeader.infoVersion;

	g_obj_list_dot_cohe_send.dotCoheObjNum = htons(dotCohe_list->dotCoheObjNum);
	g_obj_list_dot_cohe_send.dotCoheObjByte = htons(dotCohe_list->dotCoheObjByte);
	g_obj_list_dot_cohe_send.tasTargetId = htons(dotCohe_list->tasTargetId);
	g_obj_list_dot_cohe_send.scanType = dotCohe_list->scanType;
	g_obj_list_dot_cohe_send.condceDone = dotCohe_list->condceDone;

	for (i = 0; dotCohe_list->dotCoheObjNum > i; i++)
	{
		g_obj_list_dot_cohe_send.dotCohePoint[i].id = htons(dotCohe_list->dotCohePoint[i].id);
		g_obj_list_dot_cohe_send.dotCohePoint[i].azimuth = htons(dotCohe_list->dotCohePoint[i].azimuth);
		g_obj_list_dot_cohe_send.dotCohePoint[i].range = htonl( dotCohe_list->dotCohePoint[i].range );
		g_obj_list_dot_cohe_send.dotCohePoint[i].elevation = htons( dotCohe_list->dotCohePoint[i].elevation );
		g_obj_list_dot_cohe_send.dotCohePoint[i].velocity = htons( dotCohe_list->dotCohePoint[i].velocity );
		g_obj_list_dot_cohe_send.dotCohePoint[i].x = htonl( dotCohe_list->dotCohePoint[i].x );
		g_obj_list_dot_cohe_send.dotCohePoint[i].y = htonl( dotCohe_list->dotCohePoint[i].y );
		g_obj_list_dot_cohe_send.dotCohePoint[i].z = htonl( dotCohe_list->dotCohePoint[i].z );
		g_obj_list_dot_cohe_send.dotCohePoint[i].mag = htons( dotCohe_list->dotCohePoint[i].mag );
		g_obj_list_dot_cohe_send.dotCohePoint[i].snr = htons( dotCohe_list->dotCohePoint[i].snr );
		g_obj_list_dot_cohe_send.dotCohePoint[i].rcs = htons( dotCohe_list->dotCohePoint[i].rcs );
		g_obj_list_dot_cohe_send.dotCohePoint[i].sinAzim = htons( dotCohe_list->dotCohePoint[i].sinAzim );
		g_obj_list_dot_cohe_send.dotCohePoint[i].cosAzim = htons( dotCohe_list->dotCohePoint[i].cosAzim );
		g_obj_list_dot_cohe_send.dotCohePoint[i].unambigVel = htons( dotCohe_list->dotCohePoint[i].unambigVel );
		g_obj_list_dot_cohe_send.dotCohePoint[i].classification = htons( dotCohe_list->dotCohePoint[i].classification );
		g_obj_list_dot_cohe_send.dotCohePoint[i].aziBeamSin = htons( dotCohe_list->dotCohePoint[i].aziBeamSin );
		g_obj_list_dot_cohe_send.dotCohePoint[i].eleBeamSin = htons( dotCohe_list->dotCohePoint[i].eleBeamSin );
		g_obj_list_dot_cohe_send.dotCohePoint[i].waveType = htons( dotCohe_list->dotCohePoint[i].waveType );
		g_obj_list_dot_cohe_send.dotCohePoint[i].flag_moving = dotCohe_list->dotCohePoint[i].flag_moving;
		g_obj_list_dot_cohe_send.dotCohePoint[i].unambigVelDone = dotCohe_list->dotCohePoint[i].unambigVelDone;
		g_obj_list_dot_cohe_send.dotCohePoint[i].detectionNum = htons( dotCohe_list->dotCohePoint[i].detectionNum );
		for( j = 0; sizeof(g_obj_list_dot_cohe_send.dotCohePoint[0].detectionId)/sizeof(g_obj_list_dot_cohe_send.dotCohePoint[0].detectionId[0]) > j; ++j )
			g_obj_list_dot_cohe_send.dotCohePoint[i].detectionId[j] = htons( dotCohe_list->dotCohePoint[i].detectionId[j] );
		for( j = 0; sizeof(g_obj_list_dot_cohe_send.dotCohePoint[0].detectionFrameId)/sizeof(g_obj_list_dot_cohe_send.dotCohePoint[0].detectionFrameId[0]) > j; ++j )
			g_obj_list_dot_cohe_send.dotCohePoint[i].detectionFrameId[j] = htonl( dotCohe_list->dotCohePoint[i].detectionFrameId[j] );
		for( j = 0; sizeof(g_obj_list_dot_cohe_send.dotCohePoint[0].MagBeamList)/sizeof(g_obj_list_dot_cohe_send.dotCohePoint[0].MagBeamList[0]) > j; ++j )
			g_obj_list_dot_cohe_send.dotCohePoint[i].MagBeamList[j] = htons( dotCohe_list->dotCohePoint[i].MagBeamList[j] );
		for( j = 0; sizeof(g_obj_list_dot_cohe_send.dotCohePoint[0].aziBeamList)/sizeof(g_obj_list_dot_cohe_send.dotCohePoint[0].aziBeamList[0]) > j; ++j )
			g_obj_list_dot_cohe_send.dotCohePoint[i].aziBeamList[j] = htons( dotCohe_list->dotCohePoint[i].aziBeamList[j] );
		for( j = 0; sizeof(g_obj_list_dot_cohe_send.dotCohePoint[0].eleBeamList)/sizeof(g_obj_list_dot_cohe_send.dotCohePoint[0].eleBeamList[0]) > j; ++j )
			g_obj_list_dot_cohe_send.dotCohePoint[i].eleBeamList[j] = htons( dotCohe_list->dotCohePoint[i].eleBeamList[j] );
		g_obj_list_dot_cohe_send.dotCohePoint[i].ifClacAzi = dotCohe_list->dotCohePoint[i].ifClacAzi;
		g_obj_list_dot_cohe_send.dotCohePoint[i].ifClacEle = dotCohe_list->dotCohePoint[i].ifClacEle;
		for( j = 0; sizeof(g_obj_list_dot_cohe_send.dotCohePoint[0].reserve)/sizeof(g_obj_list_dot_cohe_send.dotCohePoint[0].reserve[0]) > j; ++j )
			g_obj_list_dot_cohe_send.dotCohePoint[i].reserve[j] = htons(g_obj_list_dot_cohe_send.dotCohePoint[i].reserve[j]);
	}

	crc = crc_16bits_compute((uint8_t *)&g_obj_list_dot_cohe_send, sizeof(g_obj_list_dot_cohe_send) - sizeof(g_obj_list_dot_cohe_send.stInfoTail));
	g_obj_list_dot_cohe_send.stInfoTail.crc = htons(crc);
}

void trans_byte_order_beam_scheduling(protocol_beam_scheduling_t *beam_scheduling)
{
	uint16_t crc = 0;

	memcpy(&g_beam_scheduling_send, beam_scheduling, sizeof(g_beam_scheduling_send));
	g_beam_scheduling_send.stInfoHeader.infoSync = htonl(INFO_HEAD_FLAG);
	g_beam_scheduling_send.stInfoHeader.infoLength = htonl(sizeof(g_beam_scheduling_send));
	g_beam_scheduling_send.stInfoHeader.frameID = htonl(beam_scheduling->stInfoHeader.frameID);
	g_beam_scheduling_send.stInfoHeader.timestamp = htonl(beam_scheduling->stInfoHeader.timestamp);
	g_beam_scheduling_send.stInfoHeader.infoType = htons(beam_scheduling->stInfoHeader.infoType);
	g_beam_scheduling_send.stInfoHeader.terminalID = htons(beam_scheduling->stInfoHeader.terminalID);
	g_beam_scheduling_send.aziBeamID = htons(beam_scheduling->aziBeamID);
	g_beam_scheduling_send.eleBeamID = htons(beam_scheduling->eleBeamID);
	g_beam_scheduling_send.aziBeamSin = htons(beam_scheduling->aziBeamSin);
	g_beam_scheduling_send.eleBeamSin = htons(beam_scheduling->eleBeamSin);
	g_beam_scheduling_send.tasBeamTotal = htons(beam_scheduling->tasBeamTotal);
	g_beam_scheduling_send.tasBeamCntCur = htons(beam_scheduling->tasBeamCntCur);
	g_beam_scheduling_send.tasObjId = htons(beam_scheduling->tasObjId);
	g_beam_scheduling_send.tasObjFilterNum = htons(beam_scheduling->tasObjFilterNum);
	g_beam_scheduling_send.tasObjRange = htons(beam_scheduling->tasObjRange);
	g_beam_scheduling_send.samplePntStart = htons(beam_scheduling->samplePntStart);
	g_beam_scheduling_send.samplePntDepth = htons(beam_scheduling->samplePntDepth);
	g_beam_scheduling_send.beamSwitchTime = htons(beam_scheduling->beamSwitchTime);
	g_beam_scheduling_send.wholeSpaceScanCycleCnt = htonl(beam_scheduling->wholeSpaceScanCycleCnt);
	g_beam_scheduling_send.trackTwsTasFlag = htons(beam_scheduling->trackTwsTasFlag);
	g_beam_scheduling_send.waveType = htons(beam_scheduling->waveType);

	crc = crc_16bits_compute((uint8_t *)&g_beam_scheduling_send, sizeof(g_beam_scheduling_send) - sizeof(g_beam_scheduling_send.stInfoTail));
	g_beam_scheduling_send.stInfoTail.crc = htons(crc);
}


void trans_byte_order_radar_status(protocol_radar_status_t *radar_status)
{
	uint16_t crc = 0;

	memcpy(&g_radar_status_send, radar_status, sizeof(g_radar_status_send));
	g_radar_status_send.stInfoHeader.infoSync = htonl(INFO_HEAD_FLAG);
	g_radar_status_send.stInfoHeader.infoLength = htonl(sizeof(g_radar_status_send));
	g_radar_status_send.stInfoHeader.frameID = htonl(radar_status->stInfoHeader.frameID);
	g_radar_status_send.stInfoHeader.timestamp = htonl(radar_status->stInfoHeader.timestamp);
	g_radar_status_send.stInfoHeader.infoType = htons(radar_status->stInfoHeader.infoType);
	g_radar_status_send.stInfoHeader.terminalID = htons(radar_status->stInfoHeader.terminalID);
	g_radar_status_send.isFailFlag = htons(radar_status->isFailFlag);
	g_radar_status_send.failBitData1 = htons(radar_status->failBitData1);
	g_radar_status_send.failBitData2 = htons(radar_status->failBitData2);
	g_radar_status_send.batteryPower = htons(radar_status->batteryPower);

	crc = crc_16bits_compute((uint8_t *)&g_radar_status_send, sizeof(g_radar_status_send) - sizeof(g_beam_scheduling_send.stInfoTail));
	g_radar_status_send.stInfoTail.crc = htons(crc);
}


#ifdef VELOCITY_UNAMBIGIOUS
/* write ware type timing control parameter registers */
void WaveTypeTimingCtrlPara_write_register(uint16_t waveType)
{
    PL_REG_WRITE( 0XFFFF&(XPAR_AXI2REGTIMING_0_BASEADDR + 1*4) , gTimingCtrlPara[waveType].cpiDelay);
    PL_REG_WRITE( 0XFFFF&(XPAR_AXI2REGTIMING_0_BASEADDR + 2*4) , gTimingCtrlPara[waveType].priPeriod);
    PL_REG_WRITE( 0XFFFF&(XPAR_AXI2REGTIMING_0_BASEADDR + 3*4) , gTimingCtrlPara[waveType].priNum);
    PL_REG_WRITE( 0XFFFF&(XPAR_AXI2REGTIMING_0_BASEADDR + 4*4) , gTimingCtrlPara[waveType].priPulseWidth);
    PL_REG_WRITE( 0XFFFF&(XPAR_AXI2REGTIMING_0_BASEADDR + 5*4) , gTimingCtrlPara[waveType].startSample);
    PL_REG_WRITE( 0XFFFF&(XPAR_AXI2REGTIMING_0_BASEADDR + 6*4) , gTimingCtrlPara[waveType].sampleLength);
    PL_REG_WRITE( 0XFFFF&(XPAR_AXI2REGTIMING_0_BASEADDR + 7*4) , gTimingCtrlPara[waveType].waveCode);
}
#endif



/* set AbuDhabi show scenes */
bool SetAbuDhabiShowScenes(int32_t scenes)
{
	if( g_byDummyTest )
	{
		if( 4 > scenes )
		{
			g_byAbudhabiShowScenes = scenes;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

/* get AbuDhabi show scenes */
int32_t GetAbuDhabiShowScenes(void)
{
	int32_t ret = 0;
	if( g_byDummyTest )
	{
		ret = g_byAbudhabiShowScenes;
	}
	else
	{
		ret = 0;
	}
	return ret;
}

void SetTrackingMode(int32_t mode)
{
	g_byDummyTest = mode;
}

int32_t GetTrackingMode( void )
{
	return g_byDummyTest;
}

/* get dummy configure parameter information */
void SetDummyConfigParmInfo(protocol_radar_platfrom_info_t *platformInfo, protocol_cfg_param_t *configParmInfo)
{
	if( g_byDummyTest )
	{
		// configParmInfo->workMode = 0;
	#ifdef DEBUG_LOG_THREATLEVEL
		configParmInfo->eleScanScope = (int32_t)(30.f * ONE15FORMAT_SCALE);
	#endif // DEBUG_LOG_THREATLEVEL
	//	platformInfo->pitching = (int32_t)(0.f * ONE15FORMAT_SCALE);
	//	platformInfo->heading = (int32_t)(0.f * ONE15FORMAT_SCALE);
		if (g_byAbudhabiShowScenes == 1)
		{
			configParmInfo->aziScanScope = 120;
			configParmInfo->eleScanScope = 40;
		}
		else if (g_byAbudhabiShowScenes== 2)
		{
			configParmInfo->aziScanScope = 120;
			configParmInfo->eleScanScope = 1;
		}
		else if (g_byAbudhabiShowScenes >= 3)
		{
			configParmInfo->aziScanScope = 120;
			configParmInfo->eleScanScope = 40;
		}
		else
		{
			configParmInfo->aziScanScope = 40;
			configParmInfo->eleScanScope = 1;
		}
	}
}

/* get process time of tracking */
uint32_t GetPrcoTimeTracking(void)
{
	return gPrcoTimeTracking;
}

/* get process time of report */
uint32_t GetPrcoTimeReport(void)
{
	return gPrcoTimeReport;
}

/* get version of tracking */
const char *GetTrackAlgVersion()
{
	return TRACK_ALG_VERSION;
}


void set_beam_mode(uint32_t mode)
{
	 gBeamMode = mode;
}

void set_beam_azi_scan_para(int32_t center, int32_t scope)
{
	 gConfigParmInfo[0].aziScanCenter = center;
	 gConfigParmInfo[0].aziScanScope = scope;
}

void set_beam_ele_scan_para(int32_t center, int32_t scope)
{
	 gConfigParmInfo[0].eleScanCenter = center;
	 gConfigParmInfo[0].eleScanScope = scope;
}

void set_fix_beam_para(int32_t eleCenter, int32_t aziCenter)
{
	 gConfigParmInfo[0].eleScanCenter = eleCenter;
	 gConfigParmInfo[0].aziScanCenter = aziCenter;
}

void set_scan_beam_para(int32_t eleCenter, int32_t eleScope, int32_t aziCenter, int32_t aziScope)
{
	 gConfigParmInfo[0].eleScanCenter = eleCenter;
	 gConfigParmInfo[0].eleScanScope = eleScope;
	 gConfigParmInfo[0].aziScanCenter = aziCenter;
	 gConfigParmInfo[0].aziScanScope = aziScope;
}

void set_beam_workmode(int32_t workMode)
{
	 gConfigParmInfo[0].workMode = workMode;
}
