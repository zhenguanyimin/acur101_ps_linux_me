
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "log/storage_log.h"
#include "file/storage_file.h"
#include "record/storage_record.h"

#include "../sysport/sysport.h"

static const char strFolder[] = "/home/root/storage";

static const char strStorageLogName[] = "storage";

int storage_init( void )
{
	int eRet = 0;

	storage_log_init();

	if( 0 == access(strFolder, F_OK) )
	{
		storage_log( strStorageLogName, 0, "folder %s exist", strFolder );
	}
	else
	{
		if( 0 == mkdir(strFolder, 0777) )
		{
			storage_log( strStorageLogName, 0, "create folder %s", strFolder );
		}
		else
		{
			storage_log( strStorageLogName, 0, "create folder %s err %s", strFolder, strerror(errno) );
			eRet = -1;
		}
	}

	if( 0 <= eRet )
	{
		storage_record_init();
		storage_file_init();
	}

	return eRet;
}

