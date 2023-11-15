/**
 * Copyright (C), Autoroad Tech. Co., Ltd.
 * @brief   Realize sweep frequency wave configuration and data processing function
 * @file    wave_order.c
 * @author  X22012
 * @date    2023.01.13
 *
 * -History:
 *      -# author : X22012  
 *         date   : 2023.01.13
 *         Version: V1.0
 *         details: Created
 */


/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <unistd.h>
#include "wave_order.h"
#include "../DataPath/data_path.h"
#include "../Wave_Configuration/wave_config.h"
#include "../../drv/awmf_0165/awmf_0165_drv.h"
#include "../../hal/lmx2492_hal/lmx2492_config_register.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Local Functions - Implementation
 ******************************************************************************/

/*******************************************************************************
 * API - Implementation
 ******************************************************************************/
 #include "../../bsp/reg/pl_reg.h"

#define WAVE_ORDER_WRITE_REG(addr,data)	PL_REG_WRITE(0xFFFF&addr,data)
#define WAVE_ORDER_READ_REG(addr)			PL_REG_READ(0xFFFF&addr)

extern volatile int32_t g_byDummyTest;
extern protocol_cfg_param_t gConfigParmInfo[1];
extern uint32_t gBeamMode;

extern ret_code_t awmf_get_chip_id_by_name(uint32_t awmf_name, uint32_t *awmf_id);

extern void set_beam_workmode(int32_t workMode);

#include "../../common/table_2d/table_2d.h"

ret_code_t enable_wave_generation(void)
{
	ret_code_t ret = RET_OK;

	printf( "[%s %d]\r\n", __func__, __LINE__ );

	//adc配置抽取模式
	WAVE_ORDER_WRITE_REG( 0x80020200 , 0x1b88 );			//WriteRegister 0x80020200 0x1b88
	WAVE_ORDER_WRITE_REG( 0x80020200 , 0x2406 );			//WriteRegister 0x80020200 0x2406
	WAVE_ORDER_WRITE_REG( 0x80020200 , 0x2518 );			//WriteRegister 0x80020200 0x2518
	
	// 脉冲触发，动态配置FFT 采样点数
	WAVE_ORDER_WRITE_REG( 0x8002204c , 0x01 );				//WriteRegister 0x8002204c 0x1
	WAVE_ORDER_WRITE_REG( 0x8002204c , 0x00 );				//WriteRegister 0x8002204c 0x0				 
	
	//扫频								  
	ret = target_waveform_configuration(true);		//TgtWaveCfg 1
	if (ret != RET_OK)
	{
		ret = RET_SYSTEM_ERR;
	}								  

	//将锁相环设置到24G附近
	ret = set_single_2492_register(0x26, 0x1F);		//LMXSglRegSet 0x26 0x1f
	if (ret != RET_OK)
	{
		ret = RET_SYSTEM_ERR;
	}
	ret = set_single_2492_register(0x27, 0x3a);		//LMXSglRegSet 0x27 0x3a
	if (ret != RET_OK)
	{
		ret = RET_SYSTEM_ERR;
	}

	struct table_2d* ps2492Cfg = file_to_table_2d( "/run/media/mtd_user/2492_cfg.txt" );

	if( ps2492Cfg )
	{
		if( 1 < ps2492Cfg->uRowNum )
		{
			for( uint32_t i = 0; ps2492Cfg->uLineNum > i; ++i )
			{
				set_single_2492_register( ps2492Cfg->aiTable[ps2492Cfg->uRowNum*i], ps2492Cfg->aiTable[ps2492Cfg->uRowNum*i + 1] );
			}
		}

		table_2d_free( ps2492Cfg );
	}

	//ADC数据采集
	data_path_out_en(2);
	ret = data_path_start(DATA_PATH_RDMAP_TO_UDP);
	
	//单波位
//	TrackFixBeamSet 0 0 
	//扫描范围全开					
	if( 0 == gConfigParmInfo[0].eleScanScope && 0 == gConfigParmInfo[0].aziScanScope )
	{
		gBeamMode = 2;
		gConfigParmInfo[0].eleScanCenter = 0;
		gConfigParmInfo[0].eleScanScope = 36;
		gConfigParmInfo[0].aziScanCenter = 0;
		gConfigParmInfo[0].aziScanScope = 88;
	}

	//发射功率
//	WAVE_ORDER_WRITE_REG(0x80020210, 0xeb23a7);
//	WAVE_ORDER_WRITE_REG(0x80020210, 0xeb23a7);

	struct table_2d* ps2442Cfg = file_to_table_2d( "/run/media/mtd_user/2442_cfg.txt" );

	if( ps2442Cfg )
	{
		if( 0 < ps2442Cfg->uLineNum && 0 < ps2442Cfg->uRowNum )
		{
			WAVE_ORDER_WRITE_REG( 0x80020210, ps2442Cfg->aiTable[0] );
			WAVE_ORDER_WRITE_REG( 0x80020210, ps2442Cfg->aiTable[0] );
		}

		table_2d_free( ps2442Cfg );
	}

	//配置U8增益
	uint32_t awmf_id;
	ret = awmf_get_chip_id_by_name(8, &awmf_id);
	ret = awmf_0165_drv_workmode_set(awmf_id, 1);
	usleep( 100 );
	ret = awmf_get_chip_id_by_name(8, &awmf_id);
	ret = awmf_0165_drv_reg_set(awmf_id, 0x22, 0x0555, 0x55000000);

	//发射全开
//	uint32_t awmf_id;
	ret = awmf_get_chip_id_by_name(9, &awmf_id);
	ret = awmf_0165_drv_workmode_set(awmf_id, 1);
	usleep( 100 );
	ret = awmf_get_chip_id_by_name(13, &awmf_id);
	ret = awmf_0165_drv_workmode_set(awmf_id, 1);				  
	usleep( 100 );
	 //接受全开
	 ret = awmf_get_chip_id_by_name(17, &awmf_id);
	ret = awmf_0165_drv_workmode_set(awmf_id, 2);
	usleep( 100 );
	ret = awmf_get_chip_id_by_name(21, &awmf_id);
	ret = awmf_0165_drv_workmode_set(awmf_id, 2);		
	usleep( 100 );

	struct table_2d* ps0165Tcompen = file_to_table_2d( "/run/media/mtd_user/0165_Tcompen.txt" );
	
	if( ps0165Tcompen )
	{
		if( 3 < ps0165Tcompen->uRowNum )
		{
			for( uint32_t i = 0; ps0165Tcompen->uLineNum > i; ++i )
			{
				ret = awmf_get_chip_id_by_name( ps0165Tcompen->aiTable[ps0165Tcompen->uRowNum*i], &awmf_id);
	        	ret = awmf_0165_drv_reg_set(awmf_id, ps0165Tcompen->aiTable[ps0165Tcompen->uRowNum*i + 1], ps0165Tcompen->aiTable[ps0165Tcompen->uRowNum*i + 2], ps0165Tcompen->aiTable[ps0165Tcompen->uRowNum*i + 3]);
			}
		}
	
		table_2d_free( ps0165Tcompen );
	}

#if 0
        //手动温补
//      uint32_t awmf_id = 0;
        //AwmfRegSet 17 0x05 0x0x00 0x00052d60
        ret = awmf_get_chip_id_by_name(9, &awmf_id);
        ret = awmf_0165_drv_reg_set(awmf_id, 0x05, 0x0000, 0x00050d60);
        //AwmfRegSet 18 0x05 0x0x00 0x00052d60
        ret = awmf_get_chip_id_by_name(10, &awmf_id);
        ret = awmf_0165_drv_reg_set(awmf_id, 0x05, 0x0000, 0x00050d60);
        //AwmfRegSet 19 0x05 0x0x00 0x00052d60
        ret = awmf_get_chip_id_by_name(11, &awmf_id);
        ret = awmf_0165_drv_reg_set(awmf_id, 0x05, 0x0000, 0x00050d60);
        //AwmfRegSet 20 0x05 0x0x00 0x00052d60
        ret = awmf_get_chip_id_by_name(22, &awmf_id);
        ret = awmf_0165_drv_reg_set(awmf_id, 0x05, 0x0000, 0x00050d60);
        //AwmfRegSet 21 0x05 0x0x00 0x00052d60
        ret = awmf_get_chip_id_by_name(23, &awmf_id);
        ret = awmf_0165_drv_reg_set(awmf_id, 0x05, 0x0000, 0x00050d60);
        //AwmfRegSet 22 0x05 0x0x00 0x00052d60
        ret = awmf_get_chip_id_by_name(24, &awmf_id);
        ret = awmf_0165_drv_reg_set(awmf_id, 0x05, 0x0000, 0x00050d60);
        //AwmfRegSet 23 0x05 0x0x00 0x00052d60
        ret = awmf_get_chip_id_by_name(25, &awmf_id);
        ret = awmf_0165_drv_reg_set(awmf_id, 0x05, 0x0000, 0x00050d60);
        //AwmfRegSet 24 0x05 0x0x00 0x00052d60
        ret = awmf_get_chip_id_by_name(26, &awmf_id);
        ret = awmf_0165_drv_reg_set(awmf_id, 0x05, 0x0000, 0x00050d60);

	//手动温补
//	uint32_t awmf_id = 0;
	//AwmfRegSet 17 0x05 0x0x00 0x00052d60
	ret = awmf_get_chip_id_by_name(17, &awmf_id);
	ret = awmf_0165_drv_reg_set(awmf_id, 0x05, 0x0000, 0x00052d60);
	//AwmfRegSet 18 0x05 0x0x00 0x00052d60
	ret = awmf_get_chip_id_by_name(18, &awmf_id);
	ret = awmf_0165_drv_reg_set(awmf_id, 0x05, 0x0000, 0x00052d60);
	//AwmfRegSet 19 0x05 0x0x00 0x00052d60
	ret = awmf_get_chip_id_by_name(19, &awmf_id);
	ret = awmf_0165_drv_reg_set(awmf_id, 0x05, 0x0000, 0x00052d60);
	//AwmfRegSet 20 0x05 0x0x00 0x00052d60
	ret = awmf_get_chip_id_by_name(20, &awmf_id);
	ret = awmf_0165_drv_reg_set(awmf_id, 0x05, 0x0000, 0x00052d60);
	//AwmfRegSet 21 0x05 0x0x00 0x00052d60
	ret = awmf_get_chip_id_by_name(21, &awmf_id);
	ret = awmf_0165_drv_reg_set(awmf_id, 0x05, 0x0000, 0x00052d60);
	//AwmfRegSet 22 0x05 0x0x00 0x00052d60
	ret = awmf_get_chip_id_by_name(22, &awmf_id);
	ret = awmf_0165_drv_reg_set(awmf_id, 0x05, 0x0000, 0x00052d60);
	//AwmfRegSet 23 0x05 0x0x00 0x00052d60
	ret = awmf_get_chip_id_by_name(23, &awmf_id);
	ret = awmf_0165_drv_reg_set(awmf_id, 0x05, 0x0000, 0x00052d60);
	//AwmfRegSet 24 0x05 0x0x00 0x00052d60
	ret = awmf_get_chip_id_by_name(24, &awmf_id);
	ret = awmf_0165_drv_reg_set(awmf_id, 0x05, 0x0000, 0x00052d60);	
#endif

	//5代5#雷达(中科)
//	WAVE_ORDER_WRITE_REG( 0x80022050, 0x09e7088B );
//	WAVE_ORDER_WRITE_REG( 0x80022054, 0x1000 );

	struct table_2d* psAdcCal = file_to_table_2d( "/run/media/mtd_user/adc_cal.txt" );

	if( psAdcCal )
	{
		if( 1 < psAdcCal->uRowNum )
		{
			for( uint32_t i = 0; psAdcCal->uLineNum > i; ++i )
			{
				WAVE_ORDER_WRITE_REG( psAdcCal->aiTable[psAdcCal->uRowNum*i], psAdcCal->aiTable[psAdcCal->uRowNum*i + 1] );
			}
		}

		table_2d_free( psAdcCal );
	}

	//tas模式
	set_beam_workmode( 1 );

	return ret;
}

ret_code_t disable_wave_generation(void)
{
	ret_code_t ret = RET_OK;

	data_path_out_en(5);

	ret = LMX2492_set_single_register(0X02, 0X04);

		//配置U8增益
	uint32_t awmf_id;
	ret = awmf_get_chip_id_by_name(8, &awmf_id);
	ret = awmf_0165_drv_workmode_set(awmf_id, 0);			  
	
	//发射全开
//	uint32_t awmf_id;
	ret = awmf_get_chip_id_by_name(9, &awmf_id);
	ret = awmf_0165_drv_workmode_set(awmf_id, 0);

	ret = awmf_get_chip_id_by_name(13, &awmf_id);
	ret = awmf_0165_drv_workmode_set(awmf_id, 0);				  

	//接受全开
	ret = awmf_get_chip_id_by_name(17, &awmf_id);
	ret = awmf_0165_drv_workmode_set(awmf_id, 0);
	ret = awmf_get_chip_id_by_name(21, &awmf_id);
	ret = awmf_0165_drv_workmode_set(awmf_id, 0);

	return ret;
}
