
#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

typedef enum protocol_type
{
	PROTOCOL_TYPE_NULL		= (0),
	PROTOCOL_TYPE_SKYFEND	= (1),
	PROTOCOL_TYPE_ALINK		= (2),
}protocol_type_t;

int protocol_init( void ) ;

/* command */
typedef struct protocol_command
{
	uint8_t 	cbType;
	uint8_t		cbMaster;
	uint16_t	wOrder;
	int 		(*pi_recv_handler)(void* pvData, uint32_t uLen );
	uint32_t 	(*pu_send_handler)(void* pvBuffer, uint32_t uSize, void* pvData, uint32_t uLen);
}protocol_command_t;

int protocol_register_command( struct protocol_command* psCommand );
int protocol_upload_data( enum protocol_type eType, uint16_t wOrder, void* pvData, uint32_t uLen );

/* socket */
typedef struct protocol_cfg
{
	uint8_t 	cbType;
	uint16_t	wLocalId;
	uint16_t	wRemoteId;
	uint32_t 	uRecvSize;
	void		*pvArg;
	int			(*pi_send)(void* pvArg, void* pvData, uint32_t uLen);
	uint32_t	(*pu_recv)(void* pvArg, void* pvBuffer, uint32_t uSize);
}protocol_cfg_t;

void* protocol_create_socket( struct protocol_cfg* psCfg );
int protocol_delete_socket( void* pvArg );
int protocol_connect_command( void* pvArg, uint16_t wOrder );

#endif /* __PROTOCOL_H__ */

