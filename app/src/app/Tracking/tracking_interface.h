#ifndef TRACKING_INTERFACE_H_
#define TRACKING_INTERFACE_H_


#include "src/utilities/common_struct.h"
#include "src/alg_init/alg_init.h"
#include "../../srv/protocol/protocol_if.h"


/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/
#define WAVE_TYPE_NUM				2		/* wave type number for solving velocity ambiguity */
#define WAVE_TYPE_0_CPI_DELAY		750		/* wave type 0, cip delay from ps to pl, unit = times of 40ns */
#define WAVE_TYPE_0_PRI_PERIOD		(5875 * 2)	/* wave type 0, pri period from ps to pl, unit = times of 40ns */	// fix 235us to 470us at 20221118
#define WAVE_TYPE_0_PRI_NUM			32		/* wave type 0, pri number from ps to pl, unit = 1 */
#define WAVE_TYPE_0_PRI_PULSE_WIDTH	50		/* wave type 0, pri pulse width from ps to pl, unit = times of 40ns */
#define WAVE_TYPE_0_START_SAMPLE	1000	/* wave type 0, start sample time from ps to pl, unit = times of 40ns */
#define WAVE_TYPE_0_SAMPLE_LENGTH	4125	/* wave type 0, sample length from ps to pl, unit = times of 40ns */
#define WAVE_TYPE_0_WAVE_CODE		0		/* wave type 0, wave code from ps to pl, unit = 1 */
#define WAVE_TYPE_1_CPI_DELAY		750		/* wave type 1, cip delay from ps to pl, unit = times of 40ns */
#define WAVE_TYPE_1_PRI_PERIOD		(6625 * 2)	/* wave type 1, pri period from ps to pl, unit = times of 40ns */	// fix 265us to 530us at 20221118
#define WAVE_TYPE_1_PRI_NUM			32		/* wave type 1, pri number from ps to pl, unit = 1 */
#define WAVE_TYPE_1_PRI_PULSE_WIDTH	50		/* wave type 1, pri pulse width from ps to pl, unit = times of 40ns */
#define WAVE_TYPE_1_START_SAMPLE	1000	/* wave type 1, start sample time from ps to pl, unit = times of 40ns */
#define WAVE_TYPE_1_SAMPLE_LENGTH	4125	/* wave type 1, sample length from ps to pl, unit = times of 40ns */
#define WAVE_TYPE_1_WAVE_CODE		1		/* wave type 1, wave code from ps to pl, unit = 1 */


/*=================================================================================================
*                                             ENUMS
=================================================================================================*/


/*=================================================================================================
*                                STRUCTURES AND OTHER TYPEDEFS
=================================================================================================*/
typedef struct sTimingCtrlPara
{
    uint16_t cpiDelay;					/* cip delay from ps to pl, unit = times of 40ns */
    uint16_t priPeriod;					/* pri period from ps to pl, unit = times of 40ns */
    uint8_t priNum;						/* pri number from ps to pl, unit = 1 */
    uint8_t priPulseWidth;				/* pri pulse width from ps to pl, unit = times of 40ns */
    uint16_t startSample;				/* start sample time from ps to pl, unit = times of 40ns */
    uint16_t sampleLength;				/* sample length from ps to pl, unit = times of 40ns */
    uint8_t waveCode;					/* wave code from ps to pl, unit = 1 */
}sTimingCtrlPara_t;


/*==================================================================================================
*                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/
extern int gAlgInitFlag;
extern sAlgObjData *gAlgData ;

/*==================================================================================================
*                                    FUNCTION PROTOTYPES
==================================================================================================*/
void runTrackingAlg();

void trackingAlgProcess(protocol_object_list_detected_t *detect_list, \
		protocol_object_list_tracked_t *track_list, \
		protocol_object_list_dot_cohe_t *dotCohe_list, \
		protocol_beam_scheduling_t *beam_scheduling, \
		protocol_radar_platfrom_info_t *platformInfo, \
		protocol_cfg_param_t *configParmInfo, \
		protocol_radar_status_t *radar_status);

void process_dummy3D(protocol_object_list_detected_t *detect_list, \
		protocol_object_list_tracked_t *track_list, \
		protocol_object_list_dot_cohe_t *dotCohe_list, \
		protocol_beam_scheduling_t *beam_scheduling, \
		protocol_radar_platfrom_info_t *platformInfo, \
		protocol_cfg_param_t *configParmInfo, \
		protocol_radar_status_t *radar_status);

void trackingReport(protocol_object_list_detected_t *detect_list, \
		protocol_object_list_tracked_t *track_list, \
		protocol_object_list_dot_cohe_t *dotCohe_list, \
		protocol_beam_scheduling_t *beam_scheduling, \
		protocol_radar_status_t *radar_status);

void trans_byte_order_obj_list_detected(protocol_object_list_detected_t *detect_list);
void trans_byte_order_obj_list_tracked(protocol_object_list_tracked_t *track_list);
void trans_byte_order_obj_list_dot_cohe(protocol_object_list_dot_cohe_t *dotCohe_list);
void trans_byte_order_beam_scheduling(protocol_beam_scheduling_t *beam_scheduling);
void trans_byte_order_radar_status(protocol_radar_status_t *radar_status);
void WaveTypeTimingCtrlPara_write_register(uint16_t waveType);
bool SetAbuDhabiShowScenes(int32_t scenes);
int32_t GetAbuDhabiShowScenes(void);
void SetTrackingMode(int32_t mode);
int32_t GetTrackingMode( void );
void SetDummyConfigParmInfo(protocol_radar_platfrom_info_t *platformInfo, protocol_cfg_param_t *configParmInfo);
uint32_t GetPrcoTimeTracking(void);
uint32_t GetPrcoTimeReport(void);
const char *GetTrackAlgVersion();

void set_beam_mode(uint32_t mode);
void set_beam_azi_scan_para(int32_t center, int32_t scope);
void set_beam_ele_scan_para(int32_t center, int32_t scope);
void set_fix_beam_para(int32_t eleCenter, int32_t aziCenter);
void set_scan_beam_para(int32_t eleCenter, int32_t eleScope, int32_t aziCenter, int32_t aziScope);
void set_beam_workmode(int32_t workMode);

#endif
