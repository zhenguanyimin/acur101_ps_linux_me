/**
 * Copyright (C), Autoroad Tech. Co., Ltd.
 * @brief   Realize chip function configuration
 * @file    lmx2492_config_register.c
 * @author  X22012
 * @date    2022.11.01
 *
 * -History:
 *      -# author : X22012  
 *         date   : 2022.11.01
 *         Version: V1.0
 *         details: Created
 */


/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "lmx2492_config_register.h"
#include "../../drv/lmx2492_drv/lmx2492_communication.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define LMX2492_REGISTER_START      16U   		/* Number of start register in the loop */
#define LMX2492_REGISTER_NUMBER     107U   		/* Number of valid register */

/*******************************************************************************
 * Local Variables
 ******************************************************************************/

/*******************************************************************************
 * API - Implementation
 ******************************************************************************/
lmx2492_status_t set_lmx2492_interrupt_trigger(uint16_t data)
{
	lmx2492_status_t status = LMX2492TransStatusOk;
	uint16_t val = data & LMX2492_INTERRUPT_CONTROL_REGISTER_MASK;
	if (val != data)
	{
		return LMX2492RegisterValueError;
	}

	LMX2492_read_write_interrupt_set(val);

	return status;
}

void reset_ramp_register_value(void)
{
	for (int i = 86; i < 142; i++)
	{
		lmx2492_register_data_array[i] = 0;
	}
}


/* register 0: readonly reg. */
uint8_t LMX2492_read_single_register(uint16_t reg)
{

    uint8_t rdata = 0;
    uint32_t data = 0;

    data = reg & LMX2492_REGISTER_COMMON_MASK;
    data = (data << 8) | 0x8000FF;

    LMX2492_read_register(data, &rdata);

    return rdata;

}


lmx2492_status_t LMX2492_set_single_register(uint16_t reg, uint8_t val)
{
	lmx2492_status_t status = LMX2492TransStatusOk;
	uint32_t data = 0;
	uint8_t read_val = 0;

    data = reg & LMX2492_REGISTER_COMMON_MASK;
    data = ((data << 8) | val) & 0XFFFFFF;
    LMX2492_write_single_register(data);
    /* Back read check */
    read_val = LMX2492_read_single_register(reg);
    if (read_val != val && data != 0X0204)
    {
    	status = LMX2492WriteRegisterError;
    }

    return status;
}

/* write all registers, A total of 73 registers are operated. */
lmx2492_status_t LMX2492_write_all_register(void)
{

    lmx2492_status_t status = LMX2492TransStatusOk;
    uint8_t readBakReg = LMX2492_MUXOUT_MUX_REGISTER;
    uint32_t data;
    uint16_t reg_idx;

    if (LMX2492_read_single_register(readBakReg) != LMX2492_MUX_READBACK)
    {
    	LMX2492_set_single_register(readBakReg, LMX2492_MUX_READBACK);
    }

//    for (reg_idx = LMX2492_REGISTER_START; reg_idx < LMX2492_REGISTER_NUMBER; reg_idx++)
    for (reg_idx = LMX2492_REGISTER_NUMBER - 1; reg_idx >= LMX2492_REGISTER_START; reg_idx--)
    {
    	if (reg_idx == 30 || reg_idx == 31 || reg_idx == 85 || (reg_idx > 39 && reg_idx < 58)) {
    		// Reserved registers
    		continue;
    	}

    	data = lmx2492_register_data_array[reg_idx];
    	status = LMX2492_set_single_register(reg_idx, data);
    	if (status != LMX2492TransStatusOk)
    	{
    		return status;
    	}
    }

    status = LMX2492_set_single_register(0X02, 0X01);	// power up
//    status = LMX2492_set_single_register(0X15, 0X9C);	// fractional numerator

    return status;

}

