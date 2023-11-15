
#ifndef __SYSPORT_SN_H__
#define __SYSPORT_SN_H__

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

int sysport_sn_init( void );

const char *sysport_sn( void );
const char *sysport_sn_hardware( void );
const char *sysport_sn_source( void );

#endif /* __SYSPORT_SN_H__ */

