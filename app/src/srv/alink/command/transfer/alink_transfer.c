
#include "alink_transfer.h"
#include "../../../../app/c2_network/c2_network.h"

static void transfer_set_wifi_info( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	int32_t eRet;
	uint8_t byPwdLen;
	uint8_t *ipIndex;
	uint8_t *pbyPwd;
	uint8_t *pbyIp;
	uint8_t *pbyPort;
	uint8_t strSsid[32];
	uint8_t strPassword[32];
	uint8_t strIp[16];
	uint32_t uTempIp;
	uint16_t wTempPort;
	alink_transfer_set_wifi_info_t *psWifiInfo;
	alink_transfer_set_wifi_info_ack_t *psWifiInfoAck;

	psWifiInfo = (alink_transfer_set_wifi_info_t*)(psPayload->pbyData);
	psWifiInfoAck = (alink_transfer_set_wifi_info_ack_t*)(psResp->pbyBuffer);
	byPwdLen = *((uint8_t*)(psWifiInfo->name) + psWifiInfo->nameLen);
	if( sizeof(strSsid) - 1 > psWifiInfo->nameLen && sizeof(strPassword) - 1 > byPwdLen )
	{
#if (0)
		memcpy( strSsid, psWifiInfo->name, psWifiInfo->nameLen );
		strSsid[psWifiInfo->nameLen] = '\0';
		pbyPwd = (uint8_t*)(psWifiInfo->name) + psWifiInfo->nameLen + 1;
		memcpy( strPassword, pbyPwd, byPwdLen );
		strPassword[byPwdLen] = '\0';

		pbyIp = pbyPwd + byPwdLen;
		pbyPort = pbyIp + 4;

		wTempPort = pbyPort[1];
		wTempPort <<= 8;
		wTempPort |= pbyPort[0];

		ipIndex = (uint8_t*)strIp;
		uTempIp = pbyIp[3];
		uTempIp <<= 8;
		uTempIp |= pbyIp[2];
		uTempIp <<= 8;
		uTempIp |= pbyIp[1];
		uTempIp <<= 8;
		uTempIp |= pbyIp[0];

		ipIndex = (uint8_t*)itoa( uTempIp & 0xFF, (char*)ipIndex, 10 );
		ipIndex += strlen((char*)ipIndex);
		*ipIndex++ = '.';
		uTempIp >>= 8;
		ipIndex = (uint8_t*)itoa( uTempIp & 0xFF, (char*)ipIndex, 10 );
		ipIndex += strlen((char*)ipIndex);
		*ipIndex++ = '.';
		uTempIp >>= 8;
		ipIndex = (uint8_t*)itoa( uTempIp & 0xFF, (char*)ipIndex, 10 );
		ipIndex += strlen((char*)ipIndex);
		*ipIndex++ = '.';
		uTempIp >>= 8;
		ipIndex = (uint8_t*)itoa( uTempIp & 0xFF, (char*)ipIndex, 10 );
#endif
//		eRet = wifi_ble_connect_wifi( (char*)strSsid, (char*)strPassword, (char*)strIp, wTempPort, 1000 );
		if( 0 == eRet )
		{
			psWifiInfoAck->status = 1;
			psResp->wCount = 1;
		}
		else
		{
			psWifiInfoAck->status = 0;
			psResp->wCount = 1;
		}
	}
	else
	{
		psWifiInfoAck->status = 0;
		psResp->wCount = 1;
	}
}

static void transfer_set_disconnect( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	int32_t eRet;
	alink_transfer_set_disconnect_t *psSetDisconnect;
	alink_transfer_set_disconnect_ack_t *psSetDisconnectAck;

	psSetDisconnect = (alink_transfer_set_disconnect_t*)(psPayload->pbyData);
	psSetDisconnectAck = (alink_transfer_set_disconnect_ack_t*)(psResp->pbyBuffer);

	if( 1 == psSetDisconnect->status )
	{
//		eRet = c2_network_set_disconnect();
		if( 0 == eRet )
		{
			psSetDisconnectAck->status = 0;
			psResp->wCount = 1;
		}
		else
		{
			psSetDisconnectAck->status = 1;
			psResp->wCount = 1;
		}
	}
	else
	{
		psSetDisconnectAck->status = 1;
		psResp->wCount = 1;
	}
}

static void transfer_request_channel_ack( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	uint8_t *ipIndex;
	uint8_t strIp[16];
	uint32_t uTempIp;
	uint16_t wTempPort;
	alink_transfer_request_channel_ack_t *psChannelAck;

	psChannelAck = (alink_transfer_request_channel_ack_t*)(psPayload->pbyData);

	ipIndex = (uint8_t*)strIp;
	uTempIp = psChannelAck->ip;

	ipIndex += snprintf( ipIndex, strIp + sizeof(strIp) - ipIndex, "%u", uTempIp & 0xFF );
	*ipIndex++ = '.';
	uTempIp >>= 8;
	ipIndex += snprintf( ipIndex, strIp + sizeof(strIp) - ipIndex, "%u", uTempIp & 0xFF );
	*ipIndex++ = '.';
	uTempIp >>= 8;
	ipIndex += snprintf( ipIndex, strIp + sizeof(strIp) - ipIndex, "%u", uTempIp & 0xFF );
	*ipIndex++ = '.';
	uTempIp >>= 8;
	ipIndex += snprintf( ipIndex, strIp + sizeof(strIp) - ipIndex, "%u", uTempIp & 0xFF );

	wTempPort = psChannelAck->port;

//	c2_network_set_channel_info( psPayload->ePortId, (char*)strIp, wTempPort );
	psResp->wCount = 0;
}

static uint16_t transfer_pkg_request_channel( uint8_t *pbyBuffer, uint16_t wSize, void *psNetInfo )
{
	uint32_t uIp;
	uint16_t wPort;
	char strIpTemp[16];
	char* pstrSegment;
	char* pstrIndex = NULL;
	alink_transfer_request_channel_t *psReqChannel;

	psReqChannel = (alink_transfer_request_channel_t*)pbyBuffer;

#if (0)
	pstrSegment = strtok_r( strIpTemp, ".", &pstrIndex );
	uIp = atoi( pstrSegment );
	pstrSegment = strtok_r( NULL, ".", &pstrIndex );
	uIp <<= 8;
	uIp |= atoi( pstrSegment );
	pstrSegment = strtok_r( NULL, ".", &pstrIndex );
	uIp <<= 8;
	uIp |= atoi( pstrSegment );
	pstrSegment = strtok_r( NULL, ".", &pstrIndex );
	uIp <<= 8;
	uIp |= atoi( pstrSegment );
#endif

	strncpy( (char*)psReqChannel->sn, "RADAR_12345678", sizeof(psReqChannel->sn) );
	psReqChannel->ip	= 0x37EBA8C0;
	psReqChannel->port	= 6080;
	psReqChannel->proxy	= 1;		//tcp

	return 22;
}

static uint16_t transfer_pkg_connect_status( uint8_t *pbyBuffer, uint16_t wSize, void *psStatus )
{

	alink_transfer_connect_stutas_t *psConnectStatue;

	psConnectStatue = (alink_transfer_connect_stutas_t*)pbyBuffer;

	psConnectStatue->status = ((alink_transfer_connect_stutas_t*)psStatus)->status;

	return 1;
}

static alink_cmd_list_t sAlinkCmd_TransferSetWifiInfo = { 0 };
static alink_cmd_list_t sAlinkCmd_TransferSetDisconnect = { 0 };
static alink_cmd_list_t sAlinkCmd_TransferReqChannelAck = { 0 };

static alink_package_list_t sAlinkPackage_ReqChannel = { 0 };
static alink_package_list_t sAlinkPackage_ConnectStatus = { 0 };

void alink_transfer_init( void )
{
	alink_register_cmd( &sAlinkCmd_TransferSetWifiInfo, 	0xB0,	16,												transfer_set_wifi_info );
	alink_register_cmd( &sAlinkCmd_TransferSetDisconnect, 	0xB1,	sizeof(alink_transfer_set_disconnect_t),		transfer_set_disconnect );
	alink_register_cmd( &sAlinkCmd_TransferReqChannelAck, 	0xB3,	sizeof(alink_transfer_request_channel_ack_t),	transfer_request_channel_ack );

	alink_register_package( &sAlinkPackage_ReqChannel,		0xB3,	1, 												transfer_pkg_request_channel );
	alink_register_package( &sAlinkPackage_ConnectStatus,	0xB2,	0, 												transfer_pkg_connect_status );
}

int32_t alink_upload_req_channel( void )
{
	return alink_upload_package( &sAlinkPackage_ReqChannel, NULL );
}

int32_t alink_upload_connect_status( uint8_t byState )
{
	alink_transfer_connect_stutas_t sStatus;
	
	sStatus.status = byState;
	return alink_upload_package( &sAlinkPackage_ConnectStatus, &sStatus );
}

