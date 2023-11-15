
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <mqueue.h>

#include "../../storage/storage.h"
#include "../sysport.h"

#define SYSPORT_TASK_MSG_SIZE		(10)

typedef struct sysport_task_func
{
	void 		(*pv_func)(void* pvArg, void* pvInfo, uint32_t uSize);
	uint32_t	uSize;
	char		acInfo[0];
}sysport_task_func_t;

typedef struct sysport_task_module
{
	char 		*strName;
	void		*pvArg;
	mqd_t 		mq;
	pthread_t 	pid;
}sysport_task_module_t;

static const char strTaskLogName[] = "task";

#define sysport_task_log( strFormat, ...) \
    do{ \
        storage_log( strTaskLogName, LOG_LEVEL_NOTICE, strFormat, ##__VA_ARGS__); \
    }while(0)

#define sysport_task_debug( strFormat, ...) \
    do{ \
        storage_log( strTaskLogName, LOG_LEVEL_DEBUG, strFormat, ##__VA_ARGS__); \
    }while(0)

static void *sysport_task_pthread( void* pvModule )
{
	struct sysport_task_module *psModule = (struct sysport_task_module*)pvModule;
	
	for( ; ; )
	{
		struct sysport_task_func* psFunc;
		struct mq_attr attr;
		mq_getattr( psModule->mq, &attr );
		int count = mq_receive( psModule->mq, (char*)(&psFunc), attr.mq_msgsize, NULL);

		if( sizeof(struct sysport_task_func*) == count )
		{			
			if( NULL == psFunc->pv_func )
			{
				break;
			}
			else
			{
				psFunc->pv_func( psModule->pvArg, psFunc->acInfo, psFunc->uSize );
			}
			free( psFunc );
		}
		else
		{
			sysport_task_log( "no command" );
		}
	}

	sysport_task_log( "task %d delete, arg %p", psModule->pid, psModule->pvArg );

	mq_close( psModule->mq );
	mq_unlink( psModule->strName );
	free( psModule );

	return NULL;
}

int sysport_task_init( void )
{
	return 0;
}

void* sysport_task_create( char *strName, void* pvArg )
{
	struct sysport_task_module* psModule = malloc(sizeof(struct sysport_task_module));

	if( psModule )
	{
		psModule->strName	= strName;
		psModule->pvArg		= pvArg;
	
		struct mq_attr attr;
		attr.mq_maxmsg = SYSPORT_TASK_MSG_SIZE;
		attr.mq_msgsize = sizeof(struct sysport_task_func*);
		psModule->mq = mq_open( psModule->strName, O_CREAT|O_RDWR|O_CLOEXEC, 0666, &attr );
		if( 0 > psModule->mq )
		{
			sysport_task_log( "task create mq err" );
			free( psModule );
			psModule = NULL;
		}
		else
		{
			if( 0 > pthread_create(&(psModule->pid), NULL, sysport_task_pthread, psModule) )
			{
				sysport_task_log( "task create pthread err" );
			
				mq_close( psModule->mq );
				mq_unlink( psModule->strName );
				free( psModule );
				psModule = NULL;
			}
			else
			{
				sysport_task_log( "task create %d", psModule->pid );
			}
		}
	}
	else
	{
		sysport_task_log( "task create malloc err" );
	}
	
	return psModule;
}

int sysport_task_delete( void* pvModule )
{
	int eRet = -1;

	if( pvModule )
	{
		void* pvInfo = sysport_task_malloc( pvModule, 0 );
		if( pvInfo )
		{
			eRet = sysport_task_func( pvModule, NULL, pvInfo );
		}
	}

	return eRet;
}

void* sysport_task_malloc( void* pvModule, uint32_t uSize )
{
	void* pvData = NULL;

	if( pvModule )
	{
		struct sysport_task_module *psModule = (struct sysport_task_module*)pvModule;
		struct sysport_task_func* psFunc = malloc( sizeof(struct sysport_task_func) + uSize );

		if( psFunc )
		{
			psFunc->uSize = uSize;
			pvData = (void*)(&(psFunc->acInfo));
		}
		else
		{
			sysport_task_log( "task malloc err" );
		}
	}
	return pvData;
}

int sysport_task_func( void* pvModule, void (*pv_func)(void* pvArg, void* pvInfo, uint32_t uSize), void* pvInfo )
{
	int eRet = -1;
	
	if( pvModule && pvInfo )
	{
		struct sysport_task_module *psModule = (struct sysport_task_module*)pvModule;
		struct sysport_task_func* psFunc = (struct sysport_task_func *)((char *)pvInfo - (((size_t)&((struct sysport_task_func*)0)->acInfo)));
		psFunc->pv_func = pv_func;

		struct mq_attr attr;
		mq_getattr( psModule->mq, &attr );
		eRet = mq_send( psModule->mq, (char*)(&psFunc), attr.mq_msgsize, 0 );
		if( 0 > eRet )
		{
			sysport_task_log( "task send func %p err", pv_func );
			free( psFunc );
		}
	}

	return eRet;
}

