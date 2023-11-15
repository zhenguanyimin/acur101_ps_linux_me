
#ifndef __STORAGE_LOG_H__
#define __STORAGE_LOG_H__

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

int storage_log_init( void );

void storage_log( const char* strName, int eLevel, const char* strFormat, ... );

#endif /* __STORAGE_LOG_H__ */

