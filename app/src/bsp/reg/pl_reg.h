
#ifndef __PL_REG_H__
#define __PL_REG_H__

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/mman.h>
#include <assert.h>

#define PL_REG_PHY_ADDR (0x80020000)
#define PL_REG_REGION_SIZE (0x10000)

#define PL_WAVE_POSITION_ADDRESS	0x60000000
#define PL_WAVE_POSITION_UINT_SIZE (2*1024*1024)//2M
#define PL_REG_ENABLE  1
#define PL_REG_DISABLE 0
#define PL_WAVE_MODE_NORMAL 0
#define PL_WAVE_MODE_PIL	1

#define PL_REG_WAVE_POSITION_OFFSET 		0x103c
#define PL_REG_PIL_TRIGGER_OFFSET   		0x1048
#define PL_REG_ADC_MUX_S_OFFSET				0x1050
#define PL_REG_ADC_THRESHOLD_OFFSET 		0x1004
#define PL_REG_ADC_ABNORMAL_NUM_MAX			0x100c
#define PL_REG_PIL_PRI_DELAY_OFFSET 		0x104c
#define PL_REG_INFO_WAVETYPE_OFFSET			0x1058
#define PL_REG_INFO_TIMESTAMP_L_OFFSET		0x105c
#define PL_REG_INFO_TIMESTAMP_H_OFFSET		0x1060
#define PL_REG_INFO_AZIMUTH_OFFSET			0x1064
#define PL_REG_INFO_ELEVATION_OFFSET		0x1068
#define PL_REG_INFO_AZISCANCENTER_OFFSET	0x106c
#define PL_REG_INFO_AZISCANSCOPE_OFFSET		0x1070
#define PL_REG_INFO_ELESCANCENTER_OFFSET	0x1074
#define PL_REG_INFO_ELESCANSCOPE_OFFSET		0x1078
#define PL_REG_INFO_TRACKTWSTASFLAG_OFFSET	0x107C
#define PL_REG_INFO_LASTWAVE_OFFSET			0x1080

extern void pl_reg_init( void );

extern void pl_reg_deinit( void );

extern uint32_t *pl_reg_base;
#define PL_REG_WRITE(addr_offset, value) \
        do { \
				assert( (PL_REG_REGION_SIZE>addr_offset) );	\
                pl_reg_base[(addr_offset)/sizeof(uint32_t)] = value; \
                msync(&pl_reg_base[(addr_offset)/sizeof(uint32_t)], sizeof(uint32_t), MS_SYNC); \
        }while(0)
        
#define PL_REG_READ(addr_offset) (pl_reg_base[(addr_offset)/sizeof(uint32_t)])

#endif

