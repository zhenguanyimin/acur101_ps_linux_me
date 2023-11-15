#ifndef BIAODING_DATA_PROCESS_H
#define BIAODING_DATA_PROCESS_H

#include "bk_cal.h"
#include "../../inc/radar_common.h"

ret_code_t amp_code_store( uint16_t *data_ptr , uint16_t len );
ret_code_t amp_data_store( uint16_t *data_ptr , uint16_t len );
ret_code_t phase_data_store( uint16_t *data_ptr , uint16_t len );

ret_code_t get_amp_code_store( uint16_t *data_ptr, uint16_t *pwLen );
ret_code_t get_amp_data_store( uint16_t *data_ptr , uint16_t *pwLen );
ret_code_t get_phase_data_store( uint16_t *data_ptr , uint16_t *pwLen );

#endif
