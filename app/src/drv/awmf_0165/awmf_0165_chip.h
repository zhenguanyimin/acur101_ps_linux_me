
#ifndef AWMF_0165_CHIP_H
#define AWMF_0165_CHIP_H
#include "awmf_0165_def.h"


typedef struct
{
    volatile uint32_t TX_GROUP0					  ;//0x00000000
    volatile uint32_t TX_GROUP0_REG_DATA[8]       ;//0X00000004
    volatile uint32_t CTR_REG                     ;//0X00000024
    volatile uint32_t EN_REG                      ;//0X00000028
}AWMF_0165_U8_TYPEDEF;

#define	AWMF_0165_U8_REGDEF                ((AWMF_0165_U8_TYPEDEF *)0x80020400)	// U8


typedef struct
{
    volatile uint32_t TX_GROUP0					  ;//0x00000000
    volatile uint32_t TX_GROUP1                   ;//0X00000004
    volatile uint32_t RX_GROUP0                   ;//0X00000008
    volatile uint32_t RX_GROUP1                   ;//0X0000000c
    volatile uint32_t TX_GROUP0_REG_DATA[8]       ;//0X00000010
    volatile uint32_t TX_GROUP1_REG_DATA[8]       ;//0X00000030
    volatile uint32_t RX_GROUP0_REG_DATA[8]       ;//0X00000050
    volatile uint32_t RX_GROUP1_REG_DATA[8]       ;//0X00000070
    volatile uint32_t CTR_REG                     ;//0X00000090
    volatile uint32_t EN_REG                      ;//0X00000094
}AWMF_0165_TYPEDEF;

#define	AWMF_0165_REGDEF                ((AWMF_0165_TYPEDEF *)0x80020300)	// Chains


typedef struct awmf_0165_reg_init_data {
	uint32_t reg_addr;
	uint32_t data_high;
	uint32_t data_low;
} awmf_0165_reg_init_data_t;

typedef struct awmf_0165_dev_chip_data {
	uint32_t chip_id; // ref awmf_0165_chip_id_t
	uint32_t odd_parity_en; // whether the odd parity bit is set
	uint32_t addr_offset; // ref awmf_0165_reg_addr_t
	uint32_t data_high; // the highest 16 bits of data
	uint32_t data_low; // the lowest 32 bits of data
} awmf_0165_dev_chip_data_t;

typedef struct awmf_0165_dev_chain_data {
	uint32_t chain_id; // ref awmf_0165_chain_id_t
	uint32_t chip_cnt; // unused, since it's determined by the chain_id
	awmf_0165_dev_chip_data_t chip_data[AWMF_0165_MAX_CHIP_NUM_PER_CHAIN];
} awmf_0165_dev_chain_data_t;

typedef struct awmf_0165_dev_fast_par_data {
	uint32_t chain_id; // ref awmf_0165_chain_id_t
	uint32_t type; // 0-rx, 1-tx
	uint8_t tdbs_addr_b;
	uint8_t tdbs_addr_a;
	uint16_t fbs_addr_b;
	uint16_t fbs_addr_a;
} awmf_0165_dev_fast_par_data_t;

void awmf0165_get_temperature( float * temperature );
ret_code_t awmf_0165_drv_chip_serial_write(const awmf_0165_dev_chip_data_t *chip_data);
ret_code_t awmf_0165_drv_chip_reg_write(awmf_0165_chip_id_t chip_id, uint32_t addr_offset, uint32_t data_high, uint32_t data_low);
ret_code_t awmf_0165_drv_chain_serial_write(const awmf_0165_dev_chain_data_t *chain_data);
ret_code_t awmf_0165_drv_chain_fast_parallel_write(const awmf_0165_dev_fast_par_data_t *fast_par_data);
ret_code_t awmf_0165_drv_chip_serial_read(awmf_0165_dev_chip_data_t *chip_data);
ret_code_t awmf_0165_drv_chip_reg_read(awmf_0165_chip_id_t chip_id, uint32_t addr_offset, uint32_t *data_high, uint32_t *data_low);
ret_code_t awmf_0165_drv_chain_serial_read(awmf_0165_dev_chain_data_t *chain_data);
ret_code_t awmf_0165_drv_chip_init(awmf_0165_chip_id_t chip_id);
ret_code_t awmf_0165_drv_chip_reset(awmf_0165_chip_id_t chip_id);
ret_code_t awmf_0165_drv_chip_fbs_tdbs_select(awmf_0165_chip_id_t chip_id, awmf_0165_fbs_tdbs_location_t fbs_sel, awmf_0165_fbs_tdbs_location_t tdbs_sel);

void awmf0165_get_temperature( float * temperature );
void awmf0165_get_temperature_u8( float * temperature );

void awmf0165_get_power( float* power );
void awmf0165_get_power_u8( float* power );

#endif /* AWMF_0165_CHIP_H */

