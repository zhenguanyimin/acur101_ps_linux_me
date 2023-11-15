
#ifndef __CONNECT_H__
#define __CONNECT_H__

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

typedef enum connect_mode
{
	CONNECT_MODE_NULL		= (0),
	CONNECT_MODE_TCP_CLIENT	= (1),
	CONNECT_MODE_TCP_SERVER	= (2),	
	CONNECT_MODE_UDP		= (3),
}connect_mode_t;

/* socket */
typedef struct connect_socket_cfg
{
	uint8_t		cbMode;
	union
	{
		struct
		{
			bool		bBroadcast;
			uint16_t	wLocalPort;
			uint16_t	wRemotePort;
			uint32_t	uLocalIp;
			uint32_t	uRemoteIp;
		}udp;
		struct
		{
			uint16_t	wLocalPort;
			uint16_t	wRemotePort;
			uint32_t	uLocalIp;
			uint32_t	uRemoteIp;
		}tcp;
	};
}connect_socket_cfg_t;

typedef struct connect_socket_cbk
{
	void*	pvArg;
	void	(*pv_cbk_connect)(void* pvArg);
	void	(*pv_cbk_recv)(void* pvArg, int32_t iSize);
	void	(*pv_cbk_disconnect)(void* pvArg);
	void	(*pv_cbk_delete)(void* pvArg);
}connect_socket_cbk_t;

void* connect_socket_crate( struct connect_socket_cfg* psCfg, struct connect_socket_cbk* psCbk );
int connect_socket_close( void* pvSocket );
int connect_socket_send( void* pvSocket, void* pvData, int32_t iLen );
uint32_t connect_socket_recv( void* pvSocket, void* pvBuffer, uint32_t uSize );

/* autolink */
typedef struct connect_autolink_fun
{
	void*		pvSocket;
	int 		(*pi_close)( void* pvSocket );
	int 		(*pi_send)( void* pvSocket, void* pvData, int32_t uLen );
	int 		(*pi_recv)( void* pvSocket, void* pvBuffer, int32_t iSize );
}connect_autolink_fun_t;

typedef struct connect_autolink_cfg
{
	uint8_t		cbLinkMode;
	uint8_t		cbLinkMax;
	uint16_t	wLocalPort;
	uint16_t	wType;
}connect_autolink_cfg_t;

typedef struct connect_autolink_cbk
{
	void*	(*pi_create)( struct connect_autolink_fun* psFun );
	void	(*pv_cbk_connect)(void* pvArg);
	void	(*pv_cbk_recv)(void* pvArg,int32_t (*pi_recv)(void* pvBuffer,int32_t iSize));
	void	(*pv_cbk_disconnect)(void* pvArg);
	void	(*pv_delete)( void* pvArg );
}connect_autolink_cbk_t;

int connect_autolink_register( struct connect_autolink_cfg* psCfg, struct connect_autolink_cbk* psCbk );

#endif /* __CONNECT_H__ */

