
#include "alink_control.h"

#include "../../../../app/beam_calculate/biaoding_data_process.h"
#include "../../../../app/C2_order/wave_order.h"
#include "../../../../app/C2_order/track_order.h"
#include "../../../../app/DevInfoProcess/DevInfoProcess.h"

static void control_start_detect( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	int32_t eRet;
	alink_control_start_detect_ack_t *psStartDetectAck;

	psStartDetectAck = (alink_control_start_detect_ack_t*)(psResp->pbyBuffer);
	eRet = enable_wave_generation();
	if( 0 == eRet )
	{
		psStartDetectAck->status = 0;
		psResp->wCount = sizeof(alink_control_start_detect_ack_t);
	}
	else
	{
		psStartDetectAck->status = 1;
		psResp->wCount = sizeof(alink_control_start_detect_ack_t);
	}
}

static void control_close_detect( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	int32_t eRet;
	alink_control_close_detect_ack_t *psCloseDetectAck;

	psCloseDetectAck = (alink_control_close_detect_ack_t*)(psResp->pbyBuffer);
	eRet = disable_wave_generation();
	if( 0 == eRet )
	{
		psCloseDetectAck->status = 0;
		psResp->wCount = sizeof(alink_control_close_detect_ack_t);
	}
	else
	{
		psCloseDetectAck->status = 1;
		psResp->wCount = sizeof(alink_control_close_detect_ack_t);
	}
}

static void control_set_beam_scheduling_size( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	int32_t eRet;
	alink_control_set_beam_scheduling_size_t *psBeamSchedulingSize;
	alink_control_set_beam_scheduling_size_ack_t *psBeamSchedulingSizeAck;

	psBeamSchedulingSize = (alink_control_set_beam_scheduling_size_t*)(psPayload->pbyData);
	psBeamSchedulingSizeAck = (alink_control_set_beam_scheduling_size_ack_t*)(psResp->pbyBuffer);
	eRet = set_track_scan_beam( psBeamSchedulingSize->eleScanCenter, psBeamSchedulingSize->eleScanScope, psBeamSchedulingSize->aziScanCenter, psBeamSchedulingSize->aziScanScope );
	if( 0 == eRet )
	{
		psBeamSchedulingSizeAck->status = 0;
		psResp->wCount = sizeof(alink_control_set_beam_scheduling_size_ack_t);
	}
	else
	{
		psBeamSchedulingSizeAck->status = 1;
		psResp->wCount = sizeof(alink_control_set_beam_scheduling_size_ack_t);
	}
}

static void control_mag_calibrite( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	int32_t eRet;
	alink_control_mag_calibrite_ack_t *psMagCalibriteAck;

	psMagCalibriteAck = (alink_control_mag_calibrite_ack_t*)(psResp->pbyBuffer);
//	eRet = GetMagCaliData_rpc_m();
	if( 0 == eRet )
	{		
		psMagCalibriteAck->status = 0;
		psResp->wCount	= sizeof(alink_control_mag_calibrite_ack_t);
	}
	else
	{
		psMagCalibriteAck->status = 1;
		psResp->wCount	= sizeof(alink_control_mag_calibrite_ack_t);
	}
}

static void control_amp_code( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	int32_t eRet;
	alink_control_amp_code_t *psAmpCode;
	alink_control_amp_code_ack_t *psAmpCodeAck;

	psAmpCode = (alink_control_amp_code_t*)(psPayload->pbyData);
	psAmpCodeAck = (alink_control_amp_code_ack_t*)(psResp->pbyBuffer);
	eRet = amp_code_store( (uint16_t*)psAmpCode, sizeof(alink_control_amp_code_t) );
	if( 0 == eRet )
	{
		psAmpCodeAck->status = 0;
		psResp->wCount	= sizeof(alink_control_amp_code_ack_t);
	}
	else
	{
		psAmpCodeAck->status = 1;
		psResp->wCount	= sizeof(alink_control_amp_code_ack_t);
	}
}

static void control_amp_data( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	int32_t eRet;
	alink_control_amp_data_t *psAmpData;
	alink_control_amp_data_ack_t *psAmpDataAck;

	psAmpData = (alink_control_amp_data_t*)(psPayload->pbyData);
	psAmpDataAck = (alink_control_amp_data_ack_t*)(psResp->pbyBuffer);
	eRet = amp_data_store( (uint16_t*)psAmpData, sizeof(alink_control_amp_data_t) );
	printf( "[%s %d]\r\n", __func__, __LINE__ );
	for( int i = 0; sizeof(alink_control_amp_data_t) > i; ++i )
		printf( " %02X", ((uint8_t*)psAmpData)[i] );
	printf( "\r\n" );
	if( 0 == eRet )
	{
		psAmpDataAck->status = 0;
		psResp->wCount	= sizeof(alink_control_amp_data_ack_t);
	}
	else
	{
		psAmpDataAck->status = 1;
		psResp->wCount	= sizeof(alink_control_amp_data_ack_t);
	}
}

static void control_phase_data( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	int32_t eRet;
	alink_control_phase_data_t *psPhaseData;
	alink_control_phase_data_ack_t *psPhaseDataAck;

	psPhaseData = (alink_control_phase_data_t*)(psPayload->pbyData);
	psPhaseDataAck = (alink_control_phase_data_ack_t*)(psResp->pbyBuffer);
	eRet = phase_data_store( (uint16_t*)psPhaseData, sizeof(alink_control_phase_data_t) );
	printf( "[%s %d]\r\n", __func__, __LINE__ );
	for( int i = 0; sizeof(alink_control_phase_data_t) > i; ++i )
		printf( " %02X", ((uint8_t*)psPhaseData)[i] );
	printf( "\r\n" );
	if( 0 == eRet )
	{
		psPhaseDataAck->status = 0;
		psResp->wCount	= sizeof(alink_control_phase_data_ack_t);
	}
	else
	{
		psPhaseDataAck->status = 1;
		psResp->wCount	= sizeof(alink_control_phase_data_ack_t);
	}
}

static void control_gyro_calibrite( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	int32_t eRet;
	alink_control_gyro_calibrite_ack_t *psGyroCalibriteAck;

	psGyroCalibriteAck = (alink_control_gyro_calibrite_ack_t*)(psResp->pbyBuffer);
//	eRet = GetGyroCaliData_rpc_m();
	if( 0 == eRet )
	{
		psGyroCalibriteAck->status = 0;
		psResp->wCount	= sizeof(alink_control_gyro_calibrite_ack_t);
	}
	else
	{
		psGyroCalibriteAck->status = 1;
		psResp->wCount	= sizeof(alink_control_gyro_calibrite_ack_t);
	}
}

static void control_acc_calibrite( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	int32_t eRet;
	alink_control_acc_calibrite_t *psGyroCalibrite;
	alink_control_acc_calibrite_ack_t *psAccCalibriteAck;

	psGyroCalibrite = (alink_control_acc_calibrite_t*)(psPayload->pbyData);
	psAccCalibriteAck = (alink_control_acc_calibrite_ack_t*)(psResp->pbyBuffer);
//	eRet = GetAccCaliData_rpc_m( psGyroCalibrite->byPara );
	if( 0 == eRet )
	{
		psAccCalibriteAck->status = 0;
		psResp->wCount	= sizeof(alink_control_acc_calibrite_ack_t);
	}
	else
	{
		psAccCalibriteAck->status = 1;
		psResp->wCount	= sizeof(alink_control_acc_calibrite_ack_t);
	}
}

static void control_angle_calibrite( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	alink_control_angle_calibrite_ack_t *psAngleCalibriteAck;

	psAngleCalibriteAck = (alink_control_angle_calibrite_ack_t*)(psResp->pbyBuffer);

	Devinfo_cal_data();

	psAngleCalibriteAck->status = 0;
	psResp->wCount = sizeof(alink_control_angle_calibrite_ack_t);
}

static void control_manual_calibrite_data( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	alink_control_manual_calibrite_data_t *psManualCalibriteData;
	alink_control_manual_calibrite_data_ack_t *psManualCalibriteDataAck;

	psManualCalibriteData = (alink_control_manual_calibrite_data_t*)(psPayload->pbyData);
	psManualCalibriteDataAck = (alink_control_manual_calibrite_data_ack_t*)(psResp->pbyBuffer);

	plt_cali_data_rcv( psManualCalibriteData->iLongitude, psManualCalibriteData->iLatitude, psManualCalibriteData->iAltitude, 
							psManualCalibriteData->iHeading, psManualCalibriteData->iPitching, psManualCalibriteData->iRolling );

	psManualCalibriteDataAck->status = 0;
	psResp->wCount = sizeof(alink_control_manual_calibrite_data_ack_t);
}

uint8_t bFlagPlathfrom = false;
static void control_input_platform_data( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	alink_control_input_platform_data_t *psInputPlatformData;
	alink_control_input_platform_data_ack_t *psInputPlatformDataAck;

	psInputPlatformData = (alink_control_input_platform_data_t*)(psPayload->pbyData);
	psInputPlatformDataAck = (alink_control_input_platform_data_ack_t*)(psResp->pbyBuffer);

	bFlagPlathfrom = psInputPlatformData->cbStatus;
	plt_cali_data_rcv( psInputPlatformData->iLongitude, psInputPlatformData->iLatitude, psInputPlatformData->iAltitude, 
							psInputPlatformData->iHeading, psInputPlatformData->iPitching, psInputPlatformData->iRolling );

	psInputPlatformDataAck->status = 0;
	psResp->wCount = sizeof(alink_control_input_platform_data_ack_t);
}

static uint16_t control_pkg_auto_calibrite_data( uint8_t *pbyBuffer, uint16_t wSize, void *psCaliInfo )
{
	alink_control_auto_calibrite_data_t *psAutoCalibriteData;

	psAutoCalibriteData = (alink_control_auto_calibrite_data_t*)pbyBuffer;

	psAutoCalibriteData->iPitching	= ((int32_t*)psCaliInfo)[0];
	psAutoCalibriteData->iAltitude	= ((int32_t*)psCaliInfo)[1];
	psAutoCalibriteData->iHeading	= ((int32_t*)psCaliInfo)[2];
	psAutoCalibriteData->iLatitude	= ((int32_t*)psCaliInfo)[3];
	psAutoCalibriteData->iLongitude	= ((int32_t*)psCaliInfo)[4];
	psAutoCalibriteData->iRolling	= ((int32_t*)psCaliInfo)[5];

	return sizeof(alink_control_auto_calibrite_data_t);
}

static alink_cmd_list_t sAlinkCmd_StartDetect = { 0 };
static alink_cmd_list_t sAlinkCmd_CloseDetect = { 0 };
static alink_cmd_list_t sAlinkCmd_SetBeamSchedulingSize = { 0 };
static alink_cmd_list_t sAlinkCmd_MagCalibrite = { 0 };
static alink_cmd_list_t sAlinkCmd_AmpCode = { 0 };
static alink_cmd_list_t sAlinkCmd_AmpData = { 0 };
static alink_cmd_list_t sAlinkCmd_PhaseData = { 0 };
static alink_cmd_list_t sAlinkCmd_GyroCalibrite = { 0 };
static alink_cmd_list_t sAlinkCmd_AccCalibrite = { 0 };
static alink_cmd_list_t sAlinkCmd_AngleCalibrite = { 0 };
static alink_cmd_list_t sAlinkCmd_ManualCalibrite = { 0 };
static alink_cmd_list_t sAlinkCmd_InputPlatfrom = { 0 };

static alink_package_list_t sAlinkPackage_CalibriteData = { 0 };

void alink_control_init( void )
{
	alink_register_cmd( &sAlinkCmd_StartDetect, 			0xC1,	0, 													control_start_detect );
	alink_register_cmd( &sAlinkCmd_CloseDetect, 			0xC2,	0, 													control_close_detect );
	alink_register_cmd( &sAlinkCmd_SetBeamSchedulingSize, 	0xC4,	sizeof(alink_control_set_beam_scheduling_size_t),	control_set_beam_scheduling_size );
	alink_register_cmd( &sAlinkCmd_MagCalibrite, 			0xC5,	0, 													control_mag_calibrite );
	alink_register_cmd( &sAlinkCmd_AmpCode, 				0xC6,	sizeof(alink_control_amp_code_t), 					control_amp_code );
	alink_register_cmd( &sAlinkCmd_AmpData, 				0xC7,	sizeof(alink_control_amp_data_t), 					control_amp_data );
	alink_register_cmd( &sAlinkCmd_PhaseData, 				0xC8,	sizeof(alink_control_phase_data_t), 				control_phase_data );
	alink_register_cmd( &sAlinkCmd_GyroCalibrite, 			0xC9,	0, 													control_gyro_calibrite );
	alink_register_cmd( &sAlinkCmd_AccCalibrite, 			0xCA,	sizeof(alink_control_acc_calibrite_t), 				control_acc_calibrite );
	alink_register_cmd( &sAlinkCmd_AngleCalibrite, 			0xCB,	0, 													control_angle_calibrite );
	alink_register_cmd( &sAlinkCmd_ManualCalibrite, 		0xCD,	sizeof(alink_control_manual_calibrite_data_t), 		control_manual_calibrite_data );
	alink_register_cmd( &sAlinkCmd_InputPlatfrom, 			0xCF,	sizeof(alink_control_input_platform_data_t), 		control_input_platform_data );

	alink_register_package( &sAlinkPackage_CalibriteData,	0xCC,	0, 													control_pkg_auto_calibrite_data );
}

int32_t alink_upload_calibrite_data( void *psCaliInfo )
{
	return alink_upload_package( &sAlinkPackage_CalibriteData, psCaliInfo );
}

