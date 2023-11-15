
#include <mqueue.h>
#include <sys/time.h>

#include "c2_network.h"
#include "../../srv/eth_link/eth_link.h"
#include "../../srv/alink/alink.h"
#include "../../srv/alink/check/alink_check.h"
#include "../../srv/log/log.h"

typedef struct c2_network
{
	alink_send_list_t sSendAlinkHeartBeat;
	alink_send_list_t sSendAlinkTracked;
	alink_send_list_t sSendAlinkTracked_plus;
	alink_send_list_t sSendAlinkBeamScheduling;
	alink_send_list_t sSendAlinkAttitude;
	alink_send_list_t sSendAlinkCalibriteData;
	alink_socket_t sAlinkSocket;

	eth_link_cbk_t	sCbk;
	eth_link_user_t *psUser;
}c2_network_t;

int32_t c2_network_send( void* handle, uint8_t *pbyData, uint32_t uLen )
{
	eth_link_user_t *psUser = (eth_link_user_t*)handle;
	return	psUser->pi_send( psUser->psSocket, pbyData, uLen );
}

static void c2_network_cbk_connect(void *psHandle)
{
	printf( "[%s %d]\r\n", __func__, __LINE__ );
}
static void c2_network_cbk_data(void *psHandle,void *psClient,int32_t (*pi_recv)(void *psClient,void *pvBuffer,uint32_t uSize))
{
	c2_network_t *psNetwork = (c2_network_t*)psHandle;
	uint8_t abyBuffer[ALINK_HEADER_CHECKSUM_LEN] = {0};
	uint8_t *pabybuf = NULL;
	int32_t iLength = 0, rLenght = 0;
	int32_t integrity_pkg_len = 0;
	int32_t offset = 0;

	pabybuf = abyBuffer;
	rLenght = sizeof(abyBuffer);
	//fix receving whole package
	do {
		iLength = pi_recv( psClient, pabybuf + offset, rLenght);
		if(iLength <= 0) {// we take this status as overtime or error
			if(integrity_pkg_len == 0) {
				pabybuf = NULL;
			}
			goto cbk_exit;
		}
		if(integrity_pkg_len == 0) {
			integrity_pkg_len = alink_package_integrity_check(abyBuffer, iLength);
			if(integrity_pkg_len > 0) {
				pabybuf = (uint8_t *)malloc(integrity_pkg_len);
				if(!pabybuf) {
					APP_LOG_ERROR("memory lack!!!\n");
					goto cbk_exit;
				}
				offset = alink_get_header_index(abyBuffer, iLength);
				memcpy(pabybuf, abyBuffer+offset, iLength-offset);
				iLength = iLength - offset;
				offset = 0;
			} else {//ignore none package head, depend on remote user.
				pabybuf = NULL;
				goto cbk_exit;
			}
		}

		offset += iLength;
		rLenght = integrity_pkg_len - offset;
		//APP_LOG_DEBUG("pkg len = %d, offset = %d, rlen = %d\n", integrity_pkg_len, offset, rLenght);
	}while(integrity_pkg_len > offset);

	alink_recv_import_t sImport;
	sImport.ePortId	= 1;
	sImport.pbyData	= pabybuf;
	sImport.uLength	= integrity_pkg_len;
	sImport.handle	= psNetwork->psUser->psSocket;
	sImport.pe_send	= psNetwork->psUser->pi_send;
	alink_socket_task( &psNetwork->sAlinkSocket, &sImport );

cbk_exit:
	if(pabybuf) {
		free(pabybuf);
	}
//	APP_LOG_DEBUG("%s,%d  exit!\n", __FUNCTION__, __LINE__);

	return;
}

static void c2_network_cbk_disconnect(void *psHandle)
{
	printf( "[%s %d]\r\n", __func__, __LINE__ );
}
void c2_network_cbk_delete(void *psHandle)
{
	printf( "[%s %d]\r\n", __func__, __LINE__ );
	c2_network_t *psNetwork = (c2_network_t*)psHandle;

	alink_disconnect_send( &psNetwork->sSendAlinkHeartBeat,			0xD0 );
	alink_disconnect_send( &psNetwork->sSendAlinkTracked, 			0x12 );
	alink_disconnect_send( &psNetwork->sSendAlinkTracked_plus, 		0x1F );
	alink_disconnect_send( &psNetwork->sSendAlinkBeamScheduling,	0x14 );
	alink_disconnect_send( &psNetwork->sSendAlinkAttitude,			0x13 );
	alink_disconnect_send( &psNetwork->sSendAlinkCalibriteData,		0xCC );

	free( psNetwork );
}

eth_link_cbk_t* c2_network_create( eth_link_user_t *psUser )
{
	c2_network_t *psNetwork = (c2_network_t*)malloc(sizeof(c2_network_t));
	printf( "[%s %d]\r\n", __func__, __LINE__ );
	if( psNetwork )
	{
		memset( psNetwork, 0, sizeof(c2_network_t) );
		alink_connect_send( &psNetwork->sSendAlinkHeartBeat,		0xD0, ALINK_DEV_ID_C2, psUser, c2_network_send );
		alink_connect_send( &psNetwork->sSendAlinkTracked, 			0x12, ALINK_DEV_ID_C2, psUser, c2_network_send );
		alink_connect_send( &psNetwork->sSendAlinkTracked_plus, 	0x1F, ALINK_DEV_ID_C2, psUser, c2_network_send );
		alink_connect_send( &psNetwork->sSendAlinkBeamScheduling,	0x14, ALINK_DEV_ID_C2, psUser, c2_network_send );
		alink_connect_send( &psNetwork->sSendAlinkAttitude,			0x13, ALINK_DEV_ID_C2, psUser, c2_network_send );
		alink_connect_send( &psNetwork->sSendAlinkCalibriteData,	0xCC, ALINK_DEV_ID_C2, psUser, c2_network_send );

		alink_connect_port( &psNetwork->sAlinkSocket, NULL );

		psNetwork->psUser = psUser;
		psNetwork->sCbk.psHandle = psNetwork;
		psNetwork->sCbk.pv_cbk_connect		= c2_network_cbk_connect;
		psNetwork->sCbk.pv_cbk_data			= c2_network_cbk_data;
		psNetwork->sCbk.pv_cbk_disconnect	= c2_network_cbk_disconnect;
		psNetwork->sCbk.pv_cbk_delete		= c2_network_cbk_delete;
		return &(psNetwork->sCbk);
	}
	return NULL;
}

int32_t c2_network_init( void )
{
	return eth_link_add_type( 1, RTH_LINK_NET_TCP_CLIENT, 10, c2_network_create );
}

