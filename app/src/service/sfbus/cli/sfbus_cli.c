
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../sfbus.h"

#define SFBUS_CLI_BUFFER_SIZE	(5*1024)

typedef struct sfbus_cli
{
	char strBuffer[SFBUS_CLI_BUFFER_SIZE];
}sfbus_cli_t;

inline static 
void* sfbus_cli_protocol_create( void* pvConfig )
{
	struct sfbus_cli* psCli = (struct sfbus_cli*)malloc( sizeof(struct sfbus_cli) );
	
	do{
		if( NULL == psCli )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "malloc err" );
			break;
		}

	}while( 0 );

	return (void*)psCli;
}

inline static 
int sfbus_cli_protocol_delete( void* pvProtocolArg )
{
	if( NULL == pvProtocolArg )
	{
		sfbus_log( SFBUS_LOG_LEVEL_ERR, "pvProtocolArg == null" );
		return 1;
	}
	else
	{
		free( pvProtocolArg );
		return 0;
	}
}

inline static 
int sfbus_cli_recv_anlysis( void* pvProtocolArg, struct sfbus_protocol_func* psFunc, struct sfbus_memory** ppsData, struct sfbus_memory** ppsInfo )
{
	struct sfbus_cli* psCli = (struct sfbus_cli*)(pvProtocolArg);

	
}

inline static 
int sfbus_cli_package_send( void* pvProtocolArg, struct sfbus_protocol_func* psFunc, int eCommandId, struct sfbus_memory* psData, struct sfbus_memory* psInfo )
{
	int eRet = 0;

	do
	{
		if( NULL == psData )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "psData == null" );
			break;
		}

		eRet = psFunc->pi_send_data( psFunc->pvPortArg, psData->pvDataIndex, psData->iDataLength );
	}

	return eRet;
}

inline static 
struct sfbus_memory* sfbus_cli_data_to_param( void const* pvCommamdInfo, struct sfbus_memory* psInputData )
{
	
}

inline static 
struct sfbus_memory* sfbus_cli_param_to_data( int32_t iReserveSize, int32_t iDataOffset, void const* pvCommamdInfo, void* pvOutputParam, int32_t iOutputParamLength )
{
	
}

int sfbus_cli_init( void )
{
	
}

int sfbus_cli_register( int eFunctionId, void* pvFunction, void* pvDescription )
{
	
}

