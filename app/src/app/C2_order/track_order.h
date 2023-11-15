/**
 * Copyright (C), Autoroad Tech. Co., Ltd.
 * @brief   Realize tracing parameter setting function
 * @file    track_config.h
 * @author  X22012
 * @date    2023.01.13
 *
 * -History:
 *      -# author : X22012  
 *         date   : 2023.01.13
 *         Version: V1.0
 *         details: Created
 */


#ifndef TRACK_ORDER_H_
#define TRACK_ORDER_H_


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
/** @brief config single frequency waveform.
 *  @param [in]  eleCenter      Pitch center pointing.
 *  @param [in]  aziCenter      Horizontal center pointing.
 * @return @ref ret_code_t "Status return code." */
ret_code_t set_track_fix_beam(int8_t eleCenter, int8_t aziCenter);

/** @brief config single frequency waveform.
 *  @param [in]  eleCenter      Pitch center pointing.
 *  @param [in]  eleScope       Pitch sweep range.
 *  @param [in]  aziCenter      Horizontal center pointing.
 *  @param [in]  aziScope       Horizontal sweep range.
 * @return @ref ret_code_t "Status return code." */
ret_code_t set_track_scan_beam(int8_t eleCenter, uint8_t eleScope, int8_t aziCenter, uint8_t aziScope);

#endif /* TRACK_ORDER_H_ */
