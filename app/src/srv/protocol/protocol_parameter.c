
#include <arpa/inet.h>
#include "protocol_parameter.h"
///#include "../../app/beam_calculate/bk_cal.h"
#include "../log/log.h"
///#include "../flash_nv/flash_nv.h"
STATIC cfg_param_cb_fp g_cb_fp = NULL;

ret_code_t protocol_send_beam_scheduling(protocol_beam_scheduling_t *beam_scheduling)
{
	ret_code_t ret = RET_OK;

	if (beam_scheduling == NULL)
	{
		ret = RET_INVALID_PARAM;
		goto out;
	}

	ret = protocol_udp_send_data_blocking(beam_scheduling, sizeof(*beam_scheduling));

out:
//	APP_LOG_DEBUG("[%s:%d] ret=%d\r\n", __FUNCTION__, __LINE__, ret);
	return ret;
}

ret_code_t protocol_cfg_param_reg_cb(cfg_param_cb_fp cb_fp)
{
	ret_code_t ret = RET_OK;

	g_cb_fp = cb_fp;

	return ret;
}

ret_code_t protocol_cfg_param_process_data(void *data, uint32_t len)
{
	ret_code_t ret = RET_OK;
	protocol_cfg_param_t *cfgMsg = NULL;
	protocol_cfg_param_t cfgParam = {0};

	if ((data == NULL) || (len != sizeof(*cfgMsg)))
	{
		ret = RET_INVALID_PARAM;
		goto out;
	}

	if (g_cb_fp != NULL)
	{
		cfgMsg = (protocol_cfg_param_t *)data;
		// get the data needed from the message
		cfgParam.trSwitchCtrl = cfgMsg->trSwitchCtrl;
		cfgParam.workMode = cfgMsg->workMode;
		cfgParam.workWaveCode = cfgMsg->workWaveCode;
		cfgParam.workFrqCode = cfgMsg->workFrqCode;
		cfgParam.prfPeriod = cfgMsg->prfPeriod;
		cfgParam.accuNum = cfgMsg->accuNum;

		cfgParam.noiseCoef = ntohs(cfgMsg->noiseCoef) >> 7;
		cfgParam.clutterCoef = ntohs(cfgMsg->clutterCoef) >> 7;
		cfgParam.cfarCoef = ntohs(cfgMsg->cfarCoef) >> 7;
		cfgParam.focusRangeMin = ntohs(cfgMsg->focusRangeMin);
		cfgParam.focusRangeMax = ntohs(cfgMsg->focusRangeMax);
		cfgParam.clutterCurveNum = ntohs(cfgMsg->clutterCurveNum);
		cfgParam.lobeCompCoef = ntohs(cfgMsg->lobeCompCoef) >> 7;

		cfgParam.cohesionVelThre = cfgMsg->cohesionVelThre;
		cfgParam.cohesionRgnThre = cfgMsg->cohesionRgnThre;
		cfgParam.clutterMapSwitch = cfgMsg->clutterMapSwitch;
		cfgParam.clutterMapUpdateCoef = cfgMsg->clutterMapUpdateCoef;
		cfgParam.aziCalcSlope = cfgMsg->aziCalcSlope;
		cfgParam.aziCalcPhase = cfgMsg->aziCalcPhase;
		cfgParam.eleCalcSlope = cfgMsg->eleCalcSlope;
		cfgParam.eleCalcPhase = cfgMsg->eleCalcPhase;
		cfgParam.aziScanCenter = cfgMsg->aziScanCenter;
		cfgParam.aziScanScope = cfgMsg->aziScanScope;
		cfgParam.eleScanCenter = cfgMsg->eleScanCenter;
		cfgParam.eleScanScope = cfgMsg->eleScanScope;

		cfgParam.coherentDetectSwitch = ntohs(cfgMsg->coherentDetectSwitch);

		g_cb_fp(&cfgParam);
	}

out:
	return ret;
}
ret_code_t protocol_biaoding_data_process(void *data, uint32_t len , uint16_t infoType )
{
	#if 0 //FIXME: need it???
	ret_code_t ret = RET_OK;
	uint16_t *data_ptr = NULL;
	uint16_t l_Amp_Code[12];

	if ( data == NULL )
	{
		ret = RET_INVALID_PARAM;
		goto out;
	}

	if( infoType == AMP_CODE_PARAM )
	{
		if( len == sizeof(tx_com_Amp_Code)*3+TCP_INFO_HEAD_LEN + TCP_INFO_TAIL_LEN )
		{
			data_ptr = (uint16_t *)( (uint8_t *)data + TCP_INFO_HEAD_LEN );
			for( uint16_t i = 0x00 ; i < sizeof(l_Amp_Code)/2 ; i++ )
			{
				l_Amp_Code[i] = ntohs(data_ptr[i]) ;
			}
			memcpy( tx_com_Amp_Code , &l_Amp_Code[0] , sizeof(tx_com_Amp_Code) );
			memcpy( rx_u19_Amp_Code , &l_Amp_Code[4] , sizeof(rx_u19_Amp_Code) );
			memcpy( rx_u23_Amp_Code , &l_Amp_Code[8] , sizeof(rx_u23_Amp_Code) );
			FlashNV_Set( FLASH_NV_ID_AMP_CODE , l_Amp_Code, sizeof(l_Amp_Code) );
		}
		else
		{
			ret = RET_INVALID_PARAM;
			goto out;
		}

	}
	if(  infoType == PHASE_DATA_PARAM )
	{
		if( len == sizeof(INIT_PHASE_DATA)+TCP_INFO_HEAD_LEN + TCP_INFO_TAIL_LEN )
		{
			data_ptr = (uint16_t *)( (uint8_t *)data + TCP_INFO_HEAD_LEN );
			for( uint16_t i = 0x00 ; i < sizeof(INIT_PHASE_DATA)/2 ; i++ )
			{
				INIT_PHASE_DATA[i] = ntohs(data_ptr[i]) ;
			}
			FlashNV_Set( FLASH_NV_ID_PHASE_DATA , INIT_PHASE_DATA, sizeof(INIT_PHASE_DATA) );
		}
		else
		{
			ret = RET_INVALID_PARAM;
			goto out;
		}

	}
	if(  infoType == AMP_DATA_PARAM )
	{
		if( len == sizeof(INIT_AMP_DATA)+TCP_INFO_HEAD_LEN + TCP_INFO_TAIL_LEN )
		{
			data_ptr = (uint16_t *)( (uint8_t *)data + TCP_INFO_HEAD_LEN );
			for( uint16_t i = 0x00 ; i < sizeof(INIT_AMP_DATA)/2 ; i++ )
			{
				INIT_AMP_DATA[i] = ntohs(data_ptr[i]) ;
			}
			FlashNV_Set( FLASH_NV_ID_AMP_DATA , INIT_AMP_DATA, sizeof(INIT_AMP_DATA) );
		}
		else
		{
			ret = RET_INVALID_PARAM;
			goto out;
		}
	}

out:
	return ret;
	#endif

	return RET_OK;
}
