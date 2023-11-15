#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "alink_pil.h"
#include "../../../version/version.h"

#define ID_PIL_OPEN				0x20
#define ID_PIL_CLOSE			0x24
#define ID_PIL_REQ_TRANSMIT		0x21
#define ID_PIL_TRANSMIT_ADC		0x22
#define ID_PIL_GET_STATUS		0x23
#define ID_PIL_WRITE_ADC		0x25

static alink_cmd_list_t sAlinkCmd_PilOpen = { 0 };
static alink_cmd_list_t sAlinkCmd_PilClose = { 0 };
static alink_cmd_list_t sAlinkCmd_PilRequestTransmit = { 0 };
static alink_cmd_list_t sAlinkCmd_PilTransmitAdc = { 0 };
static alink_cmd_list_t sAlinkCmd_PilGetStatus = { 0 };
static alink_cmd_list_t sAlinkCmd_PilWrtieAdc = { 0 };

static void pil_open( alink_payload_t *msg, alink_resp_t *resp )
{
	uint8_t errCode;
	pil_open_ack_t *psOpenAck = NULL;

	psOpenAck = (pil_open_ack_t*)(resp->pbyBuffer);
	if( RET_OK == pil_set_open(&errCode) )
	{
		switch( errCode )
		{
			case 0:
				psOpenAck->byStatus = 0;
				resp->wCount = sizeof(pil_open_ack_t);
				break;
			case 1:
				psOpenAck->byStatus = 1;
				resp->wCount = sizeof(pil_open_ack_t);
				break;
			case 2:
				psOpenAck->byStatus = 2;
				resp->wCount = sizeof(pil_open_ack_t);
				break;
			case 3:
				psOpenAck->byStatus = 3;
				resp->wCount = sizeof(pil_open_ack_t);
				break;
			default:
				psOpenAck->byStatus = 1;
				resp->wCount = sizeof(pil_open_ack_t);
				break;
		}
	}
	else
	{
		psOpenAck->byStatus = 1;
		resp->wCount = sizeof(pil_open_ack_t);
	}
	printf("%s > errCode %d\n", __FUNCTION__, errCode);
}

static void pil_close( alink_payload_t *msg, alink_resp_t *resp )
{
	uint8_t errCode;
	pil_close_ack_t *psOpenAck = NULL;

	psOpenAck = (pil_close_ack_t*)(resp->pbyBuffer);
	if( RET_OK == pil_set_close(&errCode) )
	{
		switch( errCode )
		{
			case 0:
				psOpenAck->byStatus = 0;
				resp->wCount = sizeof(pil_close_ack_t);
				break;
			case 1:
				psOpenAck->byStatus = 1;
				resp->wCount = sizeof(pil_close_ack_t);
				break;
			case 2:
				psOpenAck->byStatus = 2;
				resp->wCount = sizeof(pil_close_ack_t);
				break;
			case 3:
				psOpenAck->byStatus = 3;
				resp->wCount = sizeof(pil_close_ack_t);
				break;
			default:
				psOpenAck->byStatus = 1;
				resp->wCount  = sizeof(pil_close_ack_t);
				break;
		}
	}
	else
	{
		psOpenAck->byStatus = 1;
		resp->wCount = sizeof(pil_close_ack_t);
	}
	printf("%s > errCode %d\n", __FUNCTION__, errCode);
}

static void pil_req_transmit( alink_payload_t *msg, alink_resp_t *resp )
{
	uint8_t errCode;
	pil_req_transmit_t *psReqTransmit = NULL;
	pil_req_transmit_ack_t *psReqTransmitAck = NULL;

	psReqTransmit = (pil_req_transmit_t*)(msg->pbyData);
	psReqTransmitAck = (pil_req_transmit_ack_t*)(resp->pbyBuffer);
	if( RET_OK == pil_start_transmit(&(psReqTransmit->sHeader),&errCode) )
	{
		switch( errCode )
		{
			case 0:
				psReqTransmitAck->byStatus = 0;
				psReqTransmitAck->wPkgMaxLen = HIL_ADC_DATA_MAX;
				resp->wCount = sizeof(pil_req_transmit_ack_t);
				break;
			case 1:
				psReqTransmitAck->byStatus = 1;
				psReqTransmitAck->wPkgMaxLen = HIL_ADC_DATA_MAX;
				resp->wCount = sizeof(pil_req_transmit_ack_t);
				break;
			case 2:
				psReqTransmitAck->byStatus = 2;
				psReqTransmitAck->wPkgMaxLen = HIL_ADC_DATA_MAX;
				resp->wCount = sizeof(pil_req_transmit_ack_t);
				break;
			default:
				psReqTransmitAck->byStatus = 3;
				psReqTransmitAck->wPkgMaxLen = HIL_ADC_DATA_MAX;
				resp->wCount = sizeof(pil_req_transmit_ack_t);
				break;
		}
	}
	else
	{
		psReqTransmitAck->byStatus = 3;
		psReqTransmitAck->wPkgMaxLen = HIL_ADC_DATA_MAX;
		resp->wCount = sizeof(pil_req_transmit_ack_t);
	}
	printf("%s > errCode %d\n", __FUNCTION__, errCode);
}

static void pil_transmit_adc( alink_payload_t *msg, alink_resp_t *resp )
{
	uint8_t errCode;
	pil_transmit_adc_t *psTransmitAdc = NULL;
	pil_transmit_adc_ack_t *psTransmitAdcAck = NULL;

	psTransmitAdc = (pil_transmit_adc_t*)(msg->pbyData);
	psTransmitAdcAck = (pil_transmit_adc_ack_t*)(resp->pbyBuffer);
	if( RET_OK == pil_transmit_data(psTransmitAdc->byStatus,psTransmitAdc->uOffset,psTransmitAdc->adcData,psTransmitAdc->wLength,&errCode) )
	{
		switch( errCode )
		{
		case 0:
			psTransmitAdcAck->byStatus = 0;
			psTransmitAdcAck->uOffset = pil_read_data_offset();
			resp->wCount = sizeof(pil_transmit_adc_ack_t);
			break;
		case 1:
			psTransmitAdcAck->byStatus = 1;
			psTransmitAdcAck->uOffset = pil_read_data_offset();
			resp->wCount = sizeof(pil_transmit_adc_ack_t);
			break;
		case 2:
			psTransmitAdcAck->byStatus = 2;
			psTransmitAdcAck->uOffset = pil_read_data_offset();
			resp->wCount = sizeof(pil_transmit_adc_ack_t);
			break;
		default:
			psTransmitAdcAck->byStatus = 3;
			psTransmitAdcAck->uOffset = pil_read_data_offset();
			resp->wCount = sizeof(pil_transmit_adc_ack_t);
			break;
		}
	}
	else
	{
		psTransmitAdcAck->byStatus = 3;
		psTransmitAdcAck->uOffset = pil_read_data_offset();
		resp->wCount = sizeof(pil_transmit_adc_ack_t);
	}
	printf("%s > errCode %d\n", __FUNCTION__, errCode);
}

static void pil_get_status( alink_payload_t *msg, alink_resp_t *resp )
{
	pil_get_status_ack_t *psGetStatusAck = NULL;

	psGetStatusAck = (pil_get_status_ack_t*)(resp->pbyBuffer);
	switch( pil_read_status() )
	{
		case 0:
			psGetStatusAck->byStatus = 0;
			resp->wCount = sizeof(pil_get_status_ack_t);
			break;
		case 1:
			psGetStatusAck->byStatus = 1;
			resp->wCount = sizeof(pil_get_status_ack_t);
			break;
		case 2:
			psGetStatusAck->byStatus = 2;
			resp->wCount = sizeof(pil_get_status_ack_t);
			break;
		case 3:
			psGetStatusAck->byStatus = 3;
			resp->wCount = sizeof(pil_get_status_ack_t);
			break;
		default:
			psGetStatusAck->byStatus = 4;
			resp->wCount = sizeof(pil_get_status_ack_t);
			break;
	}
	printf("%s > errCode %d\n", __FUNCTION__, psGetStatusAck->byStatus);
}

static void pil_write_adc( alink_payload_t *msg, alink_resp_t *resp )
{
	uint8_t errCode;
	pil_write_adc_ack_t *psWriteAdcAck;

	psWriteAdcAck = (pil_write_adc_ack_t*)(resp->pbyBuffer);
	if( RET_OK == pil_inject_data(&errCode) )
	{
		switch( errCode )
		{
			case 0:
				psWriteAdcAck->byStatus = 0;
				resp->wCount = sizeof(pil_write_adc_ack_t);
				break;
			case 1:
				psWriteAdcAck->byStatus = 1;
				resp->wCount = sizeof(pil_write_adc_ack_t);
				break;
			case 2:
				psWriteAdcAck->byStatus = 2;
				resp->wCount = sizeof(pil_write_adc_ack_t);
				break;
			case 3:
				psWriteAdcAck->byStatus = 3;
				resp->wCount = sizeof(pil_write_adc_ack_t);
				break;
			case 4:
				psWriteAdcAck->byStatus = 4;
				resp->wCount = sizeof(pil_write_adc_ack_t);
				break;
			default:
				psWriteAdcAck->byStatus = 3;
				resp->wCount = sizeof(pil_write_adc_ack_t);
				break;
		}
	}
	else
	{
		psWriteAdcAck->byStatus = 5;
		resp->wCount = sizeof(pil_write_adc_ack_t);
	}
	printf("%s > errCode %d\n", __FUNCTION__, errCode);
}

void alink_pil_init( void )
{
	alink_register_cmd( &sAlinkCmd_PilOpen, 			ID_PIL_OPEN,			0,							pil_open );
	alink_register_cmd( &sAlinkCmd_PilClose, 			ID_PIL_CLOSE,			0,							pil_close );
	alink_register_cmd( &sAlinkCmd_PilRequestTransmit, 	ID_PIL_REQ_TRANSMIT,	sizeof(pil_req_transmit_t),	pil_req_transmit );
	alink_register_cmd( &sAlinkCmd_PilTransmitAdc, 		ID_PIL_TRANSMIT_ADC,	sizeof(pil_transmit_adc_t),	pil_transmit_adc );
	alink_register_cmd( &sAlinkCmd_PilGetStatus, 		ID_PIL_GET_STATUS,		0,							pil_get_status );
	alink_register_cmd( &sAlinkCmd_PilWrtieAdc, 		ID_PIL_WRITE_ADC,		0,							pil_write_adc );
}


