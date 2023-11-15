
#ifndef __SFBUS_INDEX_H__
#define __SFBUS_INDEX_H__

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

void* sfbus_index_create( int iTableSize );

int sfbus_index_delete( void* pvIndexTable );

int sfbus_index_get_empty_id( void* pvIndexTable );

void* sfbus_index_find_id( int eId, void* pvIndexTable );

int sfbus_index_add_id( int eId, void* pvIndex, void* pvIndexTable );

int sfbus_index_del_id( int eId, void* pvIndexTable );

#endif /* __SFBUS_INDEX_H__ */

