
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <semaphore.h>
#include <pthread.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../index/sfbus_index.h"
#include "../func/sfbus_func.h"
#include "../sfbus.h"

inline static 
mqd_t sfbus_altp_queue_create( char* strQueueName, int32_t iQueueSize, int32_t iDataLength )
{
	struct mq_attr attr;
	attr.mq_maxmsg = iQueueSize;
	attr.mq_msgsize = iDataLength;
	mqd_t mq = mq_open( strQueueName, O_CREAT|O_RDWR|O_CLOEXEC, 0666, &attr );
	if( 0 > mq )
		sfbus_log( SFBUS_LOG_LEVEL_ERR, "create queue %s err %s", strQueueName, strerror(errno) );
	else
		sfbus_log( SFBUS_LOG_LEVEL_INFO, "create queue %s result %d", strQueueName, mq );
	return mq;
}

inline static 
int sfbus_altp_queue_delete( char* strQueueName, mqd_t mq )
{
	int eRet = -1;
	
	if( 0 > mq_close(mq) )
		sfbus_log( SFBUS_LOG_LEVEL_ERR, "close queue %d err %s", mq, strerror(errno) );
	else
		sfbus_log( SFBUS_LOG_LEVEL_INFO, "close queue %d", mq );
	
	if( 0 > mq_unlink(strQueueName) )
	{
		sfbus_log( SFBUS_LOG_LEVEL_ERR, "unlink queue %s err %s", strQueueName, strerror(errno) );
	}
	else
	{
		sfbus_log( SFBUS_LOG_LEVEL_INFO, "unlink queue %s", strQueueName );
		eRet = 0;
	}

	return eRet;
}

/*******************************************************************************
@description:	pthread send
*******************************************************************************/
typedef struct sfbus_altp_package_send_info
{
	int		eSocketId;
	void*	pvSocket;
	void* 	pvCommand;
	struct sfbus_memory* psInfo;
	struct sfbus_memory* psData;
}sfbus_altp_package_send_info_t;

typedef struct sfbus_altp_pthread_package_send
{	
	mqd_t 		mq;
	pthread_t 	pid;

	int32_t		iQueueSize;
	char		strQueueName[0];
}sfbus_altp_pthread_package_send_t;

inline static 
void sfbus_socket_package_send( struct sfbus_memory* psSendInfoMemory );

inline static 
void* sfbus_altp_pthread_package_send( void* pvArg )
{	
	struct sfbus_altp_pthread_package_send* psPthread = (struct sfbus_altp_pthread_package_send*)(pvArg);
	for( ; ; )
	{
		struct sfbus_memory* psSendInfoMemory;
		struct mq_attr attr;
		mq_getattr( psPthread->mq, &attr );
		int count = mq_receive( psPthread->mq, (char*)(&psSendInfoMemory), attr.mq_msgsize, NULL);

		if( psSendInfoMemory )
		{
			sfbus_socket_package_send( psSendInfoMemory );
		}
		else
		{
			break;
		}
	}

	sfbus_log( SFBUS_LOG_LEVEL_NOTICE, "pthread exit, pid = %d", (int)psPthread->pid );

	sfbus_altp_queue_delete( psPthread->strQueueName, psPthread->mq );

	return NULL;
}

inline static 
int sfbus_altp_pthread_package_send_memory( struct sfbus_altp_pthread_package_send* psPthread, struct sfbus_memory* psSendInfoMemory )
{
 	struct mq_attr attr;
	mq_getattr( psPthread->mq, &attr );
	if( 0 > mq_send(psPthread->mq,(char*)(&psSendInfoMemory),attr.mq_msgsize,1) )
	{
		sfbus_log( SFBUS_LOG_LEVEL_ERR, "send err %p", psSendInfoMemory );

		struct sfbus_altp_package_send_info* psSendInfo = (struct sfbus_altp_package_send_info*)psSendInfoMemory->pvDataIndex;
		sfbus_memory_free( psSendInfo->psInfo );
		sfbus_memory_free( psSendInfo->psData );
		sfbus_memory_free( psSendInfoMemory );
	}
	else
	{
		sfbus_log( SFBUS_LOG_LEVEL_DEBUG, "send memory %p", psSendInfoMemory );
	}
}

inline static 
void* sfbus_altp_pthread_package_send_create( char const* strQueueNmae, int32_t iQueueSize )
{
	struct sfbus_altp_pthread_package_send* psPthread = NULL;

	do{
		if( NULL == strQueueNmae )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "strQueueNmae == null" );
			break;
		}

		psPthread = (struct sfbus_altp_pthread_package_send*)malloc( sizeof(struct sfbus_altp_pthread_package_send) + strlen(strQueueNmae) + 1 );
		if( NULL == psPthread )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "malloc err" );
			break;
		}

		psPthread->iQueueSize	= iQueueSize;
		memcpy( psPthread->strQueueName, strQueueNmae, strlen(strQueueNmae) + 1 );
		psPthread->mq = sfbus_altp_queue_create( psPthread->strQueueName, psPthread->iQueueSize, sizeof(struct sfbus_altp_package_send_info) );
		if( 0 > psPthread->mq )
		{
			free( psPthread );
			psPthread = NULL;
			break;
		}

		if( 0 > pthread_create(&(psPthread->pid),NULL,sfbus_altp_pthread_package_send,psPthread) )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "create pthread err %s", strerror(errno) );
		
			sfbus_altp_queue_delete( psPthread->strQueueName, psPthread->mq );

			free( psPthread );
			psPthread = NULL;
			break;
		}
	}while( 0 );

	return (void*)psPthread;
}

inline static 
int sfbus_altp_pthread_package_send_delete( void* pvPthread )
{
	int eRet = 0;

	do{
		if( NULL == pvPthread )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "pvPthread == null" );
			eRet = -1;
			break;
		}
		
		struct sfbus_altp_pthread_package_send* psPthread = (struct sfbus_altp_pthread_package_send*)pvPthread;

		eRet = sfbus_altp_pthread_package_send_memory( psPthread, NULL );
		if( 0 > eRet )
		{
			break;
		}
	}while( 0 );

	return eRet;
}

/*******************************************************************************
@description:	altp init
*******************************************************************************/
#define SFBUS_ALTP_PROTOCOL_INDEX_MAX		(20)
#define SFBUS_ALTP_SOCKET_INDEX_MAX			(50)

#define SFBUS_ALTP_PTHREAD_ALLOCATION_QUEUE_SZIE	(1000)
#define SFBUS_ALTP_PTHREAD_ALLOCATION_QUEUE_NAME	"/altp_send"

typedef struct sfbus_altp
{
	int			bInitFlag;
	mqd_t 		mq; 
	pthread_t 	pid;

	void* pvProtocolIndexTable;
	void* pvSocketIndexTable;
}sfbus_altp_t;

struct sfbus_altp sSfbusAltp = { 0 };

inline static 
void* sfbus_altp_pthread_allocation( void* pvArg )
{	
	struct sfbus_altp* psSfbusAltp = (struct sfbus_altp*)(pvArg);
	for( ; ; )
	{
		struct sfbus_memory* psSendInfoMemory;
		struct mq_attr attr;
		mq_getattr( psSfbusAltp->mq, &attr );
		int count = mq_receive( psSfbusAltp->mq, (char*)(&psSendInfoMemory), attr.mq_msgsize, NULL);

		struct sfbus_altp_package_send_info* psSendInfo = (struct sfbus_altp_package_send_info*)psSendInfoMemory->pvDataIndex;
		psSendInfo->pvSocket = sfbus_index_find_id( psSendInfo->eSocketId, psSfbusAltp->pvSocketIndexTable );
		if( psSendInfo->pvSocket )
		{
			sfbus_altp_pthread_package_send_memory( psSendInfo->pvSocket, psSendInfoMemory );
		}
		else
		{
			sfbus_memory_free( psSendInfo->psInfo );
			sfbus_memory_free( psSendInfo->psData );
			sfbus_memory_free( psSendInfoMemory );
		}
	}

	sfbus_log( SFBUS_LOG_LEVEL_NOTICE, "pthread exit, pid = %d", (int)(psSfbusAltp->pid) );

	return NULL;
}

inline static 
void sfbus_altp_allocation_send_memory( struct sfbus_memory* psSendInfoMemory )
{
	struct mq_attr attr;
	mq_getattr( sSfbusAltp.mq, &attr );
	if( 0 > mq_send(sSfbusAltp.mq,(char*)(&psSendInfoMemory),attr.mq_msgsize,1) )
	{
		sfbus_log( SFBUS_LOG_LEVEL_ERR, "send err" );

		struct sfbus_altp_package_send_info* psSendInfo = (struct sfbus_altp_package_send_info*)psSendInfoMemory->pvDataIndex;
		sfbus_memory_free( psSendInfo->psInfo );
		sfbus_memory_free( psSendInfo->psData );
		sfbus_memory_free( psSendInfoMemory );
	}
}

int sfbus_altp_init( void )
{
	int eRet = 0;

	if( NULL == sSfbusAltp.pvProtocolIndexTable )
	{
		sSfbusAltp.pvProtocolIndexTable = sfbus_index_create( SFBUS_ALTP_PROTOCOL_INDEX_MAX );
	}

	if( NULL == sSfbusAltp.pvSocketIndexTable )
	{
		sSfbusAltp.pvSocketIndexTable = sfbus_index_create( SFBUS_ALTP_SOCKET_INDEX_MAX );
	}

	if( false == sSfbusAltp.bInitFlag )
	{
		sSfbusAltp.mq = sfbus_altp_queue_create( SFBUS_ALTP_PTHREAD_ALLOCATION_QUEUE_NAME, SFBUS_ALTP_PTHREAD_ALLOCATION_QUEUE_SZIE, sizeof(struct sfbus_memory*) );
		if( 0 <= sSfbusAltp.mq )
		{
			if( 0 > pthread_create(&(sSfbusAltp.pid), NULL, sfbus_altp_pthread_allocation, &(sSfbusAltp)) )
			{
				sfbus_log( SFBUS_LOG_LEVEL_ERR, "create pthread err %s", strerror(errno) );
			
				sfbus_altp_queue_delete( SFBUS_ALTP_PTHREAD_ALLOCATION_QUEUE_NAME, sSfbusAltp.mq );
			}
			else
			{
				sSfbusAltp.bInitFlag = true;
			}
		}
	}

	if( NULL == sSfbusAltp.pvProtocolIndexTable || NULL == sSfbusAltp.pvSocketIndexTable || false == sSfbusAltp.bInitFlag )
	{
		sfbus_log( SFBUS_LOG_LEVEL_ALERT, "index table %p %p", sSfbusAltp.pvProtocolIndexTable, sSfbusAltp.pvSocketIndexTable );
		eRet = -1;
	}

	return eRet;
}

/*******************************************************************************
@description:	command
*******************************************************************************/
typedef struct sfbus_altp_command
{
	int		eCommandId;
	void*	pvFunction;
	
	void const*	pvCommandInfo;

	struct sfbus_memory* (*ps_data_to_param)(void const* pvCommamdInfo,struct sfbus_memory* psInputData);
	struct sfbus_memory* (*ps_param_to_data)(int32_t iReserveSize,int32_t iDataOffset,void const* pvCommamdInfo,void* pvOutputParam,int32_t iOutputParamLength);
}sfbus_altp_command_t;

/*******************************************************************************
@description:	protocol
*******************************************************************************/
typedef struct sfbus_altp_protocol
{
	int32_t	iCommandDataOffset;
	int32_t	iCommandIndexTableSize;
	int32_t	iProtocolNoPayloadSize;

	void*	pvCommandIndexTable;

	void* (*pv_protocol_create)(void* pvConfig); 
	int (*pi_protocol_delete)(void* pvProtocolArg); 
	int (*pi_recv_anlysis)(void* pvProtocolArg,struct sfbus_protocol_func* psFunc,struct sfbus_memory** ppsData,struct sfbus_memory** ppsInfo); 
	int (*pi_package_send)(void* pvProtocolArg,struct sfbus_protocol_func* psFunc,int eCommandId,struct sfbus_memory* psData,struct sfbus_memory* psInfo); 
}sfbus_altp_protocol_t;

int sfbus_register_protocol( int eProtocolId, int32_t iCommandIndexTableSize, int32_t iCommandDataOffset, int32_t iProtocolNoPayloadSize, 
								void* (*pv_protocol_create)(void* pvConfig), 
								int (*pi_protocol_delete)(void* pvProtocolArg), 
								int (*pi_recv_anlysis)(void* pvProtocolArg,struct sfbus_protocol_func* psFunc,struct sfbus_memory** ppsData,struct sfbus_memory** ppsInfo), 
								int (*pi_package_send)(void* pvProtocolArg,struct sfbus_protocol_func* psFunc,int eCommandId,struct sfbus_memory* psData,struct sfbus_memory* psInfo) )
{
	int eRet = 0;

	do{
		if( NULL == pv_protocol_create )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "pv_protocol_create == null" );
			eRet = -1;
			break;
		}

		if( NULL == pi_protocol_delete )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "pi_protocol_delete == null" );
			eRet = -1;
			break;
		}
						
		if( NULL == pi_recv_anlysis )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "pi_recv_anlysis == null" );
			eRet = -1;
			break;
		}

		if( NULL == pi_package_send )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "pi_package_send == null" );
			eRet = -1;
			break;
		}

		if( 0 == iCommandIndexTableSize )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "iCommandIndexTableSize == 0" );
			eRet = -1;
			break;
		}

		struct sfbus_altp_protocol* psProtocol = sfbus_index_find_id( eProtocolId, &(sSfbusAltp.pvProtocolIndexTable) );
		if( psProtocol )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "protocol %d exist", eProtocolId );
			eRet = -1;
			break;
		}

		psProtocol = (struct sfbus_altp_protocol*)malloc( sizeof(struct sfbus_altp_protocol) );
		if( NULL == psProtocol )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "malloc err" );
			eRet = -1;
			break;
		}

		psProtocol->pvCommandIndexTable = sfbus_index_create( iCommandIndexTableSize );
		if( NULL == psProtocol->pvCommandIndexTable )
		{
			free( psProtocol );
			psProtocol = NULL;
			eRet = -1;
			break;
		}

		psProtocol->iCommandDataOffset		= iCommandDataOffset;
		psProtocol->iProtocolNoPayloadSize	= iProtocolNoPayloadSize;
		psProtocol->iCommandIndexTableSize	= iCommandIndexTableSize;
		psProtocol->pv_protocol_create		= pv_protocol_create;
		psProtocol->pi_protocol_delete		= pi_protocol_delete;
		psProtocol->pi_recv_anlysis			= pi_recv_anlysis;
		psProtocol->pi_package_send			= pi_package_send;

		eRet = sfbus_index_add_id( eProtocolId, psProtocol, &(sSfbusAltp.pvProtocolIndexTable) );
		if( 0 > eRet )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "add index err %d", eRet );
			sfbus_index_delete( psProtocol->pvCommandIndexTable );
			free( psProtocol );
			psProtocol = NULL;
			break;
		}

		sfbus_log( SFBUS_LOG_LEVEL_INFO, "create protocol %d", eProtocolId );
	}while( 0 );

	return eRet;
}

int sfbus_register_command( int eCommandId, int eProtocolId, int eFunctionId, void const* pvCommandInfo, 
								struct sfbus_memory* (*ps_data_to_param)(void const* pvCommamdInfo,struct sfbus_memory* psInputData), 
								struct sfbus_memory* (*ps_param_to_data)(int32_t iReserveSize,int32_t iDataOffset,void const* pvCommamdInfo,void* pvOutputParam,int32_t iOutputParamLength) )
{
	int eRet = -1;

	do{	
		struct sfbus_altp_protocol* psProtocol = sfbus_index_find_id( eProtocolId, &(sSfbusAltp.pvProtocolIndexTable) );
		if( NULL == psProtocol )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "no protocol %d", eProtocolId );
			break;
		}
		
		if( NULL == ps_data_to_param )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "ps_data_to_param == null" );
			break;
		}

		if( NULL == ps_param_to_data )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "ps_param_to_data == null" );
			break;
		}

		struct sfbus_altp_command* psCommand = (struct sfbus_altp_command*)sfbus_index_find_id( eCommandId, psProtocol->pvCommandIndexTable );
		if( psCommand )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "command %d exist", eCommandId );
			psCommand = NULL;
			break;
		}

		psCommand = (struct sfbus_altp_command*)malloc( sizeof(struct sfbus_altp_command) );
		if( NULL == psCommand )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "malloc err" );
			break;
		}

		psCommand->eCommandId		= eCommandId;
		psCommand->pvCommandInfo	= pvCommandInfo;
		psCommand->ps_data_to_param	= ps_data_to_param;
		psCommand->ps_param_to_data	= ps_param_to_data;
		psCommand->pvFunction 		= sfbus_func_function_get_handle( eFunctionId );
		if( NULL == psCommand->pvFunction )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "no function id %d", eFunctionId );
			free( psCommand );
			psCommand = NULL;
			break;			
		}

		eRet = sfbus_index_add_id( eCommandId, psCommand, psProtocol->pvCommandIndexTable );
		if( 0 > eRet )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "add index err %d", eRet );
			free( psCommand );
			psCommand = NULL;
			break;
		}

		sfbus_log( SFBUS_LOG_LEVEL_INFO, "create command %d", eCommandId );
	}while( 0 );

	return eRet;
}

/*******************************************************************************
@description:	socket send
*******************************************************************************/
typedef struct sfbus_altp_socket_output_arg
{
	int32_t	iInfoLength;
	int		eSocketId;
	struct sfbus_altp_command* psCommand;
	struct sfbus_altp_protocol* psProtocol;
	char	acbInfo[0];
}sfbus_altp_socket_output_arg_t;

inline static 
void sfbus_altp_socket_output( void* pvOutputArg,void* pvOutputParam,int32_t iOutputParamLength )
{
	struct sfbus_altp_socket_output_arg* psOutputArg = (struct sfbus_altp_socket_output_arg*)(pvOutputArg);

	struct sfbus_memory* psSendInfoMemory = sfbus_memory_malloc( sizeof(struct sfbus_altp_package_send_info) );
	if( psSendInfoMemory )
	{
		

		struct sfbus_altp_package_send_info* psSendInfo = (struct sfbus_altp_package_send_info*)psSendInfoMemory->pvDataIndex;
		if( psOutputArg->iInfoLength )
		{
			psSendInfo->psInfo	= sfbus_memory_malloc( psOutputArg->iInfoLength );
			if( psSendInfo->psInfo )
				memcpy( psSendInfo->psInfo->pvDataIndex, (void*)(psOutputArg->acbInfo), psOutputArg->iInfoLength );
		}
		else
		{
			psSendInfo->psInfo 	= NULL;
		}
		psSendInfo->psData 		= psOutputArg->psCommand->ps_param_to_data( psOutputArg->psProtocol->iProtocolNoPayloadSize, psOutputArg->psProtocol->iCommandDataOffset, psOutputArg->psCommand, pvOutputParam, iOutputParamLength );
		psSendInfo->pvSocket	= NULL;
		psSendInfo->eSocketId	= psOutputArg->eSocketId;
		psSendInfo->pvCommand	= psOutputArg->psCommand;

		sfbus_altp_allocation_send_memory( psSendInfoMemory );
	}
}

/*******************************************************************************
@description:	socket listen
*******************************************************************************/
typedef struct sfbus_altp_socket_listen_list
{
	struct sfbus_altp_socket_output_arg	sOutputArg;

	struct sfbus_altp_socket_listen_list* psNext; 
}sfbus_altp_socket_listen_list_t;

inline static 
void sfbus_altp_socket_listen_send( void* pvListenArg,void* pvOutputParam,int32_t iOutputParamLength )
{
	struct sfbus_altp_socket_listen_list* psListenList = (struct sfbus_altp_socket_listen_list*)(pvListenArg);

	sfbus_altp_socket_output( &(psListenList->sOutputArg), pvOutputParam, iOutputParamLength );
}

/*******************************************************************************
@description:	socket api
*******************************************************************************/
#define SFBUS_ALTP_SOCKET_QUEUE_SIZE	(50)

typedef struct sfbus_altp_socket
{
	int eSocketId;

	struct sfbus_protocol_func sFunc;

	struct sfbus_memory* psInfo;
	struct sfbus_memory* psData;
	
	void* pvProtocolArg;
	struct sfbus_altp_protocol* psProtocol;
	void* pvCommandIndexTable;

	struct sfbus_altp_pthread_package_send* psPthread;

	struct sfbus_altp_socket_listen_list* psListenListHead;
	struct sfbus_altp_socket_listen_list* psListenListLast;
}sfbus_altp_socket_t;

void* sfbus_create_socket( int eProtocolId, void* pvProtocolConfig, void* pvPortArg, 
								int32_t	(*pi_recv_data)(void* pvPortArg,void* pvBuffer,int32_t iBufferSize), 
								int32_t	(*pi_send_data)(void* pvPortArg,void* pvData,int32_t iDataLength) )
{
	struct sfbus_altp_socket* psSocket = NULL;

	do{
		struct sfbus_altp_protocol* psProtocol = sfbus_index_find_id( eProtocolId, &(sSfbusAltp.pvProtocolIndexTable) );
		if( NULL == psProtocol )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "no protocol %d", eProtocolId );
			break;
		}

		if( NULL == pi_recv_data )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "pi_recv_data == null" );
			break;
		}

		if( NULL == pi_send_data )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "pi_send_data == null" );
			break;
		}

		void* pvProtocolArg = psProtocol->pv_protocol_create( pvProtocolConfig );
		if( NULL == pvProtocolArg )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "create protocol %d err", eProtocolId );
			break;			
		}

		psSocket = (struct sfbus_altp_socket*)malloc( sizeof(struct sfbus_altp_socket) );
		if( NULL == psSocket )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "malloc err" );
			break;
		}

		psSocket->pvCommandIndexTable = sfbus_index_create( psProtocol->iCommandIndexTableSize );
		if( NULL == psSocket->pvCommandIndexTable )
		{
			free( psSocket );
			psSocket = NULL;
			break;
		}

		psSocket->eSocketId = sfbus_index_get_empty_id( sSfbusAltp.pvSocketIndexTable );
		if( 0 > psSocket->eSocketId )
		{
			sfbus_index_delete( psSocket->pvCommandIndexTable );
			free( psSocket );
			psSocket = NULL;
			break;			
		}

		if( 0 > sfbus_index_add_id(psSocket->eSocketId,(void*)psSocket,sSfbusAltp.pvSocketIndexTable) )
		{
			sfbus_index_delete( psSocket->pvCommandIndexTable );
			free( psSocket );
			psSocket = NULL;
			break;	
		}

		char strPthreadName[128] = { 0 };
		snprintf( strPthreadName, sizeof(strPthreadName), "sfbus_socket_%d", psSocket->eSocketId );
		psSocket->psPthread = sfbus_altp_pthread_package_send_create( strPthreadName, SFBUS_ALTP_SOCKET_QUEUE_SIZE );
		if( NULL == psSocket->psPthread )
		{
			sfbus_index_delete( psSocket->pvCommandIndexTable );
			sfbus_index_del_id( psSocket->eSocketId, sSfbusAltp.pvSocketIndexTable );
			free( psSocket );
			psSocket = NULL;
			break;
		}

		psSocket->sFunc.pvPortArg		= pvPortArg;
		psSocket->sFunc.pi_send_data	= pi_send_data;
		psSocket->sFunc.pi_recv_data	= pi_recv_data;
		psSocket->pvProtocolArg			= pvProtocolArg;
		psSocket->psProtocol			= psProtocol;
		psSocket->psListenListHead		= NULL;
		psSocket->psListenListLast		= NULL;
	}while( 0 );

	return (void*)psSocket;
}

int sfbus_delete_socket( void* pvSocket )
{
	int eRet = 0;

	do{
		if( NULL == pvSocket )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "pvSocket == null" );
			eRet = -1;
			break;
		}

		struct sfbus_altp_socket* psSocket = (struct sfbus_altp_socket*)pvSocket;
		
		if( 0 > sfbus_index_del_id(psSocket->eSocketId,sSfbusAltp.pvSocketIndexTable) )
		{
			eRet = -1;
			break;
		}

		eRet = sfbus_altp_pthread_package_send_delete( (void*)(psSocket->psPthread) );
		if( eRet )
		{
			break;
		}

		for( struct sfbus_altp_socket_listen_list* psListenList = psSocket->psListenListHead; psListenList;  )
		{
			sfbus_func_function_del_listen( psListenList->sOutputArg.psCommand->pvFunction, psListenList, sfbus_altp_socket_listen_send );

			struct sfbus_altp_socket_listen_list* psListenListFree = psListenList;
			psListenList = psListenList->psNext;
			free( psListenListFree );
		}

		sfbus_index_delete( psSocket->pvCommandIndexTable );
		
		free( psSocket );
	}while( 0 );

	return eRet;
}

inline static 
void sfbus_socket_package_send( struct sfbus_memory* psSendInfoMemory )
{
	struct sfbus_altp_package_send_info* psSendInfo = (struct sfbus_altp_package_send_info*)psSendInfoMemory->pvDataIndex;

	struct sfbus_altp_socket* psSocket = (struct sfbus_altp_socket*)(psSendInfo->pvSocket);
	struct sfbus_altp_protocol* psProtocol = psSocket->psProtocol;
	struct sfbus_altp_command* psCommand = (struct sfbus_altp_command*)(psSendInfo->pvCommand);

	psProtocol->pi_package_send( psSocket->pvProtocolArg, &(psSocket->sFunc), psCommand->eCommandId, psSendInfo->psData, psSendInfo->psInfo );

	sfbus_memory_free( psSendInfoMemory );
}

void sfbus_socket_recv_anlysis( void* pvSocket )
{
	struct sfbus_altp_socket* psSocket		= (struct sfbus_altp_socket*)(pvSocket);
	struct sfbus_altp_protocol* psProtocol	= psSocket->psProtocol;
	do{
		int eCommandId = psProtocol->pi_recv_anlysis( psSocket->pvProtocolArg, &(psSocket->sFunc), &(psSocket->psData), &(psSocket->psInfo) );
		if( 0 > eCommandId )
			break;

		do{
			struct sfbus_altp_command* psCommand = (struct sfbus_altp_command*)sfbus_index_find_id( eCommandId, psSocket->pvCommandIndexTable );
			if( NULL == psCommand )
			{
				sfbus_log( SFBUS_LOG_LEVEL_WARNING, "command not find" );
				sfbus_memory_free( psSocket->psData );
				sfbus_memory_free( psSocket->psInfo );
				break;
			}

			struct sfbus_memory* psInputParam = psCommand->ps_data_to_param( psCommand->pvCommandInfo, psSocket->psData );

			struct sfbus_memory* psOutputArgMemory;
			struct sfbus_altp_socket_output_arg* psOutputArg = (struct sfbus_altp_socket_output_arg*)(psOutputArgMemory->pvDataIndex);
			if( psSocket->psInfo )
				psOutputArgMemory = sfbus_memory_malloc( sizeof(struct sfbus_altp_socket_output_arg) + psSocket->psInfo->iDataLength );
			else
				psOutputArgMemory = sfbus_memory_malloc( sizeof(struct sfbus_altp_socket_output_arg) );

			if( NULL == psOutputArgMemory )
			{
				sfbus_log( SFBUS_LOG_LEVEL_WARNING, "malloc output arg err" );
				sfbus_memory_free( psSocket->psData );
				sfbus_memory_free( psSocket->psInfo );
				break;
			}

			if( psOutputArg )
			{
				memcpy( psOutputArg->acbInfo, psSocket->psInfo->pvDataIndex, psSocket->psInfo->iDataLength );
				psOutputArg->iInfoLength = psSocket->psInfo->iDataLength;
				psOutputArg->eSocketId = psSocket->eSocketId;
				psOutputArg->psCommand = psCommand;
				psOutputArg->psProtocol = psSocket->psProtocol;
			}
			else
			{
				psOutputArg->iInfoLength = 0;
				psOutputArg->eSocketId = psSocket->eSocketId;
				psOutputArg->psCommand = psCommand;
				psOutputArg->psProtocol = psSocket->psProtocol;				
			}
			
			sfbus_func_function_run( psCommand->pvFunction, 1, psInputParam, psOutputArgMemory, sfbus_altp_socket_output );
		}while( 0 );
	}while( 0 );

	psSocket->psData = NULL;
	psSocket->psInfo = NULL;
}

int sfbus_socket_add_command( void* pvSocket, int eCommandId )
{
	int eRet = 0;
	
	do{
		struct sfbus_altp_socket* psSocket	= (struct sfbus_altp_socket*)(pvSocket);

		struct sfbus_altp_command* psCommand = (struct sfbus_altp_command*)sfbus_index_find_id( eCommandId, psSocket->psProtocol->pvCommandIndexTable );
		if( NULL == psCommand )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "command not find" );
			eRet = -1;
			break;
		}

		if( 0 > sfbus_index_add_id(eCommandId,psCommand,psSocket->pvCommandIndexTable) )
		{
			eRet = -1;
			break;
		}
	}while( 0 );
	
	return eRet;
}

int sfbus_socket_listen_command( void* pvSocket, int eCommandId )
{
	int eRet = 0;
	
	do{
		struct sfbus_altp_socket* psSocket = (struct sfbus_altp_socket*)(pvSocket);
	
		struct sfbus_altp_command* psCommand = (struct sfbus_altp_command*)sfbus_index_find_id( eCommandId, psSocket->psProtocol->pvCommandIndexTable );
		if( NULL == psCommand )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "command not find" );
			eRet = -1;
			break;
		}

		struct sfbus_altp_socket_listen_list* psListenList = (struct sfbus_altp_socket_listen_list*)malloc( sizeof(struct sfbus_altp_socket_listen_list) );
		if( NULL == psListenList )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "malloc err" );
			break;
		}

		psListenList->sOutputArg.iInfoLength	= 0;
		psListenList->sOutputArg.eSocketId		= psSocket->eSocketId;
		psListenList->sOutputArg.psCommand		= psCommand;
		psListenList->sOutputArg.psProtocol		= psSocket->psProtocol;
		psListenList->psNext					= NULL;

		if( NULL == psSocket->psListenListHead )
			psSocket->psListenListHead = psListenList;
		else
			psSocket->psListenListLast->psNext = psListenList;
		psSocket->psListenListLast = psListenList;

		eRet = sfbus_func_function_add_listen( psCommand->pvFunction, psListenList, sfbus_altp_socket_listen_send );
	}while( 0 );
	
	return eRet;
}

