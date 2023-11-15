
#include "alink_record.h"

#include "../../../record/record.h"

static int record_get_file_list_handler(void *arg, int file_type, int file_size, char* name)
{
	int eRet = 0;

	switch( file_type )
	{
		case 0:
			if( name )
			{
				record_get_list( name, arg, record_get_file_list_handler );
			}
			break;
		default:
			if( name )
			{
				alink_resp_t *psResp = (alink_resp_t *)(arg);
				uint32_t uFileNameLen = strlen( name );
				if( psResp->wSize < psResp->wCount + uFileNameLen + sizeof(alink_record_file_list_t) )
				{
					psResp->pe_send( psResp );
					alink_record_get_list_ack_t *psGetListAck = (alink_record_get_list_ack_t *)(psResp->pbyBuffer);
					psGetListAck->uPkgCount += 1;
					psResp->wCount = sizeof(alink_record_get_list_ack_t);
				}
				
				alink_record_file_list_t *psRecordList = (alink_record_file_list_t *)(psResp->pbyBuffer + psResp->wCount);
				psRecordList->uNameLen = uFileNameLen;
				psRecordList->uDataLen = file_size;
				
				memcpy( psRecordList->abyData, name, uFileNameLen );
				psResp->wCount += (uFileNameLen + sizeof(alink_record_file_list_t));
			}
			break;
	}

	return eRet;
}

static void record_get_file_list( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	alink_record_get_list_ack_t *psGetListAck = NULL;

	psGetListAck = (alink_record_get_list_ack_t *)(psResp->pbyBuffer);

	psGetListAck->uPkgCount = 1;
	psGetListAck->uPkgSize = 0;
	psResp->wCount = sizeof(alink_record_get_list_ack_t);
	
	record_get_list( NULL, psResp, record_get_file_list_handler );

	psGetListAck->uPkgSize = psGetListAck->uPkgCount;
}

static void record_get_file_data( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	alink_record_get_file_t *psGetFile = (alink_record_get_file_t *)(psPayload->pbyData);
	alink_record_get_file_ack_t *psGetFileAck = (alink_record_get_file_ack_t *)(psResp->pbyBuffer);
	uint32_t uMaxSize = psResp->wSize - sizeof(alink_record_get_file_ack_t);
	alink_record_get_list_t *psGetList = psGetFile->sList;
	
	for( uint32_t i = 0; psGetFile->uNum > i; ++i )
	{
		alink_record_get_list_t *psGetListNext = (alink_record_get_list_t *)((uint8_t*)(psGetList) + psGetList->uNameLen + sizeof(alink_record_get_list_t));
		uint32_t uIdTemp = psGetListNext->uId;
	
		psGetFileAck->uId = psGetList->uId;
		psGetFileAck->uPkgSize = 0;
		psGetFileAck->uPkgCount = 0;
		psGetList->abyData[psGetList->uNameLen] = '\0';

		for( uint32_t uOffset = 0; ; )
		{
			psGetFileAck->uDataLen = record_read( psGetList->abyData, uOffset, psGetFileAck->abyData, uMaxSize );
			psResp->wCount = psGetFileAck->uDataLen + sizeof(alink_record_get_file_ack_t);
			psGetFileAck->uPkgCount += 1;

			printf( "[%s %d] %d %d %d\r\n", __func__, __LINE__, psGetFileAck->uDataLen, uMaxSize, uOffset );

			if( uMaxSize == psGetFileAck->uDataLen )
			{
				psResp->pe_send( psResp );
				uOffset += psGetFileAck->uDataLen;
			}
			else
			{
				psGetFileAck->uPkgSize = psGetFileAck->uPkgCount;
				psResp->pe_send( psResp );
				break;
			}
		}

		psGetList = psGetListNext;
		psGetList->uId = uIdTemp;
	}

	psGetFileAck->uPkgSize = 0;
	psGetFileAck->uPkgCount = 0;
	psGetFileAck->uDataLen = 0;
	psResp->wCount = sizeof(alink_record_get_file_ack_t);
}

static void record_delete_file( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	int eRet = 0;

	alink_record_delete_file_t *psDeleteFile = (alink_record_delete_file_t *)(psPayload->pbyData);
	alink_record_delete_file_ack_t *psDeleteFileAck = (alink_record_delete_file_ack_t *)(psResp->pbyBuffer);

	psDeleteFile->abyName[psDeleteFile->uNameLen] = '\0';
	eRet = record_delete( psDeleteFile->abyName );
	if( 0 == eRet )
		psDeleteFileAck->byStatus = 1;
	else
		psDeleteFileAck->byStatus = 0;

	psResp->wCount = sizeof(alink_record_delete_file_ack_t);
}

static alink_cmd_list_t sAlinkCmd_RecordGetList = { 0 };
static alink_cmd_list_t sAlinkCmd_RecordGetFile = { 0 };
static alink_cmd_list_t sAlinkCmd_RecordDelete = { 0 };

void alink_record_init( void )
{
	alink_register_cmd( &sAlinkCmd_RecordGetList, 		0x28,	0,	record_get_file_list );
	alink_register_cmd( &sAlinkCmd_RecordGetFile, 		0x29,	0,	record_get_file_data );
	alink_register_cmd( &sAlinkCmd_RecordDelete, 		0x2A,	0,	record_delete_file );
}
