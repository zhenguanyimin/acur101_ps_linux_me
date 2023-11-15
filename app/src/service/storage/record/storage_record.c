
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

#define STORAGE_RECORD_PRINTF_BUFFER	(5*1024)
#define STORAGE_RECORD_FILE_SIZE_MAX	(1024*1024)
#define STORAGE_RECORD_NAME_LEN_MAX		(256)
#define STORAGE_RECORD_TASK_MSG_SIZE	(200)

typedef struct storage_record_info
{
	const char*	strName;
	uint32_t	uLen;
	char		acRecord[0];
}storage_record_info_t;

typedef struct storage_record_name
{
	int			fd;
	uint32_t	uWriteCount;
	uint32_t	uNameNum;
	char*		strName;
	struct storage_record_name* psFather;
	struct storage_record_name* psChild;
}storage_record_name_t;

typedef struct storage_record_module
{
	struct storage_record_name*	psName;

	bool		bFlagInit;
	char 		*strName;
	mqd_t 		mq;
	pthread_t 	pid;
}storage_record_module_t;

static const char strRecordLogName[] = "record";

static const char strRecordFolder[] = "/home/root/storage/record/";

static volatile uint32_t uRecordFolderNum = 1;

static struct storage_record_module sRecord = { 0 };

#define storage_record_log( strFormat, ...) \
    do{ \
        storage_log( strRecordLogName, LOG_LEVEL_NOTICE, strFormat, ##__VA_ARGS__); \
    }while(0)

#define storage_record_debug( strFormat, ...) \
    do{ \
        storage_log( strRecordLogName, LOG_LEVEL_DEBUG, strFormat, ##__VA_ARGS__); \
    }while(0)

static int32_t storage_record_folder_max( void )
{
	int32_t iFolderNum = 0;
	
	DIR* dir = opendir(strRecordFolder);
	if( NULL == dir )
	{
		storage_record_log( "open folder err %s", strerror(errno) );
		iFolderNum = -1;
	}
	else
	{
		struct dirent* entry;
		while( NULL != (entry = readdir(dir)) )
		{
			if( '0' < entry->d_name[0] && '9' >= entry->d_name[0] )
			{
				int32_t iTemp = atoi(entry->d_name);
				if( iTemp > iFolderNum )
					iFolderNum = iTemp;
			}
		}
		closedir(dir);
	}
	
	return iFolderNum;
}

static int32_t storage_record_folder_min( void )
{
	int32_t iFolderNum = -1;
	
	DIR* dir = opendir(strRecordFolder);
	if (dir == NULL)
	{
		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL) 
		{
			storage_record_log( "folder name %s", entry->d_name );
			if( '0' < entry->d_name[0] && '9' >= entry->d_name[0] )
			{
				int32_t iTemp = atoi(entry->d_name);
				if( iTemp < (uint32_t)(iFolderNum) )
					iFolderNum = iTemp;
			}
		}
		closedir(dir);
	}
	else
	{
		storage_record_log( "open folder err %s", strerror(errno) );
	}

	return iFolderNum;
}

static int storage_record_folder_inspect( void )
{
	int eRet = 0;
	
	if( 0 == access(strRecordFolder, F_OK) )
	{
		storage_record_log( "folder %s exist", strRecordFolder );
	}
	else
	{
		eRet = mkdir( strRecordFolder, 0777 );
		if( 0 == eRet )
		{
			storage_record_log( "create folder %s", strRecordFolder );
		}
		else
		{
			storage_record_log( "create folder %s err %s", strRecordFolder, strerror(errno) );
		}
	}

	return eRet;
}

static int storage_record_folder_create( void )
{
	int eRet = -1;

	char strFolderName[STORAGE_RECORD_NAME_LEN_MAX];
	memcpy( strFolderName, strRecordFolder, sizeof(strRecordFolder) );
	
	snprintf( strFolderName + sizeof(strRecordFolder) - 1, sizeof(strFolderName) - sizeof(strRecordFolder) - 1, "%d", uRecordFolderNum );
	
	eRet = mkdir( strFolderName, 0777 );
	if( 0 == eRet )
	{
		storage_record_log( "create folder %s", strRecordFolder );
	}
	else
	{
		storage_record_log( "create folder %s err %s", strRecordFolder, strerror(errno) );
	}
	
    return eRet;
}

static int storage_record_file_create( struct storage_record_name* psName )
{
	int eRet = -1;

	if( psName )
	{
		char* pstrIndex;
		if( NULL == psName->strName )
			pstrIndex = "log";
		else
			pstrIndex = psName->strName;
		for( ; '/' == *pstrIndex; ++pstrIndex );
		char strFolderName[STORAGE_RECORD_NAME_LEN_MAX];
		memcpy( strFolderName, strRecordFolder, sizeof(strRecordFolder) );
		snprintf( strFolderName + sizeof(strRecordFolder) - 1, sizeof(strFolderName) - sizeof(strRecordFolder) - 1, "%d/%s_%d.txt", uRecordFolderNum, pstrIndex, psName->uNameNum );		

		psName->fd = open( strFolderName, O_RDWR | O_CREAT, 0666 );
		if( 0 > psName->fd )
		{
			storage_record_log( "open err %s", strerror(errno) );
		}
		else
		{
			psName->uWriteCount = 0;
			storage_record_log( "open %s", strFolderName );
			eRet = 0;
		}
	}

	return eRet;
}

static int storage_record_file_write( struct storage_record_name* psName, struct storage_record_info* psInfo )
{
	int eRet = 0;

	if( STORAGE_RECORD_FILE_SIZE_MAX < psName->uWriteCount )
	{
		close(psName->fd);

		psName->uNameNum += 1;
		eRet = storage_record_file_create( psName );
	}

	if( 0 <= eRet )
	{
		eRet = write( psName->fd, psInfo->acRecord, psInfo->uLen );
		if( 0 > eRet )
		{
			storage_record_log( "write err %s", strerror(errno) );
		}
		else
		{
			psName->uWriteCount += psInfo->uLen;
		}
	}

	return eRet;
}

static int storage_record_file_add( struct storage_record_info* psInfo )
{
	int eRet = -1;
		
	struct storage_record_name* psName = (struct storage_record_name*)malloc( sizeof(struct storage_record_name) );

	if( psName )
	{
		psName->uNameNum	= 1;
		psName->strName		= (char*)(psInfo->strName);
		eRet = storage_record_file_create( psName );
		if( 0 > eRet )
		{
			free( psName );
		}
		else
		{
			if( NULL == sRecord.psName )
			{
				psName->psFather	= NULL;
				psName->psChild		= NULL;
				sRecord.psName		= psName;
			}
			else
			{
				struct storage_record_name* psIndex = sRecord.psName;
				for( ; psIndex->psChild; psIndex = psIndex->psChild );
				psName->psFather	= psIndex;
				psName->psChild		= NULL;
				psIndex->psChild	= psName;
			}
			eRet = storage_record_file_write( psName, psInfo );
		}
	}

	return eRet;
}

static void* storage_record_task_pthread( void* pvArg )
{	
	for( ; ; )
	{
		struct storage_record_info* psInfo;
		struct mq_attr attr;
		mq_getattr( sRecord.mq, &attr );
		int uLen = mq_receive( sRecord.mq, (char*)(&psInfo), attr.mq_msgsize, NULL);
		if( sizeof(struct storage_record_info*) == uLen )
		{
			struct storage_record_name* psName = sRecord.psName;
			for( ; psName; psName = psName->psChild )
			{
				if( psName->strName == psInfo->strName )
				{
					storage_record_file_write( psName, psInfo );
					break;
				}
			}
			if( NULL == psName )
			{
				storage_record_file_add( psInfo );
			}
		}
		else
		{
			storage_record_log( "no command" );
		}
		free( psInfo );
	}

	storage_record_log( "task delete %d", sRecord.pid );
	mq_close( sRecord.mq );
	mq_unlink( sRecord.strName );

	return NULL;
}

int storage_record_task_create( void )
{
	int eRet = -1;

	sRecord.strName	= "/storage_record";
	struct mq_attr attr;
	attr.mq_maxmsg = STORAGE_RECORD_TASK_MSG_SIZE;
	attr.mq_msgsize = sizeof(struct storage_record_info*);
	sRecord.mq = mq_open( sRecord.strName, O_CREAT|O_RDWR|O_CLOEXEC, 0666, &attr );
	if( 0 > sRecord.mq )
	{
		storage_record_log( "task create mq err" );
	}
	else
	{
		if( 0 > pthread_create(&(sRecord.pid), NULL, storage_record_task_pthread, &sRecord) )
		{
			storage_record_log( "task create pthread err" );
			mq_close( sRecord.mq );
			mq_unlink( sRecord.strName );
		}
		else
		{
			sRecord.bFlagInit = true;
			storage_record_log( "task create %d", sRecord.pid );
		}
	}

	return eRet;
}

int storage_record_init( void )
{
	int eRet = storage_record_folder_inspect();

	if( 0 <= eRet && false == sRecord.bFlagInit )
	{
		int32_t iNum = storage_record_folder_max();
		if( 0 <= iNum )
		{
			uRecordFolderNum = iNum + 1;
			eRet = storage_record_folder_create();
			if( 0 <= eRet )
			{
				eRet = storage_record_task_create();
			}
		}
	}

	return eRet;
}

void* storage_record_malloc( const char* strName, uint32_t uSize )
{
	void* pvRecord = NULL;

	if( true == sRecord.bFlagInit )
	{
		struct storage_record_info* psInfo = malloc( sizeof(struct storage_record_info) + uSize );

		if( psInfo )
		{
			psInfo->strName	= strName;
			pvRecord = psInfo->acRecord;
		}
	}
	
	return pvRecord;
}

int storage_record_send_malloc( void* pvRecord, uint32_t uLen )
{
	int eRet = -1;

	if( pvRecord )
	{
		struct storage_record_info* psInfo = (struct storage_record_info *)((char *)pvRecord - (((size_t)&((struct storage_record_info*)0)->acRecord)));
		psInfo->uLen = uLen;

		struct mq_attr attr;
		mq_getattr( sRecord.mq, &attr );
		eRet = mq_send( sRecord.mq, (char*)(&psInfo), attr.mq_msgsize, 0 );
		if( 0 > eRet )
		{
			storage_record_log( "send mq err %p", psInfo );
			free( psInfo );
		}
	}
	
	return eRet;
}

int storage_record_printf( const char* strName, const char* strFormat, ... )
{
	int eRet = -1;

	char* strRecord = storage_record_malloc( strName, STORAGE_RECORD_PRINTF_BUFFER );

	if( strRecord )
	{
		uint32_t uLen = 0;
		va_list args;
	    va_start( args, strFormat );
		uLen = snprintf( strRecord, STORAGE_RECORD_PRINTF_BUFFER, strFormat, args );
	    va_end( args );
		eRet = storage_record_send_malloc( strRecord, uLen );
	}

	return eRet;
}

int storage_record( const char* strName, void* pvRecord, uint32_t uLen )
{
	int eRet = -1;

	char* strRecord = storage_record_malloc( strName, uLen );

	if( strRecord )
	{
		memcpy( strRecord, pvRecord, uLen );
		eRet = storage_record_send_malloc( strRecord, uLen );
	}

	return eRet;
}



