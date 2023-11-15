
#ifndef PROTOCOL_DBGDAT_H
#define PROTOCOL_DBGDAT_H

#include "protocol_common.h"

#define RAW_ADC_DATA_LEN_PER_FRAME (512 * 1024)
#define RAW_RDMAP_DATA_LEN_PER_FRAME (256 * 1024)

#define PROTOCOL_ADC_DATA_VER (0x00)

#pragma pack(1)
typedef struct protocol_adc_data {
	protocol_info_head_t stInfoHeader;

	uint8_t	 info[128];
	uint8_t  rawData[RAW_ADC_DATA_LEN_PER_FRAME];

	protocol_info_tail_t stInfoTail;
} protocol_adc_data_t;
#pragma pack()

#pragma pack(1)
typedef struct protocol_rdmap_data {
	protocol_info_head_t stInfoHeader;

	uint8_t	 info[128];
	uint8_t  rawData[RAW_RDMAP_DATA_LEN_PER_FRAME];

	protocol_info_tail_t stInfoTail;
} protocol_rdmap_data_t;
#pragma pack()

ret_code_t protocol_send_adc_data(protocol_adc_data_t *adc_data, send_data_cb_fp cb_fp);

ret_code_t protocol_send_rdmap_data(protocol_rdmap_data_t *rdmap_data, send_data_cb_fp cb_fp);

ret_code_t protocol_adcpkg_set_head(protocol_adc_data_t *adc_data, uint32_t frameId, uint32_t type, uint32_t length);

ret_code_t protocol_rdmappkg_set_head(protocol_rdmap_data_t *rdmap_data, uint32_t frameId, uint32_t type, uint32_t length);


#endif /* PROTOCOL_DBGDAT_H */
