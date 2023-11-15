
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include "monitor.h"
#include "temp/monitor_temp.h"
#include "network/network_monitor.h"
#include "power/monitor_power.h"
#include "../sysport/sysport.h"

static void* pvMonitorTask = NULL;

int monitor_init( void )
{
	int eRet = 0;

	pvMonitorTask = sysport_task_create( "/monitor_task", NULL );

	monitor_temp_init( pvMonitorTask );
	monitor_power_init( pvMonitorTask );
	monitor_network_init(pvMonitorTask);

	return eRet;
}

