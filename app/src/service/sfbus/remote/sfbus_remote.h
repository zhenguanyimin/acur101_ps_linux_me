
#ifndef __SFBUS_REMOTE_H__
#define __SFBUS_REMOTE_H__

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

int sfbus_remote_init( void );

int sfbus_remote_register( int eFunctionId, void* pvFunction, void* pvDescription );

#endif /* __SFBUS_REMOTE_H__ */

