
#ifndef PIL_H
#define PIL_H

#include "../../inc/radar_common.h"

#pragma pack(1)
	typedef struct pil_header
	{
		uint8_t 	byReserved;
		uint32_t	uDataSize;
		uint32_t	uDataCrc;
	} pil_header_t;
#pragma pack()

ret_code_t pil_set_open( uint8_t *pErrCode );

ret_code_t pil_set_close( uint8_t *pErrCode );

ret_code_t pil_start_transmit( pil_header_t *psHeader, uint8_t *pErrCode );

ret_code_t pil_transmit_data( uint8_t byStatus, uint32_t uOffset, uint8_t* pbyData, uint32_t uLength, uint8_t *pErrCode );

ret_code_t pil_inject_data( uint8_t *pErrCode );

uint8_t pil_read_status( void );

uint32_t pil_read_data_offset( void );

#endif
