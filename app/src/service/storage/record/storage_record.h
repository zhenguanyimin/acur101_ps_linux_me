
#ifndef __STORAGE_RECORD_H__
#define __STORAGE_RECORD_H__

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

int storage_record_init( void );

void* storage_record_malloc( const char* strName, uint32_t uSize );
int storage_record_send_malloc( void* pvRecord, uint32_t uLen );
int storage_record_printf( const char* strName, const char* strFormat, ... );
int storage_record( const char* strName, void* pvRecord, uint32_t uLen );

#endif /* __STORAGE_RECORD_H__ */

