#ifndef DETECTION_INIT_H_
#define DETECTION_INIT_H_


/*==================================================================================================
*                                        INCLUDE FILES
==================================================================================================*/
#include "../../inc/radar_common.h"
#include "../../srv/protocol/protocol_if.h"
#include "../DataPath/data_path.h"

/***** Detection algorithm version  *****
det_01.110110
     | --------- main detection algorithm version number
	   | ------- version number of wave type
         | ------- version number of band width, adc sampling frequency, sampling time, fft window function ...
          | ------- version number of coherent / non - coherent accumulation, CFAR ...
           | ------- version number of range, velocity estimation ...
            | ------- version number of DOA
             | ------- version number of detection postprocess
*/
#define DETEC_ALG_VERSION "DET_1.104150"
#define DETEC_VERSION_STR_LEN (14)
/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/
#define DEFAULT_RANGE_BINS_EXP			9//11	// 2^11
#define DEFAULT_RANGE_BINS				(1 << DEFAULT_RANGE_BINS_EXP) 	// 2048=2^11
#define DEFAULT_DOPPLER_BINS_EXP		5	// 2^5
#define DEFAULT_DOPPLER_BINS			(1 << DEFAULT_DOPPLER_BINS_EXP)	// 32=2^5
#define DEFAULT_HIST_BINS_EXP			6	// 2^6
#define DEFAULT_HIST_BINS				(1 << DEFAULT_HIST_BINS_EXP)	// 64=2^6
#define PACKED_BITMAP_ELEMENT_SIZE_EXP	5	// 2^5
#define PACKED_BITMAP_ELEMENT_SIZE 		(1 << PACKED_BITMAP_ELEMENT_SIZE_EXP)	// 32=2^5
#define PACKED_BITMAP_NUM_PER_DOPPLER_EXP	(DEFAULT_RANGE_BINS_EXP - PACKED_BITMAP_ELEMENT_SIZE_EXP) // 2^(11-5)
#define PACKED_BITMAP_NUM_PER_DOPPLER 	(1 <<PACKED_BITMAP_NUM_PER_DOPPLER_EXP) // 64=2^(11-5)
#define PACKED_BITMAP_NUM_ALL_RDM	 	(DEFAULT_RANGE_BINS / PACKED_BITMAP_ELEMENT_SIZE * DEFAULT_DOPPLER_BINS)
#define DEFAULT_CHANNEL_NUM_EXP			2	// 2^2 = 4 = 4*1bit

/*=================================================================================================
*                                STRUCTURES AND OTHER TYPEDEFS
=================================================================================================*/
typedef struct sDetectObjData
{
	protocol_object_list_detected_t *detectList; // the output of signal process module
	uint32_t *rdmapData;
	uint32_t *rdmapTransposeData;
	uint16_t *histBuff;
	uint16_t *hististMaxIndex;
	uint16_t *threshold;
	uint32_t *peakBitmap;
	uint8_t *rdmapDooplerDimPeakFlag;
	int16_t *sumSubData;
}sDetectObjData_t;


/*==================================================================================================
*                                    FUNCTION PROTOTYPES
==================================================================================================*/

const char *GetDetectAlgVersion();
uint32_t GetFrameID(void);
void SetFrameID(uint32_t uiFrameID);
uint32_t GetPrcoTimeDetection(void);
void DataPathCB(data_path_data_t *pBuf);
int32_t DetectBufferAlloc(sDetectObjData_t *detectObj);
void DetectAlgInit(void);
int32_t runDetectAlgBlocking(data_path_data_t *pDataPathDataBuf);
int32_t runDetectAlgBlockingPil(data_path_data_t *pDataPathDataBuf);
const char *GetTrackAlgVersion();

#endif /*DETECTION_INIT_H_*/
