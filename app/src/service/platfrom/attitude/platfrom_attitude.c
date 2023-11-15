
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include "platfrom_attitude.h"
#include "../platfrom.h"
#include "../../sysport/sysport.h"
#include "../../../drv/gyro/gyro_uart.h"
#include "../../../srv/log/log.h"

typedef struct platfrom_attitude_cfg
{
	uint16_t	wFreqTime;
}platfrom_attitude_cfg_t;

typedef struct platfrom_attitude_gyro
{
	float	x;
	float	y;
	float	z;
}platfrom_attitude_gyro_t;

typedef struct platfrom_attitude_acc
{
	float	x;
	float	y;
	float	z;
}platfrom_attitude_acc_t;

typedef  struct platfrom_attitude_mag
{
	float	x;
	float	y;
	float	z;	
}platfrom_attitude_mag_t;

typedef struct platfrom_attitude
{
	struct platfrom_attitude_cfg	sCfg;

	struct platfrom_attitude_gyro	sGyro;
	struct platfrom_attitude_acc	sAcc;
	struct platfrom_attitude_mag 	sMag;
	struct platfrom_attitude_angle	sAngle;

	void * sg_gyro_handle;
	struct ev_loop *loop;

	void*	pvModuleTask;
	void*	pvModuleTimer;
}platfrom_attitude_t;

static struct platfrom_attitude sAttitude = { 0 };

static void platfrom_attitude_read_handler( void* pvArg, void* pvInfo, uint32_t uSize )
{
	gyro_angle_info_t angleinfo = {0};
	int ret;

	ret = gyro_get_angle_info(&angleinfo, 1);	
	if(ret < 0) {
		APP_LOG_ERROR("get gyro info fail %d\n", ret);
		return;
	}
	
	//APP_LOG_DEBUG("===> gyro:\nroll = %f, pitch = %f, yaw = %f\n", angleinfo.roll, angleinfo.pitch, angleinfo.yaw);

}

static void platfrom_attitude_read_module( void* pvModule )
{
	if( pvModule )
	{
		struct platfrom_attitude* psModule = (struct platfrom_attitude*)(pvModule);

		void* pvInfo = sysport_task_malloc( psModule->pvModuleTask, 0 );
		if( pvInfo )
		{
			sysport_task_func( psModule->pvModuleTask, platfrom_attitude_read_handler, pvInfo );
		}
	}
}

static void platfrom_attitude_read( void )
{
	platfrom_attitude_read_module( &sAttitude );
}

int platfrom_attitude_init( void )
{
	int eRet = -1;
	
	eRet = gyro_init();
	if(0 != eRet) {
		APP_LOG_ERROR("init gyro fail\n");
		return eRet;
	}

	sAttitude.pvModuleTask = sysport_task_create( "/platfrom_attitude_task", &sAttitude );	
	if( sAttitude.pvModuleTask )
	{
		sAttitude.pvModuleTimer = sysport_timer_create( "/platfrom_attitude_timer", PLATFROM_ATTITUDE_FREQ_TIME, 0, &sAttitude, platfrom_attitude_read_module );
		if( sAttitude.pvModuleTimer )
		{
			return 0;
		}
		else
		{
			sysport_task_delete( sAttitude.pvModuleTask );
		}
	}

	return eRet;
}

