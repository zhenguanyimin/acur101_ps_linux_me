
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
#include "sys/types.h"
#include <sys/stat.h>
#include <pthread.h>

#include "../storage.h"
#include "../../sysport/sysport.h"

#define STORAGE_FILE_NAME_LEN_MAX	(256)

static const char strFileLogName[] = "file";

static const char strFileFolder[] = "/home/root/storage/file/";

typedef struct storage_file_module
{
	pthread_mutex_t 	mutex;
}storage_file_module_t;

static struct storage_file_module sFile = { 0 };

#define storage_file_log( strFormat, ...) \
    do{ \
        storage_log( strFileLogName, LOG_LEVEL_NOTICE, strFormat, ##__VA_ARGS__); \
    }while(0)

#define storage_file_debug( strFormat, ...) \
    do{ \
        storage_log( strFileLogName, LOG_LEVEL_DEBUG, strFormat, ##__VA_ARGS__); \
    }while(0)

static void storage_file_calculate_name( char* strFileName, const char* strName )
{
	memcpy( strFileName, strFileFolder, sizeof(strFileFolder) );
	for( ; '/' == *strName; ++strName );
	strcpy( strFileName + sizeof(strFileFolder) - 1, strName );
}

int storage_file_folder_create( void )
{
	int eRet = 0;
	
	if( 0 == access(strFileFolder, F_OK) )
	{
		storage_file_log( "folder %s exist", strFileFolder );
	}
	else
	{
		if( 0 == mkdir(strFileFolder, 0777) )
		{
			storage_file_log( "create folder %s", strFileFolder );
		}
		else
		{
			storage_file_log( "create folder %s err %s", strFileFolder, strerror(errno) );
			eRet = -1;
		}
	}

	return eRet;
}

int storage_file_init( void )
{	
	int eRet;

	eRet = storage_file_folder_create();

	if( 0 <= eRet )
	{
		eRet = pthread_mutex_init( &(sFile.mutex), NULL );
		if( 0 > eRet )
		{
			storage_file_log( "create mutex err %s", strerror(errno) );
		}
		else
		{
			storage_file_log( "create mutex" );
		}
	}

	return eRet;
}

int storage_file_write( const char* strName, void* pvData, uint32_t uLen )
{
	int ret = 0;

	if( strName && pvData )
	{
		char strFileName[STORAGE_FILE_NAME_LEN_MAX];
		storage_file_calculate_name( strFileName, strName );

		pthread_mutex_lock( &(sFile.mutex) );
		
		int fd = open( strFileName, O_WRONLY | O_CREAT | O_TRUNC, 0666 );
		if(fd < 0)
		{
			storage_file_log( "write open file %s err %s", strFileName, strerror(errno) );
			ret = -1;
		}
		else
		{
			if( 0 > write( fd, pvData, uLen ) )
			{
				storage_file_log( "write file %s err %s", strFileName, strerror(errno) );
				ret = -1;
			}
			
			close(fd);
		}

		pthread_mutex_unlock( &(sFile.mutex) );
	}
	
	return ret;
}

int storage_file_read( const char* strName, void* pvBuffer, uint32_t uSize )
{
	int len = 0;

	if( strName && pvBuffer )
	{
		char strFileName[STORAGE_FILE_NAME_LEN_MAX];
		storage_file_calculate_name( strFileName, strName );
		
		int fd = open( strFileName, O_RDONLY, 0666 );
		if(fd < 0)
		{
			storage_file_log( "read open file %s err %s", strFileName, strerror(errno) );
		}
		else
		{
			len = read( fd, pvBuffer, uSize );
			if( 0 > len )
			{
				storage_file_log( "read file %s err %s", strFileName, strerror(errno) );
				len = 0;
			}
			
			close(fd);
		}
	}
	
	return len;
}

int storage_file_del( const char* strName )
{
	int ret = -1;

	if( strName )
	{
		char strFileName[STORAGE_FILE_NAME_LEN_MAX];
		storage_file_calculate_name( strFileName, strName );
		
		ret = unlink( strFileName );
		if( 0 > ret )
		{
			storage_file_log( "del file %s err %s", strFileName, strerror(errno) );
			ret = -1;
		}
	}
	
	return ret;
}

