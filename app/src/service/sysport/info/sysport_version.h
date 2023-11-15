
#ifndef __SYSPORT_VERSION_H__
#define __SYSPORT_VERSION_H__

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

int sysport_version_init( void );

const char *sysport_version( void );
const char *sysport_version_ps( void );
const char *sysport_version_pl( void );
const char *sysport_version_alg_detect( void );
const char *sysport_version_alg_track( void );
const char *sysport_version_hw_code( void );
const char *sysport_version_hw_source( void );

#endif /* __SYSPORT_VERSION_H__ */

