
#ifndef _RECORD_H_
#define _RECORD_H_

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#define RECORD_DEBUG	"RECORD_DEBUG"
#define RECORD_INFO		"RECORD_INFO"
#define RECORD_WARNING	"RECORD_WARNING"
#define RECORD_ERROR	"RECORD_ERROR"

int record_f_write( int type, char *src, char *record, uint32_t len );


/******************************************************************************
* @name		the folder name, it can be NULL when get all list

* @arg 		the arg use for @fun_cbk function

* @fun_cbk 	output the name for file or folder
			@file_type = 0 --folder, other --file
			@file_size is the bytes of the file
			@name is the name for file or folder, it containing the path

* return	0 --success, other --fail
******************************************************************************/
int record_get_list( char *name, void *arg, int (*fun_cbk)(void *arg, int file_type, int file_size, char* name) );

/******************************************************************************
* @name		the file name containing the path

* @offset 	read data offset addr for byte

* @buffer 	read data buffer

* @size 	the buffer size for 

* return	0 is success, other is fail
******************************************************************************/
int record_read( char *name, int offset, char *buffer, int size );

/******************************************************************************
* @name		the name for file or folder, it containing the path

* return	0 is success, other is fail
******************************************************************************/
int record_delete( char *name );

#endif

