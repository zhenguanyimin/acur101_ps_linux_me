
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include "sysport/sysport.h"
#include "storage/storage.h"
#include "platfrom/platfrom.h"
#include "monitor/monitor.h"

void service_init( void )
{
	sysport_init();

	storage_init();

	platfrom_init();

	monitor_init();
}

