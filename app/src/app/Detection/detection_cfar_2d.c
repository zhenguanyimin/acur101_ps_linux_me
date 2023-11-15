
#ifdef __cplusplus
extern "C" {
#endif

#include "../DataPath/data_path.h"
#include <math.h>
#include <sys/time.h>
#include <stdio.h>
#include <time.h>
#include "detection_cfar_2d.h"
#include "detection_dummy.h"
#include "../Tracking/tracking_interface.h"

float gTable_Zheng[TABLE_LEN_ZHENG] = { 0.0025, 0.0049, 0.0074, 0.0098, 0.0123, 0.0147, 0.0172, 0.0196, 0.0221, 0.0245, 0.0270, 0.0294, 0.0319, 0.0343, 0.0368, 0.0392, 0.0417, 0.0442, 0.0466, 0.0491, 0.0515, 0.0540, 0.0564, 0.0589, 0.0613, 0.0638, 0.0662, 0.0687, 0.0712, 0.0736, 0.0761, 0.0785, 0.0810, 0.0835, 0.0859, 0.0884, 0.0908, 0.0933, 0.0958, 0.0982, 0.1007, 0.1031, 0.1056, 0.1081, 0.1105, 0.1130, 0.1155, 0.1179, 0.1204, 0.1229, 0.1253, 0.1278, 0.1303, 0.1327, 0.1352, 0.1377, 0.1401, 0.1426, 0.1451, 0.1476, 0.1500, 0.1525, 0.1550, 0.1575, 0.1599, 0.1624, 0.1649, 0.1674, 0.1699, 0.1723, 0.1748, 0.1773, 0.1798, 0.1823, 0.1848, 0.1872, 0.1897, 0.1922, 0.1947, 0.1972, 0.1997, 0.2022, 0.2047, 0.2072, 0.2097, 0.2122, 0.2147, 0.2172, 0.2197, 0.2222, 0.2247, 0.2272, 0.2297, 0.2322, 0.2347, 0.2372, 0.2397, 0.2422, 0.2447, 0.2472, 0.2497, 0.2522, 0.2548, 0.2573, 0.2598, 0.2623, 0.2648, 0.2674, 0.2699, 0.2724, 0.2749, 0.2775, 0.2800, 0.2825, 0.2851, 0.2876, 0.2901, 0.2927, 0.2952, 0.2977, 0.3003, 0.3028, 0.3054, 0.3079, 0.3104, 0.3130, 0.3155, 0.3181, 0.3206, 0.3232, 0.3258, 0.3283, 0.3309, 0.3334, 0.3360, 0.3386, 0.3411, 0.3437, 0.3463, 0.3488, 0.3514, 0.3540, 0.3566, 0.3591, 0.3617, 0.3643, 0.3669, 0.3695, 0.3721, 0.3746, 0.3772, 0.3798, 0.3824, 0.3850, 0.3876, 0.3902, 0.3928, 0.3954, 0.3980, 0.4006, 0.4033, 0.4059, 0.4085, 0.4111, 0.4137, 0.4163, 0.4190, 0.4216, 0.4242, 0.4269, 0.4295, 0.4321, 0.4348, 0.4374, 0.4400, 0.4427, 0.4453, 0.4480, 0.4506, 0.4533, 0.4559, 0.4586, 0.4612, 0.4639, 0.4666, 0.4692, 0.4719, 0.4746, 0.4773, 0.4799, 0.4826, 0.4853, 0.4880, 0.4907, 0.4934, 0.4961, 0.4987, 0.5014, 0.5041, 0.5069 };
float gTable_Fu[TABLE_LEN_FU] = { 0.0025, 0.0049, 0.0074, 0.0098, 0.0123, 0.0147, 0.0172, 0.0196, 0.0221, 0.0245, 0.0270, 0.0294, 0.0319, 0.0343, 0.0368, 0.0392, 0.0417, 0.0442, 0.0466, 0.0491, 0.0515, 0.0540, 0.0564, 0.0589, 0.0613, 0.0638, 0.0662, 0.0687, 0.0712, 0.0736, 0.0761, 0.0785, 0.0810, 0.0835, 0.0859, 0.0884, 0.0908, 0.0933, 0.0958, 0.0982, 0.1007, 0.1031, 0.1056, 0.1081, 0.1105, 0.1130, 0.1155, 0.1179, 0.1204, 0.1229, 0.1253, 0.1278, 0.1303, 0.1327, 0.1352, 0.1377, 0.1401, 0.1426, 0.1451, 0.1476, 0.1500, 0.1525, 0.1550, 0.1575, 0.1599, 0.1624, 0.1649, 0.1674, 0.1699, 0.1723, 0.1748, 0.1773, 0.1798, 0.1823, 0.1848, 0.1872, 0.1897, 0.1922, 0.1947, 0.1972, 0.1997, 0.2022, 0.2047, 0.2072, 0.2097, 0.2122, 0.2147, 0.2172, 0.2197, 0.2222, 0.2247, 0.2272, 0.2297, 0.2322, 0.2347, 0.2372, 0.2397, 0.2422, 0.2447, 0.2472, 0.2497, 0.2522, 0.2548, 0.2573, 0.2598, 0.2623, 0.2648, 0.2674, 0.2699, 0.2724, 0.2749, 0.2775, 0.2800, 0.2825, 0.2851, 0.2876, 0.2901, 0.2927, 0.2952, 0.2977, 0.3003, 0.3028, 0.3054, 0.3079, 0.3104, 0.3130, 0.3155, 0.3181, 0.3206, 0.3232, 0.3258, 0.3283, 0.3309, 0.3334, 0.3360, 0.3386, 0.3411, 0.3437, 0.3463, 0.3488, 0.3514, 0.3540, 0.3566, 0.3591, 0.3617, 0.3643, 0.3669, 0.3695, 0.3721, 0.3746, 0.3772, 0.3798, 0.3824, 0.3850, 0.3876, 0.3902, 0.3928, 0.3954, 0.3980, 0.4006, 0.4033, 0.4059, 0.4085, 0.4111, 0.4137, 0.4163, 0.4190, 0.4216, 0.4242, 0.4269, 0.4295, 0.4321, 0.4348, 0.4374, 0.4400, 0.4427, 0.4453, 0.4480, 0.4506, 0.4533, 0.4559, 0.4586, 0.4612, 0.4639, 0.4666, 0.4692, 0.4719, 0.4746, 0.4773, 0.4799, 0.4826, 0.4853, 0.4880, 0.4907, 0.4934, 0.4961, 0.4987, 0.5014, 0.5041, 0.5069 };

int32_t gPeakNum = 0;
int32_t gRdmDummyFrameID = 0;

int32_t gRangeCfarMethod = RANGE_CFAR_METHOD;
int32_t gDopplerCfarMethod = DOPPLER_CFAR_METHOD;
int32_t gRemoveMinFlag = REMOVE_MIN_FLAG;
int32_t gRangeCfarThreshold[6] = { CFAR_RANGE_THRESHOLD_SEG_ST_1ST, CFAR_RANGE_THRESHOLD_SEG_ST_2ND, CFAR_RANGE_THRESHOLD_SEG_ST_3RD, CFAR_RANGE_THRESHOLD_SEG_ST_4TH, CFAR_RANGE_THRESHOLD_SEG_ST_5TH, CFAR_RANGE_THRESHOLD_SEG_ST_6TH };
int32_t gDopplerCfarThreshold[6] = { CFAR_DOPPLER_THRESHOLD_SEG_ST_1ST, CFAR_DOPPLER_THRESHOLD_SEG_ST_2ND, CFAR_DOPPLER_THRESHOLD_SEG_ST_3RD, CFAR_DOPPLER_THRESHOLD_SEG_ST_4TH, CFAR_DOPPLER_THRESHOLD_SEG_ST_5TH, CFAR_DOPPLER_THRESHOLD_SEG_ST_6TH };
int32_t gGlobalCfarThreshold[6] = { CFAR_GLOBAL_THRESHOLD_SEG_ST_1ST, CFAR_GLOBAL_THRESHOLD_SEG_ST_2ND, CFAR_GLOBAL_THRESHOLD_SEG_ST_3RD, CFAR_GLOBAL_THRESHOLD_SEG_ST_4TH, CFAR_GLOBAL_THRESHOLD_SEG_ST_5TH, CFAR_GLOBAL_THRESHOLD_SEG_ST_6TH };

//int32_t gRangeCfarThreshold_end[6] = { 6804, 6804, 6804, 6804, 6804, 6804 };
//int32_t gDopplerCfarThreshold_end[6] = { 6804, 6804, 6804, 6804, 6804, 6804 };
//int32_t gGlobalCfarThreshold_end[6] = { 6804, 6804, 6804, 6804, 6804, 6804 };

int32_t gRangeCfarThreshold_end[6] = { CFAR_RANGE_THRESHOLD_SEG_ED_1ST, CFAR_RANGE_THRESHOLD_SEG_ED_2ND, CFAR_RANGE_THRESHOLD_SEG_ED_3RD, CFAR_RANGE_THRESHOLD_SEG_ED_4TH, CFAR_RANGE_THRESHOLD_SEG_ED_5TH, CFAR_RANGE_THRESHOLD_SEG_ED_6TH };
int32_t gDopplerCfarThreshold_end[6] = { CFAR_DOPPLER_THRESHOLD_SEG_ED_1ST, CFAR_DOPPLER_THRESHOLD_SEG_ED_2ND, CFAR_DOPPLER_THRESHOLD_SEG_ED_3RD, CFAR_DOPPLER_THRESHOLD_SEG_ED_4TH, CFAR_DOPPLER_THRESHOLD_SEG_ED_5TH, CFAR_DOPPLER_THRESHOLD_SEG_ED_6TH };
int32_t gGlobalCfarThreshold_end[6] = { CFAR_GLOBAL_THRESHOLD_SEG_ED_1ST, CFAR_GLOBAL_THRESHOLD_SEG_ED_2ND, CFAR_GLOBAL_THRESHOLD_SEG_ED_3RD, CFAR_GLOBAL_THRESHOLD_SEG_ED_4TH, CFAR_GLOBAL_THRESHOLD_SEG_ED_5TH, CFAR_GLOBAL_THRESHOLD_SEG_ED_6TH };

int32_t gRangeBinCutIdx[6] = { RANGE_CUT_INDEX_1ST, RANGE_CUT_INDEX_2ND, RANGE_CUT_INDEX_3RD, RANGE_CUT_INDEX_4TH, RANGE_CUT_INDEX_5TH, RANGE_CUT_INDEX_6TH };

sResolutionDetect_t gResolutionDetect =
{
	.RangeResotion = 			DEFAULT_RANGE_RESO,
	.VelocityResotion[0] = 		DEFAULT_VELOCITY_RESO_0,
	.VelocityResotion[1] = 		DEFAULT_VELOCITY_RESO_1,
	.MagResotion = 				DEFAULT_MAG_RESO,
};

sCfarParameter_t gCfarParameter =
{
	.rangeBins = 				DEFAULT_RANGE_BINS,
	.dopplerBins = 				DEFAULT_DOPPLER_BINS,
    .rangeWin = 				DEFAULT_RANGE_WIN,
    .rangeGuard = 				DEFAULT_RANGE_GUARD,
    .dopplerWin = 				DEFAULT_DOPPLER_WIN,
    .dopplerGuard = 			DEFAULT_DOPPLER_GUARD,
    .osRange = 					DEFAULT_OS_RANGE,
    .osRangeK =					(OS_K * DEFAULT_RANGE_WIN * 2),
};

struct timespec g_tUsDetectProcCur[32] = { 0 };
struct timespec g_tUsDetectProcEnd[32] = { 0 };
uint32_t g_tUsDetectProcUsed[32] = { 0 };


void GetRdmapData(uint32_t *rdmBuff)
{
//	memcpy(rdmBuff, &rdmDummyData_xingren_33frame[DUMMY_FRAME_NUM_1_SIZE * gRdmDummyFrameID], sizeof(uint32_t) * DEFAULT_RANGE_BINS * DEFAULT_DOPPLER_BINS);
//	memcpy(rdmBuff, &rdmDummyData_3target_10frameDUMMY_[FRAME_NUM_1_SIZE * gRdmDummyFrameID], sizeof(uint32_t) * DEFAULT_RANGE_BINS * DEFAULT_DOPPLER_BINS);
//	memcpy(rdmBuff, rdmDummyData_1target, sizeof(uint32_t) * DUMMY_DEFAULT_RANGE_BINS * DUMMY_DEFAULT_DOPPLER_BINS);
//	memcpy(rdmBuff, rdmDummyData_20target, sizeof(uint32_t) * DUMMY_DEFAULT_RANGE_BINS * DUMMY_DEFAULT_DOPPLER_BINS);

	gRdmDummyFrameID++;
	if (gRdmDummyFrameID == DUMMY_FRAME_NUM_REAL)
	{
		gRdmDummyFrameID = 0;
	}
}


ret_code_t CalcHistogram(uint32_t *rdmBuff, uint16_t *histBuff)
{
	int32_t i = 0, j = 0;
	uint32_t *rdmMagAux = NULL;
	uint16_t *histAux = NULL;

    if ((rdmBuff == NULL) || (histBuff == NULL))
    {
    	return RET_INVALID_PARAM;
    }

	memset(histBuff, 0, sizeof(uint16_t) * (DEFAULT_RANGE_BINS * DEFAULT_HIST_BINS));

	for (i = 0; i < DEFAULT_RANGE_BINS; i++)
	{
		rdmMagAux = &rdmBuff[(i << DEFAULT_DOPPLER_BINS_EXP)];	// rdmBuff[i * DEFAULT_DOPPLER_BINS]
		histAux = &histBuff[(i << DEFAULT_HIST_BINS_EXP)];	// histBuff[i * DEFAULT_HIST_BINS]
		for (j = 0; j < DEFAULT_DOPPLER_BINS; j++)
		{
			histAux[(rdmMagAux[j] >> RDM_MAG_PER_HIST_BIN_EXP)]++;	//normal process, input in 256*log2, 20220916
//			histAux[((uint32_t)(256 * log2f(rdmMagAux[j])) >> RDM_MAG_PER_HIST_BIN_EXP)]++;	//test process, input in 256*log2, 20220916
		}
	}

    return RET_OK;
}


ret_code_t CalcThreshold(uint16_t *histBuff, uint16_t *histMaxIndex, uint16_t *threshBuff, int16_t nrRangeBins)
{
    uint16_t i, j;
    uint16_t maxInd, thBin;
    uint16_t maxVal, thVal;
    uint16_t *cHist;
    float tmpThrd = 0.f;
    uint16_t iR = 0, iL = 0;
	uint16_t threshBuff_5mean[DEFAULT_RANGE_BINS] = {0U};

    if ((histBuff == NULL) || (threshBuff == NULL))
    {
        return RET_INVALID_PARAM;
    }

	/* Compute separate threshold for each range bin */
	for (i = 0; i < nrRangeBins; i++)
	{
		maxVal = 0;
		maxInd = 0;
		cHist = &histBuff[(i << DEFAULT_HIST_BINS_EXP)];	// &histBuff[i * DEFAULT_HIST_BINS];

		/* Find index of first max value in histogram, ignoring bins corresponding to value 0*/
		for (j = 2; j < DEFAULT_HIST_BINS; j++)
		{
			if (cHist[j] > maxVal)
			{
				maxVal = cHist[j];
				maxInd = j;
			}
		}

		thBin = maxInd + HIST_THRESHOLD_GUARD; // AEB:8;	/* Use a guard*/
//		thVal = (threshBuff[i] >> 1) + (thBin << (RDM_MAG_PER_HIST_BIN_EXP - 1)); /* Turn to spt_log2 format and average with previous threshold*/
		thVal = (thBin << (RDM_MAG_PER_HIST_BIN_EXP)); /* Turn to spt_log2 format and average with previous threshold*/
		threshBuff[i] = thVal;
		histMaxIndex[i] = maxInd;
    }

	/* The average of the current + two of left and right rangeBin */
	for (i = 0; i < nrRangeBins; i++)
	{
		tmpThrd = 0.f;
		for (j = 1; j <= 2; j++)
		{
			if ( (i - j) < 0 )
			{
				iL = i + 2 + j;
			}
			else
			{
				iL = i - j;
			}

			if ( (i + j) >= nrRangeBins )
			{
				iR = i - 2 - j;
			}

			else
			{
				iR = i + j;
			}

			tmpThrd = tmpThrd + (threshBuff[iL] / 5.f) + (threshBuff[iR] / 5.f);


		}

		// threshBuff[i] = tmpThrd + (threshBuff[i] / 5);
		threshBuff_5mean[i] = (uint16_t)(tmpThrd + (threshBuff[i] / 5.f));
	}

	memcpy(threshBuff,threshBuff_5mean,sizeof(uint16_t)*DEFAULT_RANGE_BINS);

    return RET_OK;
}


ret_code_t CalcTransposeRdm(uint32_t *rdmBuff, uint32_t *rdmTransposeBuff, int32_t rangeBins, int32_t dopplerBins)
{
	int32_t i = 0, j = 0;
	uint32_t *rdmTransposeBuffAux = NULL;
	uint32_t *rdmBuffAux = NULL;

    if ((rdmBuff == NULL) || (rdmTransposeBuff == NULL) || (rangeBins <= 0) || (dopplerBins <= 0))
    {
        return RET_INVALID_PARAM;
    }

    for (i = 0; i < rangeBins; i++)
    {
        rdmTransposeBuffAux = &rdmTransposeBuff[i];
		rdmBuffAux = &rdmBuff[(i << DEFAULT_DOPPLER_BINS_EXP)];	// rdmBuff + i * dopplerBins;
        for (j = 0; j < dopplerBins; j++)
        {
            rdmTransposeBuffAux[(j << DEFAULT_RANGE_BINS_EXP)] = rdmBuffAux[j];
        }
    }
    return RET_OK;
}


ret_code_t CalcPeakSearchBitmap(uint32_t *rdmBuff, uint32_t *rdmTransposeBuff, uint16_t *threshBuff, \
		uint32_t *detectBitmap, uint8_t *rdmapDooplerDimPeakFlag, int32_t rangeBins, int32_t dopplerBins)
{
	int32_t i = 0, j = 0;
	uint32_t rdmMag = 0;
	uint32_t rdmMagLeft = 0;
	uint32_t rdmMagRight = 0;
	uint32_t eachRangeBinStartIndex = 0;
	uint32_t eachDooplerBinStartIndex = 0;
	uint32_t eachPeakBitmapIndex = 0;
	uint32_t *rdmMagAux = NULL;
	uint32_t *rdmTransposeMagAux = NULL;
	uint8_t *rdmapDooplerDimPeakFlagAux = NULL;

    if ((rdmBuff == NULL) || (threshBuff == NULL) || (detectBitmap == NULL))
    {
    	return RET_INVALID_PARAM;
    }
    gPeakNum = 0;
	memset(detectBitmap, 0, sizeof(uint32_t) * PACKED_BITMAP_NUM_ALL_RDM);
	memset(rdmapDooplerDimPeakFlag, 0, sizeof(uint8_t) * DEFAULT_RANGE_BINS * DEFAULT_DOPPLER_BINS);

	// 1. doppler dimension peak search
	for (i = DETECT_RANGE_START_INDEX; i < DEFAULT_RANGE_BINS; i++)
	{
		eachRangeBinStartIndex = (i << DEFAULT_DOPPLER_BINS_EXP);
		rdmMagAux = &rdmBuff[eachRangeBinStartIndex];
		rdmapDooplerDimPeakFlagAux = &rdmapDooplerDimPeakFlag[eachRangeBinStartIndex];
		for (j = 0; j < DEFAULT_DOPPLER_BINS; j++)
		{
			rdmMag = rdmMagAux[j];
			if (rdmMag < threshBuff[i])
			{
				continue;
			}
			else
			{
				if (j == 0)
				{
					rdmMagLeft = rdmMagAux[DEFAULT_DOPPLER_BINS - 1];
					rdmMagRight = rdmMagAux[j + 1];
				}
				else if (j == (DEFAULT_DOPPLER_BINS - 1))
				{
					rdmMagLeft = rdmMagAux[j - 1];
					rdmMagRight = rdmMagAux[0];
				}
				else
				{
					rdmMagLeft = rdmMagAux[j - 1];
					rdmMagRight = rdmMagAux[j + 1];
				}

				if ((rdmMag > rdmMagLeft) && (rdmMag > rdmMagRight))
				{
					rdmapDooplerDimPeakFlagAux[j] = 1;
				}
			}
		}
	}

	// 2. range dimension peak search
#if ENABLE_PS_STATIC
	for (i = 0; i < DEFAULT_DOPPLER_BINS; i++)
#else
//	for (i = 2; i < (DEFAULT_DOPPLER_BINS - 1); i++)// ignore [-1,0,1]static doppler bin
	for (i = 1; i < DEFAULT_DOPPLER_BINS; i++)// ignore [0]static doppler bin

#endif
	{
		eachDooplerBinStartIndex = (i << DEFAULT_RANGE_BINS_EXP);
		rdmTransposeMagAux = &rdmTransposeBuff[eachDooplerBinStartIndex];
		for (j = DETECT_RANGE_START_INDEX; j < DEFAULT_RANGE_BINS; j++)
		{
			if (rdmapDooplerDimPeakFlag[(j << DEFAULT_DOPPLER_BINS_EXP) + i] == 0)
			{
				continue;
			}
			else
			{
				rdmMag = rdmTransposeBuff[eachDooplerBinStartIndex + j];
				if (j == 0)
				{
					rdmMagLeft = 0;
					rdmMagRight = rdmTransposeMagAux[j + 1];
				}
				else if (j == (DEFAULT_RANGE_BINS - 1))
				{
					rdmMagLeft = rdmTransposeMagAux[j - 1];
					rdmMagRight = 0;
				}
				else
				{
					rdmMagLeft = rdmTransposeMagAux[j - 1];
					rdmMagRight = rdmTransposeMagAux[j + 1];
				}

				if ((rdmMag > rdmMagLeft) && (rdmMag > rdmMagRight))
				{
					eachPeakBitmapIndex = (i << PACKED_BITMAP_NUM_PER_DOPPLER_EXP) + (j >> PACKED_BITMAP_ELEMENT_SIZE_EXP);
					detectBitmap[eachPeakBitmapIndex] |= (1 << (j % PACKED_BITMAP_ELEMENT_SIZE));
					gPeakNum++;
				}
			}
		}
	}

    return RET_OK;
}


/* 1.Range CFAR mode */
int32_t Range_OSCFAR(int32_t idx, int32_t ROW, int32_t COL, int32_t CFAR_WINDOW, int32_t CFAR_GUARD, uint32_t *RangeData, int32_t osRangeK)
{
	int32_t i = 0, j = 0;
	int32_t index_RangeData = 0;
	int32_t value_min = 65535;
	int32_t value_max = 0;
	int32_t min_index = 0;
	int32_t max_index = 0;
	int32_t min_temp = 0;
	int32_t max_temp = 0;
	int32_t winsize = CFAR_WINDOW * 2;
	int32_t needSortsz[DEFAULT_RANGE_WIN * 2] = { 0 };
	int32_t Range_NoiseFloor_OS = 0;

	/* front edge protection of range */
	if (idx < (CFAR_WINDOW + CFAR_GUARD))
	{
		if (idx < (CFAR_GUARD + 1))	/* insufficient of window units */
		{
			for (i = 0; i < winsize; i++)
			{
				index_RangeData = (idx + CFAR_GUARD + i + 1) * COL;
				needSortsz[i] = RangeData[index_RangeData];
				if (needSortsz[i] > value_max)
				{
					value_max = needSortsz[i];
					max_index = i;
				}
				else if (needSortsz[i] < value_min)
				{
					value_min = needSortsz[i];
					min_index = i;
				}
				else
				{
					/* do nothing */
				}
			}
		}
		else
		{
			for (i = 0; i < winsize; i++)
			{
				if (i < (CFAR_GUARD + CFAR_WINDOW - idx))
				{
					index_RangeData = (idx + CFAR_GUARD + CFAR_WINDOW + i + 1) * COL;
					needSortsz[i] = RangeData[index_RangeData];
				}
				else if (i < CFAR_WINDOW)
				{
					index_RangeData = (idx - CFAR_GUARD - CFAR_WINDOW + i) * COL;
					needSortsz[i] = RangeData[index_RangeData];
				}
				else
				{
					index_RangeData = (idx + CFAR_GUARD - CFAR_WINDOW + i + 1) * COL;
					needSortsz[i] = RangeData[index_RangeData];
				}

				if (needSortsz[i] > value_max)
				{
					value_max = needSortsz[i];
					max_index = i;
				}
				else if (needSortsz[i] < value_min)
				{
					value_min = needSortsz[i];
					min_index = i;
				}
				else
				{
					/* do nothing */
				}
			}
		}
	}
	/* rear edge protection of range */
	else if ((idx < ROW) && (idx >= (ROW - CFAR_WINDOW - CFAR_GUARD)))
	{
		if (idx >= (ROW - CFAR_GUARD - 1))	/* insufficient of window units */
		{
			for (i = 0; i < winsize; i++)
			{
				index_RangeData = (idx - CFAR_GUARD - winsize + i) * COL;
				needSortsz[i] = RangeData[index_RangeData];
				if (needSortsz[i] > value_max)
				{
					value_max = needSortsz[i];
					max_index = i;
				}
				else if (needSortsz[i] < value_min)
				{
					value_min = needSortsz[i];
					min_index = i;
				}
				else
				{
					/* do nothing */
				}
			}
		}
		else
		{
			for (i = 0; i < winsize; i++)
			{
				if (i < (ROW - idx - CFAR_GUARD - 1))
				{
					index_RangeData = (idx + CFAR_GUARD + i + 1) * COL;
					needSortsz[i] = RangeData[index_RangeData];
				}
				else
				{
					index_RangeData = (idx - CFAR_GUARD - winsize + i) * COL;
					needSortsz[i] = RangeData[index_RangeData];
				}

				if (needSortsz[i] > value_max)
				{
					value_max = needSortsz[i];
					max_index = i;
				}
				else if (needSortsz[i] < value_min)
				{
					value_min = needSortsz[i];
					min_index = i;
				}
				else
				{
					/* do nothing */
				}
			}
		}
	}
	/* non-edge */
	else
	{
		for (i = 0; i < CFAR_WINDOW; i++)
		{
			index_RangeData = (idx - CFAR_GUARD - CFAR_WINDOW + i) * COL;
			needSortsz[i] = RangeData[index_RangeData];
			index_RangeData = (idx + CFAR_GUARD + i + 1) * COL;
			needSortsz[i + CFAR_WINDOW] = RangeData[index_RangeData];
			if (needSortsz[i] <= needSortsz[i + CFAR_WINDOW])
			{
				if (needSortsz[i + CFAR_WINDOW] > value_max)
				{
					value_max = needSortsz[i + CFAR_WINDOW];
					max_index = i + CFAR_WINDOW;
				}
				if (needSortsz[i] < value_min)
				{
					value_min = needSortsz[i];
					min_index = i;
				}
			}
			else
			{
				if (needSortsz[i] > value_max)
				{
					value_max = needSortsz[i];
					max_index = i;
				}
				if (needSortsz[i + CFAR_WINDOW] < value_min)
				{
					value_min = needSortsz[i + CFAR_WINDOW];
					min_index = i + CFAR_WINDOW;
				}
			}
		}
	}

/* OS sort */
	max_temp = needSortsz[0];
	needSortsz[0] = needSortsz[max_index];
	needSortsz[max_index] = max_temp;
	if (winsize == 2)
	{
		Range_NoiseFloor_OS = needSortsz[winsize - 1];
	}
	else
	{
		if (min_index == 0)
		{
			min_temp = needSortsz[winsize - 1];
			needSortsz[winsize - 1] = needSortsz[max_index];
			needSortsz[max_index] = min_temp;
		}
		else
		{
			min_temp = needSortsz[winsize - 1];
			needSortsz[winsize - 1] = needSortsz[min_index];
			needSortsz[min_index] = min_temp;
		}

		for (i = 1; i < (winsize - osRangeK + 1); i++)
		{
			for (j = (i + 1); j < (winsize - 1); j++)
			{
				if (needSortsz[i] < needSortsz[j])
				{
					min_temp = needSortsz[i];
					needSortsz[i] = needSortsz[j];
					needSortsz[j] = min_temp;
				}
			}
		}
		Range_NoiseFloor_OS = needSortsz[winsize - osRangeK];
	}

	return Range_NoiseFloor_OS;
}


int32_t Range_CACFAR(int32_t idx, int32_t ROW, int32_t COL, int32_t CFAR_WINDOW, int32_t CFAR_GUARD, uint32_t *RangeData)
{
	int32_t i = 0;
	int32_t index_RangeData = 0;
	uint32_t data_sum = 0;
	int32_t i_temp = 0;
	int32_t winsize = CFAR_WINDOW * 2;
	int32_t Range_NoiseFloor_CA = 0;

	/* front edge protection of range */
	if (idx < (CFAR_WINDOW + CFAR_GUARD))
	{
		if (idx < (CFAR_GUARD + 1))	/* insufficient of window units */
		{
			for (i = 0; i < winsize; i++)
			{
				index_RangeData = (idx + CFAR_GUARD + i + 1) * COL;
				if (RangeData[index_RangeData] != 0)
				{
					data_sum += RangeData[index_RangeData];
					i_temp += 1;
				}
			}
		}
		else
		{
			for (i = 0; i < winsize; i++)
			{
				if (i < (CFAR_GUARD + CFAR_WINDOW - idx))
				{
					index_RangeData = (idx + CFAR_GUARD + CFAR_WINDOW + i + 1) * COL;
					if (RangeData[index_RangeData] != 0)
					{
						data_sum += RangeData[index_RangeData];
						i_temp += 1;
					}
				}
				else if (i < CFAR_WINDOW)
				{
					index_RangeData = (idx - CFAR_GUARD- CFAR_WINDOW + i) * COL;
					if (RangeData[index_RangeData] != 0)
					{
						data_sum += RangeData[index_RangeData];
						i_temp += 1;
					}
				}
				else
				{
					index_RangeData = (idx + CFAR_GUARD - CFAR_WINDOW + i + 1) * COL;
					if (RangeData[index_RangeData] != 0)
					{
						data_sum += RangeData[index_RangeData];
						i_temp += 1;
					}
				}
			}
		}
	}
	/* rear edge protection of range */
	else if ((idx < ROW) && (idx >= (ROW - CFAR_WINDOW - CFAR_GUARD)))
	{
		if (idx >= (ROW - CFAR_GUARD - 1))	/* insufficient of window units */
		{
			for (i = 0; i < winsize; i++)
			{
				index_RangeData = (idx - CFAR_GUARD - winsize + i) * COL;
				if (RangeData[index_RangeData] != 0)
				{
					data_sum += RangeData[index_RangeData];
					i_temp += 1;
				}
			}
		}
		else
		{
			for (i = 0; i < winsize; i++)
			{
				if (i < (ROW - idx - CFAR_GUARD - 1))
				{
					index_RangeData = (idx + CFAR_GUARD + i + 1) * COL;
					if (RangeData[index_RangeData] != 0)
					{
						data_sum += RangeData[index_RangeData];
						i_temp += 1;
					}
				}
				else
				{
					index_RangeData = (idx - CFAR_GUARD - winsize + i) * COL;
					if (RangeData[index_RangeData] != 0)
					{
						data_sum += RangeData[index_RangeData];
						i_temp += 1;
					}
				}
			}
		}
	}
	/* non-edge */
	else
	{
		for (i = 0; i < CFAR_WINDOW; i++)
		{
			index_RangeData = (idx - CFAR_GUARD - CFAR_WINDOW + i) * COL;
			if (RangeData[index_RangeData] != 0)
			{
				data_sum += RangeData[index_RangeData];
				i_temp += 1;
			}
			index_RangeData = (idx + CFAR_GUARD + i + 1) * COL;
			if (RangeData[index_RangeData] != 0)
			{
				data_sum += RangeData[index_RangeData];
				i_temp += 1;
			}
		}
	}

	if (i_temp >= 1)
	{
		Range_NoiseFloor_CA = (int32_t)(data_sum / (i_temp));
	}
	else
	{
		Range_NoiseFloor_CA = 0;
	}

	return Range_NoiseFloor_CA;
}


int32_t Range_GOCFAR(int32_t idx, int32_t ROW, int32_t COL, int32_t CFAR_WINDOW, int32_t CFAR_GUARD, uint32_t *RangeData)
{
	int32_t i = 0;
	int32_t index_R = 0, index_L = 0;
	uint32_t dataR_sum = 0, dataL_sum = 0;
	int32_t iR_temp = 0, iL_temp = 0;
	int32_t Range_NoiseFloor_GO = 0;
	int32_t mean_R = 0, mean_L = 0;

	/* just right window  */
	if (idx < (CFAR_WINDOW + CFAR_GUARD))
	{
		for (i = 0; i < CFAR_WINDOW; i++)
		{
			index_R = (idx + CFAR_GUARD + i + 1) * COL;
			if (RangeData[index_R] != 0)
			{
				dataR_sum += RangeData[index_R];
				iR_temp += 1;
			}
		}
	}
	/* just left window */
	else if ((idx < ROW) && (idx >= (ROW - CFAR_WINDOW - CFAR_GUARD)))
	{
		for (i = 0; i < CFAR_WINDOW; i++)
		{
			index_L = (idx - CFAR_GUARD - CFAR_WINDOW + i) * COL;
			if (RangeData[index_L] != 0)
			{
				dataL_sum += RangeData[index_L];
				iL_temp += 1;
			}
		}
	}
	/* left and right window */
	else
	{
		for (i = 0; i < CFAR_WINDOW; i++)
		{
			index_R = (idx + CFAR_GUARD + i + 1) * COL;
			if (RangeData[index_R] != 0)
			{
				dataR_sum += RangeData[index_R];
				iR_temp += 1;
			}

			index_L = (idx - CFAR_GUARD - CFAR_WINDOW + i) * COL;
			if (RangeData[index_L] != 0)
			{
				dataL_sum += RangeData[index_L];
				iL_temp += 1;
			}
		}
	}

	/* max(mean_R,mean_L) */
	if ((iL_temp < 1) && (iR_temp >= 1))
	{
		Range_NoiseFloor_GO = (int32_t)(dataR_sum / (iR_temp));
	}
	else if ((iL_temp >= 1) && (iR_temp < 1))
	{
		Range_NoiseFloor_GO = (int32_t)(dataL_sum / (iL_temp));
	}
	else if ((iL_temp >= 1) && (iR_temp >= 1))
	{
		mean_R = (int32_t)(dataR_sum / (iR_temp));
		mean_L = (int32_t)(dataL_sum / (iL_temp));
		if (mean_R > mean_L)
		{
			Range_NoiseFloor_GO = mean_R;
		}
		else
		{
			Range_NoiseFloor_GO = mean_L;
		}
	}
	else
	{
		Range_NoiseFloor_GO = 0;
	}

	return Range_NoiseFloor_GO;
}

/* 1.1 remove the min value win  */
int32_t Range_CACFAR_new(int32_t idx, int32_t ROW, int32_t COL, int32_t CFAR_WINDOW, int32_t CFAR_GUARD, uint32_t *RangeData, bool RemoveMin_Flag)
{
	int32_t i_R = 0, i_L = 0;
	int32_t index_R = 0, index_L = 0;
	int32_t index_RangeData = 0;
	uint32_t data_sum = 0;
	int32_t i_temp = 0;
	int32_t winsize = CFAR_WINDOW * 2;
	int32_t Range_NoiseFloor_CA = 0;
	int32_t value_min = 65535;

	/* the right window */
	for (i_R = 1; i_R <= CFAR_WINDOW; i_R++)
	{
		index_R = idx + CFAR_GUARD + i_R;
		if ( index_R < ROW )
		{
			index_RangeData = index_R * COL;
			if (RangeData[index_RangeData] != 0)
			{
				data_sum += RangeData[index_RangeData];
				i_temp += 1;

				// find the min value
			  if ( (RemoveMin_Flag) && ( RangeData[index_RangeData] < value_min ) )
			  {
				  value_min = RangeData[index_RangeData];
			  }
			}
		}
		else //ÓÒ±ß²»¹»£¬Ïò×óÈ¡£¬Ìø¹ý×ó´°¸²¸Ç·¶Î§£¬ÇÒ>×ó±ß½ç£¬²î¼¸¸öÁ¬×ÅÈ¡¼¸¸ö
		{
			index_R = idx - CFAR_GUARD - CFAR_WINDOW - (CFAR_WINDOW - i_R + 1);
			if ( (index_R >= 0) && (index_R < ROW) )
			{
				index_RangeData = index_R * COL;
				if (RangeData[index_RangeData] != 0)
				{
					data_sum += RangeData[index_RangeData];
					i_temp += 1;

					// find the min value
				  if ( (RemoveMin_Flag) && ( RangeData[index_RangeData] < value_min ) )
				  {
					  value_min = RangeData[index_RangeData];
				  }
				}
			}
		}

	}

	/* the lift window */
	for (i_L = 1; i_L <= CFAR_WINDOW; i_L++)
	{
		index_L = idx - CFAR_GUARD - i_L;
		if ( index_L >= 0 )
		{
			index_RangeData = index_L * COL;
			if (RangeData[index_RangeData] != 0)
			{
				data_sum += RangeData[index_RangeData];
				i_temp += 1;

				// find the min value
			  if ( (RemoveMin_Flag) && ( RangeData[index_RangeData] < value_min ) )
			  {
				  value_min = RangeData[index_RangeData];
			  }
			}
		}
		else //×ó±ß²»¹»£¬ÏòÓÒÈ¡£¬Ìø¹ýÓÒ´°¸²¸Ç·¶Î§£¬ÇÒ<ÓÒ±ß½ç£¬²î¼¸¸öÁ¬×ÅÈ¡¼¸¸ö
		{
			index_L = idx + CFAR_GUARD + CFAR_WINDOW + (CFAR_WINDOW - i_L + 1);
			if ( (index_L >= 0) && (index_L < ROW) )
			{
				index_RangeData = index_L * COL;
				if (RangeData[index_RangeData] != 0)
				{
					data_sum += RangeData[index_RangeData];
					i_temp += 1;

					// find the min value
				  if ( (RemoveMin_Flag) && ( RangeData[index_RangeData] < value_min ) )
				  {
					  value_min = RangeData[index_RangeData];
				  }
				}
			}
		}

	}

	/* take the average */
	if (i_temp >= 1)
	{
		if ( (RemoveMin_Flag) && (value_min != 0) && (i_temp > 2) )
		{
			data_sum -= value_min;
			i_temp -= 1;
		}
		Range_NoiseFloor_CA = (int32_t)(data_sum / (i_temp));
	}
	else
	{
		Range_NoiseFloor_CA = 0;
	}

	return Range_NoiseFloor_CA;
}


int32_t Range_GOCFAR_new(int32_t idx, int32_t ROW, int32_t COL, int32_t CFAR_WINDOW, int32_t CFAR_GUARD, uint32_t *RangeData, bool RemoveMin_Flag)
{
	int32_t i = 0;
	int32_t index_R = 0, index_L = 0;
	uint32_t dataR_sum = 0, dataL_sum = 0;
	int32_t iR_temp = 0, iL_temp = 0;
	int32_t Range_NoiseFloor_GO = 0;
	int32_t mean_R = 0, mean_L = 0;
	int32_t valueR_min = 65535, valueL_min = 65535;

	/* just right window  */
	if (idx < (CFAR_WINDOW + CFAR_GUARD))
	{
		for (i = 1; i <= CFAR_WINDOW; i++)
		{
			index_R = (idx + CFAR_GUARD + i) * COL;
			if (RangeData[index_R] != 0)
			{
				dataR_sum += RangeData[index_R];
				iR_temp += 1;
				// find the right min value
				if ( (RemoveMin_Flag) && ( RangeData[index_R] < valueR_min ) )
				{
					valueR_min = RangeData[index_R];
				}
			}
		}
	}
	/* just left window */
	else if ( (idx >= (ROW - CFAR_WINDOW - CFAR_GUARD)) && (idx < ROW) )
	{
		for (i = 1; i <= CFAR_WINDOW; i++)
		{
			index_L = (idx - CFAR_GUARD - CFAR_WINDOW - 1 + i) * COL;
			if (RangeData[index_L] != 0)
			{
				dataL_sum += RangeData[index_L];
				iL_temp += 1;
				// find the left min value
				if ( (RemoveMin_Flag) && ( RangeData[index_L] < valueL_min ) )
				{
					valueL_min = RangeData[index_L];
				}
			}
		}
	}
	/* left and right window */
	else
	{
		for (i = 1; i <= CFAR_WINDOW; i++)
		{
			index_R = (idx + CFAR_GUARD + i) * COL;
			if (RangeData[index_R] != 0)
			{
				dataR_sum += RangeData[index_R];
				iR_temp += 1;
				// find the right min value
				if ( (RemoveMin_Flag) && ( RangeData[index_R] < valueR_min ) )
				{
					valueR_min = RangeData[index_R];
				}
			}

			index_L = (idx - CFAR_GUARD - CFAR_WINDOW - 1 + i) * COL;
			if (RangeData[index_L] != 0)
			{
				dataL_sum += RangeData[index_L];
				iL_temp += 1;
				// find the left min value
				if ( (RemoveMin_Flag) && ( RangeData[index_L] < valueL_min ) )
				{
					valueL_min = RangeData[index_L];
				}
			}
		}
	}

	/* max(mean_R,mean_L) */
	if ((iL_temp < 1) && (iR_temp >= 1))
	{
		if ( (RemoveMin_Flag) && (valueR_min != 0)  && (iR_temp > 2) )
		{
			dataR_sum -= valueR_min;
			iR_temp -= 1;
		}
		Range_NoiseFloor_GO = (int32_t)(dataR_sum / (iR_temp));
	}
	else if ((iL_temp >= 1) && (iR_temp < 1))
	{
		if ( (RemoveMin_Flag) && (valueL_min != 0) && (iL_temp > 2) )
		{
			dataL_sum -= valueL_min;
			iL_temp -= 1;
		}
		Range_NoiseFloor_GO = (int32_t)(dataL_sum / (iL_temp));
	}
	else if ((iL_temp >= 1) && (iR_temp >= 1))
	{
		if ( (RemoveMin_Flag) && (valueR_min != 0)  && (iR_temp > 2) )
		{
			dataR_sum -= valueR_min;
			iR_temp -= 1;
		}
		if ( (RemoveMin_Flag) && (valueL_min != 0) && (iL_temp > 2) )
		{
			dataL_sum -= valueL_min;
			iL_temp -= 1;
		}

		mean_R = (int32_t)(dataR_sum / (iR_temp));
		mean_L = (int32_t)(dataL_sum / (iL_temp));
		if (mean_R > mean_L)
		{
			Range_NoiseFloor_GO = mean_R;
		}
		else
		{
			Range_NoiseFloor_GO = mean_L;
		}
	}
	else
	{
		Range_NoiseFloor_GO = 0;
	}

	return Range_NoiseFloor_GO;
}


/*  Bubble Sort:descending order  */
void Bubble_Sort(int32_t* indexList, int32_t Num_win, uint32_t *RangeData)
{
	int32_t i_win = 0, j_win = 0, tmpIdx = 0;;
	uint32_t tmpMag_j = 0, tmpMag_j1 = 0;
	

	for (i_win = 0; i_win < Num_win-1; i_win++)
	{
		for (j_win = 0; j_win < Num_win-i_win; j_win++)
		{
			tmpMag_j = RangeData[indexList[j_win]];
			tmpMag_j1 = RangeData[indexList[j_win+1]];  
			if (tmpMag_j < tmpMag_j1)	
			{
				tmpIdx = indexList[j_win+1];
				indexList[j_win+1] = indexList[j_win];
				indexList[j_win] = tmpIdx;
			}
		}
	}
}



/*  remove the min value win  */
int32_t Range_CMLD_CFAR(int32_t idx, int32_t ROW, int32_t COL, int32_t CFAR_WINDOW, int32_t CFAR_GUARD, uint32_t *RangeData, uint8_t removeMax_r, uint8_t removeMin_r)
{
	int32_t i = 0, i_temp = 0;
	int32_t index_R = 0, index_L = 0;
	int32_t iR_temp = 0, iL_temp = 0;
	uint32_t data_sum = 0;

	int32_t Range_NoiseFloor_CMLD = 0;

	int32_t indexList_R[64] = {0};
	int32_t indexList_L[64] = {0};

	int32_t i_R, i_L;

//	memset(indexList_R, 0, sizeof(indexList_R));

	/* the right window */
	for (i_R = 1; i_R <= CFAR_WINDOW + removeMax_r + removeMin_r; i_R++)
	{
		if (iR_temp >= CFAR_WINDOW) 
		{
			break;
		}
		
		if ( idx+CFAR_GUARD+i_R < ROW )
		{
			indexList_R[iR_temp] = (idx+CFAR_GUARD+i_R) * COL; 
			iR_temp = iR_temp + 1;
		}
		else //right side is not enough, take it to the left side
		{
			if ( (idx-CFAR_GUARD-CFAR_WINDOW-(CFAR_WINDOW-i_R+1) >= 0) && (idx-CFAR_GUARD-CFAR_WINDOW-(CFAR_WINDOW-i_R+1) < ROW) )
			{
				indexList_R[iR_temp] = ( idx-CFAR_GUARD-CFAR_WINDOW-(CFAR_WINDOW-i_R+1) ) * COL; 
				iR_temp = iR_temp + 1;
			}
		}
	}

	/* the lift window */
	for (i_L = 1; i_L <= CFAR_WINDOW + removeMax_r + removeMin_r; i_L++)
	{
		if (iL_temp >= CFAR_WINDOW) 
		{
			break;
		}
		
		if ( idx-CFAR_GUARD-i_L >= 0 )
		{
			// iL_temp = iL_temp + 1;
			indexList_L[iL_temp] = (idx-CFAR_GUARD-i_L) * COL; 
			iL_temp = iL_temp + 1;
		}
		else //left side is not enough, take it to the right side
		{
			if ( (idx+CFAR_GUARD+CFAR_WINDOW+(CFAR_WINDOW-i_L+1) >= 0) && (idx+CFAR_GUARD+CFAR_WINDOW+(CFAR_WINDOW-i_L+1) < ROW) )
			{
				// iL_temp = iL_temp + 1;
				indexList_L[iL_temp] = ( idx+CFAR_GUARD+CFAR_WINDOW+(CFAR_WINDOW-i_L+1) ) * COL; 
				iL_temp = iL_temp + 1;
			}
		}

	}

	/* Bubble Sort:descending order */
	Bubble_Sort(indexList_R, iR_temp, RangeData);
	Bubble_Sort(indexList_L, iL_temp, RangeData);


	/* take the average */
	for (i_R = 0+removeMax_r; i_R < iR_temp-removeMin_r; i_R++)
	{
		index_R = indexList_R[i_R];
		if ( RangeData[index_R] != 0 )
		{
			i_temp = i_temp + 1;
			data_sum = data_sum +  RangeData[index_R];
		}
	}
	for (i_L = 0+removeMax_r; i_L < iL_temp-removeMin_r; i_L++)
	{
		index_L = indexList_L[i_L];
		if ( RangeData[index_L] != 0 )
		{
			i_temp = i_temp + 1;
			data_sum = data_sum +  RangeData[index_L];
		}
	}
	
	if (i_temp >= 1)
	{
		 Range_NoiseFloor_CMLD = (int32_t)(data_sum / i_temp);
	}
	else
	{
		Range_NoiseFloor_CMLD = 0;
	}

	return Range_NoiseFloor_CMLD;
	
}



/* 2.Doppler CFAR mode */
int32_t Doppler_CACFAR(int32_t idx, int32_t ROW, int32_t COL, int32_t CFAR_WINDOW, int32_t CFAR_GUARD, uint32_t *DopplerData, bool RemoveMax_Flag)
{
	int32_t i = 0;
	int32_t index_left = 0;
	int32_t index_right = 0;
	int32_t i_temp = 0;
	uint32_t data_sum = 0;
	uint32_t value_max = 0;
	int32_t Doppler_NoiseFloor_CA = 0;
	int32_t iL_temp = 0, iR_temp = 0;
	int32_t use_left = 0, use_right = 0;

	for (i = 0; i < CFAR_WINDOW + 3; i++)
	{
		if ( (iL_temp >= CFAR_WINDOW) && (iR_temp >= CFAR_WINDOW) )
		{
			break;
		}

		use_left = 0;
		use_right = 0;

		index_left = idx - CFAR_GUARD - CFAR_WINDOW + i;
		index_right = idx + CFAR_GUARD + i + 1;

		if (index_left < 0)
		{
			index_left += COL;
		}
		if (index_right > (COL - 1))
		{
			index_right -= COL;
		}

		if (iL_temp < CFAR_WINDOW)
		{
			if (DopplerData[index_left] != 0)
			{
				if ( (index_left != 0) && (index_left != 1) && (index_left != (COL - 1)) )//add:remove the 1th,2nd and 31th dopplerBin
				{
					use_left = 1;//current left index is useful
					data_sum += DopplerData[index_left];
					iL_temp += 1;
				}
			}
		}
		if (iR_temp < CFAR_WINDOW)
		{
			if (DopplerData[index_right] != 0)
			{
				if ( (index_right != 0) && (index_right != 1) && (index_right != (COL - 1)) )//add:remove the 1th,2nd and 31th dopplerBin
				{
					use_right = 1;//current right index is useful
					data_sum += DopplerData[index_right];
					iR_temp += 1;
				}
			}
		}

		if ( (use_left == 1) && (DopplerData[index_left] > value_max) )
		{
			value_max = DopplerData[index_left];
		}
		if ( (use_right == 1) && (DopplerData[index_right] > value_max) )
		{
			value_max = DopplerData[index_right];
		}

	}

	i_temp = iL_temp + iR_temp;

	if (i_temp >= 1)
	{
		if ((i_temp >= 2) && (RemoveMax_Flag) && (value_max != 0))
		{
			Doppler_NoiseFloor_CA = (int32_t)((data_sum - value_max) / (i_temp - 1));
		}
		else
		{
			Doppler_NoiseFloor_CA = (int32_t)((data_sum) / (i_temp));
		}
	}
	else
	{
		Doppler_NoiseFloor_CA = 0;
	}

	return Doppler_NoiseFloor_CA;
}


int32_t Doppler_GOCFAR(int32_t idx, int32_t ROW, int32_t COL, int32_t CFAR_WINDOW, int32_t CFAR_GUARD, uint32_t *DopplerData, bool RemoveMax_Flag)
{
	int32_t i = 0;
	int32_t index_left = 0;
	int32_t index_right = 0;
	int32_t iR_temp = 0, iL_temp = 0;
	uint32_t dataR_sum = 0, dataL_sum = 0;
	uint32_t valueR_max = 0, valueL_max = 0;
	int32_t Doppler_NoiseFloor_GO = 0;
	int32_t mean_R = 0, mean_L = 0;
	int32_t use_left = 0, use_right = 0;

	/* left and right window */
	for (i = 0; i < CFAR_WINDOW + 3; i++)
	{
		if ( (iL_temp >= CFAR_WINDOW) && (iR_temp >= CFAR_WINDOW) )
		{
			break;
		}

		use_left = 0;
		use_right = 0;

		index_left = idx - CFAR_GUARD - CFAR_WINDOW + i;
		index_right = idx + CFAR_GUARD + i + 1;

		if (index_left < 0)
		{
			index_left += COL;
		}
		if (index_right > (COL - 1))
		{
			index_right -= COL;
		}

		if (iL_temp < CFAR_WINDOW)
		{
			if (DopplerData[index_left] != 0)
			{
				if ( (index_left != 0) && (index_left != 1) && (index_left != (COL - 1)) )//add:remove the 1th,2nd and 31th dopplerBin
				{
					use_left = 1;//current left index is useful
					dataL_sum += DopplerData[index_left];
					iL_temp += 1;
				}
			}
		}

		if (iR_temp < CFAR_WINDOW)
		{
			if (DopplerData[index_right] != 0)
			{
				if ( (index_right != 0) && (index_right != 1) && (index_right != (COL - 1)) )//add:remove the 1th,2nd and 31th dopplerBin
				{
					use_right = 1;//current right index is useful
					dataR_sum += DopplerData[index_right];
					iR_temp += 1;
				}
			}
		}

		if ( (use_left == 1) && (DopplerData[index_left] > valueL_max) )
		{
			valueL_max = DopplerData[index_left];
		}
		if ( (use_right == 1) && (DopplerData[index_right] > valueR_max) )
		{
			valueR_max = DopplerData[index_right];
		}

	}

	/* mean(meanR,meanL) */
	if ((iL_temp >= 1) && (iR_temp >= 1))
	{
		// the left part
		if ((iL_temp >= 2) && (RemoveMax_Flag) && (valueL_max != 0))
		{
			mean_L = (dataL_sum - valueL_max) / (iL_temp - 1);
		}
		else
		{
			mean_L = (dataL_sum) / (iL_temp);
		}

		// the right part
		if ((iR_temp >= 2) && (RemoveMax_Flag) && (valueR_max != 0))
		{
			mean_R = (dataR_sum - valueR_max) / (iR_temp - 1);
		}
		else
		{
			mean_R = (dataR_sum) / (iR_temp);
		}

		if (mean_R > mean_L)
		{
			Doppler_NoiseFloor_GO = mean_R;
		}
		else
		{
			Doppler_NoiseFloor_GO = mean_L;
		}
	}
	else if ((iL_temp >= 1) && (iR_temp < 1))
	{
		// only the left part
		if ((iL_temp >= 2) && (RemoveMax_Flag) && (valueL_max != 0))
		{
			Doppler_NoiseFloor_GO = (int32_t)((dataL_sum - valueL_max) / (iL_temp - 1));
		}
		else
		{
			Doppler_NoiseFloor_GO = (int32_t)((dataL_sum) / (iL_temp));
		}
	}
	else if ((iL_temp < 1) && (iR_temp >= 1))
	{
		// only the right part
		if ((iR_temp >= 2) && (RemoveMax_Flag) && (valueR_max != 0))
		{
			Doppler_NoiseFloor_GO = (int32_t)((dataR_sum - valueR_max) / (iR_temp - 1));
		}
		else
		{
			Doppler_NoiseFloor_GO = (int32_t)((dataR_sum) / (iR_temp));
		}
	}
	else
	{
		Doppler_NoiseFloor_GO = 0;
	}

	return Doppler_NoiseFloor_GO;
}


/* 2.1 remove the min value win  */
int32_t Doppler_CACFAR_new(int32_t idx, int32_t ROW, int32_t COL, int32_t CFAR_WINDOW, int32_t CFAR_GUARD, uint32_t *DopplerData, bool RemoveMax_Flag, bool RemoveMin_Flag)
{
	int32_t i = 0;
	int32_t index_left = 0;
	int32_t index_right = 0;
	int32_t i_temp = 0;
	uint32_t data_sum = 0;
	uint32_t value_max = 0,value_min = 65535;
	int32_t Doppler_NoiseFloor_CA = 0;
	int32_t iL_temp = 0, iR_temp = 0;
	int32_t use_left = 0, use_right = 0;

	for (i = 1; i <= CFAR_WINDOW + 3; i++)
	{
		if ( (iL_temp >= CFAR_WINDOW) && (iR_temp >= CFAR_WINDOW) )
		{
			break;
		}

		use_left = 0;
		use_right = 0;

		// index_left = idx - CFAR_GUARD - CFAR_WINDOW - 1 - i;
		index_left = idx - CFAR_GUARD - i;
		index_right = idx + CFAR_GUARD + i;

		if (index_left < 0)
		{
			index_left += COL;
		}
		if (index_right > (COL - 1))
		{
			index_right -= COL;
		}

		if (iL_temp < CFAR_WINDOW)
		{
			if (DopplerData[index_left] != 0)
			{
				if ( (index_left != 0) && (index_left != 1) && (index_left != (COL - 1)) )//add:remove the 1th,2nd and 31th dopplerBin
				{
					use_left = 1;//current left index is useful
					data_sum += DopplerData[index_left];
					iL_temp += 1;
				}
			}
		}
		if (iR_temp < CFAR_WINDOW)
		{
			if (DopplerData[index_right] != 0)
			{
				if ( (index_right != 0) && (index_right != 1) && (index_right != (COL - 1)) )//add:remove the 1th,2nd and 31th dopplerBin
				{
					use_right = 1;//current right index is useful
					data_sum += DopplerData[index_right];
					iR_temp += 1;
				}
			}
		}

		//the max value
		if ( (use_left == 1) && (DopplerData[index_left] > value_max) )
		{
			value_max = DopplerData[index_left];
		}
		if ( (use_right == 1) && (DopplerData[index_right] > value_max) )
		{
			value_max = DopplerData[index_right];
		}
		//the min value
		if ( (use_left == 1) && (DopplerData[index_left] < value_min) )
		{
			value_min = DopplerData[index_left];
		}
		if ( (use_right == 1) && (DopplerData[index_right] < value_min) )
		{
			value_min = DopplerData[index_right];
		}

	}

	i_temp = iL_temp + iR_temp;

	if (i_temp >= 1)
	{
		if ( (i_temp > 3) )
		{
			if ( (RemoveMax_Flag) && (value_max != 0) )
			{
			   data_sum -= value_max;
			   i_temp -= 1;
			}

		   if ( (RemoveMin_Flag) && (value_min != 0) )
		   {
			   data_sum -= value_min;
			   i_temp -= 1;
		   }

		}
		Doppler_NoiseFloor_CA = (int32_t)( data_sum / i_temp );
	}
	else
	{
		Doppler_NoiseFloor_CA = 0;
	}

	return Doppler_NoiseFloor_CA;
}


int32_t Doppler_GOCFAR_new(int32_t idx, int32_t ROW, int32_t COL, int32_t CFAR_WINDOW, int32_t CFAR_GUARD, uint32_t *DopplerData, bool RemoveMax_Flag, bool RemoveMin_Flag)
{
	int32_t i = 0;
	int32_t index_left = 0;
	int32_t index_right = 0;
	int32_t iR_temp = 0, iL_temp = 0;
	uint32_t dataR_sum = 0, dataL_sum = 0;
	uint32_t valueR_max = 0, valueL_max = 0;
	uint32_t valueR_min = 65535, valueL_min = 65535;
	int32_t Doppler_NoiseFloor_GO = 0;
	int32_t mean_R = 0, mean_L = 0;
	int32_t use_left = 0, use_right = 0;

	/* left and right window */
	for (i = 1; i <= CFAR_WINDOW + 3; i++)
	{
		if ( (iL_temp >= CFAR_WINDOW) && (iR_temp >= CFAR_WINDOW) )
		{
			break;
		}

		use_left = 0;
		use_right = 0;

		// index_left = idx - CFAR_GUARD - CFAR_WINDOW - 1 - i;
		index_left = idx - CFAR_GUARD - i;
		index_right = idx + CFAR_GUARD + i;

		if (index_left < 0)
		{
			index_left += COL;
		}
		if (index_right > (COL - 1))
		{
			index_right -= COL;
		}

		if (iL_temp < CFAR_WINDOW)
		{
			if (DopplerData[index_left] != 0)
			{
				if ( (index_left != 0) && (index_left != 1) && (index_left != (COL - 1)) )//add:remove the 1th,2nd and 31th dopplerBin
				{
					use_left = 1;//current left index is useful
					dataL_sum += DopplerData[index_left];
					iL_temp += 1;
				}
			}
		}

		if (iR_temp < CFAR_WINDOW)
		{
			if (DopplerData[index_right] != 0)
			{
				if ( (index_right != 0) && (index_right != 1) && (index_right != (COL - 1)) )//add:remove the 1th,2nd and 31th dopplerBin
				{
					use_right = 1;//current right index is useful
					dataR_sum += DopplerData[index_right];
					iR_temp += 1;
				}
			}
		}

		//the max value
		if ( (use_left == 1) && (DopplerData[index_left] > valueL_max) )
		{
			valueL_max = DopplerData[index_left];
		}
		if ( (use_right == 1) && (DopplerData[index_right] > valueR_max) )
		{
			valueR_max = DopplerData[index_right];
		}
		//the min value
		if ( (use_left == 1) && (DopplerData[index_left] < valueL_min) )
		{
			valueL_min = DopplerData[index_left];
		}
		if ( (use_right == 1) && (DopplerData[index_right] < valueR_min) )
		{
			valueR_min = DopplerData[index_right];
		}

	}

	/* mean(meanR,meanL) */
	if ((iL_temp >= 1) && (iR_temp >= 1))
	{
		// the left part
		if ( (iL_temp > 3) )
		{
			if ( (RemoveMax_Flag) && (valueL_max != 0) )
			{
				dataL_sum -= valueL_max;
				iL_temp -= 1;
			}

			if ( (RemoveMin_Flag) && (valueL_min != 0) )
			{
			   dataL_sum -= valueL_min;
			   iL_temp -= 1;
			}
		}
		mean_L = (int32_t)( dataL_sum / iL_temp );

		// the right part
		if ( (iR_temp > 3) )
		{
			if ( (RemoveMax_Flag) && (valueR_max != 0) )
			{
				dataR_sum -= valueR_max;
				iR_temp -= 1;
			}

			if ( (RemoveMin_Flag) && (valueR_min != 0) )
			{
			   dataR_sum -= valueR_min;
			   iR_temp -= 1;
			}
		}
		mean_R = (int32_t)( dataR_sum / iR_temp );

		if (mean_R > mean_L)
		{
			Doppler_NoiseFloor_GO = mean_R;
		}
		else
		{
			Doppler_NoiseFloor_GO = mean_L;
		}
	}
	else if ((iL_temp >= 1) && (iR_temp < 1))
	{
		// only the left part
		if ( (iL_temp > 3) )
		{
			if ( (RemoveMax_Flag) && (valueL_max != 0) )
			{
				dataL_sum -= valueL_max;
				iL_temp -= 1;
			}

			if ( (RemoveMin_Flag) && (valueL_min != 0) )
			{
			   dataL_sum -= valueL_min;
			   iL_temp -= 1;
			}
		}
		Doppler_NoiseFloor_GO = (int32_t)( dataL_sum / iL_temp );

	}
	else if ((iL_temp < 1) && (iR_temp >= 1))
	{
		// only the right part
		if ( (iR_temp > 3) )
		{
			if ( (RemoveMax_Flag) && (valueR_max != 0) )
			{
				dataR_sum -= valueR_max;
				iR_temp -= 1;
			}

			if ( (RemoveMin_Flag) && (valueR_min != 0) )
			{
			   dataR_sum -= valueR_min;
			   iR_temp -= 1;
			}
		}
		Doppler_NoiseFloor_GO = (int32_t)( dataR_sum / iR_temp );

	}
	else
	{
		Doppler_NoiseFloor_GO = 0;
	}

	return Doppler_NoiseFloor_GO;
}


//ret_code_t CfarDetection(uint32_t *cfarInput, uint32_t *detectBitmapInput, protocol_object_list_detected_t *detectList)
ret_code_t CfarDetection(uint32_t *cfarInput, uint32_t *detectBitmapInput, uint16_t *threshBuff,
		protocol_object_list_detected_t *detectList, data_path_data_t *pDataPathDataBuf)

{
	uint16_t usCfarTargetNo = 0;
    uint32_t rdmSize = gCfarParameter.rangeBins * gCfarParameter.dopplerBins;
    uint32_t inputDetectBitmapSize = rdmSize / PACKED_BITMAP_ELEMENT_SIZE;// The status of every 32 bins are saved by a uint32_t variable
    uint32_t rdmapMag = 0;
    int32_t uiPeakIndexinRdm = 0;
    int32_t i_range = 0, j_doppler = 0;
	int32_t Range_NoiseFloor = 0;
	int32_t Doppler_NoiseFloor = 0;
	int32_t RangeCfarThreshold = 0, RangeCfarThreshold_end = 0;
	int32_t DopplerCfarThreshold = 0, DopplerCfarThreshold_end = 0;
	int32_t GlobalCfarThreshold = 0, GlobalCfarThreshold_end = 0;
    int32_t peakIndex = 0;
	uint32_t aux = 0;
    int32_t i_BitmapNum = 0;
    int32_t i_index = 0, j_index = 0;
    int32_t startIndex_Doppler = 0;
    int32_t dopplerShift = 0;


    if ((cfarInput == NULL) || (detectBitmapInput == NULL) || (detectList == NULL))
    {
        return RET_INVALID_PARAM;
    }

    //gRangeBinCutIdx[5] = RANGE_CUT_INDEX_6TH_GEN3;

	for (i_BitmapNum = 0; i_BitmapNum < inputDetectBitmapSize; i_BitmapNum++)
	{
		i_index = i_BitmapNum;
		j_index = 0;
		aux = detectBitmapInput[i_index];
		// Decode input peaks
		while (aux)
		{
			// Check if current index is a detected peak
			if (aux & 0x1)
			{
				peakIndex = (i_index << PACKED_BITMAP_ELEMENT_SIZE_EXP) + j_index;
				j_doppler = (peakIndex >> DEFAULT_RANGE_BINS_EXP);
				startIndex_Doppler = (j_doppler << DEFAULT_RANGE_BINS_EXP);
				i_range = peakIndex - startIndex_Doppler;
				uiPeakIndexinRdm = (i_range << DEFAULT_DOPPLER_BINS_EXP) + j_doppler;
				rdmapMag = cfarInput[uiPeakIndexinRdm];

				/* 1.get CFAR threshold */
				Range_NoiseFloor = 0;
				Doppler_NoiseFloor = 0;

				RangeCfarThreshold = 0;
				DopplerCfarThreshold = 0;
				GlobalCfarThreshold = 0;

				RangeCfarThreshold_end = 0;
				DopplerCfarThreshold_end = 0;
				GlobalCfarThreshold_end = 0;

				if ((i_range >= gRangeBinCutIdx[0]) && (i_range <= gRangeBinCutIdx[1]))	/* [2,10]bin,Ç°2-RANGE_CUT_INDEX_FRONTbin CFARï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ */
				{
					RangeCfarThreshold = gRangeCfarThreshold[0];
					DopplerCfarThreshold = gDopplerCfarThreshold[0];
					GlobalCfarThreshold = gGlobalCfarThreshold[0];

					RangeCfarThreshold_end = gRangeCfarThreshold_end[0];
					DopplerCfarThreshold_end = gDopplerCfarThreshold_end[0];
					GlobalCfarThreshold_end = gGlobalCfarThreshold_end[0];

				}
				else if ((i_range > gRangeBinCutIdx[5]) && (i_range < gCfarParameter.rangeBins))	/* (430,end]bin,RANGE_CUT_INDEX_REARbinï¿½ï¿½ CFARï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ */
				{
					RangeCfarThreshold = gRangeCfarThreshold[5];
					DopplerCfarThreshold = gDopplerCfarThreshold[5];
					GlobalCfarThreshold = gGlobalCfarThreshold[5];

					RangeCfarThreshold_end = gRangeCfarThreshold_end[5];
					DopplerCfarThreshold_end = gDopplerCfarThreshold_end[5];
					GlobalCfarThreshold_end = gGlobalCfarThreshold_end[5];
				}
				else if ((i_range > gRangeBinCutIdx[1]) && (i_range <= gRangeBinCutIdx[2]))	/* (10,32]bin,RANGE_CUT_INDEX_REARbinï¿½ï¿½ CFARï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ */
				{
					RangeCfarThreshold = gRangeCfarThreshold[1];
					DopplerCfarThreshold = gDopplerCfarThreshold[1];
					GlobalCfarThreshold = gGlobalCfarThreshold[1];

					RangeCfarThreshold_end = gRangeCfarThreshold_end[1];
					DopplerCfarThreshold_end = gDopplerCfarThreshold_end[1];
					GlobalCfarThreshold_end = gGlobalCfarThreshold_end[1];
				}
				else if ((i_range > gRangeBinCutIdx[2]) && (i_range <= gRangeBinCutIdx[3]))	/* (32,54]bin,RANGE_CUT_INDEX_REARbinï¿½ï¿½ CFARï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ */
				{
					RangeCfarThreshold = gRangeCfarThreshold[2];
					DopplerCfarThreshold = gDopplerCfarThreshold[2];
					GlobalCfarThreshold = gGlobalCfarThreshold[2];

					RangeCfarThreshold_end = gRangeCfarThreshold_end[2];
					DopplerCfarThreshold_end = gDopplerCfarThreshold_end[2];
					GlobalCfarThreshold_end = gGlobalCfarThreshold_end[2];
				}
				else if ((i_range > gRangeBinCutIdx[3]) && (i_range <= gRangeBinCutIdx[4])) /* (54,72]bin */
				{
					RangeCfarThreshold = gRangeCfarThreshold[3];
					DopplerCfarThreshold = gDopplerCfarThreshold[3];
					GlobalCfarThreshold = gGlobalCfarThreshold[3];

					RangeCfarThreshold_end = gRangeCfarThreshold_end[3];
					DopplerCfarThreshold_end = gDopplerCfarThreshold_end[3];
					GlobalCfarThreshold_end = gGlobalCfarThreshold_end[3];
				}
				else if ((i_range > gRangeBinCutIdx[4]) && (i_range <= gRangeBinCutIdx[5]))  /* [73,430]bin */
				{
					RangeCfarThreshold = gRangeCfarThreshold[4];
					DopplerCfarThreshold = gDopplerCfarThreshold[4];
					GlobalCfarThreshold = gGlobalCfarThreshold[4];

					RangeCfarThreshold_end = gRangeCfarThreshold_end[4];
					DopplerCfarThreshold_end = gDopplerCfarThreshold_end[4];
					GlobalCfarThreshold_end = gGlobalCfarThreshold_end[4];
				}
				else
				{
					RangeCfarThreshold = gRangeCfarThreshold[2];
					DopplerCfarThreshold = gDopplerCfarThreshold[2];
					GlobalCfarThreshold = gGlobalCfarThreshold[2];

					RangeCfarThreshold_end = gRangeCfarThreshold_end[2];
					DopplerCfarThreshold_end = gDopplerCfarThreshold_end[2];
					GlobalCfarThreshold_end = gGlobalCfarThreshold_end[2];
				}
				

				/* 2.calculate noise */
				if  ((i_range > gRangeBinCutIdx[2]) && (i_range <= gRangeBinCutIdx[3])) //210-501m
				{
					Range_NoiseFloor = Range_CMLD_CFAR(i_range, gCfarParameter.rangeBins, gCfarParameter.dopplerBins,
													6, 2, (cfarInput + j_doppler), NUM_REMOVE_MAX_R,NUM_REMOVE_MIN_R);

					Doppler_NoiseFloor = Doppler_CACFAR_new(j_doppler, gCfarParameter.rangeBins, gCfarParameter.dopplerBins,
							gCfarParameter.dopplerWin, gCfarParameter.dopplerGuard, (cfarInput + i_range * gCfarParameter.dopplerBins), 1, gRemoveMinFlag);

				}				
				else if ((i_range > gRangeBinCutIdx[3]) && (i_range <= gRangeBinCutIdx[4])) //501-801m
				{
					Range_NoiseFloor = Range_CACFAR_new(i_range, gCfarParameter.rangeBins, gCfarParameter.dopplerBins,
								5, 3, (cfarInput + j_doppler), 1);

					Doppler_NoiseFloor = Doppler_GOCFAR_new(j_doppler, gCfarParameter.rangeBins, gCfarParameter.dopplerBins,
							gCfarParameter.dopplerWin, gCfarParameter.dopplerGuard, (cfarInput + i_range * gCfarParameter.dopplerBins), 1, gRemoveMinFlag);

				}
				else if ((i_range > gRangeBinCutIdx[4]) && (i_range <= gRangeBinCutIdx[5])) //801-1200m
				{
					Range_NoiseFloor = Range_CMLD_CFAR(i_range, gCfarParameter.rangeBins, gCfarParameter.dopplerBins,
													6, 2, (cfarInput + j_doppler), NUM_REMOVE_MAX_R,NUM_REMOVE_MIN_R);

					Doppler_NoiseFloor = Doppler_GOCFAR_new(j_doppler, gCfarParameter.rangeBins, gCfarParameter.dopplerBins,
							gCfarParameter.dopplerWin, gCfarParameter.dopplerGuard, (cfarInput + i_range * gCfarParameter.dopplerBins), 1, gRemoveMinFlag);

				}
				else
				{
					// range
					if (gRangeCfarMethod == 4)
					{
						Range_NoiseFloor = Range_OSCFAR(i_range, gCfarParameter.rangeBins, gCfarParameter.dopplerBins,
								gCfarParameter.rangeWin, gCfarParameter.rangeGuard, (cfarInput + j_doppler), gCfarParameter.osRangeK);
					}
					else if (gRangeCfarMethod == 3)
					{
						Range_NoiseFloor = Range_CMLD_CFAR(i_range, gCfarParameter.rangeBins, gCfarParameter.dopplerBins,
									gCfarParameter.rangeWin, gCfarParameter.rangeGuard, (cfarInput + j_doppler), NUM_REMOVE_MAX_R,NUM_REMOVE_MIN_R);
					}
					else if (gRangeCfarMethod == 2)
					{
						Range_NoiseFloor = Range_GOCFAR_new(i_range, gCfarParameter.rangeBins, gCfarParameter.dopplerBins,
								gCfarParameter.rangeWin, gCfarParameter.rangeGuard, (cfarInput + j_doppler), gRemoveMinFlag);
					}
					else
					{
						Range_NoiseFloor = Range_CACFAR_new(i_range, gCfarParameter.rangeBins, gCfarParameter.dopplerBins,
								gCfarParameter.rangeWin, gCfarParameter.rangeGuard, (cfarInput + j_doppler), gRemoveMinFlag);
					}

					//doppler
					if (gDopplerCfarMethod == 2)
					{
						Doppler_NoiseFloor = Doppler_GOCFAR_new(j_doppler, gCfarParameter.rangeBins, gCfarParameter.dopplerBins,
								gCfarParameter.dopplerWin, gCfarParameter.dopplerGuard, (cfarInput + i_range * gCfarParameter.dopplerBins), 1, gRemoveMinFlag);
					}
					else
					{
						Doppler_NoiseFloor = Doppler_CACFAR_new(j_doppler, gCfarParameter.rangeBins, gCfarParameter.dopplerBins,
								gCfarParameter.dopplerWin, gCfarParameter.dopplerGuard, (cfarInput + i_range * gCfarParameter.dopplerBins), 1, gRemoveMinFlag);
					}
				}



				/* 3.noise output exception handling = global noise*/
				//Range
				if (Range_NoiseFloor == 0)
				{
					Range_NoiseFloor = (threshBuff[i_range] - ((HIST_THRESHOLD_GUARD)<<RDM_MAG_PER_HIST_BIN_EXP));
				}
				//doppler
				if (Doppler_NoiseFloor == 0)
				{
					Doppler_NoiseFloor = (threshBuff[i_range] - ((HIST_THRESHOLD_GUARD)<<RDM_MAG_PER_HIST_BIN_EXP));
				}


				/* 4.detection output */
				if (j_doppler >= (gCfarParameter.dopplerBins / 2))
				{
					dopplerShift = j_doppler - gCfarParameter.dopplerBins + 0;
				}
				else
				{
					dopplerShift = j_doppler + 0;
				}

				if ( (rdmapMag > (uint32_t)(RangeCfarThreshold + Range_NoiseFloor)) && (rdmapMag < (uint32_t)(RangeCfarThreshold_end + Range_NoiseFloor))
					&& (rdmapMag > (uint32_t)(DopplerCfarThreshold + Doppler_NoiseFloor)) && (rdmapMag < (uint32_t)(DopplerCfarThreshold_end + Doppler_NoiseFloor))
					&& (rdmapMag > (uint32_t)(GlobalCfarThreshold + (threshBuff[i_range] - ((HIST_THRESHOLD_GUARD)<<RDM_MAG_PER_HIST_BIN_EXP))))
					&& (rdmapMag < (uint32_t)(GlobalCfarThreshold_end + (threshBuff[i_range] - ((HIST_THRESHOLD_GUARD)<<RDM_MAG_PER_HIST_BIN_EXP)))) ) //add global SNR Threshold, threshBuff has been increased by HIST_THRESHOLD_GUARD dB for peak search
				{
					detectList->detectPoint[usCfarTargetNo].id = usCfarTargetNo;
					detectList->detectPoint[usCfarTargetNo].range = (uint32_t)(i_range * gResolutionDetect.RangeResotion * MUL_ONE6_SCALE);	//i_range;
					detectList->detectPoint[usCfarTargetNo].velocity = (int16_t)(1 * dopplerShift * gResolutionDetect.VelocityResotion[detectList->waveType] * MUL_ONE6_SCALE);
					detectList->detectPoint[usCfarTargetNo].dopplerChn = (int16_t)j_doppler;
					detectList->detectPoint[usCfarTargetNo].mag = (uint16_t)(rdmapMag * gResolutionDetect.MagResotion * MUL_ONE6_SCALE); //rdmapMag;
					detectList->detectPoint[usCfarTargetNo].azimuth = (int16_t)((pDataPathDataBuf->sInfo.azimuth - 180) * MUL_ONE6_SCALE);
					detectList->detectPoint[usCfarTargetNo].elevation = (int16_t)((pDataPathDataBuf->sInfo.elevation - 180) * MUL_ONE6_SCALE);
					detectList->detectPoint[usCfarTargetNo].classification = (uint8_t)((rdmapMag - Range_NoiseFloor) * gResolutionDetect.MagResotion); //rangeSNR;
					detectList->detectPoint[usCfarTargetNo].cohesionOkFlag = (uint8_t)((rdmapMag - Doppler_NoiseFloor) * gResolutionDetect.MagResotion); //dopplerSNR;
					detectList->detectPoint[usCfarTargetNo].cohesionPntNum = (uint8_t)((rdmapMag - (threshBuff[i_range] - ((HIST_THRESHOLD_GUARD)<<RDM_MAG_PER_HIST_BIN_EXP))) * gResolutionDetect.MagResotion); //globalSNR;
					detectList->detectPoint[usCfarTargetNo].classifyProb = 0U;

					usCfarTargetNo ++;
				}

				if (usCfarTargetNo >= MAX_OUTPUT_CFAR_TARGET)
				{
					usCfarTargetNo = MAX_OUTPUT_CFAR_TARGET;
					break;
				}
			}
			// Go to the next bit in current element
			j_index += 1;
			aux >>= 1;
		}

		if (usCfarTargetNo >= MAX_OUTPUT_CFAR_TARGET)
		{
			usCfarTargetNo = MAX_OUTPUT_CFAR_TARGET;
			break;
		}
	}

	detectList->detectObjNum = usCfarTargetNo;

	// for (i_index = 0; i_index < usCfarTargetNo; i_index++)
	// {
	// 	printf("444444444-cfar-444444444 Num[ %d ], Id[ %d ], R[ %.5f ], V[ %.5f ], mag[ %.5f ], azimuth[ %.5f ], elevation[ %.5f ], rSNR[ %d ], dSNR[ %d ], gSNR[ %d ]\n", \
	// 		usCfarTargetNo, i_index, detectList->detectPoint[i_index].range*INV_ONE6_SCALE, detectList->detectPoint[i_index].velocity*INV_ONE6_SCALE, detectList->detectPoint[i_index].mag*INV_ONE6_SCALE, \
	// 		detectList->detectPoint[i_index].azimuth*INV_ONE6_SCALE, detectList->detectPoint[i_index].elevation*INV_ONE6_SCALE, detectList->detectPoint[i_index].classification, \
	// 		detectList->detectPoint[i_index].cohesionOkFlag, detectList->detectPoint[i_index].cohesionPntNum);
	// }

	return RET_OK;
}

ret_code_t calcAzimuth_heCha(protocol_object_list_detected_t *detectList, int16_t *sumSubData)
{
	uint8_t i_tar, i_L, useful;
	int32_t i_range = 0, i_doppler = 0;
	uint32_t CfarTargetNum = 0;//number of targets after CFAR
	uint32_t idx_sumI = 0, idx_sumQ = 0, idx_subI = 0, idx_subQ = 0;
	uint32_t len, Index;
	float sum_i,sum_q,sub_i,sub_q,sub_i_temp,sub_q_temp;
	float sum_square,sub_square;
	float ratio,K_slope,symbol,azi_diff;
	float aziBeam, eleBeam;
	float symbol_ZF = 1.f;
	float *table_heChaBi;
	float tan_halfPhase, sin_deltaTheta, deltaTheta;


	if ((sumSubData == NULL) || (detectList == NULL))
	{
		return RET_INVALID_PARAM;
	}

	CfarTargetNum = detectList->detectObjNum;

	for (i_tar = 0; i_tar < CfarTargetNum; i_tar++)
	{

	    i_range = (int32_t)( detectList->detectPoint[i_tar].range * INV_ONE6_SCALE / gResolutionDetect.RangeResotion );
	    i_doppler = detectList->detectPoint[i_tar].dopplerChn;
	    aziBeam = (float)( detectList->detectPoint[i_tar].azimuth * INV_ONE6_SCALE * DEG2RAD );
		eleBeam = (float)( detectList->detectPoint[i_tar].elevation * INV_ONE6_SCALE * DEG2RAD );

	    // get the IQ data of sub and sum channel
	    idx_subI = ( i_range << (DEFAULT_DOPPLER_BINS_EXP + DEFAULT_CHANNEL_NUM_EXP) ) + ( i_doppler << DEFAULT_CHANNEL_NUM_EXP ) + 0;
	    idx_subQ = ( i_range << (DEFAULT_DOPPLER_BINS_EXP + DEFAULT_CHANNEL_NUM_EXP) ) + ( i_doppler << DEFAULT_CHANNEL_NUM_EXP ) + 1;
	    idx_sumI = ( i_range << (DEFAULT_DOPPLER_BINS_EXP + DEFAULT_CHANNEL_NUM_EXP) ) + ( i_doppler << DEFAULT_CHANNEL_NUM_EXP ) + 2;
	    idx_sumQ = ( i_range << (DEFAULT_DOPPLER_BINS_EXP + DEFAULT_CHANNEL_NUM_EXP) ) + ( i_doppler << DEFAULT_CHANNEL_NUM_EXP ) + 3;

	    sum_i = (float)(sumSubData[idx_sumI]);
	    sum_q = (float)(sumSubData[idx_sumQ]);

	    sub_i_temp = (float)(sumSubData[idx_subI]);
	    sub_q_temp = (float)(sumSubData[idx_subQ]);

	    // sub channel compensate phase
//	    sub_i = sub_i_temp * cosf(THETA_INCRESS_SUB) - sub_q_temp * sinf(THETA_INCRESS_SUB);
//	    sub_q = sub_q_temp * cosf(THETA_INCRESS_SUB) + sub_i_temp * sinf(THETA_INCRESS_SUB);
	    sub_i = sub_i_temp;
	    sub_q = sub_q_temp;

	    // abs(sub)/abs(sum) = abs(sub/sum)
	    sum_square = sum_i * sum_i + sum_q * sum_q;
	    sub_square = sub_i * sub_i + sub_q * sub_q;
	    ratio = sqrtf ( sub_square / sum_square );

	    // compensate for angle offsets in other directions relative to the normal direction
	    // K_slope = cosf(aziBeam);
	    // ratio = ratio / K_slope;

	    // calculate the sign of the offset angle
	    if( (0U) == SYS_PHASE_POLE )
	    {
	        symbol = sub_i * sum_q - sub_q * sum_i;// phase_sub - phase_sum = -90 degree,the imaginary of (sum/sub)
	    }
	    else
	    {
	        symbol = sub_q * sum_i - sub_i * sum_q;//phase_sub - phase_sum = 90 degree,the imaginary of (sub/sum),now use
	    }

		if( symbol >= 0 )
		{
			symbol_ZF = 1.f;//symbol of step
		}
		else
		{
			symbol_ZF = -1.f;//symbol of step
		}

//	    // check the abs(sub)/abs(sum) table,add or reduce
//	    if( symbol >= 0 )
//	    {
//	        len = TABLE_LEN_ZHENG; //total number of elements in the table
//	        table_heChaBi = (float*)gTable_Zheng;
//	        symbol_ZF = 1.f;//symbol of step
//	    }
//	    else
//	    {
//	        len = TABLE_LEN_FU; //total number of elements in the table
//	        table_heChaBi = (float*)gTable_Fu;
//	        symbol_ZF = -1.f;//symbol of step
//	    }
//
//	    //************ (1)compare amplitude *************//
//	    Index = 0;
//	    useful = 1;
//	    if( ratio > table_heChaBi[len - 2] )
//	    {
//	    	Index = len - 1;
//	    	useful = 0;//exceed the range of table, output as a mark
//	    }
//	    else if ( ratio == table_heChaBi[len - 1] )
//	    {
//			Index = len - 1;
//	    }
//	    else
//	    {
//	        for (i_L = 0; i_L < len-1; i_L++)
//			{
//	            if( (ratio >= table_heChaBi[i_L]) && (ratio < table_heChaBi[i_L + 1]) )
//	            {
//	                Index = i_L;
//	                break;
//	            }
//			}
//	    }

	    // // check the abs(sub)/abs(sum) table,add or reduce
		// if( symbol >= 0 )
		// {
		// 	symbol_ZF = 1.f;//symbol of step

		// 	useful = 1;
		// 	if( ratio > RATIO_BOUNDARY_ZHENG )
		// 	{
		// 		azi_diff = 0;//==aziBeam
		// 		useful = 0;//exceed the range of table, output as a mark
		// 	}
		// 	else
		// 	{
		// 		azi_diff = K_RATIO_ZHENG * ratio + B_RATIO_ZHENG;
		// 	}
		// }
		// else
		// {
		// 	symbol_ZF = -1.f;//symbol of step

		// 	useful = 1;
		// 	if( ratio > RATIO_BOUNDARY_FU )
		// 	{
		// 		azi_diff = 0;//==aziBeam
		// 		useful = 0;//exceed the range of table, output as a mark
		// 	}
		// 	else
		// 	{
		// 		azi_diff = K_RATIO_FU * ratio + B_RATIO_FU;
		// 	}
		// }

//		//************ (1)compare amplitude *************//
//		azi_diff = 0;
//		useful = 1;
//		if( ratio > RATIO_BOUNDARY )
//		{
//			azi_diff = AZI_BOUNDARY;
//			useful = 0;//exceed the range of table, output as a mark
//		}
//		else if ( ratio == RATIO_BOUNDARY )
//		{
//			azi_diff = AZI_BOUNDARY;
//		}
//		else
//		{
//			azi_diff = K_RATIO * ratio + B_RATIO;
//		}

// 	    //Update azimuth and characteristic
//         detectList->detectPoint[i_tar].azimuth = (int16_t)( ( aziBeam * RAD2DEG + azi_diff ) * MUL_ONE6_SCALE  );//azimuth = aziBeam + deltaTheta,step = 0.05 degree
// //        detectList->detectPoint[i_tar].azimuth = (int16_t)( ( aziBeam * RAD2DEG + symbol_ZF * ANGLE_STEP * Index ) * MUL_ONE6_SCALE  );//azimuth = aziBeam + deltaTheta,step = 0.05 degree
// 	    if ( useful )
// 	    {
// 	        detectList->detectPoint[i_tar].reserve[0] = 1U;
// 	    }
// 	    else
// 	    {
// 	    	detectList->detectPoint[i_tar].reserve[0] = 0U;
// 	    }

	    //************ (2)compare phase *************//
		//abs(sub/sum)=tan(Phase/2),Phase = 2*pi*D/lambda*sin(deltaTheta)
		tan_halfPhase = ratio;// abs(sub)/abs(sum) = abs(sub/sum)
		sin_deltaTheta = DEFAULT_LAMBDA_M * atanf(tan_halfPhase) / ( M_PI * D_SUBARRAY * sqrtf( 1-cosf(eleBeam)*sinf(aziBeam)*cosf(eleBeam)*sinf(aziBeam) ) ); 
		deltaTheta = asinf(sin_deltaTheta);
		// if ( deltaTheta < -0.088 || deltaTheta > 0.088 ) //2* pi / 180 = 0.0349,5* pi / 180 = 0.0873
		// {
		// 	deltaTheta = 0.f; //exceeds threshold, output the aziBeam 
		// }

		detectList->detectPoint[i_tar].azimuth = (int16_t)( ( (aziBeam + symbol_ZF * deltaTheta) * RAD2DEG ) * MUL_ONE6_SCALE  );//azimuth = aziBeam + deltaTheta,degree
		
		// printf("555555555-calcAzi-555555555 R[ %.5f ], V[ %.5f ], ratio[ %.5f ], aziBeam[ %.5f ], eleBeam[ %.5f ], symbol_ZF[ %.5f ], deltaTheta_bef[ %.5f ], deltaTheta_aft[ %.5f ], azimuth[ %.5f ]\n", \
		// 			detectList->detectPoint[i_tar].range*INV_ONE6_SCALE, detectList->detectPoint[i_tar].velocity*INV_ONE6_SCALE, ratio, aziBeam*RAD2DEG, eleBeam*RAD2DEG, symbol_ZF, asinf(sin_deltaTheta)*RAD2DEG, deltaTheta*RAD2DEG,detectList->detectPoint[i_tar].azimuth*INV_ONE6_SCALE);

	}


}

static int GetFsmWorkMode( void )
{
	return 0;
}

ret_code_t DetectionAlgProcess(sDetectObjData_t *detectObjData, data_path_data_t *pDataPathDataBuf)
{
	struct timespec tCur1, tCur2, tCur3, tCur4, tCur5, tCur6, tCur7, tCurAll;
	struct timespec tEnd1, tEnd2, tEnd3, tEnd4, tEnd5, tEnd6, tEnd7, tEndAll;
	uint32_t tUsed1, tUsed2, tUsed3, tUsed4, tUsed5, tUsed6, tUsed7, tUsedAll;
	ret_code_t status = RET_OK;

	clock_gettime(CLOCK_MONOTONIC, &tCurAll);

	clock_gettime(CLOCK_MONOTONIC, &tCur1);
	GetRdmapData(detectObjData->rdmapData);
	clock_gettime(CLOCK_MONOTONIC, &tEnd1);
	tUsed1 = (tEnd1.tv_sec - tCur1.tv_sec) * 1000000 + (tEnd1.tv_nsec - tCur1.tv_nsec)/1000;

	clock_gettime(CLOCK_MONOTONIC, &tCur2);
	if (status == RET_OK)
	{
		status = CalcHistogram(detectObjData->rdmapData, detectObjData->histBuff);
	}
	clock_gettime(CLOCK_MONOTONIC, &tEnd2);
	tUsed2 = (tEnd2.tv_sec - tCur2.tv_sec) * 1000000 + (tEnd2.tv_nsec - tCur2.tv_nsec)/1000;

	clock_gettime(CLOCK_MONOTONIC, &tCur3);
	if (status == RET_OK)
	{
		status = CalcThreshold(detectObjData->histBuff, detectObjData->hististMaxIndex, detectObjData->threshold, gCfarParameter.rangeBins);
	}
	clock_gettime(CLOCK_MONOTONIC, &tEnd3);
	tUsed3 = (tEnd3.tv_sec - tCur3.tv_sec) * 1000000 + (tEnd3.tv_nsec - tCur3.tv_nsec)/1000;

	clock_gettime(CLOCK_MONOTONIC, &tCur4);
	if (status == RET_OK)
	{
		status = CalcTransposeRdm(detectObjData->rdmapData, detectObjData->rdmapTransposeData, gCfarParameter.rangeBins, gCfarParameter.dopplerBins);
	}
	clock_gettime(CLOCK_MONOTONIC, &tEnd4);
	tUsed4 = (tEnd4.tv_sec - tCur4.tv_sec) * 1000000 + (tEnd4.tv_nsec - tCur4.tv_nsec)/1000;

	clock_gettime(CLOCK_MONOTONIC, &tCur5);
	if (status == RET_OK)
	{
		status = CalcPeakSearchBitmap(detectObjData->rdmapData, detectObjData->rdmapTransposeData, detectObjData->threshold, detectObjData->peakBitmap, \
				detectObjData->rdmapDooplerDimPeakFlag, gCfarParameter.rangeBins, gCfarParameter.dopplerBins);
	}
	clock_gettime(CLOCK_MONOTONIC, &tEnd5);
	tUsed5 = (tEnd5.tv_sec - tCur5.tv_sec) * 1000000 + (tEnd5.tv_nsec - tCur5.tv_nsec)/1000;

	clock_gettime(CLOCK_MONOTONIC, &tCur6);
	if (status == RET_OK)
	{
		if (0 == GetFsmWorkMode())
		{
			status = CfarDetection(detectObjData->rdmapData, detectObjData->peakBitmap, detectObjData->threshold, detectObjData->detectList, pDataPathDataBuf);
            status = microDopplerDetect( detectObjData->rdmapData, detectObjData->detectList);
        }
		else if (1 == GetFsmWorkMode())
		{
//			status = CfarDetection(detectObjData->rdmapData, detectObjData->peakBitmap, detectObjData->threshold, detectObjData->detectList, pDataPathDataBuf);
		}
		else
		{
			/* do nothing */
		}

	}
	clock_gettime(CLOCK_MONOTONIC, &tEnd6);
	tUsed6 = (tEnd6.tv_sec - tCur6.tv_sec) * 1000000 + (tEnd6.tv_nsec - tCur6.tv_nsec)/1000;

	clock_gettime(CLOCK_MONOTONIC, &tCur7);
		if (status == RET_OK)
		{
			status = calcAzimuth_heCha(detectObjData->detectList,detectObjData->sumSubData);
		}
		clock_gettime(CLOCK_MONOTONIC, &tEnd7);
		tUsed7 = (tEnd7.tv_sec - tCur7.tv_sec) * 1000000 + (tEnd7.tv_nsec - tCur7.tv_nsec)/1000;


	clock_gettime(CLOCK_MONOTONIC, &tEndAll);
	tUsedAll = (tEndAll.tv_sec - tCurAll.tv_sec) * 1000000 + (tEndAll.tv_nsec - tCurAll.tv_nsec)/1000;
	tUsedAll = tUsed1 + tUsed2 + tUsed3 + tUsed4 + tUsed5 + tUsed6 + tUsed7;

//	g_tUsDetectProcUsed[0] = tUsed1;
	g_tUsDetectProcUsed[1] = tUsed2;
	g_tUsDetectProcUsed[2] = tUsed3;
	g_tUsDetectProcUsed[3] = tUsed4;
	g_tUsDetectProcUsed[4] = tUsed5;
	g_tUsDetectProcUsed[5] = tUsed6;
	g_tUsDetectProcUsed[6] = tUsed7;

	return status;
}


ret_code_t microDopplerDetect(uint32_t *cfarInput,protocol_object_list_detected_t *detectList)
{
  uint16_t inum = 0, inum2 = 0, i5 = 0, i6 = 0;
  uint32_t i1 = 0,id = 0;
  uint32_t cnum = 0;
  uint32_t tmpRangeBin = 0, tmpRangeBin2 = 0;
  int8_t   flagD = 0;
  uint32_t sumcfar=0,rdmean=0;
  if ((cfarInput == NULL) || (detectList == NULL))
  {
        return RET_INVALID_PARAM;
  }

  uint16_t usCfarTargetNo = detectList->detectObjNum;

//   protocol_object_list_detected_t *detectList2 = (protocol_object_list_detected_t*)malloc(sizeof (protocol_object_list_detected_t));
//   memcpy(detectList2,detectList,sizeof(protocol_object_list_detected_t));
//   memset(detectList2->detectPoint, 0, MAX_NUM_DETECTS * sizeof(protocol_object_item_detected_t));

  for (inum = 0; inum < usCfarTargetNo; inum++) 
  {
	  detectList->detectPoint[inum].classifyProb = 0U;
  }


  for (inum = 0; inum < usCfarTargetNo; inum++) 
  {	
	tmpRangeBin = (uint32_t)(detectList->detectPoint[inum].range / MUL_ONE6_SCALE / gResolutionDetect.RangeResotion);	
	
	//sumcfar = 0;
	//rdmean = 0;
	//    if (tmpRangeBin == 0) 
	//    {
	//         for (i1 = 0; i1 < DEFAULT_DOPPLER_BINS; i1++) 
	// 		{
	// 			rdmean = rdmean + cfarInput[ ((tmpRangeBin+1) << DEFAULT_DOPPLER_BINS_EXP) + i1 ] / DEFAULT_DOPPLER_BINS;
	// 		}
	//     }   
	//    else if (tmpRangeBin == DEFAULT_RANGE_BINS-1) 
	//    {

	// 		for (i1 = 0; i1 < DEFAULT_DOPPLER_BINS; i1++) 
	// 		{
	// 			rdmean = rdmean + cfarInput[ ((tmpRangeBin-1) << DEFAULT_DOPPLER_BINS_EXP) + i1 ] / DEFAULT_DOPPLER_BINS;
	// 		}
	//    }
	//    else //if (tmpRangeBin > 0 && tmpRangeBin < DEFAULT_RANGE_BINS-1)
	//    {
	// 	    for (i1 = 0; i1 < DEFAULT_DOPPLER_BINS; i1++) 
	// 				
	// 			sumcfar = sumcfar + cfarInput[ ((tmpRangeBin-1) << DEFAULT_DOPPLER_BINS_EXP) + i1 ] / DEFAULT_DOPPLER_BINS + cfarInput[ ((tmpRangeBin+1) << DEFAULT_DOPPLER_BINS_EXP) + i1 ] / DEFAULT_DOPPLER_BINS;
	// 	    }
	// 	    rdmean = sumcfar / 2;
	//     }
	   	   	
	//     for (id = 0; id < DEFAULT_DOPPLER_BINS; id++) 
	// 	{
	// 		if ( cfarInput[ (tmpRangeBin << DEFAULT_DOPPLER_BINS_EXP) + id ] > (rdmean + 150) )
	// 		{
	// 			cnum++;
	// 		}
	// 	}

	// 	if ( cnum > DEFAULT_DOPPLER_BINS * 0.25 ) 
	// 	{
	// 		flagD = 1;
	// 	} 
	// 	else 
	// 	{
	// 		flagD = 0;
	// 	}

	// only consider the targets in 600m
	if ( tmpRangeBin < 200 ) 
	{
		flagD = 1;
	} 
	else 
	{
		flagD = 0;
	}
		
	if (flagD == 1) 
	{
		for ( inum2 = 0; inum2 < usCfarTargetNo; inum2++ ) 
		{
			tmpRangeBin2 = (uint32_t)(detectList->detectPoint[inum2].range / MUL_ONE6_SCALE / gResolutionDetect.RangeResotion);	

			if ( (tmpRangeBin == tmpRangeBin2) && (detectList->detectPoint[inum].mag > detectList->detectPoint[inum2].mag) )
			{
				detectList->detectPoint[inum2].classifyProb = (uint8_t)(1 * MUL_ONE6_SCALE);//0 is target, 1 is micro doppler point
			}
		}
	}

 }

// 	i6 = 0;

// 	for ( i5 = 0; i5 < usCfarTargetNo; i5++ ) 
// 	{
// 		if ( detectList-> detectPoint[i5].range > 0) 
// 		{
			
// 			printf("before1===%d,%d\n",detectList->detectPoint[i5].range,detectList->detectPoint[i5].id );
// 			printf("before===%d\n",detectList2->detectPoint[i6].range);
// 			printf( "&detectList2->detectPoint[i6] = %p %d\n", &detectList2->detectPoint[i6], sizeof(protocol_object_item_detected_t) );
// 			printf( "detectList2->detectPoint[i6].id = %p\n", &(detectList2->detectPoint[i6].id) );
// 			memcpy( &detectList2->detectPoint[i6], &detectList->detectPoint[i5], sizeof(protocol_object_item_detected_t) );
// 			detectList2->detectPoint[i6].id = i6;//new id
// 			printf("after===%d,%d\n",detectList2->detectPoint[i6].range,detectList2->detectPoint[i6].id );
// 			i6++;

// 		}
// 	}

// 	printf( "detectList2->detectObjNum = %p\n", &(detectList2->detectObjNum) );
// 	detectList->detectObjNum = i6;

	
// 	 printf("i6==%d\n",i6);
// 	printf( "detectList = %p %d\n", detectList, sizeof(protocol_object_list_detected_t) );
// //	memcpy(detectList,detectList2,sizeof(protocol_object_list_detected_t));

// 	memcpy(detectList->detectPoint,detectList2->detectPoint,sizeof(detectList2->detectPoint));
// 	free(detectList2);
		
// 	printf( "psUpload.buffer = %p\n", psUploadUpload->pbyBuffer );

	return RET_OK;
	
}





/*================================================================================================*/
#ifdef __cplusplus
}
#endif

/*******************************************************************************
 * EOF
 ******************************************************************************/
