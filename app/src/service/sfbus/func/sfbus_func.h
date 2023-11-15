
#ifndef __SFBUS_FUNC_H__
#define __SFBUS_FUNC_H__

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

#include "../sfbus.h"

int sfbus_func_function_add_listen( void* pvFunction, void* pvListenArg, void (*pv_listen)(void* pvListenArg,void* pvOutputParam,int32_t iOutputParamLength) );

int sfbus_func_function_del_listen( void* pvFunction, void* pvListenArg, void (*pv_listen)(void* pvListenArg,void* pvOutputParam,int32_t iOutputParamLength) );

int sfbus_func_function_run( void* pvFunction, int eLevel, struct sfbus_memory* psInputParam, 
							struct sfbus_memory* psArgMemory, void (*pv_output)(void* psArg,void* pvOutputParam,int32_t iOutputParamLength) );

void* sfbus_func_function_get_handle( int eFunctionId );

void* sfbus_func_function_register( int eFunctionId, uint32_t 	uTiggerTimeMs, const struct sfbus_function_description* psDescription, 
									int (*pi_function)(void* pvOutputArg,void* pvInputParam,int32_t iInputParamLength) );

int sfbus_func_init( void );

#endif /* __SFBUS_FUNC_H__ */

