
#ifndef __SFBUS_H__
#define __SFBUS_H__

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <syslog.h>

/*******************************************************************************
@description:	log
*******************************************************************************/
#define SFBUS_LOG_LEVEL_EMERG		(LOG_EMERG)		//紧急情况
#define SFBUS_LOG_LEVEL_ALERT		(LOG_ALERT)		//高优先级故障，例如数据库崩溃
#define SFBUS_LOG_LEVEL_CRIT		(LOG_CRIT)		//严重错误，例如硬件故障
#define SFBUS_LOG_LEVEL_ERR			(LOG_ERR)		//错误
#define SFBUS_LOG_LEVEL_WARNING		(LOG_WARNING)	//警告
#define SFBUS_LOG_LEVEL_NOTICE		(LOG_NOTICE)	//需要注意的特殊情况
#define SFBUS_LOG_LEVEL_INFO		(LOG_INFO)		//一般信息
#define SFBUS_LOG_LEVEL_DEBUG		(LOG_DEBUG)		//调试信息

#define SFBUS_LOG_OUTPUT_FUNC		(0)

#if (SFBUS_LOG_OUTPUT_FUNC)
#define sfbus_log(eLevel, strFormat, ...) \
	do { \
        syslog(eLevel, "[%d:%s:%d] " strFormat "\n", (int)pthread_self(), __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)
#else
#define sfbus_log(eLevel, strFormat, ...) \
	do { \
        syslog(eLevel, strFormat "\n", ##__VA_ARGS__); \
    } while (0)
#endif

/*******************************************************************************
@description:	memory
*******************************************************************************/
typedef struct sfbus_memory
{
	int32_t		iBufferSize;
	int32_t		iDataLength;
	void*		pvDataIndex;
	char		acbBuffer[0];
}sfbus_memory_t;

inline static
struct sfbus_memory* sfbus_memory_malloc( int32_t iBufferSize )
{
	do{
		struct sfbus_memory* psMemory = (struct sfbus_memory*)malloc( sizeof(struct sfbus_memory) + iBufferSize );
		if( psMemory )
		{
			psMemory->iBufferSize = iBufferSize;
			psMemory->iDataLength = 0;
			psMemory->pvDataIndex = psMemory->acbBuffer;
		}
		return psMemory;
	}while( 0 );
}

inline static 
void sfbus_memory_free( struct sfbus_memory* psMemory )
{
	do
	{
		if( psMemory )
			free( psMemory );
	}while( 0 );
}

/*******************************************************************************
@description:	function
*******************************************************************************/
typedef struct sfbus_function_description
{
	const char* strPthreadName;
	const char* strFunctionName;
	const char* strInputParam;
	const char* strOutputParam;

	const char* strCliCommand;
	const char* strCliOption;
	const char* strCliInputFormat;
	const char* strCliOutputFormat;
}sfbus_function_description_t;

void sfbus_function_output( void* pvOutputArg, void* pvOutputParam, int32_t iOutputParamLength );

void* sfbus_register_function( int eFunctionId, uint32_t 	uTiggerTimeMs, const struct sfbus_function_description* psDescription, 
									int (*pi_function)(void* pvOutputArg,void* pvInputParam,int32_t iInputParamLength) );

int sfbus_run_function( void* psFunction, int eLevel, struct sfbus_memory* psInputParam );

/*******************************************************************************
@description:	protocol
*******************************************************************************/
typedef struct sfbus_protocol_func
{
	void*	pvPortArg;
	int32_t	(*pi_send_data)(void* pvPortArg,void* pvData,int32_t iDataLength);
	int32_t	(*pi_recv_data)(void* pvPortArg,void* pvBuffer,int32_t iBufferSize);	
}sfbus_protocol_func_t;

int sfbus_register_protocol( int eProtocolId, int32_t iCommandIndexTableSize, int32_t iCommandDataOffset, int32_t iProtocolNoPayloadSize, 
								void* (*pv_protocol_create)(void* pvConfig), 
								int (*pi_protocol_delete)(void* pvProtocolArg), 
								int (*pi_recv_anlysis)(void* pvProtocolArg,struct sfbus_protocol_func* psFunc,struct sfbus_memory** ppsData,struct sfbus_memory** ppsInfo), 
								int (*pi_package_send)(void* pvProtocolArg,struct sfbus_protocol_func* psFunc,int eCommandId,struct sfbus_memory* psData,struct sfbus_memory* psInfo) );

/*******************************************************************************
@description:	command
*******************************************************************************/
int sfbus_register_command( int eCommandId, int eProtocolId, int eFunctionId, void const* pvCommandInfo, 
							struct sfbus_memory* (*ps_data_to_param)(void const* pvCommamdInfo,struct sfbus_memory* psInputData), 
							struct sfbus_memory* (*ps_param_to_data)(int32_t iReserveSize,int32_t iDataOffset,void const* pvCommamdInfo,void* pvOutputParam,int32_t iOutputParamLength) ); 

/*******************************************************************************
@description:	socket
*******************************************************************************/
void* sfbus_create_socket( int eProtocolId, void* pvProtocolConfig, void* pvPortArg, 
								int32_t	(*pi_recv_data)(void* pvPortArg,void* pvBuffer,int32_t iBufferSize), 
								int32_t	(*pi_send_data)(void* pvPortArg,void* pvData,int32_t iDataLength) );

int sfbus_delete_socket( void* pvSocket );

void sfbus_socket_recv_anlysis( void* pvSocket );

int sfbus_socket_add_command( void* pvSocket, int eCommandId );

int sfbus_socket_listen_command( void* pvSocket, int eCommandId );

/*******************************************************************************
@description:	init
*******************************************************************************/
int sfbus_init( void );

#endif /* __SFBUS_H__ */

