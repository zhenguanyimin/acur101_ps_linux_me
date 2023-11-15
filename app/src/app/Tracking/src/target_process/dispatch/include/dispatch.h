#ifndef DISPATCH_H_
#define DISPATCH_H_

/*========================================================================================
*                                     INCLUDE FILES
=========================================================================================*/
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../../utilities/common_struct.h"
#include "../../../target_process/tracking/include/tracking.h"


/*========================================================================================
*                                      LOCAL MACROS
=========================================================================================*/
#define DEFAULT_AZI_FOV					120		// default fov angle of azimuth direction
#define DEFAULT_ELE_FOV				42//40		// default fov angle of elevation direction
#define DEFAULT_AZI_FOV_CENTER			0		// default fov center angle of azimuth direction
#define DEFAULT_ELE_FOV_CENTER			0		// default fov center angle of elevation direction
//#define DEFAULT_AZI_FOV_LOWER			-60		// default fov lower boundary angle of azimuth direction
#define DEFAULT_AZI_FOV_UPPER			(DEFAULT_AZI_FOV / 2)		// default fov upper boundary angle of azimuth direction
//#define DEFAULT_ELE_FOV_LOWER			-20		// default fov lower boundary angle of elevation direction
#define DEFAULT_ELE_FOV_UPPER			(DEFAULT_ELE_FOV / 2)		// default fov upper boundary angle of elevation direction
#define DEFAULT_AZI_BEAM_INTER			4		// default interval angle of each azimuth direction beam, in degree
#define DEFAULT_ELE_BEAM_INTER			6//10		// default interval angle of each elevation direction beam, in degree
#define DEFAULT_AZI_BEAM_NUM			(DEFAULT_AZI_FOV / DEFAULT_AZI_BEAM_INTER)		// default num of azimuth direction beam
#define DEFAULT_ELE_BEAM_NUM			(DEFAULT_ELE_FOV / DEFAULT_ELE_BEAM_INTER)		// default num of elevation direction beam
#define INT16_SCALE						32767	// 2^15-1
#define INVALID_TARGET_ID				0xFFFF	// invalid target id
#define INVALID_FIELD_DATA				0xFFFF	// invalid field data

#define DEFAULT_RANGE_UNIT_M			3		// default range unit, in m
#define DEFAULT_CPI_MS					8		// default CPI(coherent processing time), in ms, changed from 8 to 16 ms on 20221025
#define DEFAULT_TAS_CYCLE_MS			192		// default cycle of tas scan per target, in ms, changed from 192 to 384 ms on 20221102
#define DEFAULT_SAMPLE_PNT_START		200		// default start of sample point, uint = 0.2us, default is 40us/0.2us=200
#define DEFAULT_SAMPLE_PNT_DEPTH		4096	// depth of sample point, default 4096
#define DEFAULT_BEAM_SWITCH_TIME		150		// default time of beam switch, uint = 0.2us, default is 30us/0.2us=150
#define TAS_AZI_BEAM_NUM				2		// default azimuth direction beam number of tas scan, per target
#define TAS_ELE_BEAM_NUM				2		// default elevation direction beam number of tas scan, per target
#define TAS_CYCLE_TOTAL_BEAM_NUM		(TAS_AZI_BEAM_NUM * TAS_ELE_BEAM_NUM)		// default total beam number of tas scan, per target
#define TAS_CYCLE_BEAM_NUM				(DEFAULT_TAS_CYCLE_MS / DEFAULT_CPI_MS)				// beam number of tas scan cycle per target
#define TAS_TARGET_MAX_NUM				(TAS_CYCLE_BEAM_NUM / TAS_CYCLE_TOTAL_BEAM_NUM)	// max target number of tas scan
#define TAS_REAL_TARGET_MAX_NUM			(TAS_TARGET_MAX_NUM / 2)					// realized max target number of tas scan
#define INIT_CYCLE_TOTAL_BEAM_NUM		(1)		// default total beam number of init track scan, per target
#define INIT_REAL_TARGET_MAX_NUM		(TAS_CYCLE_BEAM_NUM / 8)					// realized max target number of init track scan
#define DEFAULT_TAS_ELE_OFFSET_DOWN		-2		// default down offset angle of tas target elevation angle, in degree
#define DEFAULT_TAS_ELE_OFFSET_UP		2		// default up offset angle of tas target elevation angle, in degree
#define DEFAULT_TAS_AZI_OFFSET_LEFT		-2		// default left offset angle of tas target azimuth angle, in degree
#define DEFAULT_TAS_AZI_OFFSET_RIGHT	2		// default right offset angle of tas target azimuth angle, in degree
#define TAS_ELE_OFFSET_LOWER_BOUND		1		// offset lower bound of tas target elevation angle, in degree
#define TAS_ELE_OFFSET_UPPER_BOUND		2		// offset upper bound of tas target elevation angle, in degree
#define TAS_AZI_OFFSET_LOWER_BOUND		1		// offset lower bound of tas target azimuth angle, in degree
#define TAS_AZI_OFFSET_UPPER_BOUND		2		// offset upper bound of tas target azimuth angle, in degree

#define FIX_BEAM_MODE					1		// mode of fix beam
#define SCAN_BEAM_MODE					2		// mode of scan beam

#define GRID_SEARCH_BEAM_INTER			2		// grid search beam interval angle is 2 degree
#define GRID_SEARCH_ROUND_NUM			6		// round num of whole space grid search
#define SCAN_MODE						1		// 1: gird search mode, 0: fix search mode

/*========================================================================================
*                       LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
=========================================================================================*/
typedef struct sDispatchTrackInfo {
	float timestamp;
	uint16_t id;				// track's uid
	uint16_t tid;				// track's tid
	uint16_t waveType;			// the type of wave, 0-A frame, 1-B frame
	float range;				// track's real range, in m
	float azimuth;				// track's real azimuth angle, in rad
	float elevation;			// track's real elevation angle, in rad
	float velocity;				// track's real radial velocity, in m/s
	float aziOffset[TAS_CYCLE_TOTAL_BEAM_NUM];
	float eleOffset[TAS_CYCLE_TOTAL_BEAM_NUM];
}sDispatchTrackInfo;

typedef struct sDispatchConfigParam {
	uint8_t valid;				// the flag of config param whether or not valid, 0-invalid, 1-valid
	uint8_t	workMode;			// the type of scan work mode, 0-tws, 1-tas
	int8_t aziScanCenter;		// the center of azimuth direction scan, -60~60(degree), default is 0 */
	int8_t aziScanScope;		// the fov scope of azimuth direction scan, typical value is 0,20,40,90,120, default is 120 */
	int8_t eleScanCenter;		// the center of elevation beam scan, -20~20(degree), default is 0 */
	int8_t eleScanScope;		// the fov scope of elevation direction scan, typical value is 0,10,20,40, default is 120 */
}sDispatchConfigParam;

typedef struct sDispatchTasTrackInfo {
	uint8_t tasDispatchFlag;	// tas dispatch process flag, 0:tas search, 1:tas track
	uint16_t trackTasNum;		// track number in one tas cycle
	uint16_t trackTasProcCnt;	// track processed counter in one tas cycle, each track has processed add 1, when add full then reset 0
	uint16_t tasScanBeamInCycleCnt;	// beam counter in one tas cycle, each beam add 1, when add full then reset 0
	sDispatchTrackInfo tasTrackCurrentCycle[TAS_REAL_TARGET_MAX_NUM];	// tas track info in current tas scan cycle
}sDispatchTasTrackInfo;

typedef struct sDispatchInitTrackInfo {
	uint8_t tasDispatchFlag;	// tas dispatch process flag, 0:tas search, 1:tas track
	uint16_t trackInitNum;		// track number in one tas cycle
	uint16_t trackTasProcCnt;	// track processed counter in one tas cycle, each track has processed add 1, when add full then reset 0
	uint16_t tasScanBeamInCycleCnt;	// beam counter in one tas cycle, each beam add 1, when add full then reset 0
	sDispatchTrackInfo tasTrackCurrentCycle[INIT_REAL_TARGET_MAX_NUM];	// tas track info in current tas scan cycle
}sDispatchInitTrackInfo;

typedef struct sDispatchInst {
	sDispatchConfigParam *dispatchConfigParam;
	sDispatchTasTrackInfo *dispatchTasTrackInfo;
	sDispatchInitTrackInfo *dispatchInitTrackInfo;
	sDispatchOutput *dispatchOutput;	// == curr frame output
	sDispatchOutput *dispatchOutputCurr;// == curr frame output
	sDispatchOutput *dispatchOutputNext;// == next frame output
}sDispatchInst;


/*========================================================================================
*                                    FUNCTIONS
=========================================================================================*/
void DispatchProcess(sAlgObjData *algObj);
void DispatchCfgParaUpdate(sDispatchConfigParam *param);
void DispatchTwsScan(sDispatchInst *dispatch_inst, sTrackingFrame *tracksListOutput);
void DispatchTasScan(sDispatchInst *dispatch_inst, sTrackingFrame *tracksListOutput, sTracking_moduleInstance *inst);
void GetTasTrackCurrentCycle(sDispatchInst *dispatch_inst, sTracking_moduleInstance *inst);
void GetInitTrackCurrentCycle(sDispatchInst *dispatch_inst, sTracking_moduleInstance *inst);
void TasTrackBeamProcAndOut(sDispatchInst *dispatch_inst, sTrackingFrame *tracksListOutput, sTracking_moduleInstance *inst);
void InitTrackBeamProcAndOut(sDispatchInst *dispatch_inst, sTrackingFrame *tracksListOutput, sTracking_moduleInstance *inst);
void TasScanNormalSearchBeamProcAndOut(sDispatchInst *dispatch_inst, sTrackingFrame *tracksListOutput);
void GetTasTrackOffsetAngle(float* statePredict, float* P_hat, sTracking_measurementSphUnion trackerPolar, float* aziOffset, float* eleOffset, sTracking_moduleInstance* inst);
// grid search
void DispatchCfgParaGridReset(void);
void DispatchCfgParaUpdateEachGrid(void);

int32_t GetTempTrackNum(sTracking_moduleInstance* inst);
void DispatchTwsOutputWaveType(sDispatchInst* dispatch_inst);
void DispatchTwsOutputFinalBeamFlag(sDispatchInst* dispatch_inst);
void TrackBeamProcAndOut(sDispatchInst* dispatch_inst, sTrackingFrame* tracksListOutput, sDispatchOutput dispatchOutputTmp);
#endif // !DISPATCH_H_
