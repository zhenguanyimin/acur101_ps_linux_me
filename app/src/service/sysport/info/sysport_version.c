
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include "../../storage/storage.h"
#include "../sysport.h"

typedef struct sysport_version
{
	char	strVersion[64];
	char	strVersionPs[32];
	char	strVersionPl[32];
	char	strVersionAlgDet[32];
	char	strVersionAlgTrack[32];
	char	strVersionHwCode[32];
	char	strVersionHwSource[32];
}sysport_version_t;

static struct sysport_version sVersion= 
{
	.strVersion				= "N/A",
	.strVersionPs			= "N/A",
	.strVersionPl			= "N/A",
	.strVersionAlgDet		= "N/A",
	.strVersionAlgTrack		= "N/A",
	.strVersionHwCode		= "N/A",
	.strVersionHwSource		= "N/A",
};

static const char strVersionLogName[] = "version";

#define sysport_version_log( strFormat, ...) \
    do{ \
        storage_log( strVersionLogName, LOG_LEVEL_NOTICE, strFormat, ##__VA_ARGS__); \
    }while(0)

#define sysport_version_debug( strFormat, ...) \
    do{ \
        storage_log( strVersionLogName, LOG_LEVEL_DEBUG, strFormat, ##__VA_ARGS__); \
    }while(0)

int sysport_version_init( void )
{
	return 0;
}

const char *sysport_version( void )
{
	return (const char*)(sVersion.strVersion);
}

const char *sysport_version_ps( void )
{
	return (const char*)(sVersion.strVersionPs);
}

const char *sysport_version_pl( void )
{
	return (const char*)(sVersion.strVersionPl);}

const char *sysport_version_alg_detect( void )
{
	return (const char*)(sVersion.strVersionAlgDet);
}

const char *sysport_version_alg_track( void )
{
	return (const char*)(sVersion.strVersionAlgTrack);
}

const char *sysport_version_hw_code( void )
{
	return (const char*)(sVersion.strVersionHwCode);
}

const char *sysport_version_hw_source( void )
{
	return (const char*)(sVersion.strVersionHwSource);
}

