
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

#include <dirent.h>
#include <mqueue.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "sys/socket.h"
#include "sys/types.h"
#include <sys/stat.h>
#include <unistd.h>

#include "../storage.h"

#define STORAGE_LOG_BUFFER_SIZE		(5*1024)
#define STORAGE_LOG_MSG_MAX			(200)

typedef struct storage_log_info
{
	int			eLevel;
	const char*	strName;
	uint32_t	uLen;
	char		acLog[0];
}storage_log_info_t;

typedef struct storage_log_save_list
{
	char*		strName;
	struct storage_log_save_list* psNext;
}storage_log_save_list_t;

typedef struct storage_log
{
	int			eSysLog;
	int			eSaveLog;
	int 		eLevel;
	const char* strName;

	struct storage_log_save_list* psSaveList;

	mqd_t 		mq;
	pthread_t 	pid;
}storage_log_t;

static struct storage_log sLog = { 0 };

#define storage_log_printf( eLevel, strFormat, ...) \
    do{ \
		if( sLog.eSysLog ) \
			syslog( eLevel, strFormat, ##__VA_ARGS__ ); \
		else \
			printf( strFormat, ##__VA_ARGS__ ); \
    }while(0)

static void* storage_log_task_pthread( void* pvArg )
{
	for( ; ; )
	{
		struct storage_log_info* psInfo;
		struct mq_attr attr;
		mq_getattr( sLog.mq, &attr );
		int uLen = mq_receive( sLog.mq, (char*)(&psInfo), attr.mq_msgsize, NULL);
		if( sizeof(struct storage_log_info*) == uLen )
		{
			if( sLog.eSysLog )
			{
				syslog( psInfo->eLevel, "%s", psInfo->acLog );
			}
			else
			{
				int eRet = write( 1, psInfo->acLog, psInfo->uLen );
			}

			if( sLog.eSaveLog )
			{
				storage_record( "radarlog", psInfo->acLog, psInfo->uLen );
			}
			
			struct storage_log_save_list* psSaveList = sLog.psSaveList;
			for( ; psSaveList; psSaveList = psSaveList->psNext )
			{
				if( psSaveList->strName == psInfo->strName )
				{
					uint32_t uOffset = strlen(psSaveList->strName) + 1;
					storage_record( psSaveList->strName, psInfo->acLog + uOffset, psInfo->uLen - uOffset );
					break;
				}
			}
		}
		else
		{
			storage_log_printf( LOG_LEVEL_WARNING, "log no command" );
		}
		free( psInfo );
	}

	storage_log_printf( LOG_LEVEL_ERR, "log delete %ld", sLog.pid );
	if( sLog.eSysLog )
		closelog();
	mq_close( sLog.mq );
	mq_unlink( sLog.strName );
	return NULL;
}

int storage_log_init( void )
{
	int eRet = -1;

	sLog.eLevel		= LOG_LEVEL_WARNING;
	sLog.eSysLog	= true;
	sLog.eSaveLog	= true;
	sLog.strName 	= "/storage_log";

	if( sLog.eSysLog )
		openlog("radar_linux", LOG_CONS | LOG_PID | LOG_PERROR, LOG_USER);
	
	struct mq_attr attr;
	attr.mq_maxmsg = STORAGE_LOG_MSG_MAX;
	attr.mq_msgsize = sizeof(struct storage_log_info*);
	sLog.mq = mq_open( sLog.strName, O_CREAT|O_RDWR|O_CLOEXEC, 0666, &attr );
	if( 0 > sLog.mq )
	{
		storage_log_printf( LOG_LEVEL_ERR, "log create mq err" );
	}
	else
	{	
		if( 0 > pthread_create(&(sLog.pid), NULL, storage_log_task_pthread, &sLog) )
		{
			storage_log_printf( LOG_LEVEL_ERR, "log create pthread err" );
			mq_close( sLog.mq );
			mq_unlink( sLog.strName );
		}
		else
		{
			storage_log_printf( LOG_LEVEL_INFO, "log create %ld", sLog.pid );
		}
	}

	return eRet;
}

void storage_log( const char* strName, int eLevel, const char* strFormat, ... )
{
	if( eLevel > sLog.eLevel )
		return ;

	struct storage_log_info* psInfo = (struct storage_log_info*)malloc(sizeof(struct storage_log_info)+STORAGE_LOG_BUFFER_SIZE);

	if( psInfo )
	{
		psInfo->eLevel	= eLevel;
		psInfo->strName = strName;
		
		if( strName )
		{
			psInfo->uLen = strlen( strName );
			memcpy( psInfo->acLog, strName, psInfo->uLen );
			psInfo->acLog[psInfo->uLen++] = ':';
		}
		else
		{
			psInfo->uLen = 0;
		}

		psInfo->uLen += snprintf( (void*)(psInfo->acLog) + psInfo->uLen, STORAGE_LOG_BUFFER_SIZE - psInfo->uLen - 3, "%ld:", time( NULL ) );
		
		va_list args;
	    va_start( args, strFormat );
	    psInfo->uLen += vsnprintf( (void*)(psInfo->acLog) + psInfo->uLen, STORAGE_LOG_BUFFER_SIZE - psInfo->uLen - 3, strFormat, args );
	    va_end( args );

		psInfo->acLog[psInfo->uLen++] = '\r';
		psInfo->acLog[psInfo->uLen++] = '\n';
		psInfo->acLog[psInfo->uLen++] = 0;

		struct mq_attr attr;
		mq_getattr( sLog.mq, &attr );
		if( 0 > mq_send(sLog.mq,(char*)(&psInfo),attr.mq_msgsize,0) )
		{
			storage_log_printf( LOG_LEVEL_WARNING, "log send err %s", psInfo->acLog );
			free( psInfo );
		}
	}
}

