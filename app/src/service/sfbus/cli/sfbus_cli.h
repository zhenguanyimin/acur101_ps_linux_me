
#ifndef __SFBUS_CLI_H__
#define __SFBUS_CLI_H__

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

int sfbus_cli_init( void );

int sfbus_cli_register( int eFunctionId, void* pvFunction, void* pvDescription );

#endif /* __SFBUS_CLI_H__ */

