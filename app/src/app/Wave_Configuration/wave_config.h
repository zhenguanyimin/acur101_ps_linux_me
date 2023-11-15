/**
 * Copyright (C), Autoroad Tech. Co., Ltd.
 * @brief   Realize wave configuration function
 * @file    wave_config.h
 * @author  X22012
 * @date    2022.11.15
 *
 * -History:
 *      -# author : X22012  
 *         date   : 2022.11.15
 *         Version: V1.0
 *         details: Created
 */
#ifndef WAVE_CONFIG_H_
#define WAVE_CONFIG_H_


/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "wave_parameter.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/
/** @addtogroup API
 * @{ */
///** @brief Init chc2442 value.
// * @return nothing */
void VCOPLLInitFunction(void);

/** @brief Set MUXout to readback pin.
 * @return @ref lmx2492_status_t "Status return code." */
lmx2492_status_t set_pin_MUXout_readback(void);

/** @brief Set TRIG1 to DLD pin.
 * @return @ref lmx2492_status_t "Status return code." */
lmx2492_status_t set_pin_trig1_DLD(void);

/** @brief Set MOD to trig source.
 * @return @ref lmx2492_status_t "Status return code." */
lmx2492_status_t set_pin_MOD_trig_source(void);

chc2442_status_t get_chc2442_value(uint32_t *value);
/** @brief config single frequency waveform.
 *  @param [in]  value      register value.
 * @return @ref uint8_t "Status return code." */
void set_chc2442_value(uint32_t value);

/** @brief config single frequency waveform.
 *  @param [in]  regAddr      register.
 * @return @ref uint8_t register value */
uint8_t get_single_2492_register(uint16_t regAddr);

/** @brief Set single PLL register.
 *  @param [in]  regAddr    register.
 *  @param [in]  value      register value.
 * @return @ref." */
lmx2492_status_t set_single_2492_register(uint16_t regAddr, uint8_t value);

///** @brief Set ramp high and low limit frequency.
// * @return @ref lmx2492_status_t "Status return code." */
//lmx2492_status_t set_ramp_limit_parameter(void);

/** @brief config single frequency waveform.
 *  @param [in]  ramp_dura      ramp duration(us).
 * @return @ref lmx2492_status_t "Status return code." */
lmx2492_status_t single_frequency_waveform_configuration(wave_fre_point_t frePoint);

 /** @brief Config target waveform.
  * @param [in]  idle_time      Idle time(us).
  * @param [in]  isTrigFlag     Trigger mode.
 * @return @ref lmx2492_status_t "Status return code." */
//lmx2492_status_t target_waveform_configuration(uint16_t idle_time, bool isTrigFlag);
lmx2492_status_t target_waveform_configuration(bool isTrigFlag);

#endif /* WAVE_CONFIG_H_ */

