
#include "eth_link.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <fcntl.h>

#include "socket/udp_socket.h"
#include "socket/tcp_client.h"
#include "../alink/alink.h"

/*******************************************************************************
自动组网
*******************************************************************************/
static udp_socket_t sUdpSocket = { 0 };

static uint8_t abyBuffer[64] = { 0 };
static udp_socket_cfg_t sUdpCfg = 
{
	.bFlagRecv	= true,
	.bBroadcast	= true,
	.wRecvPort	= 1800,
	.uRecvIp	= INADDR_ANY,
	.wSendPort	= 1810,
	.uSendIp	= 0xFFFFFFFF,	//0xC0A8EBFF, //0xFFFFFFFF,
};

static void eth_link_udp_cbk_init( void* psHandle, void* result )
{
	printf( "[%s %d] %p\r\n", __func__, __LINE__, result );
}

static void eth_protocol_handler( void* msg, uint32_t len, uint32_t uIp );
static void eth_link_udp_cbk_data( void	*psHandle, int32_t (*pi_recv)(void *psSocket,void *pvBuffer,uint32_t uSize,uint32_t *puIp) )
{
	uint8_t abyBuffer[64];
	int32_t iLength;
	uint32_t uIp = 0xFFFFFFFF;
	
	iLength = pi_recv( &sUdpSocket, abyBuffer, sizeof(abyBuffer), &uIp );

	sUdpCfg.uSendIp = uIp | 0xFF;

#if 0
	printf( "[%s %d]", __func__, __LINE__ );
	for( int i = 0; iLength > i; ++i )
		printf( " %02X", abyBuffer[i] );
	printf( "\r\n" );
#endif

	eth_protocol_handler( abyBuffer, iLength, uIp );

	sUdpCfg.uSendIp = 0xFFFFFFFF;
}

static void eth_link_udp_cbk_delete( void *psHandle )
{
	printf( "[%s %d]\r\n", __func__, __LINE__ );
}

static volatile bool bFlagTick = true;
static int eth_link_broadcast(uint8_t *pbyData, uint32_t uLength)
{
	int eRet = 0;
	if( bFlagTick )
	{
		eRet = udp_socket_send( &sUdpSocket, pbyData, uLength );
		eRet = udp_socket_send( &sUdpSocket, pbyData, uLength );
		eRet = udp_socket_send( &sUdpSocket, pbyData, uLength );
		bFlagTick = false;
	}
	return eRet;
}

static void *eth_link_task(void *p_arg)
{
	for( ; ; )
	{
		sleep(1);
		alink_upload_heartbeat();
		bFlagTick = true;
	}
}

static udp_socket_cbk_t sUdpCbk = 
{
	.psHandle = NULL,
	.pv_cbk_init = eth_link_udp_cbk_init,
	.pv_cbk_data = eth_link_udp_cbk_data,
	.pv_cbk_delete = eth_link_udp_cbk_delete,
};

int eth_link_init( void )
{
	int32_t eRet = 0;

	pthread_t pid;
	if( 0 == pthread_create(&pid, NULL, eth_link_task, NULL) )
	{
		eRet = udp_socket_crate( &sUdpSocket, &sUdpCfg, &sUdpCbk );
	}
	
	return eRet;
}

/*******************************************************************************
自动组网处理列表
*******************************************************************************/
typedef struct eth_link_list
{
	uint8_t				 net;
	uint8_t				 max;
	uint16_t			 type;
	eth_link_cbk_t* 	 (*ps_init)(eth_link_user_t *psUser);
	void				 *list;
	struct eth_link_list *next;
}eth_link_list_t;
static eth_link_list_t *psEthList = NULL;
static void eth_link_add_tcp_client_list( eth_link_list_t* psEthList, uint32_t uIp, uint16_t wPort, uint16_t wConnectTime );
int eth_link_add_type( uint16_t type, uint8_t net, uint8_t max, eth_link_cbk_t* (*ps_init)(eth_link_user_t *psUser) )
{
	int eRet = 0;

	if( psEthList )
	{
		eth_link_list_t *psList = psEthList;
		for( ; psList->next; psList = psList->next )
		{
			if( psList->type == type )
			{
				eRet = 1;
				break;
			}
		}
		if( psList->type != type )
		{
			eth_link_list_t* list = (eth_link_list_t*)malloc( sizeof(eth_link_list_t) );
			memset( list, 0, sizeof(eth_link_list_t) );
			printf( "[%s %d]\r\n", __func__, __LINE__ );
			if( list )
			{
				psList->next = (eth_link_list_t*)(list);
				psList->next->type		= type;
				psList->next->net		= net;
				psList->next->max		= max;
				psList->next->ps_init	= ps_init;				
				psList->next->list 		= NULL;
				psList->next->next 		= NULL;
				if( type == 1 )
				{
					printf( "[%s %d]\r\n", __func__, __LINE__ );
					eth_link_add_tcp_client_list( psList->next, htonl(inet_addr("192.168.235.99")), 8060, 0 );
				}
			}
			else
			{
				free( list );
				eRet = -1;
			}
		}
	}
	else
	{
		eth_link_list_t* list = (eth_link_list_t*)malloc( sizeof(eth_link_list_t) );
		memset( list, 0, sizeof(eth_link_list_t) );
		printf( "[%s %d]\r\n", __func__, __LINE__ );
		if( list )
		{
			psEthList = (eth_link_list_t*)(list);
			psEthList->type		= type;
			psEthList->net		= net;
			psEthList->max		= max;
			psEthList->ps_init	= ps_init;
			psEthList->list		= NULL;
			psEthList->next		= NULL;
			if( type == 1 )
			{
				printf( "[%s %d]\r\n", __func__, __LINE__ );
				eth_link_add_tcp_client_list( psEthList, htonl(inet_addr("192.168.235.99")), 8060, 0 );
			}
		}
		else
		{
			free( list );
			eRet = -1;
		}
	}

	return eRet;
}

int eth_link_del_type( uint16_t type )
{
	int eRet = 0;
	
	eth_link_list_t *psList = psEthList;
	if( psList )
	{
		eth_link_list_t *psLast = psList;
		for( ; psList; psList = psList->next )
		{
			if( psList->type == type )
			{
				psLast->next = psList->next;
				free( psList );
				break;
			}
			psLast = psList;
		}
	}

	return eRet;
}

/*******************************************************************************
TCP服务端连接列???
*******************************************************************************/
typedef struct eth_link_tcp_client_list
{
	tcp_client_cfg_t sCfg;
	tcp_client_cbk_t sCbk;
	tcp_client_t 	 sClient;
	eth_link_user_t	 sUser;
	eth_link_cbk_t	 *psUserCbk;
	eth_link_list_t	 *psRoot;
	struct eth_link_tcp_client_list *psNext;
}eth_link_tcp_client_list_t;

static void eth_link_del_tcp_client_list( eth_link_tcp_client_list_t *psList )
{
	eth_link_tcp_client_list_t *psIndex = (eth_link_tcp_client_list_t*)psList->psRoot->list;
	if( psList == psIndex )
	{
		psList->psRoot->list = psIndex->psNext;
	}
	else
	{
		eth_link_tcp_client_list_t *psLast = psIndex;
		for( ; psIndex; psIndex = psIndex->psNext )
		{
			if( psList == psIndex )
			{
				psLast->psNext = psIndex->psNext;
				break;
			}
		}
	}
	free( psList );
}

static void eth_link_socket_cbk_connect( void *psHandle )
{
	eth_link_tcp_client_list_t *psList = (eth_link_tcp_client_list_t*)psHandle;
	if( psList && psList->psUserCbk && psList->psUserCbk->pv_cbk_connect )
		psList->psUserCbk->pv_cbk_connect( psList->psUserCbk->psHandle );
}

static void eth_link_socket_cbk_data( void *psHandle, void *psClient, int32_t (*pi_recv)(void *psClient,void *pvBuffer,uint32_t uSize) )
{
	eth_link_tcp_client_list_t *psList = (eth_link_tcp_client_list_t*)psHandle;
	if( psList && psList->psUserCbk && psList->psUserCbk->pv_cbk_data )
	{
		psList->psUserCbk->pv_cbk_data( psList->psUserCbk->psHandle, psClient, pi_recv );
	}
	else
	{
		uint8_t abyBuffer[128];
		int32_t iLength = pi_recv( psClient, abyBuffer, sizeof(abyBuffer) );
	}
}

static void eth_link_socket_cbk_disconnect( void *psHandle )
{
	eth_link_tcp_client_list_t *psList = (eth_link_tcp_client_list_t*)psHandle;
	if( psList && psList->psUserCbk && psList->psUserCbk->pv_cbk_disconnect )
		psList->psUserCbk->pv_cbk_disconnect( psList->psUserCbk->psHandle );
}

static void eth_link_socket_cbk_delete( void *psHandle )
{
	eth_link_tcp_client_list_t *psList = (eth_link_tcp_client_list_t*)psHandle;
	if( psList && psList->psUserCbk && psList->psUserCbk->pv_cbk_delete )
		psList->psUserCbk->pv_cbk_delete( psList->psUserCbk->psHandle );
	
	eth_link_del_tcp_client_list( psList );
}

static void *eth_link_create_tcp_client_list( uint16_t wConnectTime, uint32_t uIp, uint16_t wPort, eth_link_cbk_t* (*ps_init)(eth_link_user_t *psUser) )
{
	eth_link_tcp_client_list_t *psList = (eth_link_tcp_client_list_t*)malloc(sizeof(eth_link_tcp_client_list_t));
	memset( psList, 0, sizeof(eth_link_tcp_client_list_t) );
//	printf( "[%s %d] %x %d\r\n", __func__, __LINE__, uIp, wPort );
	if( psList )
	{
		psList->sCfg.uRemoteIp		= uIp;
		psList->sCfg.wRemotePort	= wPort;
		psList->sCfg.wConnectTime	= wConnectTime;

		psList->sCbk.psHandle			= psList;
		psList->sCbk.pv_cbk_connect		= eth_link_socket_cbk_connect;
		psList->sCbk.pv_cbk_data		= eth_link_socket_cbk_data;
		psList->sCbk.pv_cbk_disconnect	= eth_link_socket_cbk_disconnect;
		psList->sCbk.pv_cbk_delete		= eth_link_socket_cbk_delete;

//		printf( "[%s %d]\r\n", __func__, __LINE__ );
		int eRet = tcp_client_create( &(psList->sClient), &(psList->sCfg), &(psList->sCbk) );
		if( 0 == eRet )
		{
//			printf( "[%s %d]\r\n", __func__, __LINE__ );
			psList->sUser.psSocket	= (void*)(&(psList->sClient));
			psList->sUser.pi_send	= (int32_t (*)(void*,uint8_t*,uint32_t))tcp_client_send;
			psList->sUser.pi_close	= (int32_t (*)(void*))tcp_client_close;
			psList->psUserCbk		= ps_init( &(psList->sUser) );
//			printf( "[%s %d]\r\n", __func__, __LINE__ );
		}
		else
		{
			free( psList );
			psList = NULL;
		}
	}

	return psList;
}

static void eth_link_add_tcp_client_list( eth_link_list_t* psEthList, uint32_t uIp, uint16_t wPort, uint16_t wConnectTime )
{
	eth_link_tcp_client_list_t *psList = (eth_link_tcp_client_list_t*)psEthList->list;
	if( psList )
	{
		uint32_t uCount = 1;
		for( ; psList->psNext; psList = psList->psNext )
		{
			if( psList->sCfg.uRemoteIp == uIp && psList->sCfg.wRemotePort == wPort )
				break;
			uCount++;
		}
		if( psList->sCfg.uRemoteIp != uIp && psList->sCfg.wRemotePort != wPort && psEthList->max > uCount )
		{
//			printf( "[%s %d]\r\n", __func__, __LINE__ );
			psList->psNext = eth_link_create_tcp_client_list( wConnectTime, uIp, wPort, psEthList->ps_init );
			if( psList->psNext )
			{
				psList = psList->psNext;
				psList->psNext = NULL;
				psList->psRoot = psEthList;
			}
		}
	}
	else
	{
//		printf( "[%s %d]\r\n", __func__, __LINE__ );
		psEthList->list = eth_link_create_tcp_client_list( wConnectTime, uIp, wPort, psEthList->ps_init );
		if( psEthList->list )
		{
			psList = (eth_link_tcp_client_list_t*)psEthList->list;
			psList->psNext = NULL;
			psList->psRoot = psEthList;
		}
	}
}

/*******************************************************************************
创建连接
*******************************************************************************/
static int eth_link_create_socket( uint16_t type, uint32_t ip, uint16_t port )
{
	int eRet = 0;

	for( eth_link_list_t *psList = psEthList; psList; psList = psList->next )
	{
		if( psList->type == type )
		{
			switch( psList->net )
			{
				case RTH_LINK_NET_TCP_CLIENT:
//					printf( "[%s %d]\r\n", __func__, __LINE__ );
					eth_link_add_tcp_client_list( psList, ip, port, 5 );
					break;
				case RTH_LINK_NET_UDP:
					break;
				default:
					break;
			}
			break;
		}
	}

	return eRet;
}

/*******************************************************************************
交互协议
*******************************************************************************/
char* eth_protocol_get_sn( void )
{
	static char sn[64] = {0};
	int fd;
	int ret;
	
	fd = open("/run/media/mtd_user/sn", O_RDONLY);
	if (fd < 0)
	{
		return "RADAR-12345678";
	}

	ret = read(fd, sn, sizeof(sn) - 1);
	close(fd);

	if (ret > 0)
	{
		return sn;
	}

	return "RADAR-12345678";
}

#pragma pack(1)
	typedef struct eth_protocol_head
	{
		uint8_t magic;
		uint8_t len_lo;
		uint8_t len_hi;
		uint8_t seq;
		uint8_t destid;
		uint8_t sourceid;
		uint8_t msgid;
		uint8_t ans;
		uint8_t checksum;
	}eth_protocol_head_t;
#pragma pack()

static uint8_t* eth_protocol_analysis( void *msg, uint32_t len )
{
	uint8_t* pData = NULL;
	eth_protocol_head_t *psHead = (eth_protocol_head_t*)(msg);

	do{
		if( 0xFD != psHead->magic )
			break;

		uint32_t size;
		size = psHead->len_hi;
		size <<= 8;
		size |= psHead->len_lo;
		if( (size + 11) != (len) )
			break;

		uint8_t checksum = 0;
		for( int i = 0; 8 > i; ++i )
			checksum += ((uint8_t*)(psHead))[i];
		if( psHead->checksum != checksum )
			break;

		uint16_t crc = 0xFFFF;
		uint32_t count = len - 3;
		uint8_t *data = (uint8_t*)(psHead) + 1;
	    while( count-- )
	    {
			uint8_t tmp;
		    tmp = *data ^ (uint8_t)(crc & 0xFF);
		    tmp ^= (tmp << 4);
		    crc = (crc >> 8) ^ (tmp << 8) ^ (tmp << 3) ^ (tmp >> 4);
			data++;
	    }
		uint16_t check = (data[1]<<8) | (data[0]<<0);
		if( crc != check )
			break;

		pData = ((uint8_t*)(msg)) + sizeof(eth_protocol_head_t);
	}while(0);

	return pData;
}

static uint32_t eth_protocol_package( uint8_t msgid, void* msg, uint32_t len )
{
	eth_protocol_head_t *psHead = (eth_protocol_head_t*)(msg);
	psHead->magic = 0xFD;
	psHead->len_lo = (0xFF & (len>>0));
	psHead->len_hi = (0xFF & (len>>8));
	psHead->seq = 0;
	psHead->destid = 0xFF;
	psHead->sourceid = 0x03;	//radar id
	psHead->msgid = msgid;
	psHead->ans = 0;

	uint8_t checksum = 0;
	for( int i = 0; 8 > i; ++i )
		checksum += ((uint8_t*)(psHead))[i];
	psHead->checksum = checksum;

	uint16_t crc = 0xFFFF;
	uint32_t count = len + 11 - 3;
	uint8_t *data = (uint8_t*)(psHead);
    while( count-- )
    {
		uint8_t tmp;
		++data;
	    tmp = *data ^ (uint8_t)(crc & 0xFF);
	    tmp ^= (tmp << 4);
	    crc = (crc >> 8) ^ (tmp << 8) ^ (tmp << 3) ^ (tmp >> 4);
    }
	data[1] = (0xFF & (crc>>0));
	data[2] = (0xFF & (crc>>8));

	return (len+11);
}

#pragma pack(1)
	typedef struct eth_protocol_devinfo
	{
		eth_protocol_head_t head;
			
		uint32_t	protocol;
		uint8_t 	sn[32];
		uint16_t	type;
		uint16_t	status;
		char		version[64];

		uint16_t	crc;
	}eth_protocol_devinfo_t;
#pragma pack()
static int eth_protocol_devinfo_pkg_send( eth_protocol_devinfo_t *psDevInfo )
{
//	printf( "[%s %d] %p\r\n", __func__, __LINE__, psDevInfo );
	psDevInfo->protocol = 0x03;
	strncpy( psDevInfo->sn, eth_protocol_get_sn(), sizeof(psDevInfo->sn) );
	psDevInfo->type = 3;
	psDevInfo->status = 1;
	strncpy( psDevInfo->version, "acur101_app_v00.01.02_dev", sizeof(psDevInfo->version) );
	eth_protocol_package( 0xBB, (uint8_t*)psDevInfo, sizeof(eth_protocol_devinfo_t) - 11 );
	return eth_link_broadcast( (uint8_t*)psDevInfo, sizeof(eth_protocol_devinfo_t) );
}

#pragma pack(1)
	typedef struct eth_protocol_search
	{
		uint32_t	protocol;
		uint8_t		sn[32];
	}eth_protocol_search_t;
#pragma pack()
static void eth_protocol_search_handler( eth_protocol_search_t *psSearch, uint32_t uIp )
{
	if( psSearch->sn[0] )
	{
		uint32_t len = strlen(psSearch->sn);
		if( strlen(eth_protocol_get_sn()) == len )
		{
			if( 0 == memcmp(psSearch->sn, eth_protocol_get_sn(), len) )
			{
				eth_protocol_devinfo_t sDevInfo;
				eth_protocol_devinfo_pkg_send( &sDevInfo );
			}
		}
	}
	else
	{
		eth_link_list_t *psList = psEthList;
		for( ; psList; psList = psList->next )
		{
			if( RTH_LINK_NET_TCP_CLIENT == psList->net )
			{
				eth_link_tcp_client_list_t *psClient = (eth_link_tcp_client_list_t *)psList->list;
				for( ; psClient; psClient = psClient->psNext )
				{
					if( htonl(psClient->sCfg.uRemoteIp) == uIp )
						break;
				}
				if( psClient )
					break;
			}
		}
		if( NULL == psList )
		{
			eth_protocol_devinfo_t sDevInfo;
			eth_protocol_devinfo_pkg_send( &sDevInfo );
		}
	}
}

#pragma pack(1)
	typedef struct eth_protocol_cntinfo
	{
		uint8_t		sn[32];
		uint32_t	ip;
		uint16_t	port;
		uint16_t	type;
	}eth_protocol_cntinfo_t;
#pragma pack()
static void eth_protocol_cntinfo_handler( eth_protocol_cntinfo_t *psCntInfo, uint32_t uIp )
{
	if( psCntInfo->sn[0] )
	{
		printf( "[%s %d]\r\n", __func__, __LINE__ );
		uint32_t len = strlen(psCntInfo->sn);
		if( strlen(eth_protocol_get_sn()) == len )
		{
			if( 0 == memcmp(psCntInfo->sn, eth_protocol_get_sn(), len) )
			{
				eth_link_create_socket( psCntInfo->type, htonl(psCntInfo->ip), psCntInfo->port );
			}
		}
	}
	else
	{
		printf( "[%s %d] %x %d\r\n", __func__, __LINE__, psCntInfo->ip, psCntInfo->port );
		eth_link_create_socket( psCntInfo->type, htonl(psCntInfo->ip), psCntInfo->port );
	}
}

static void eth_protocol_handler( void* msg, uint32_t len, uint32_t uIp )
{
	uint8_t* pbyData = eth_protocol_analysis( msg, len );

	if( pbyData )
	{
		eth_protocol_head_t *psHead = (eth_protocol_head_t*)(msg);
		switch( psHead->msgid )
		{
			case 0xBA:
				eth_protocol_search_handler( (eth_protocol_search_t*)pbyData, uIp );
				break;
			case 0xBC:
				eth_protocol_cntinfo_handler( (eth_protocol_cntinfo_t*)pbyData, uIp );
				break;
			default:
				break;
		}
	}
}

