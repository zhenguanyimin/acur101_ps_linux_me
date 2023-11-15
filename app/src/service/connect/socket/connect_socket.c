
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include "../connect.h"

void* connect_socket_crate( struct connect_socket_cfg* psCfg, struct connect_socket_cbk* psCbk )
{
	return NULL;
}

int connect_socket_close( void* pvSocket )
{
	return -1;
}

int connect_socket_send( void* pvSocket, void* pvData, int32_t iLen )
{
	return -1;
}

uint32_t connect_socket_recv( void* pvSocket, void* pvBuffer, uint32_t uSize )
{
	return 0;
}

