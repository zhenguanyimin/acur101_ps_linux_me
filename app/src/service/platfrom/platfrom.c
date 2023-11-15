
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include "platfrom.h"
#include "attitude/platfrom_attitude.h"
#include "gps/platfrom_gps.h"

#include "../sysport/sysport.h"

static void* pvPlatfromTask = NULL;

void platfrom_init( void )
{
	pvPlatfromTask = sysport_task_create( "/platfrom_task", NULL );

	platfrom_attitude_init();
	platfrom_gps_init();
}

