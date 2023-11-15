
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

#include "../sfbus.h"
#include "../index/sfbus_index.h"

/*******************************************************************************
@description:	function
*******************************************************************************/
typedef struct sfbus_func_function_Listen_list
{
	void* pvListenArg;
	void (*pv_listen)(void* pvListenArg,void* pvOutputParam,int32_t iOutputParamLength);
	struct sfbus_func_function_Listen_list* psNext;
}sfbus_func_function_Listen_list_t;

typedef struct sfbus_func_function
{
	pthread_mutex_t	mutex;
	
	void* pvPthread;

	struct sfbus_function_description const* psDescription;
	
	int32_t (*pi_function)(void* pvOutputArg,void* pvInputParam,int32_t iInputParamLength);
	
	struct sfbus_func_function_Listen_list* psListenListHead;
	struct sfbus_func_function_Listen_list* psListenListLast;
}sfbus_func_function_t;

typedef struct sfbus_func_function_run_info
{
	struct sfbus_func_function* psFunction;
	struct sfbus_memory* psInputParam;

	struct sfbus_memory* psArgMemory;
	void	(*pv_output)(void* psArg,void* pvOutputParam,int32_t iOutputParamLength);	
}sfbus_func_function_run_info_t;

void sfbus_function_output( void* pvOutputArg, void* pvOutputParam, int32_t iOutputParamLength )
{
	struct sfbus_func_function_run_info* psInfo = (struct sfbus_func_function_run_info*)(pvOutputArg);

	struct sfbus_func_function* psFunction = (struct sfbus_func_function*)(psInfo->psFunction);

	if( psInfo->pv_output )
	{
		psInfo->pv_output( psInfo->psArgMemory, pvOutputParam, iOutputParamLength );
		
		sfbus_log( SFBUS_LOG_LEVEL_DEBUG, "run output function %p", psInfo->pv_output );
	}

	if( psFunction->psListenListHead )
	{
		pthread_mutex_lock( &psFunction->mutex );
		
		struct sfbus_func_function_Listen_list* psListenList = psFunction->psListenListHead;
		for( ; psListenList; psListenList = psListenList->psNext )
		{
			psListenList->pv_listen( psListenList->pvListenArg, pvOutputParam, iOutputParamLength );

			sfbus_log( SFBUS_LOG_LEVEL_DEBUG, "run listen function %p", psListenList->pv_listen );
		}
		
		pthread_mutex_unlock( &psFunction->mutex );
	}
}

//inline static 
int sfbus_func_function_add_listen( void* pvFunction, void* pvListenArg, void (*pv_listen)(void* pvListenArg,void* pvOutputParam,int32_t iOutputParamLength) )
{
	int eRet = 0;
	
	do{
		if( NULL == pvFunction )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "pvFunction == null" );
			eRet = -1;
			break;
		}

		if( NULL == pv_listen )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "pv_listen == null" );
			eRet = -1;
			break;
		}

		struct sfbus_func_function_Listen_list* psListenList = (struct sfbus_func_function_Listen_list*)malloc(sizeof(struct sfbus_func_function_Listen_list));
		if( NULL == psListenList )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "malloc err" );
			eRet = -1;
			break;
		}

		psListenList->pvListenArg 	= pvListenArg;
		psListenList->pv_listen		= pv_listen;
		psListenList->psNext		= NULL;

		struct sfbus_func_function* psFunction = (struct sfbus_func_function*)(pvFunction);
		pthread_mutex_lock( &psFunction->mutex );
		if( NULL == psFunction->psListenListHead )
			psFunction->psListenListHead = psListenList;
		else
			psFunction->psListenListLast->psNext = psListenList;
		psFunction->psListenListLast = psListenList;
		pthread_mutex_unlock( &psFunction->mutex );
		
		sfbus_log( SFBUS_LOG_LEVEL_DEBUG, "add function=%p listen=%p arg=%p pv_listen=%p", pvFunction, psListenList, pvListenArg, pv_listen );
	}while( 0 );

	return eRet;
}

//inline static 
int sfbus_func_function_del_listen( void* pvFunction, void* pvListenArg, void (*pv_listen)(void* pvListenArg,void* pvOutputParam,int32_t iOutputParamLength) )
{
	int eRet = 0;
	
	do{
		if( NULL == pvFunction )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "pvFunction == null" );
			eRet = -1;
			break;
		}

		if( NULL == pv_listen )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "pv_listen == null" );
			eRet = -1;
			break;
		}

		struct sfbus_func_function* psFunction = (struct sfbus_func_function*)(pvFunction);
		struct sfbus_func_function_Listen_list* psListenListDelete = NULL;
		
		pthread_mutex_lock( &psFunction->mutex );
		
		if( NULL == psFunction->psListenListHead )
		{
			sfbus_log( SFBUS_LOG_LEVEL_INFO, "has no listen function" );
		}
		else
		{
			struct sfbus_func_function_Listen_list* psListenListPrev = NULL;
			struct sfbus_func_function_Listen_list* psListenList = psFunction->psListenListHead;
			for( ; psListenList; psListenList = psListenList->psNext )
			{
				if( psListenList->pvListenArg == pvListenArg && psListenList->pv_listen == pv_listen )
				{
					if( psListenListPrev )
						psListenListPrev->psNext = psListenList->psNext;
					else
						psFunction->psListenListHead->psNext = psListenList->psNext;

					psListenListDelete = psListenList;
					free( psListenList );
					break;
				}
				psListenListPrev = psListenList;
			}
		}
		
		pthread_mutex_unlock( &psFunction->mutex );
		
		sfbus_log( SFBUS_LOG_LEVEL_DEBUG, "del function=%p listen=%p arg=%p pv_listen=%p", pvFunction, psListenListDelete, pvListenArg, pv_listen );
	}while( 0 );

	return eRet;
}

inline static 
void* sfbus_func_function_create( void* pvIndexTable, int eFunctionId, void* pvPthread, struct sfbus_function_description const* psDescription, 
										int32_t (*pi_function)(void* pvOutputArg,void* pvInputParam,int32_t iInputParamLength) )
{
	struct sfbus_func_function* psFunction = NULL;

	do{
		if( NULL == pi_function )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "pi_function = null" );
			break;
		}

		if( NULL == psDescription )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "psDescription = null" );
			break;
		}

		if( NULL == psDescription->strPthreadName )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "strPthreadName = null" );
			break;
		}

		if( NULL == psDescription->strFunctionName )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "strFunctionName = null" );
			break;
		}

		if( NULL == psDescription->strInputParam )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "strInputParam = null" );
			break;
		}

		if( NULL == psDescription->strOutputParam )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "strOutputParam = null" );
			break;
		}

		psFunction = sfbus_index_find_id( eFunctionId, pvIndexTable );
		if( psFunction )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "function %d exist", eFunctionId );
			psFunction = NULL;
			break;
		}
	
		psFunction = (struct sfbus_func_function*)malloc( sizeof(struct sfbus_func_function) );
		if( NULL == psFunction )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "malloc err" );
			break;
		}

		psFunction->pvPthread			= pvPthread;
		psFunction->psDescription 		= psDescription;
		psFunction->pi_function 		= pi_function;
		psFunction->psListenListHead	= NULL;
		psFunction->psListenListLast	= NULL;
		
		if( 0 != pthread_mutex_init(&(psFunction->mutex),NULL) )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "create mutex err %s", strerror(errno) );
			free( psFunction );
			psFunction = NULL;
			break;
		}

		if( 0 > sfbus_index_add_id(eFunctionId,(void*)psFunction,pvIndexTable) )
		{
			if( 0 != pthread_mutex_destroy(&(psFunction->mutex)) )
				sfbus_log( SFBUS_LOG_LEVEL_ERR, "delete mutex err %s", strerror(errno) );
			free( psFunction );
			psFunction = NULL;
			break;
		}

		sfbus_log( SFBUS_LOG_LEVEL_INFO, "create function=%p pi_function=%p", psFunction, pi_function );
	}while( 0 );

	return psFunction;
}


/*******************************************************************************
@description:	pthread
*******************************************************************************/
#define SFBUS_FUNC_PTHREAD_NAME_LENGTH_MAX		(128)

#define SFBUS_FUNC_PTHREAD_QUEUE_SZIE			(50)

typedef struct sfbus_func_pthread
{	
	mqd_t 		mq;
	pthread_t 	pid;

	uint32_t	uQueueMax;
	char const*	strPthreadName;
}sfbus_func_pthread_t;

inline static 
int sfbus_func_pthread_queue_create( struct sfbus_func_pthread* psPthread )
{
	int eRet = -1;

	struct mq_attr attr;
	attr.mq_maxmsg = psPthread->uQueueMax;
	attr.mq_msgsize = sizeof(struct sfbus_func_function_run_info);
	char strQueueName[SFBUS_FUNC_PTHREAD_NAME_LENGTH_MAX];
	strQueueName[0] = '/';
	memcpy( &strQueueName[1], psPthread->strPthreadName, strlen(psPthread->strPthreadName) + 1 );
	psPthread->mq = mq_open( strQueueName, O_CREAT|O_RDWR|O_CLOEXEC, 0666, &attr );
	if( 0 > psPthread->mq )
	{
		sfbus_log( SFBUS_LOG_LEVEL_ERR, "create queue %s err %s", strQueueName, strerror(errno) );
	}
	else
	{
		sfbus_log( SFBUS_LOG_LEVEL_INFO, "create queue %s result %d", strQueueName, psPthread->mq );
		eRet = 0;
	}

	return eRet;
}

inline static 
int sfbus_func_pthread_queue_delete( struct sfbus_func_pthread* psPthread )
{
	int eRet = -1;

	if( 0 > mq_close(psPthread->mq) )
	{
		sfbus_log( SFBUS_LOG_LEVEL_ERR, "close queue %d err %s", psPthread->mq, strerror(errno) );
	}
	else
	{
		sfbus_log( SFBUS_LOG_LEVEL_INFO, "close queue %d", psPthread->mq );
	}
	
	char strQueueName[SFBUS_FUNC_PTHREAD_NAME_LENGTH_MAX];
	strQueueName[0] = '/';
	memcpy( &strQueueName[1], psPthread->strPthreadName, strlen(psPthread->strPthreadName) + 1 );
	
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

inline static 
void* sfbus_func_pthread( void* pvPthread )
{
	struct sfbus_func_pthread *psPthread = (struct sfbus_func_pthread*)pvPthread;
	
	for( ; ; )
	{
		struct sfbus_func_function_run_info sInfo;
		struct mq_attr attr;
		mq_getattr( psPthread->mq, &attr );
		int count = mq_receive( psPthread->mq, (char*)(&sInfo), attr.mq_msgsize, NULL);
		
		int eRet;
		if( sInfo.psInputParam )
			eRet = sInfo.psFunction->pi_function( (void*)(&sInfo), sInfo.psInputParam->pvDataIndex, sInfo.psInputParam->iDataLength );
		else
			eRet = sInfo.psFunction->pi_function( (void*)(&sInfo), NULL, 0 ); 
	
		if( 0 > eRet )
		{
			char* strInputParam = (char*)malloc( 2*(sInfo.psInputParam->iDataLength) + 1 );
			if( NULL == strInputParam )
			{
				sfbus_log( SFBUS_LOG_LEVEL_WARNING, "function=%p param printf err", sInfo.psFunction );
			}
			else
			{
				uint8_t* pcDataIndex = sInfo.psInputParam->pvDataIndex;
				for( uint32_t i = 0; sInfo.psInputParam->iDataLength > i; ++i )
				{
					*strInputParam++ = '0' + (0x0F & (pcDataIndex[i] >> 4));
					*strInputParam++ = '0' + (0x0F & (pcDataIndex[i] >> 0));
				}
				*strInputParam = '\0';
				sfbus_log( SFBUS_LOG_LEVEL_WARNING, "function=%p run err, param=%s", sInfo.psFunction, strInputParam );
				free( strInputParam );
			}
		}

		sfbus_memory_free( sInfo.psArgMemory );
		sfbus_memory_free( sInfo.psInputParam );
	}

	sfbus_log( SFBUS_LOG_LEVEL_NOTICE, "pthread exit, pid = %d", (int)(psPthread->pid) );

	sfbus_func_pthread_queue_delete( psPthread );
	
	free( psPthread );

	return NULL;
}

//inline static 
int sfbus_func_function_run( void* pvFunction, int eLevel, struct sfbus_memory* psInputParam, 
							struct sfbus_memory* psArgMemory, void (*pv_output)(void* psArg,void* pvOutputParam,int32_t iOutputParamLength) )
{
	int eRet = 0;

	do{
		if( NULL == pvFunction )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "pvFunction = null" );
			eRet = -1;
			break;
		}
		
		struct sfbus_func_function* psFunction = (struct sfbus_func_function*)pvFunction;
		struct sfbus_func_function_run_info sInfo;

		sInfo.psFunction	= psFunction;
		sInfo.psInputParam	= psInputParam;
		sInfo.psArgMemory	= psArgMemory;
		sInfo.pv_output 	= pv_output;

		struct sfbus_func_pthread *psPthread = (struct sfbus_func_pthread*)(psFunction->pvPthread);
		struct mq_attr attr;
		mq_getattr( psPthread->mq, &attr );
		if( 0 > mq_send(psPthread->mq,(char*)(&sInfo),attr.mq_msgsize,eLevel) )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "send function %p err", psFunction );

			sfbus_memory_free( sInfo.psArgMemory );
			sfbus_memory_free( sInfo.psInputParam );
			
			eRet = -1;
		}
	}while( 0 );

	return eRet;
}

inline static 
void* sfbus_func_pthread_find_create( void* pvIndexTable, struct sfbus_function_description const* psFunctionDescription )
{
	char const* strPthreadName = psFunctionDescription->strPthreadName;
	//hash for pthread name id
	int ePthreadId = (strPthreadName[0]<<24) | (strPthreadName[1]<<16) | (strPthreadName[2]<<8) | (strPthreadName[3]<<0);

	struct sfbus_func_pthread* psPthread = (struct sfbus_func_pthread*)sfbus_index_find_id( ePthreadId, pvIndexTable );
	do{
		if( psPthread )
		{
			sfbus_log( SFBUS_LOG_LEVEL_INFO, "pthread %d %s exist", ePthreadId, strPthreadName );
			break;
		}

		if( NULL == strPthreadName )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "strPthreadName == null" );
			break;
		}

		psPthread = (struct sfbus_func_pthread*)malloc(sizeof(struct sfbus_func_pthread));
		if( NULL == psPthread )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "malloc err" );
			break;
		}

		psPthread->uQueueMax		= SFBUS_FUNC_PTHREAD_QUEUE_SZIE;
		psPthread->strPthreadName	= strPthreadName;

		if( 0 > sfbus_func_pthread_queue_create(psPthread) )
		{			
			free( psPthread );
			psPthread = NULL;
			break;
		}

		if( 0 > pthread_create(&(psPthread->pid), NULL, sfbus_func_pthread, psPthread) )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "create pthread err %s", strerror(errno) );
		
			sfbus_func_pthread_queue_delete( psPthread );
				
			free( psPthread );
			psPthread = NULL;
			break;
		}

		sfbus_log( SFBUS_LOG_LEVEL_INFO, "create pthread %d", (int)(psPthread->pid) );
		
		if( 0 > sfbus_index_add_id(ePthreadId,psPthread,pvIndexTable) )
		{
			psPthread = NULL;
			break;
		}
	}while( 0 );

	return (void*)psPthread;
}

/*******************************************************************************
@description:	timer
*******************************************************************************/
typedef struct sfbus_func_timer_callback_list
{
	void* pvFunction;
	struct sfbus_func_timer_callback_list* psNext;
}sfbus_func_timer_callback_list_t;

typedef struct sfbus_func_timer
{
	timer_t 	timerid;
	
	pthread_mutex_t	mutex;
	
	struct sfbus_func_timer_callback_list* psCallbackListHead;
	struct sfbus_func_timer_callback_list* psCallbackListLast;
}sfbus_func_timer_t;

inline static 
void func_timer_thread( union sigval sv )
{
    struct sfbus_func_timer* psTimer = (struct sfbus_func_timer*)sv.sival_ptr;

	struct sfbus_func_timer_callback_list* psCallbackList = psTimer->psCallbackListHead;
	for( ; psCallbackList; psCallbackList = psCallbackList->psNext )
	{
		sfbus_func_function_run( psCallbackList->pvFunction, 1, NULL, NULL, NULL );
	}
}

inline static 
void* sfbus_func_timer_find_create( void* pvIndexTable, uint32_t uTimeMs )
{
	struct sfbus_func_timer* psTimer = sfbus_index_find_id( uTimeMs, pvIndexTable );
	do{	
		if( psTimer )
		{
			sfbus_log( SFBUS_LOG_LEVEL_INFO, "timer %d exist", uTimeMs );
			break;
		}
	
		psTimer = (struct sfbus_func_timer*)malloc( sizeof(struct sfbus_func_timer) );
		if( NULL == psTimer )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "malloc err" );
			break;
		}
		
		memset( psTimer, 0, sizeof(struct sfbus_func_timer) );
		if( 0 != pthread_mutex_init(&(psTimer->mutex),NULL) )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "create mutex err %s", strerror(errno) );
			free( psTimer );
			psTimer = NULL;
			break;
		}
		
		if( 0 == uTimeMs )
		{
			sfbus_log( SFBUS_LOG_LEVEL_WARNING, "uTimeMs = 0" );
			break;
		}
		
		psTimer = (struct sfbus_func_timer*)malloc( sizeof(struct sfbus_func_timer) );
		if( NULL == psTimer )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "timer malloc result null" );
			break;
		}
		
		struct sigevent sev;
		sev.sigev_notify = SIGEV_THREAD;
		sev.sigev_notify_function = func_timer_thread;
		sev.sigev_value.sival_ptr = psTimer;
		sev.sigev_notify_attributes = NULL;
		if( 0 > timer_create(CLOCK_REALTIME, &sev, &psTimer->timerid) )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "create timer err %s", strerror(errno) );

			free( psTimer );
			psTimer = NULL;
			break;
		}

		struct itimerspec its;
		its.it_value.tv_sec = uTimeMs/1000;
		its.it_value.tv_nsec = 1000000*(uTimeMs%1000);
		its.it_interval.tv_sec = uTimeMs/1000;
		its.it_interval.tv_nsec = 1000000*(uTimeMs%1000);
		if( 0 > timer_settime(psTimer->timerid, 0, &its, NULL) )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "start timer %p err %p", psTimer->timerid, strerror(errno) );

			if( 0 > timer_delete(psTimer->timerid) )
				sfbus_log( SFBUS_LOG_LEVEL_ERR, "timer delete %p err %s", psTimer->timerid, strerror(errno) );
			else
				sfbus_log( SFBUS_LOG_LEVEL_INFO, "timer delete %p", psTimer->timerid );

			free( psTimer );
			psTimer = NULL;
			break;
	    }

		sfbus_log( SFBUS_LOG_LEVEL_INFO, "time=%ums, id=%p", uTimeMs, psTimer->timerid );

		if( 0 > sfbus_index_add_id(uTimeMs,(void*)psTimer,pvIndexTable) )
		{
			if( 0 != pthread_mutex_destroy(&(psTimer->mutex)) )
				sfbus_log( SFBUS_LOG_LEVEL_ERR, "delete mutex err %s", strerror(errno) );
			free( psTimer );
			psTimer = NULL;
			break;
		}
	}while( 0 );
	
	return psTimer;
}

inline static 
int sfbus_func_timer_add_function( struct sfbus_func_timer* psTimer, void* pvFunction )
{
	int eRet = 0;
	
	do{
		struct sfbus_func_timer_callback_list* psCallbackList = (struct sfbus_func_timer_callback_list*)malloc(sizeof(struct sfbus_func_timer_callback_list));
		if( NULL == psCallbackList )
		{
			sfbus_log( SFBUS_LOG_LEVEL_ERR, "malloc err" );
			eRet = -1;
			break;
		}

		psCallbackList->pvFunction	= pvFunction;
		psCallbackList->psNext		= NULL;

		pthread_mutex_lock( &psTimer->mutex );
		if( NULL == psTimer->psCallbackListHead )
			psTimer->psCallbackListHead = psCallbackList;
		else
			psTimer->psCallbackListLast->psNext = psCallbackList;
		psTimer->psCallbackListLast = psCallbackList;
		pthread_mutex_unlock( &psTimer->mutex );

		sfbus_log( SFBUS_LOG_LEVEL_INFO, "timer %p add function %p", psTimer, pvFunction );
	}while( 0 );

	return eRet;
}

/*******************************************************************************
@description:	func manager
*******************************************************************************/
#define SFBUS_FUNC_TIMER_INDEX_TABLE_SIZE		(50)
#define SFBUS_FUNC_PTHREAD_INDEX_TABLE_SIZE		(100)
#define SFBUS_FUNC_FUNCITON_INDEX_TABLE_SIZE	(1000)

typedef struct sfbus_func
{
	void* pvPthreadIndexTable;
	void* pvTimerIndexTable;
	void* pvFunctionIndexTable;
}sfbus_func_t;

struct sfbus_func sSfbusFunc = { 0 };

void* sfbus_func_function_get_handle( int eFunctionId )
{
	struct sfbus_func_function* psFunction = sfbus_index_find_id( eFunctionId, sSfbusFunc.pvFunctionIndexTable );
	
	if( psFunction )
	{
		sfbus_log( SFBUS_LOG_LEVEL_INFO, "function find %d %p", eFunctionId, psFunction );
	}
	else
	{
		sfbus_log( SFBUS_LOG_LEVEL_WARNING, "function not find %d", eFunctionId );
	}

	return (void*)psFunction;
}

void* sfbus_func_function_register( int eFunctionId, uint32_t 	uTiggerTimeMs, const struct sfbus_function_description* psDescription, 
									int (*pi_function)(void* pvOutputArg,void* pvInputParam,int32_t iInputParamLength) )
{
	struct sfbus_func_function* psFunction = NULL;

	do{
		void* pvPthread = sfbus_func_pthread_find_create( sSfbusFunc.pvPthreadIndexTable, psDescription );
		if( NULL == pvPthread )
			break;

		psFunction = sfbus_func_function_create( sSfbusFunc.pvFunctionIndexTable, eFunctionId, pvPthread, psDescription, pi_function );
		if( NULL == psFunction )
			break;

		if( uTiggerTimeMs )
		{
			void* pvTimer = sfbus_func_timer_find_create( sSfbusFunc.pvTimerIndexTable, uTiggerTimeMs );
			if( NULL == pvTimer )
				break;

			sfbus_func_timer_add_function( (struct sfbus_func_timer*)pvTimer, (void*)psFunction );
		}
	}while( 0 );

	return (void*)psFunction;
}

int sfbus_func_init( void )
{
	int eRet = 0;

	if( NULL == sSfbusFunc.pvTimerIndexTable )
	{
		sSfbusFunc.pvTimerIndexTable = sfbus_index_create( SFBUS_FUNC_TIMER_INDEX_TABLE_SIZE );
	}

	if( NULL == sSfbusFunc.pvPthreadIndexTable )
	{
		sSfbusFunc.pvPthreadIndexTable = sfbus_index_create( SFBUS_FUNC_PTHREAD_INDEX_TABLE_SIZE );
	}

	if( NULL == sSfbusFunc.pvFunctionIndexTable )
	{
		sSfbusFunc.pvFunctionIndexTable = sfbus_index_create( SFBUS_FUNC_FUNCITON_INDEX_TABLE_SIZE );
	}

	if( NULL == sSfbusFunc.pvTimerIndexTable || NULL == sSfbusFunc.pvPthreadIndexTable || NULL == sSfbusFunc.pvFunctionIndexTable )
	{
		sfbus_log( SFBUS_LOG_LEVEL_ALERT, "index table %p %p %p", sSfbusFunc.pvTimerIndexTable, sSfbusFunc.pvPthreadIndexTable, sSfbusFunc.pvFunctionIndexTable );
	}

	return eRet;
}

