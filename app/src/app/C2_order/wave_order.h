/**
 * Copyright (C), Autoroad Tech. Co., Ltd.
 * @brief   Realize sweep frequency wave configuration and data processing function
 * @file    wave_order.h
 * @author  X22012
 * @date    2023.01.13
 *
 * -History:
 *      -# author : X22012  
 *         date   : 2023.01.13
 *         Version: V1.0
 *         details: Created
 */


#ifndef WAVE_ORDER_H_
#define WAVE_ORDER_H_


/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "../../inc/radar_error.h"
#include "../../inc/radar_common.h"

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
/** @brief Enable waveform generation and data processing function.
 * @return @ref ret_code_t "Status return code." */
ret_code_t enable_wave_generation(void);

/** @brief Disable waveform generation and data processing function.
 * @return @ref ret_code_t "Status return code." */
ret_code_t disable_wave_generation(void);

#endif /* WAVE_ORDER_H_ */
