
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

#include "../../storage/storage.h"
#include "../sysport.h"

typedef struct sysport_timer_module
{
	timer_t 	timerid;

	char*		strName;
	uint32_t	uRunTime;
	void*		pvArg;
	void 		(*pv_func)(void* pvArg);
}sysport_timer_module_t;

static const char strTimerLogName[] = "timer";

#define sysport_timer_log( strFormat, ...) \
    do{ \
        storage_log( strTimerLogName, LOG_LEVEL_NOTICE, strFormat, ##__VA_ARGS__); \
    }while(0)

#define sysport_timer_debug( strFormat, ...) \
    do{ \
        storage_log( strTimerLogName, LOG_LEVEL_DEBUG, strFormat, ##__VA_ARGS__); \
    }while(0)

static void sysport_timer_thread( union sigval sv )
{
    struct sysport_timer_module* psModule = (struct sysport_timer_module*)sv.sival_ptr;

	if( psModule && psModule->pv_func )
	{
		psModule->pv_func( psModule->pvArg );

		if( psModule->uRunTime )
		{
			if( 0 == --psModule->uRunTime )
			{
				if( 0 > timer_delete(psModule->timerid) )
				{
					sysport_timer_log( psModule->strName, 0, "delete timer %d err", psModule->timerid );
					psModule->uRunTime = 1;
				}
				else
				{
					sysport_timer_log( psModule->strName, 0, "delete timer %d", psModule->timerid );
					free( psModule );
				}
			}
		}
	}
}

int sysport_timer_init( void )
{
	return 0;
}

void* sysport_timer_create( char *strName, uint32_t uTimeMs, uint32_t uRunTime, void* pvArg, void (*pv_func)(void* pvArg) )
{
	struct sysport_timer_module* psModule = (struct sysport_timer_module*)malloc( sizeof(struct sysport_timer_module) );
	
	if( psModule )
	{
		psModule->strName	= strName;
		psModule->uRunTime	= uRunTime;
		psModule->pvArg		= pvArg;
		psModule->pv_func	= pv_func;
	
		struct sigevent sev;
		struct itimerspec its;

		sev.sigev_notify = SIGEV_THREAD;
		sev.sigev_notify_function = sysport_timer_thread;
		sev.sigev_value.sival_ptr = psModule;
		sev.sigev_notify_attributes = NULL;
	    if( 0 > timer_create(CLOCK_REALTIME, &sev, &psModule->timerid) )
		{
			sysport_timer_log( psModule->strName, 0, "create timer err" );
			free( psModule );
			psModule = NULL;
	    }
		else
		{
			its.it_value.tv_sec = uTimeMs/1000;
			its.it_value.tv_nsec = 1000000*(uTimeMs%1000);
			its.it_interval.tv_sec = uTimeMs/1000;
			its.it_interval.tv_nsec = 1000000*(uTimeMs%1000);

		    if( 0 > timer_settime(psModule->timerid, 0, &its, NULL) )
			{
				sysport_timer_log( psModule->strName, 0, "start timer %d err", psModule->timerid );
				free( psModule );
				psModule = NULL;
		    }
			else
			{
				sysport_timer_log( psModule->strName, 0, "start timer %d", psModule->timerid );
			}
		}
	}

	return psModule;
}

int sysport_timer_delete( void* pvModule )
{
	int eRet = 0;

	struct sysport_timer_module* psModule = (struct sysport_timer_module*)pvModule;

	if( 0 > timer_delete(psModule->timerid) )
	{
		sysport_timer_log( "delete timer %d err", psModule->timerid );
		eRet = -1;
	}
	else
	{
		sysport_timer_log( "delete timer %d", psModule->timerid );
		free( psModule );
	}

	return eRet;
}

