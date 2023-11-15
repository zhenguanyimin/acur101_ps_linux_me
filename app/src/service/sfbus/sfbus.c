
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

#include "func/sfbus_func.h" 
#include "altp/sfbus_altp.h"
#include "cli/sfbus_cli.h"
#include "remote/sfbus_remote.h"

int sfbus_init( void )
{
    sfbus_func_init();
	sfbus_altp_init();
	sfbus_cli_init();
	sfbus_remote_init();
	return 0;
}

void* sfbus_register_function( int eFunctionId, uint32_t 	uTiggerTimeMs, const struct sfbus_function_description* psDescription, 
									int (*pi_function)(void* pvOutputArg,void* pvInputParam,int32_t iInputParamLength) )
{
	void* pvFunction = sfbus_func_function_register( eFunctionId, uTiggerTimeMs, psDescription, pi_function );
	if( pvFunction )
	{
		sfbus_cli_register( eFunctionId, pvFunction, (void*)psDescription );
		sfbus_remote_register( eFunctionId, pvFunction, (void*)psDescription );
	}

	return pvFunction;
}

int sfbus_run_function( void* psFunction, int eLevel, struct sfbus_memory* psInputParam )
{
	return sfbus_func_function_run( psFunction, eLevel, psInputParam, NULL, NULL );
}

