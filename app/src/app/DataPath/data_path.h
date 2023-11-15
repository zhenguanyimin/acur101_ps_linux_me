
#ifndef __DATA_PATH_H__
#define __DATA_PATH_H__

#include <stdint.h>

#define DATA_PATH_PIL_STATUS_INFORM 1
#define DATA_PATH_PIL_STATUS_LOADED	2
#define DATA_PATH_PIL_STATUS_DONE	3

#ifndef s32
#define s32 int32_t
#endif

#include "../../srv/protocol/protocol_if.h"

extern protocol_object_list_detected_t gDetectList[1];
extern protocol_object_list_tracked_t gTrackList[1];
extern protocol_object_list_dot_cohe_t gDotCoheList[1];
extern protocol_beam_scheduling_t gBeamInfo[1];
extern protocol_cfg_param_t gConfigParmInfo[1];
extern protocol_radar_platfrom_info_t gPlatformInfo[1];
extern protocol_radar_status_t gRadarStatus[1];

enum 
{
	DATA_PATH_ADC_TO_UDP		= 1,
	DATA_PATH_RDMAP_TO_UDP		= 2,
	DATA_PATH_TCP_TO_ADC		= 3,
	DATA_PATH_LOCAL_TO_PSPL		= 4,
};

typedef enum dpath_data_tpye
{
	DATA_PATH_ADC_DATA = 1,
	DATA_PATH_RFFT_DATA = 2,
	DATA_PATH_DFFT_DATA = 3,
	DATA_PATH_RDMAP_DATA = 4,
	DATA_PATH_PS_PL_ADC_DATA = 5,
	DATA_PATH_DATA_TYPE_MAX,
} data_path_data_type;

#pragma pack(1)
	typedef struct data_path_info
	{
		uint32_t	frameID;
		uint32_t	waveType;
		uint64_t	timestamp;
		int16_t		azimuth;
		int16_t		elevation;
		int8_t		aziScanCenter;
		int8_t		aziScanScope;
		int8_t		eleScanCenter;
		int8_t		eleScanScope;
		int8_t		trackTwsTasFlag;
		int8_t		wholeSpaceScanCycleFinalBeamFlag;
		uint16_t	chirpTime;
	}data_path_info_t;
#pragma pack()

typedef struct
{
	uint32_t	adc_src_len;
	uint32_t	adc_iq_len;
	uint32_t	rdmap_len;
	uint32_t	info_len;
	void		*adc_src;
	void		*adc_iq;
	void		*rdmap;
	data_path_info_t sInfo;
	uint32_t	frameID;
	uint32_t	chirpTime;
}data_path_data_t;

typedef struct {
	void *pilinfodat;
	int pilinfodat_len;
	void *piladcdat;
	int piladcdat_len;
	uint32_t frameid;
}data_path_pil_data;


typedef void (*pil_status_cbk)(bool s);

int data_path_init( void );

extern s32 data_path_start(s32 type);

extern s32 data_path_stop(s32 type);

extern void data_path_out_en(s32 type);

int data_path_set_pil_data(data_path_pil_data *dppd);
int data_path_pil_uinit(void);
int data_path_pil_init(pil_status_cbk cbk);
int check_datapath_pil_status(void);
void simple_dcache_init(void);
void simple_dcache_uinit(void);
int simple_dcache_flush_range(unsigned long upaddr, unsigned long ulen);

#endif

