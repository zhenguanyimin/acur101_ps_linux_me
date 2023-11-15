
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include <dirent.h>
#include <mqueue.h>
#include <unistd.h>
#include "sys/types.h"
#include <sys/stat.h>
#include <pthread.h>

#include "../../storage/storage.h"
#include "../sysport.h"

typedef struct sysport_sn
{
	char	strSn[64];
}sysport_sn_t;

static struct sysport_sn sSn =
{
	.strSn	= "N/A",
};

static const char strSnLogName[] = "sn";

#define sysport_sn_log( strFormat, ...) \
    do{ \
        storage_log( strSnLogName, LOG_LEVEL_NOTICE, strFormat, ##__VA_ARGS__); \
    }while(0)

#define sysport_sn_debug( strFormat, ...) \
    do{ \
        storage_log( strSnLogName, LOG_LEVEL_DEBUG, strFormat, ##__VA_ARGS__); \
    }while(0)

int sysport_sn_init( void )
{
	int eRet = -1;

	int fd = open( "/run/media/mtd_user/sn", O_RDONLY, 0666 );
	if(fd < 0)
	{
		sysport_sn_log( "read sn err %s", strerror(errno) );
	}
	else
	{
		int len = read( fd, sSn.strSn, sizeof(sSn.strSn) );
		if( 0 > len )
		{
			sysport_sn_log( "read sn err %s", strerror(errno) );
		}
		else
		{
			eRet = 0;
		}
		close(fd);
	}

	return eRet;
}

const char *sysport_sn( void )
{	
	return (const char*)(sSn.strSn);
}

const char *sysport_sn_hardware( void )
{
	return "N/A";
}

const char *sysport_sn_source( void )
{
	return "N/A";
}

