/**
 * Copyright (C), Autoroad Tech. Co., Ltd.
 * @brief   Realize wave configuration function
 * @file    wave_parameter.c
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
#include <unistd.h>
 #include <math.h>
#include "wave_parameter.h"
#include "../../hal/lmx2492_hal/lmx2492_ramp_config.h"
#include "../../hal/lmx2492_hal/lmx2492_ramp_config.h"
#include "../../hal/lmx2492_hal/lmx2492_config_register.h"
#include "../../hal/lmx2492_hal/lmx2492_divider_config.h"
#include "../../hal/lmx2492_hal/lmx2492_mux_pin_config.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define PHASE_DETECT_Frequency			      40              // 40MHz
#define FRANCTIONAL_DENOMIANTOR               16777216        // 2^24
#define RAMP_INCREMENT_MAX                    1073741824      // 2^30

#if SRC_RF_PLUS
	#define SINGLE_FRAC_24_07G_NUM_VALUE		  0x700000        // 24.07GHz
	#define SINGLE_FRAC_24_15G_NUM_VALUE		  0xBC0000        // 24.15GHz
	#define SINGLE_FRAC_24_25G_NUM_VALUE		  0xE40000        // 24.25GHz
#else
	#define SINGLE_FRAC_24_07G_NUM_VALUE		  0X9C0000        // 24.07GHz
	#define SINGLE_FRAC_24_15G_NUM_VALUE		  0XBC0000        // 24.15GHz
	#define SINGLE_FRAC_24_25G_NUM_VALUE		  0XE40000        // 24.25GHz
#endif

#ifndef XPAR_AXI2REG2492_0_BASEADDR
#define XPAR_AXI2REG2492_0_BASEADDR (0x50) // TODO: fixme
#endif /* XPAR_AXI2REG2492_0_BASEADDR */

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

/*******************************************************************************
 * Local Functions - Implementation
 ******************************************************************************/
static lmx2492_status_t waveform_configuration(lmx2492_ramp_index_t index, ramp_configuration_parameter_t *parameter)
{
    if (parameter == NULL)
    {
        return LMX2492TransStatusError;
    }
    lmx2492_status_t status;

    // Set rampx increment
    status = rampx_increment_value_setting(index, parameter->increment);
    // fastlock and cycle slip reduction for ramp x
    if (parameter->fastlock_flag)
    {
        status = rampx_fastlock_enable(index);
    }
    // Number of PFD clocks
    status = rampx_length_setting(index, parameter->length);
    // ramp flag set
    status = rampx_flag_setting(index, parameter->flag_index);
    // ramp reset set
    if (parameter->reset_flag)
    {
        status = rampx_reset_enable(index);
    }
    // Next ramp trigger
    status = rampx_next_trigger_setting(index, parameter->trigger_index);
    // Next ramp
    status = rampx_next_setting(index, parameter->next_ramp_index);

    return status;
    
}

/*******************************************************************************
 * API - Implementation
 ******************************************************************************/
uint16_t get_ramp_length(float duration)
{
	uint16_t len_max = (uint16_t)(65535 / PHASE_DETECT_Frequency);

	if (duration > len_max) {
        duration = len_max;
    }

    return (uint16_t)(duration * PHASE_DETECT_Frequency);
}


uint32_t get_ramp_increment(float diff_fre, uint16_t ramp_len)
{
    uint32_t increment;
    uint32_t frac_deno = FRANCTIONAL_DENOMIANTOR;
    float Fpd = PHASE_DETECT_Frequency;

    increment = (uint32_t)((fabs(diff_fre) / Fpd) / ramp_len * frac_deno);
    
    return (diff_fre < 0) ? (RAMP_INCREMENT_MAX - increment) : increment;

}

lmx2492_status_t set_ramp_limit_frequency(uint64_t limit_high, uint64_t limit_low)
{
    lmx2492_status_t statue; 

    statue = ramp_limit_high_setting(limit_high);
    statue = ramp_limit_low_setting(limit_low);

    return statue;
}

lmx2492_status_t set_vco_output_frequency(uint16_t R_divider, uint32_t N_divider, uint8_t cpg_value, uint32_t num_value, uint32_t den_value, uint8_t frac_order, uint8_t frac_dither)
{
    lmx2492_status_t statue; 

    // set PLL_R value
    statue = PLL_R_counter_divider_setting(R_divider);
    // set Fractional Modulator order
	statue = fractional_modulator_order_setting(frac_order);
	// set Fractional Modulator order
	statue = fractional_modulator_dither_setting(frac_dither);
    // set PLL_N value
    statue = PLL_N_counter_divider_setting(N_divider);
    // set charge pump gain value
    statue = charge_pump_gain_setting(cpg_value);
    // PLL_NUM
    statue = fractional_numerator_value_setting(num_value);
    // PLL_DEN 2^24
    statue = fractional_denominator_value_setting(den_value);

    return statue;
}


/* Single frequency point waveform configuration function */
lmx2492_status_t single_frequency_waveform_parameter(ramp_configuration_parameter_t *ramp_para, wave_fre_point_t fre)
{
    lmx2492_status_t status;

    reset_ramp_register_value();
    status = waveform_configuration(Ramp0Index, ramp_para);
    if (fre == FrequencyPoint24_15GHz)
    {
    	fractional_numerator_value_setting(SINGLE_FRAC_24_15G_NUM_VALUE);
    }
    else if(fre == FrequencyPoint24_25GHz)
    {
    	fractional_numerator_value_setting(SINGLE_FRAC_24_25G_NUM_VALUE);
    }
    else
    {
    	fractional_numerator_value_setting(SINGLE_FRAC_24_07G_NUM_VALUE);
    }

    status = LMX2492_write_all_register();
    usleep(2000);
    status = LMX2492_write_all_register();
    usleep(2000);

    return status;

}

/* Target waveform configuration function, Use four ramps, and trigger A source is MOD pin */
lmx2492_status_t target_waveform_parameter(uint16_t ramp_cnt, ramp_configuration_parameter_t *ramp0_para, ramp_configuration_parameter_t *ramp1_para,
											ramp_configuration_parameter_t *ramp2_para)
{
    lmx2492_status_t status;

#if SRC_RF_PLUS
    for( int i = 0; 2 > i; ++i )
    {
		LMX2492_set_single_register(0x8D, 0x00);
		LMX2492_set_single_register(0x8C, 0x0F);
		LMX2492_set_single_register(0x8B, 0xA0);
		LMX2492_set_single_register(0x8A, 0x00);
		LMX2492_set_single_register(0x89, 0x00);
		LMX2492_set_single_register(0x88, 0x00);
		LMX2492_set_single_register(0x87, 0x00);
		LMX2492_set_single_register(0x86, 0x00);
		LMX2492_set_single_register(0x85, 0x0F);
		LMX2492_set_single_register(0x84, 0xA0);
		LMX2492_set_single_register(0x83, 0x00);
		LMX2492_set_single_register(0x82, 0x00);
		LMX2492_set_single_register(0x81, 0x00);
		LMX2492_set_single_register(0x80, 0x00);
		LMX2492_set_single_register(0x7F, 0x00);
		LMX2492_set_single_register(0x7E, 0x0F);
		LMX2492_set_single_register(0x7D, 0xA0);
		LMX2492_set_single_register(0x7C, 0x00);
		LMX2492_set_single_register(0x7B, 0x00);
		LMX2492_set_single_register(0x7A, 0x00);
		LMX2492_set_single_register(0x79, 0x00);
		LMX2492_set_single_register(0x78, 0x00);
		LMX2492_set_single_register(0x77, 0x0F);
		LMX2492_set_single_register(0x76, 0xA0);
		LMX2492_set_single_register(0x75, 0x00);
		LMX2492_set_single_register(0x74, 0x00);
		LMX2492_set_single_register(0x73, 0x00);
		LMX2492_set_single_register(0x72, 0x00);
		LMX2492_set_single_register(0x71, 0x00);
		LMX2492_set_single_register(0x70, 0x0F);
		LMX2492_set_single_register(0x6F, 0xA0);
		LMX2492_set_single_register(0x6E, 0x00);
		LMX2492_set_single_register(0x6D, 0x00);
		LMX2492_set_single_register(0x6C, 0x00);
		LMX2492_set_single_register(0x6B, 0x00);
		LMX2492_set_single_register(0x6A, 0x00);
		LMX2492_set_single_register(0x69, 0x00);
		LMX2492_set_single_register(0x68, 0xD0);
		LMX2492_set_single_register(0x67, 0x3F);
		LMX2492_set_single_register(0x66, 0xFF);
		LMX2492_set_single_register(0x65, 0x86);
		LMX2492_set_single_register(0x64, 0xD7);
		LMX2492_set_single_register(0x63, 0x40);
		LMX2492_set_single_register(0x62, 0x27);
		LMX2492_set_single_register(0x61, 0xD0);
		LMX2492_set_single_register(0x60, 0x00);
		LMX2492_set_single_register(0x5F, 0x00);
		LMX2492_set_single_register(0x5E, 0x02);
		LMX2492_set_single_register(0x5D, 0x79);
		LMX2492_set_single_register(0x5C, 0x0C);
		LMX2492_set_single_register(0x5B, 0x31);
		LMX2492_set_single_register(0x5A, 0x38);
		LMX2492_set_single_register(0x59, 0x00);
		LMX2492_set_single_register(0x58, 0x00);
		LMX2492_set_single_register(0x57, 0x02);
		LMX2492_set_single_register(0x56, 0x82);
		LMX2492_set_single_register(0x55, 0x00);
		LMX2492_set_single_register(0x54, 0x20);
		LMX2492_set_single_register(0x53, 0x00);
		LMX2492_set_single_register(0x52, 0x02);
		LMX2492_set_single_register(0x51, 0x80);
		LMX2492_set_single_register(0x50, 0x00);
		LMX2492_set_single_register(0x4F, 0x00);
		LMX2492_set_single_register(0x4E, 0x00);
		LMX2492_set_single_register(0x4D, 0x00);
		LMX2492_set_single_register(0x4C, 0x00);
		LMX2492_set_single_register(0x4B, 0x00);
		LMX2492_set_single_register(0x4A, 0x00);
		LMX2492_set_single_register(0x49, 0x00);
		LMX2492_set_single_register(0x48, 0x00);
		LMX2492_set_single_register(0x47, 0x00);
		LMX2492_set_single_register(0x46, 0x00);
		LMX2492_set_single_register(0x45, 0x00);
		LMX2492_set_single_register(0x44, 0x00);
		LMX2492_set_single_register(0x43, 0x70);
		LMX2492_set_single_register(0x42, 0x00);
		LMX2492_set_single_register(0x41, 0x00);
		LMX2492_set_single_register(0x40, 0x00);
		LMX2492_set_single_register(0x3F, 0x00);
		LMX2492_set_single_register(0x3E, 0x70);
		LMX2492_set_single_register(0x3D, 0x00);
		LMX2492_set_single_register(0x3C, 0x00);
		LMX2492_set_single_register(0x3B, 0x00);
		LMX2492_set_single_register(0x3A, 0x31);
		LMX2492_set_single_register(0x39, 0x00);
		LMX2492_set_single_register(0x2D, 0x00);
		LMX2492_set_single_register(0x2C, 0x00);
		LMX2492_set_single_register(0x2B, 0x00);
		LMX2492_set_single_register(0x2A, 0x00);
		LMX2492_set_single_register(0x29, 0x00);
		LMX2492_set_single_register(0x28, 0x00);
		LMX2492_set_single_register(0x27, 0x3A);
		LMX2492_set_single_register(0x26, 0x1F);
		LMX2492_set_single_register(0x25, 0x10);
		LMX2492_set_single_register(0x24, 0x5A);
		LMX2492_set_single_register(0x23, 0x41);
		LMX2492_set_single_register(0x22, 0x80);
		LMX2492_set_single_register(0x21, 0x20);
		LMX2492_set_single_register(0x20, 0x00);
		LMX2492_set_single_register(0x1F, 0x32);
		LMX2492_set_single_register(0x1E, 0x0A);
		LMX2492_set_single_register(0x1D, 0x00);
		LMX2492_set_single_register(0x1C, 0x04);
		LMX2492_set_single_register(0x1B, 0x18);
		LMX2492_set_single_register(0x1A, 0x00);
		LMX2492_set_single_register(0x19, 0x01);
		LMX2492_set_single_register(0x18, 0x00);
		LMX2492_set_single_register(0x17, 0x00);
		LMX2492_set_single_register(0x16, 0x00);
		LMX2492_set_single_register(0x15, 0x70);
		LMX2492_set_single_register(0x14, 0x00);
		LMX2492_set_single_register(0x13, 0x00);
		LMX2492_set_single_register(0x12, 0x3C);
		LMX2492_set_single_register(0x11, 0x00);
		LMX2492_set_single_register(0x10, 0x96);
		LMX2492_set_single_register(0x0F, 0x00);
		LMX2492_set_single_register(0x0E, 0x00);
		LMX2492_set_single_register(0x0D, 0x00);
		LMX2492_set_single_register(0x0C, 0x00);
		LMX2492_set_single_register(0x0B, 0x00);
		LMX2492_set_single_register(0x0A, 0x00);
		LMX2492_set_single_register(0x09, 0x00);
		LMX2492_set_single_register(0x08, 0x00);
		LMX2492_set_single_register(0x07, 0x00);
		LMX2492_set_single_register(0x06, 0x00);
		LMX2492_set_single_register(0x05, 0x00);
		LMX2492_set_single_register(0x04, 0x00);
		LMX2492_set_single_register(0x03, 0x00);
		LMX2492_set_single_register(0x02, 0x01);
		LMX2492_set_single_register(0x01, 0x00);
		LMX2492_set_single_register(0x00, 0x18);


		usleep(2000);
	}
#else
    lmx2492_ramp_trigger_source_index_t trig_source_idx = MODRisingEdgeIndex;

    status = fractional_numerator_value_setting(SINGLE_FRAC_24_07G_NUM_VALUE);
    status = waveform_configuration(Ramp0Index, ramp0_para);
    status = waveform_configuration(Ramp1Index, ramp1_para);
    status = waveform_configuration(Ramp2Index, ramp2_para);
    status = ramp_counter_value_setting(ramp_cnt);

    triggera_source_setting(trig_source_idx);

    status = LMX2492_write_all_register();
    usleep(2000);
    status = LMX2492_write_all_register();
    usleep(2000);
#endif
    return status;

}
