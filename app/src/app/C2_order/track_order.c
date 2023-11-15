/**
 * Copyright (C), Autoroad Tech. Co., Ltd.
 * @brief   Realize tracing parameter setting function
 * @file    track_config.c
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
#include "track_order.h"
#include "../../srv/protocol/protocol_parameter.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CHC2442_REGISTER_VALUE                0XEB23F0

/*******************************************************************************
 * Local Functions - Implementation
 ******************************************************************************/

/*******************************************************************************
 * API - Implementation
 ******************************************************************************/
ret_code_t set_track_fix_beam(int8_t eleCenter, int8_t aziCenter)
{
	ret_code_t ret = RET_OK;

	if (((eleCenter >= -20) && (eleCenter <= 20)) && ((aziCenter >= -60) && (aziCenter <= 60)))
	{

	}
	else
	{
		ret = RET_GENERAL_ERR;
	}

	return ret;
}

extern uint32_t gBeamMode ;
extern protocol_cfg_param_t gConfigParmInfo[1];
ret_code_t set_track_scan_beam(int8_t eleCenter, uint8_t eleScope, int8_t aziCenter, uint8_t aziScope)
{
	ret_code_t ret = RET_OK;

	if (((eleCenter >= -20) && (eleCenter <= 20)) && ((eleScope >= 0) && (eleScope <= 40)) && \
				((aziCenter >= -60) && (aziCenter <= 60)) && ((aziScope >= 0) && (aziScope <= 120)))
	{
		gBeamMode = 2;
		gConfigParmInfo[0].eleScanCenter = eleCenter;
		gConfigParmInfo[0].eleScanScope = eleScope;
		gConfigParmInfo[0].aziScanCenter = aziCenter;
		gConfigParmInfo[0].aziScanScope = aziScope;
	}
	else
	{
		gBeamMode = 1;
		ret = RET_GENERAL_ERR;
	}

	return ret;
}

