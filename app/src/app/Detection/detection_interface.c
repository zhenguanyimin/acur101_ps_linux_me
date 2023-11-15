
#ifdef __cplusplus
extern "C" {
#endif
#include "math.h"
#include "detection_interface.h"
#include "detection_cfar_2d.h"

char g_det_version[DETEC_VERSION_STR_LEN] = DETEC_ALG_VERSION;

sDetectObjData_t gDetectObjData[1] = { 0 };

int32_t gDetectAlgInitFlag = -1;

uint32_t gFrameID = 0;
uint32_t gTimeStampCur = 0;	/* time from system startup at new frame beginning */
uint32_t gTimeStampLast = 0;
uint32_t gPrcoTimeDetection = 0;
int16_t gRdmapHeadAzi = 0;
int16_t gRdmapHeadEle = 0;

uint32_t gRdmapData[DEFAULT_RANGE_BINS * DEFAULT_DOPPLER_BINS] = { 0 };	// 256k Byte, 2048*32*4 Byte
uint32_t gRdmapTransposeData[DEFAULT_RANGE_BINS * DEFAULT_DOPPLER_BINS] = { 0 };	// 256k Byte, 2048*32*4 Byte
uint16_t gHistBuff[DEFAULT_RANGE_BINS * DEFAULT_HIST_BINS] = { 0 };	// 256k Byte, 2048*64*2 Byte
uint16_t gHististMaxIndex[DEFAULT_RANGE_BINS] = { 0 };	// 4k Byte, 2048*2 Byte
uint16_t gThreshold[DEFAULT_RANGE_BINS] = { 0 };	// 4k Byte, 2048*2 Byte
uint32_t gPeakBitmap[PACKED_BITMAP_NUM_ALL_RDM] = { 0 };	// 8k Byte, 2048/32*32*4 Byte
uint8_t gRdmapDooplerDimPeakFlag[DEFAULT_RANGE_BINS * DEFAULT_DOPPLER_BINS];	// 64k Byte, 2048*32*1 Byte
int16_t g_sumSubData[512*1024/2] = { 0 };

/* get version of detection */
const char *GetDetectAlgVersion()
{
	return DETEC_ALG_VERSION;
}

uint32_t GetFrameID(void)
{
	return gFrameID;
}

void SetFrameID(uint32_t uiFrameID)
{
	gFrameID = uiFrameID;
}

uint32_t GetPrcoTimeDetection(void)
{
	return gPrcoTimeDetection;
}

static int GetFsmWorkMode()
{
	return 0;
}

long long cprdmap;
long long cpriq;
void DataPathCB(data_path_data_t *pBuf)
{
	struct timespec start1 = { 0 } ;
	struct timespec start2 = { 0 } ;
	struct timespec end1 = { 0 } ;
	struct timespec end2 = { 0 } ;

#if 0
	usleep( 2000 );
	long long cpadc;
	int32_t adc_data[512*1024/4] = { 0 };
	struct timespec start3 = { 0 } ;
	struct timespec end3 = { 0 } ;
	clock_gettime(CLOCK_MONOTONIC, &start3);
	memcpy( adc_data, pBuf->adc_src, 512*1024 );
	clock_gettime(CLOCK_MONOTONIC, &end3);

	cpadc = (end3.tv_sec - start3.tv_sec) * 1000000 + (end3.tv_nsec - start3.tv_nsec)/1000;

	printf( "pBuf->adc_data %p %d\r\n", pBuf->adc_src, cpadc );
#endif

	clock_gettime(CLOCK_MONOTONIC, &g_tUsDetectProcCur[0]);

//	memcpy(gDetectObjData[0].rdmapData, pBuf, len);	// total len = 2048*32*4=256KB
	clock_gettime(CLOCK_MONOTONIC, &start1);
	memcpy(gDetectObjData[0].rdmapData, pBuf->rdmap, sizeof(uint32_t) * DEFAULT_RANGE_BINS * DEFAULT_DOPPLER_BINS);	// current len = 512*32*4=64K
//	memcpy(gDetectObjData[0].rdmapData, pBuf->rdmap, 256*1024);
//	printf( "pBuf->rdmap %p\r\n", pBuf->rdmap );
	clock_gettime(CLOCK_MONOTONIC, &end1);
	cprdmap = (end1.tv_sec - start1.tv_sec) * 1000000 + (end1.tv_nsec - start1.tv_nsec)/1000;
	//pBuf->chirpTime;

//for( int i = 0; 512 > i; ++i )
{
//	uint32_t *adcip_data = (uint32_t*)(pBuf->adc_iq);
	clock_gettime(CLOCK_MONOTONIC, &start2);
	memcpy( gDetectObjData[0].sumSubData, pBuf->adc_iq, 512*1024 );
//	printf( "pBuf->adc_iq %p\r\n", pBuf->adc_iq );
//	memcpy( (uint8_t*)(gDetectObjData[0].sumSubData) + i*1024, (uint8_t*)(pBuf->adc_iq) + i*1024, 1024 );
	clock_gettime(CLOCK_MONOTONIC, &end2);
	cpriq = (end2.tv_sec - start2.tv_sec) * 1000000 + (end2.tv_nsec - start2.tv_nsec)/1000;
//	printf( "%d = %d\r\n", i, cpriq );
}
	gFrameID = (uint32_t)pBuf->frameID;
	memset(gDetectObjData[0].rdmapData, 0, sizeof(uint32_t) * DETECT_RANGE_START_INDEX * DEFAULT_DOPPLER_BINS);

	clock_gettime(CLOCK_MONOTONIC, &g_tUsDetectProcEnd[0]);

	
	g_tUsDetectProcUsed[0] = (g_tUsDetectProcEnd[0].tv_sec - g_tUsDetectProcCur[0].tv_sec) * 1000000 + (g_tUsDetectProcEnd[0].tv_nsec - g_tUsDetectProcCur[0].tv_nsec)/1000;
}

int32_t DetectBufferAlloc(sDetectObjData_t *detectObj)
{
	detectObj->detectList = gDetectList;
	detectObj->rdmapData = gRdmapData;
	detectObj->rdmapTransposeData = gRdmapTransposeData;
	detectObj->histBuff = gHistBuff;
	detectObj->hististMaxIndex = gHististMaxIndex;
	detectObj->threshold = gThreshold;
	detectObj->peakBitmap = gPeakBitmap;
	detectObj->rdmapDooplerDimPeakFlag = gRdmapDooplerDimPeakFlag;
	detectObj->sumSubData = g_sumSubData;
	return 0;
}

void DetectAlgInit(void)
{
	if (DetectBufferAlloc(gDetectObjData) < 0)
	{
		gDetectAlgInitFlag = -1;
	}

	gDetectAlgInitFlag = 0;
}

int32_t runDetectAlgBlocking(data_path_data_t *pDataPathDataBuf)
{
	struct timespec tCur0 = { 0 };
	struct timespec tEnd0 = { 0 };
	uint32_t tUsed0;
	uint32_t timeStampDiff = 0;


	clock_gettime(CLOCK_MONOTONIC, &tCur0);

	gTimeStampCur = tCur0.tv_sec*1000 + tCur0.tv_nsec/1000000;
//	printf( "gTimeStampCur %d\r\n", gTimeStampCur );

	gDetectList[0].stInfoHeader.frameID = pDataPathDataBuf->sInfo.frameID;
	gDetectList[0].stInfoHeader.timestamp = pDataPathDataBuf->sInfo.timestamp;
	gDetectList[0].trackTwsTasFlag = pDataPathDataBuf->sInfo.trackTwsTasFlag;
	gDetectList[0].waveType = pDataPathDataBuf->sInfo.waveType;

	DetectionAlgProcess(gDetectObjData, pDataPathDataBuf);
//	printf( "gTimeStampCur %d\r\n", gTimeStampCur );

	timeStampDiff = gTimeStampCur - gTimeStampLast;
	gTimeStampLast = gTimeStampCur;

	clock_gettime(CLOCK_MONOTONIC, &tEnd0);
	tUsed0 = (tEnd0.tv_sec - tCur0.tv_sec) * 1000000 + (tEnd0.tv_nsec - tCur0.tv_nsec)/1000;

	/* process time of detection */
	gDetectList[0].detectPoint[0].reserve[0] = (uint16_t)tUsed0;
	gPrcoTimeDetection = tUsed0;

	/* process time of detection internal module */
	gDetectList[0].detectPoint[1].aziBeamID = (uint16_t)g_tUsDetectProcUsed[0];		/* process time of GetRdmapData */
	gDetectList[0].detectPoint[1].eleBeamID = (uint16_t)g_tUsDetectProcUsed[1];		/* process time of CalcHistogram */
	gDetectList[0].detectPoint[1].reserve[0] = (uint16_t)g_tUsDetectProcUsed[2];	/* process time of CalcThreshold */
	gDetectList[0].detectPoint[1].reserve[1] = (uint16_t)g_tUsDetectProcUsed[3];	/* process time of CalcTransposeRdm */
	gDetectList[0].detectPoint[1].reserve[2] = (uint16_t)g_tUsDetectProcUsed[4];	/* process time of CalcPeakSearchBitmap */
	gDetectList[0].detectPoint[1].reserve[3] = (uint16_t)g_tUsDetectProcUsed[5];	/* process time of CfarDetection */

#if 0
	struct timespec tCurTest = { 0 };
	struct timespec tEndTest = { 0 };
	uint32_t tUsedTest = 0;

	clock_gettime(CLOCK_MONOTONIC, &tCurTest);
	usleep( 1000 );
	clock_gettime(CLOCK_MONOTONIC, &tEndTest);
	tUsedTest = (tEndTest.tv_sec - tCurTest.tv_sec) * 1000000 + (tEndTest.tv_nsec - tCurTest.tv_nsec)/1000;

	printf( "%d %d %d %d %d %d %d %d %d %d\r\n", tUsedTest,
										cprdmap,
										cpriq,
										g_tUsDetectProcUsed[0], 
										gPrcoTimeDetection, 
										g_tUsDetectProcUsed[1], 
										g_tUsDetectProcUsed[2], 
										g_tUsDetectProcUsed[3], 
										g_tUsDetectProcUsed[4], 
										g_tUsDetectProcUsed[5] );
#endif
	return 0;
}

/*================================================================================================*/
#ifdef __cplusplus
}
#endif
