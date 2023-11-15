
#ifndef __CONTROL_H__
#define __CONTROL_H__

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

/* cli */
typedef struct control_cli_fun
{
	const char* const case;
	const char* const help;
	const void* const func;
}control_cli_fun_t;

int control_cli_register( control_cli_fun_t* fun );

#endif /* __CONTROL_H__ */

