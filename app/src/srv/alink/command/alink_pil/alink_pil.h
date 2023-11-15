
#ifndef ALINK_PIL_H
#define ALINK_PIL_H

#include "../../alink.h"
#include "../../../../inc/radar_common.h"
#include "../../../../app/pil/pil.h"

#define HIL_ADC_DATA_MAX	(4096)

#pragma pack(1)
	typedef struct pil_open_ack
	{
		uint8_t byStatus;
	} pil_open_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct pil_close_ack
	{
		uint8_t byStatus;
	} pil_close_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct pil_req_transmit
	{
		pil_header_t sHeader;
	} pil_req_transmit_t;
	typedef struct pil_req_transmit_ack
	{
		uint8_t 	byStatus;
		uint16_t	wPkgMaxLen;
	} pil_req_transmit_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct pil_transmit_adc
	{
		uint8_t 	byStatus;
		uint16_t 	wLength;
		uint32_t 	uOffset;
		uint8_t		adcData[HIL_ADC_DATA_MAX];
	} pil_transmit_adc_t;
	typedef struct pil_transmit_adc_ack
	{
		uint8_t 	byStatus;
		uint32_t	uOffset;
	} pil_transmit_adc_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct pil_get_status_ack
	{
		uint8_t byStatus;
	} pil_get_status_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct pil_write_adc_ack
	{
		uint8_t byStatus;
	} pil_write_adc_ack_t;
#pragma pack()


void alink_pil_init( void );

#endif /* UPGRADE_H */
