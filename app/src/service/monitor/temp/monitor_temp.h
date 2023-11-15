
#ifndef __MONITOR_TEMP_H__
#define __MONITOR_TEMP_H__

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#define MONITOR_TEMP_FREQ_TIME	(1000)

int monitor_temp_init( void* pvModuleTask );
int get_temp_tx_fu(int uindex, char *strfu, int strlen);
int get_temp_code(char *rbuf, int rbuf_len);
int get_temp_vco(float *vco);

#endif /* __MONITOR_TEMP_H__ */

