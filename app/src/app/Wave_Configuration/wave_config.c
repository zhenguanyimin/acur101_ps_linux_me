/**
 * Copyright (C), Autoroad Tech. Co., Ltd.
 * @brief   Realize wave configuration function
 * @file    wave_config.c
 * @author  X22012
 * @date    2022.11.15
 *
 * -History:
 *      -# author : X22012  
 *         date   : 2022.11.15
 *         Version: V1.0
 *         details: Created
 */


/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdbool.h>
#include "wave_config.h"
#include "../../hal/chc2442_hal/chc2442_config.h"
#include "../../hal/lmx2492_hal/lmx2492_ramp_config.h"
#include "../../hal/lmx2492_hal/lmx2492_power_config.h"
#include "../../hal/lmx2492_hal/lmx2492_config_register.h"
#include "../../hal/lmx2492_hal/lmx2492_divider_config.h"
#include "../../hal/lmx2492_hal/lmx2492_mux_pin_config.h"
#include "../../hal/lmx2492_hal/lmx2492_ld_cpm_config.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CHC2442_REGISTER_VALUE                0XEB23F0

/* OSC 40MHz */
#define RAMP_LIMIT_HIGH_VALUE                 0X1C00000       // 1550MHz
#define RAMP_LIMIT_LOW_VALUE                  0X0800000       // 1500MHz

#define RAMP_START_FREQUENCY                  1504.375        // 1504.375MHz

// Dividers and Fractional Controls
#if SRC_RF_PLUS
	#define PLL_N_DIVIDER                         0x96
	#define PLL_R_DIVIDER                         1U
	#define FRACTIONAL_MODUALATOR_ORDER			  3U
	#define FRACTIONAL_MODUALATOR_DITHER     	  3U
	#define TARGECT_FRACTIONAL_NUMERATOR          0x700000
	#define TARGECT_FRACTIONAL_DENOMINATOR        0x1000000
#else
	#define PLL_N_DIVIDER                         0x25
	#define PLL_R_DIVIDER                         1U
    #define FRACTIONAL_MODUALATOR_ORDER			  3U
	// #define FRACTIONAL_MODUALATOR_ORDER			  4U
	#define FRACTIONAL_MODUALATOR_DITHER     	  3U
	#define TARGECT_FRACTIONAL_NUMERATOR          0X9C0000
	#define TARGECT_FRACTIONAL_DENOMINATOR        0X1000000
#endif
//#define CHARGE_PUMP_PULSE_WIDTH		    	  3U			  // 1500ps
//#define CHARGE_PUMP_GAIN_VALUE                8U
#define CHARGE_PUMP_GAIN_VALUE                14U
// #define CHARGE_PUMP_GAIN_VALUE                4U

// Lock Detect and Charge Pump Monitoring
#define DIGITAL_LOCK_DETECT_FILTER_AMOUNT	  32U
#define DIGITAL_LOCK_DETECT_EDGE_WINDOW		  4U				// 10ns, (30MHz < Fpd > 45MHz)


// Single frequency point waveform
#define SINGLE_DESIRED_END_FREQUENCY          RAMP_START_FREQUENCY
#define SINGLE_RAMP_INCREMENT                 0
#define SINGLE_RAMP_COUNT                     0
#define SINGLE_RAMP_DURATION                  100             // 100us

// Target waveform for Anti-UVA
#define TARGECT_WAVEFORM_BANDWIDTH            62.2069         // Total bandwidth is 62.2069MHz, valid 50MHz
// #define RAMP0_DIFFERENT_FREQUENCY             0     
#define RAMP0_DIFFERENT_FREQUENCY             (4.80625)        
#define RAMP1_DIFFERENT_FREQUENCY             (3.88793125)    // 3.88793125MHz
#define RAMP2_DIFFERENT_FREQUENCY             (-3.88793125)   // -3.88793125MHz
#define RAMP3_DIFFERENT_FREQUENCY             0 

#define TARGECT_WAVEFORM_RAMP0_DURATION       315               // 5us
#define TARGECT_WAVEFORM_RAMP1_DURATION       254.8           // 254.8us
#define TARGECT_WAVEFORM_RAMP2_DURATION       5.2             // 5.2us

#define TARGECT_WAVEFORM_CHIRP_COUNT          0

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

/*******************************************************************************
 * Local Functions - Implementation
 ******************************************************************************/
static lmx2492_status_t set_ramp_limit_parameter(void)
{
	lmx2492_status_t status = LMX2492TransStatusOk;

    uint64_t ramp_limit_high = RAMP_LIMIT_HIGH_VALUE;
    uint64_t ramp_limit_low  = RAMP_LIMIT_LOW_VALUE;
    uint16_t PLL_R_divider = PLL_R_DIVIDER;
    uint32_t PLL_N_divider = PLL_N_DIVIDER;
    uint32_t PLL_num_val = TARGECT_FRACTIONAL_NUMERATOR;
    uint32_t PLL_den_val = TARGECT_FRACTIONAL_DENOMINATOR;
    uint8_t  charge_pump_gain = CHARGE_PUMP_GAIN_VALUE;
//    uint8_t  charge_pump_width = CHARGE_PUMP_PULSE_WIDTH;
    uint8_t frac_order = FRACTIONAL_MODUALATOR_ORDER;
    uint8_t frac_dither = FRACTIONAL_MODUALATOR_DITHER;
    uint8_t dld_pass_cnt = DIGITAL_LOCK_DETECT_FILTER_AMOUNT;
    uint8_t dld_tol = DIGITAL_LOCK_DETECT_EDGE_WINDOW;

    status = digital_lock_detect_filter_amount_setting(dld_pass_cnt);
    status = digital_lock_detect_edge_value_setting(dld_tol);
//    status = set_charge_pump_pulse_width_min(charge_pump_width);
    status = set_ramp_limit_frequency(ramp_limit_high, ramp_limit_low);
    status = set_vco_output_frequency(PLL_R_divider, PLL_N_divider, charge_pump_gain, PLL_num_val, PLL_den_val, frac_order, frac_dither);
    // To successfully configure the waveform, you must rewrite the R2 register
    LMX2492_power_up_setting();

    return status;
}

/*******************************************************************************
 * API - Implementation
 ******************************************************************************/
chc2442_status_t get_chc2442_value(uint32_t *value)
{
	chc2442_status_t status = chc2442TransStatusOk;

	status = CHC2442_reading(value);
	if (status != chc2442TransStatusOk)
	{
		status = chc2442TransStatusError;
	}

	return status;
}

void set_chc2442_value(uint32_t value)
{

	CHC2442_writing(value);
}

uint8_t get_single_2492_register(uint16_t regAddr)
{
	uint8_t val = 0;

    if (regAddr < 0 || regAddr > 142)
    {
    	return 0XFF;
    }

	val = LMX2492_read_single_register(regAddr);

	return val;
}

lmx2492_status_t set_single_2492_register(uint16_t regAddr, uint8_t value)
{
	lmx2492_status_t status = LMX2492TransStatusOk;

	status = LMX2492_set_single_register(regAddr, value);

	return status;

}


void VCOPLLInitFunction(void)
{
	uint32_t chc2442_val = CHC2442_REGISTER_VALUE;

	(void)set_chc2442_value(chc2442_val);
	(void)ramp_functions_enable();
	(void)set_pin_MUXout_readback();		// reg: 0x27, val: 0x3A
	(void)set_pin_trig1_DLD();				// reg: 0x24, val: 0x5A
	(void)set_pin_MOD_trig_source();		// reg: 0x26, val: 0x1F
	(void)set_ramp_limit_parameter();
	(void)auto_clear_ramp_en_enable();
	(void)set_lmx2492_interrupt_trigger(0x180);
//	LMX2492_write_all_register();

}

lmx2492_status_t set_pin_MOD_trig_source(void)
{
	lmx2492_status_t status = LMX2492TransStatusOk;

	lmx2492_mux_drive_pin_state_index_t drive_idx = InputIndex;
	lmx2492_mux_state_index_t state_idx = InputMODIndex;

	status = mod_drive_state_setting(drive_idx);
	status = mod_state_setting(state_idx);

    return status;
}


lmx2492_status_t set_pin_trig1_DLD(void)
{
	lmx2492_status_t status = LMX2492TransStatusOk;

	lmx2492_mux_drive_pin_state_index_t drive_idx = PullUpPullDownOutputIndex;
	lmx2492_mux_state_index_t state_idx = OutputDLDIndex;

	status = trig1_drive_state_setting(drive_idx);
	status = trig1_state_setting(state_idx);

    return status;
}

lmx2492_status_t set_pin_MUXout_readback(void)
{
	lmx2492_status_t status = LMX2492TransStatusOk;

	lmx2492_mux_drive_pin_state_index_t drive_idx = PullUpPullDownOutputIndex;
	lmx2492_mux_state_index_t state_idx = OutputInReadbackIndex;

	status = muxout_drive_state_setting(drive_idx);
	status = muxout_state_setting(state_idx);

	status = LMX2492_set_single_register(LMX2492_MUXOUT_MUX_REGISTER, LMX2492_MUX_READBACK);

    return status;
}

lmx2492_status_t single_frequency_waveform_configuration(wave_fre_point_t frePoint)
{
    lmx2492_status_t status;

    uint16_t ramp_duration = SINGLE_RAMP_DURATION;

    ramp_configuration_parameter_t ramp0_para = {
        .increment = SINGLE_RAMP_INCREMENT,
        .fastlock_flag = RampDelayDisable,
        .flag_index = RampFlagsClear,
        .reset_flag = RampResetEnable,
        .trigger_index = RampxLength,
        .next_ramp_index = Ramp0Index };

    ramp0_para.length = get_ramp_length(ramp_duration);

    status = single_frequency_waveform_parameter(&ramp0_para, frePoint);

    return status;

}

lmx2492_status_t target_waveform_configuration(bool isTrigFlag)
{
    
	lmx2492_status_t status = LMX2492TransStatusOk;

    uint16_t ramp_count = TARGECT_WAVEFORM_CHIRP_COUNT;
    float ramp0_duration = TARGECT_WAVEFORM_RAMP0_DURATION;
    float ramp1_duration = TARGECT_WAVEFORM_RAMP1_DURATION;
    float ramp2_duration = TARGECT_WAVEFORM_RAMP2_DURATION;
    float ramp0_diff_fre = RAMP0_DIFFERENT_FREQUENCY;
    float ramp1_diff_fre = RAMP1_DIFFERENT_FREQUENCY;
    float ramp2_diff_fre = RAMP2_DIFFERENT_FREQUENCY;

    ramp_configuration_parameter_t ramp0_parameter = {
        .fastlock_flag = RampDelayDisable,
        .flag_index = RampFlagsClear,
        // .reset_flag = RampResetDisable,              // ramp0 reset
        .reset_flag = RampResetEnable,              // ramp0 reset
        .trigger_index = RampxLength,
        .next_ramp_index = Ramp0Index };

    ramp_configuration_parameter_t ramp1_parameter = {
        .fastlock_flag = RampDelayDisable,
        .flag_index = RampFlagsClear,
        .reset_flag = RampResetEnable,
        .trigger_index = RampxLength,
        .next_ramp_index = Ramp0Index };

    ramp_configuration_parameter_t ramp2_parameter = {
        .fastlock_flag = RampDelayDisable,
        .flag_index = RampFlagsClear,
        .reset_flag = RampResetDisable,
        .trigger_index = RampxLength,
        .next_ramp_index = Ramp0Index };

//     Set MOD_pin to TriggerA
    if (isTrigFlag)
    {
 	   ramp0_parameter.trigger_index = TriggerA;
    }

    ramp0_parameter.length = get_ramp_length(ramp0_duration);
    ramp1_parameter.length = get_ramp_length(ramp1_duration);
    ramp2_parameter.length = get_ramp_length(ramp2_duration);
    ramp0_parameter.increment = get_ramp_increment(ramp0_diff_fre, ramp0_parameter.length);
    ramp1_parameter.increment = get_ramp_increment(ramp1_diff_fre, ramp1_parameter.length) + 1;
    ramp2_parameter.increment = get_ramp_increment(ramp2_diff_fre, ramp2_parameter.length) - 1;

    status = target_waveform_parameter(ramp_count, &ramp0_parameter, &ramp1_parameter, &ramp2_parameter);

    return status;
}


