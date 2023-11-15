/**
 * Copyright (C), Autoroad Tech. Co., Ltd.
 * @brief   Realize chip function configuration
 * @file    lmx2492_config_register.h
 * @author  X22012
 * @date    2022.11.01
 *
 * -History:
 *      -# author : X22012  
 *         date   : 2022.11.01
 *         Version: V1.0
 *         details: Created
 */

#ifndef LMX2492_CONFIG_REGISTER_CONFIG_H_
#define LMX2492_CONFIG_REGISTER_CONFIG_H_


/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "../../drv/lmx2492_drv/lmx2492_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define LMX2492_REGISTER0_ADDRESS         			0X00U
#define LMX2492_REGISTER0_MASK            			0X00U
#define LMX2492_REGISTER0_PORVALUE        			0X18U
#define LMX2492_INTERRUPT_CONTROL_REGISTER_MASK     0XFFF

#define LMX2492_MONITOR_REGISTER                    0X00
#define LMX2492_TRIG1_MUX_REGISTER         			0X24
#define LMX2492_TRIG2_MUX_REGISTER            		0X25
#define LMX2492_MOD_MUX_REGISTER        			0X26
#define LMX2492_MUXOUT_MUX_REGISTER     			0X27

#define LMX2492_REGISTER0_POR_VALUE                 0X18
#define LMX2492_MUX_DLD_DETECT         				0X5A
#define LMX2492_MUX_READBACK            			0X3A


/*******************************************************************************
 * API
 ******************************************************************************/
/** @addtogroup API
 * @{ */
/** @brief Reset ramp register */
void reset_ramp_register_value(void);

/** @brief Set the frequency of interrupt trigger.
 * @param [in]  data    Read/write frequency of interrupt trigger.
 * 						bit[11:8]:lmx2492_batch_rd(read), bit[7:0]:lmx2492_batch_wr (write)
 * @return @ref uint16_t value." */
lmx2492_status_t set_lmx2492_interrupt_trigger(uint16_t data);

/** @brief Read the value of register 0, which can be used to judge whether the chip works normally after power-on.
 * @return @ref lmx2492_status_t "Status return code." */
lmx2492_status_t LMX2492_register0_judging(void);

/** @brief Read the value of register.
 * @param [in]  reg      register.
 * @return @ref uint16_t value." */
uint8_t LMX2492_read_single_register(uint16_t reg);

/** @brief Set the value of register.
 * @param [in]  reg      register.
 * @param [in]  val      Value to be set.
 * @return @ref lmx2492_status_t "Status return code." */
lmx2492_status_t LMX2492_set_single_register(uint16_t reg, uint8_t val);

/** @brief Set IF LPF value.
 *
 * Write all registers 0f LMX2492.
 * @return @ref lmx2492_status_t "Status return code." */
lmx2492_status_t LMX2492_write_all_register(void);

#endif /* LMX2492_CONFIG_REGISTER_CONFIG_H_ */
