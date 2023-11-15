
#include "tcp_client.h"

#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <netinet/tcp.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>
#include "../../net_cfg/net_cfg.h"

static int32_t tcp_client_recv( void *p_arg, void* pvBuffer, uint32_t uSize )
{
	int32_t iLength = 0;
	
	tcp_client_t *psClient = (tcp_client_t*)p_arg;
	if( psClient && pvBuffer )
	{
		iLength = recv(psClient->iSocket, pvBuffer, uSize, 0);
		if( 0 >= iLength ) {
			psClient->bFlagConnect = false;
			iLength = 0;
		}
#if 0
		else {
			char *tp = (char *)pvBuffer;
			int32_t plen = (iLength > 128?128:iLength);
			printf("\n%s:%d=> recv len %d(%d), data:\n", __FUNCTION__, __LINE__, iLength, uSize);
			for(int i=0; i<plen; i++) {
				printf("%02x ", tp[i]);
			}
			printf("\n%s\n", (plen == iLength?" ":"...."));
			if(iLength > plen) {
				for(int i=iLength-1; i>(iLength-128); i--) {
					printf("%02x ", tp[i]);
				}
				printf("\n\n");
			}

		}
#endif

	}

	return iLength;
}

static void *tcp_client_task(void *p_arg)
{
	tcp_client_t *psClient	= (tcp_client_t*)p_arg;
	struct sockaddr_in	remoteAddr;
	struct timeval timeout;
	fd_set		readSet;
	int32_t		iRet;
	int32_t		iLength;
	uint32_t	uConnectCount;

	uConnectCount = 0;
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons(psClient->psCfg->wRemotePort);
	remoteAddr.sin_addr.s_addr = htonl(psClient->psCfg->uRemoteIp);

	for( ; ; )
	{
		if( psClient->bFlagClose )
			break;

		timeout.tv_sec			= 0;
		timeout.tv_usec			= 500 * 1000;
		setsockopt(psClient->iSocket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
		// connect to the server
		iRet = connect(psClient->iSocket, (struct sockaddr *)&(remoteAddr), sizeof(remoteAddr));
		if (iRet < 0)
		{
			printf( "tcp connect %d %s::%d\r\n", iRet, inet_ntoa(remoteAddr.sin_addr), psClient->psCfg->wRemotePort );
			if( psClient->psCfg->wConnectTime )
			{
				if( psClient->psCfg->wConnectTime <= ++uConnectCount )
					break;
			}
			close( psClient->iSocket );
			psClient->iSocket = socket(AF_INET, SOCK_STREAM, 0);
			sleep( 1 );
		}
		else
		{
			psClient->bFlagConnect	= true;
			timeout.tv_sec			= TCP_CLIENT_TIMEOUT_VAL;
			timeout.tv_usec			= 0;
			uConnectCount			= 0;
			if( psClient->psCbk->pv_cbk_connect )
				psClient->psCbk->pv_cbk_connect( psClient->psCbk->psHandle );
			for( ; ; )
			{
				/* waiting for data */
				FD_ZERO(&readSet);
				FD_SET(psClient->iSocket, &readSet);
				timeout.tv_sec			= TCP_CLIENT_TIMEOUT_VAL;
				timeout.tv_usec			= 0;
				iRet = select(psClient->iSocket + 1, &readSet, NULL, NULL, &timeout);

				if( 0 < iRet )
				{
					// read the data
					if(FD_ISSET(psClient->iSocket, &readSet))
					{
						if( psClient->psCbk->pv_cbk_data )
						{
							psClient->psCbk->pv_cbk_data( psClient->psCbk->psHandle, psClient, tcp_client_recv );
						}
						else
						{
							uint8_t abyBuffer[128];
							int32_t iLength = tcp_client_recv( psClient, abyBuffer, sizeof(abyBuffer) );
							if( 0 >= iLength )
								break;
						}
						if( false == psClient->bFlagConnect )
							break;
					}
				}
				else if( 0 > iRet )
				{
					// error, reconnect
					if (errno == EBADF && FD_ISSET(psClient->iSocket, &readSet))
						break;
				} else { //0 == iRet
					iRet = get_netcard_status();
					if(iRet == NETLINK_STATUS_DISABLE) {
						break;
					}
				}
				if( psClient->bFlagClose )
					break;
			}
			// disconnect, reconnect
			psClient->bFlagConnect	= false;
			if( psClient->psCbk->pv_cbk_disconnect )
				psClient->psCbk->pv_cbk_disconnect( psClient->psCbk->psHandle );
		}

	}

	if( 0 <= psClient->iSocket )
		close( psClient->iSocket );
	psClient->iSocket 	= -1;
	psClient->bFlagInit	= false;
	psClient->bFlagClose = false;
	pthread_mutex_destroy( &psClient->mutex );
	if( psClient->psCbk->pv_cbk_delete )
		psClient->psCbk->pv_cbk_delete( psClient->psCbk->psHandle );
	return 0;
}

int32_t tcp_client_create( tcp_client_t *psClient, tcp_client_cfg_t *psCfg, tcp_client_cbk_t *psCbk )
{
	int32_t eRet = 0;

	if( false == psClient->bFlagInit )
	{
		psClient->bFlagInit	= true;
		psClient->bFlagClose = false;
		psClient->bFlagConnect = false;
		if( psCfg && psCbk )
		{
			psClient->psCfg = psCfg;
			psClient->psCbk = psCbk;
			psClient->iSocket = socket(AF_INET, SOCK_STREAM, 0);
			if( 0 <= psClient->iSocket )
			{
				int keepalive = 1;
				int keepidle = 1;
				int keepinterval = 1;
				int keepcount = 5;
				struct timeval tv_timeout = {0};

				tv_timeout.tv_sec = TCP_CLIENT_TIMEOUT_VAL;
				tv_timeout.tv_usec = 0;
				setsockopt(psClient->iSocket, SOL_SOCKET, SO_KEEPALIVE, &tv_timeout, sizeof(struct timeval));

				setsockopt(psClient->iSocket, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));
				setsockopt(psClient->iSocket, IPPROTO_TCP, TCP_KEEPIDLE, &keepidle, sizeof(keepidle));
				setsockopt(psClient->iSocket, IPPROTO_TCP, TCP_KEEPINTVL, &keepinterval, sizeof(keepinterval));
				setsockopt(psClient->iSocket, IPPROTO_TCP, TCP_KEEPCNT, &keepcount, sizeof(keepcount));
				if( 0 == pthread_mutex_init( &(psClient->mutex), NULL ) )
				{
					if( 0 == pthread_create(&(psClient->pid), NULL, tcp_client_task, psClient) )
					{
						eRet = 0;
					}
					else
					{
						pthread_mutex_destroy( &psClient->mutex );
						close( psClient->iSocket );
						psClient->bFlagInit	= false;
						eRet = -2;
					}
				}
				else
				{
					close( psClient->iSocket );
					psClient->bFlagInit	= false;
					eRet = -2;
				}
			}
			else
			{
				psClient->bFlagInit = false;
				eRet = -2;
			}
		}
		else
		{
			psClient->bFlagInit	= false;
			eRet = -1;
		}
	}

	return eRet;
}

int32_t tcp_client_send( tcp_client_t *psClient, void *pvData, uint32_t uLength )
{
	int32_t eRet = 0;
	
	if( psClient && pvData && psClient->bFlagConnect )
	{
		pthread_mutex_lock( &psClient->mutex );
		send( psClient->iSocket, pvData, uLength, 0);
		pthread_mutex_unlock( &psClient->mutex );
	}
	else
	{
		eRet = -2;
	}

	return eRet;
}

int32_t tcp_client_close( tcp_client_t *psClient )
{
	int32_t eRet = 0;

	if( psClient && psClient->bFlagConnect )
	{
		psClient->bFlagClose = true;
	}

	return eRet;
}


