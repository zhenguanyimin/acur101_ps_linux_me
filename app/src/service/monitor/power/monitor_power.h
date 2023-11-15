
#ifndef __MONITOR_POWER_H__
#define __MONITOR_POWER_H__

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#define MONITOR_POWER_FREQ_TIME	(1000)

int monitor_power_init( void* pvModuleTask );
int get_power_tx_fu(int uindex, char *strfu, int strlen);
int get_power_code(char *rbuf, int rbuf_len);
int get_power_vco(float *vco);

#endif /* __MONITOR_POWER_H__ */

