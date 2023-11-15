
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <mqueue.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>
#include <math.h>
#include <semaphore.h>
#include <mqueue.h>

#include <sys/time.h>
#include "sys/types.h"
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "monitor_temp.h"
#include "../monitor.h"
#include "../../storage/storage.h"
#include "../../sysport/sysport.h"
#include "../../../bsp/reg/pl_reg.h"
#include "../../../hal/chc2442_hal/chc2442_config.h"

#define TEMP_READ_PL_REG(addr)		PL_REG_READ(0xFFFF&addr)

typedef struct monitor_temp_code
{
	float	fBoard;
	float	fPs;
	float	fPl;
}monitor_temp_code_t;

typedef struct monitor_temp_source
{
	float	fVco;
}monitor_temp_source_t;

typedef struct monitor_temp_tx
{
	float	fU8;
	float	fU9;
	float	fU10;
	float	fU11;
	float	fU12;
	float	fU13;
	float	fU14;
	float	fU15;
	float	fU16;
}monitor_temp_tx_t;

typedef struct monitor_temp_rx
{
	float	fU17;
	float	fU18;
	float	fU19;
	float	fU20;
	float	fU21;
	float	fU22;
	float	fU23;
	float	fU24;
}monitor_temp_rx_t;

typedef struct monitor_temp
{
	struct monitor_temp_code 	sCode;
	struct monitor_temp_tx		sTx;
	struct monitor_temp_rx		sRx;
	struct monitor_temp_source	sSource;

	void*	pvModuleTask;
	void*	pvModuleTimer;
}monitor_temp_t;

static struct monitor_temp sMonitorTemp = { 0 };

static const char strTempLogName[] = "temp";

#define monitor_temp_log( strFormat, ...) \
    do{ \
        storage_log( strTempLogName, LOG_LEVEL_NOTICE, strFormat, ##__VA_ARGS__); \
    }while(0)

#define monitor_temp_debug( strFormat, ...) \
    do{ \
        storage_log( strTempLogName, LOG_LEVEL_DEBUG, strFormat, ##__VA_ARGS__); \
    }while(0)

static void monitor_temp_code_read( void )
{
	int fd = open( "/sys/class/hwmon/hwmon0/temp1_input", O_RDONLY, 0777 );
	if( 0 <= fd )
	{
		char temp[16] = { 0 };
		int len = read( fd, &temp, sizeof(temp) );
		sMonitorTemp.sCode.fBoard = atol(temp)/1000.0;
	}
	close( fd );
	
	fd = open( "/sys/bus/iio/devices/iio:device0/in_temp0_ps_temp_raw", O_RDONLY, 0777 );
	if( 0 <= fd )
	{
		char temp[16] = { 0 };
		int len = read( fd, &temp, sizeof(temp) );
		sMonitorTemp.sCode.fPs = 507.5921310*atol(temp)/65536.0 - 279.42657680;
	}
	close( fd );

	sMonitorTemp.sCode.fPl = 507.5921310*TEMP_READ_PL_REG(0x80020804)/65536.0 - 279.42657680;
}

static void monitor_temp_source_read( void )
{
	CHC2442_MUX_mode_setting( 1 );

	usleep( 10000 );
	
	int fd = open( "/dev/i2c-0", O_RDWR );
    if( 0 > fd )
	{
    	monitor_temp_log( "iic open err %s", strerror(errno) );
    }
	else
	{
		if( 0 > ioctl(fd,I2C_SLAVE,0x49) )
		{
        	monitor_temp_log( "acquire bus access err %s", strerror(errno) );
    	}
		else
		{
    		char buf[3] = { 0x01, 0x42, 0xE3 };
		    if( 0 > write(fd,buf,3) )
			{
		        monitor_temp_log( "write device err", strerror(errno) );
		    }
		}

		int32_t iTemp = 0;
		for( int i = 0; 8 > i; ++i )
		{
			usleep( 1000 );
			if( 0 > ioctl(fd,I2C_SLAVE,0x49) )
			{
	        	monitor_temp_log( "acquire bus access err %s", strerror(errno) );
	    	}
			else
			{
				int reg = 0x00;
	    		char buf[2] = { 0x42, 0xE3 };
				if( 0 > write(fd,&reg,1) )
				{
			        monitor_temp_log( "write reg addr err %s", strerror(errno) );
			    }

			    if( 0 > read(fd,buf,2) )
				{
			        monitor_temp_log( "read device err", strerror(errno) );
			    }
				else
				{
					iTemp += (int16_t)((buf[0]<<8) | buf[1]);
				}
			}
		}
		sMonitorTemp.sSource.fVco = ((4.096*(iTemp/8.0)/32767.0) - 1.9351)/0.0067;
		close( fd );
	}
}

static void monitor_temp_read_handler( void* pvArg, void* pvInfo, uint32_t uSize )
{
	monitor_temp_code_read();
	monitor_temp_source_read();

	monitor_temp_log( 
					"\r\nCODE: BOARD %f, PS %f, PL %f\r\nSOURCE: VCO %f\r\nTX: U8 %f, U9 %f, U10 %f, U11 %f, U12 %f, U13 %f, U14 %f, U15 %f, U16 %f\r\nRX: U17 %f, U18 %f, U19 %f, U20 %f, U21 %f, U22 %f, U23 %f, U24 %f\r\n",
					
					sMonitorTemp.sCode.fBoard,
					sMonitorTemp.sCode.fPs,
					sMonitorTemp.sCode.fPl,
	
					sMonitorTemp.sSource.fVco,

					sMonitorTemp.sTx.fU8,
					sMonitorTemp.sTx.fU9,
					sMonitorTemp.sTx.fU10,
					sMonitorTemp.sTx.fU11,
					sMonitorTemp.sTx.fU12,
					sMonitorTemp.sTx.fU13,
					sMonitorTemp.sTx.fU14,
					sMonitorTemp.sTx.fU15,
					sMonitorTemp.sTx.fU16,
					
					sMonitorTemp.sRx.fU17,
					sMonitorTemp.sRx.fU18,
					sMonitorTemp.sRx.fU19,
					sMonitorTemp.sRx.fU20,
					sMonitorTemp.sRx.fU21,
					sMonitorTemp.sRx.fU22,
					sMonitorTemp.sRx.fU23,
					sMonitorTemp.sRx.fU24
					);
}

static void monitor_temp_read( void* pvArg )
{
	struct monitor_temp* psModule = (struct monitor_temp*)(&sMonitorTemp);

	void* pvInfo = sysport_task_malloc( psModule->pvModuleTask, 0 );
	if( pvInfo )
	{
		sysport_task_func( psModule->pvModuleTask, monitor_temp_read_handler, pvInfo );
	}
}

int monitor_temp_init( void* pvModuleTask )
{
	int eRet = 0;

	sMonitorTemp.pvModuleTask = pvModuleTask;
	sMonitorTemp.pvModuleTimer = sysport_timer_create( "/monitor_temp_timer", MONITOR_TEMP_FREQ_TIME, 0, &sMonitorTemp, monitor_temp_read );
		
	return eRet;
}

static void monitor_temp_txrx_updata_handler( void* pvArg, void* pvInfo, uint32_t uSize )
{
	float *piTemp = (float*)pvInfo;
	sMonitorTemp.sTx.fU8	= piTemp[16];
	sMonitorTemp.sTx.fU9	= piTemp[0];
	sMonitorTemp.sTx.fU10	= piTemp[1];
	sMonitorTemp.sTx.fU11	= piTemp[2];
	sMonitorTemp.sTx.fU12	= piTemp[3];
	sMonitorTemp.sTx.fU13	= piTemp[4];
	sMonitorTemp.sTx.fU14	= piTemp[5];
	sMonitorTemp.sTx.fU15	= piTemp[6];
	sMonitorTemp.sTx.fU16	= piTemp[7];

	sMonitorTemp.sRx.fU17	= piTemp[8];
	sMonitorTemp.sRx.fU18	= piTemp[9];
	sMonitorTemp.sRx.fU19	= piTemp[10];
	sMonitorTemp.sRx.fU20	= piTemp[11];
	sMonitorTemp.sRx.fU21	= piTemp[12];
	sMonitorTemp.sRx.fU22	= piTemp[13];
	sMonitorTemp.sRx.fU23	= piTemp[14];
	sMonitorTemp.sRx.fU24	= piTemp[15];
}

void monitor_temp_updata_txrx( void* pvData )
{
	struct monitor_temp* psModule = (struct monitor_temp*)(&sMonitorTemp);

	void* pvInfo = sysport_task_malloc( psModule->pvModuleTask, 17*sizeof(float) );
	memcpy( pvInfo, pvData, 17*sizeof(float) );
	if( pvInfo )
	{
		sysport_task_func( psModule->pvModuleTask, monitor_temp_txrx_updata_handler, pvInfo );
	}
}

int get_temp_vco(float *vco)
{
	*vco = sMonitorTemp.sSource.fVco;

	return 0;
}

int get_temp_code(char *rbuf, int rbuf_len)
{
	snprintf(rbuf, rbuf_len,\ 
		"fBoard:%f\r\n"
		"fPs:%f\r\n"
		"fPs:%f\r\n",\
		sMonitorTemp.sCode.fBoard,\
		sMonitorTemp.sCode.fPs,\
		sMonitorTemp.sCode.fPl\
		);

	return 0;
}

int get_temp_tx_fu(int uindex, char *strfu, int strlen)
{
	int ret = -1;
	float fu = 0;

	if(uindex < 8 || uindex > 16) {
		return ret;	
	}

	switch(uindex) {
		case 8:
			fu = sMonitorTemp.sTx.fU8;
		break;
		case 9:
			fu = sMonitorTemp.sTx.fU9;
		break;
		case 10:
			fu = sMonitorTemp.sTx.fU10;
		break;
		case 11:
			fu = sMonitorTemp.sTx.fU11;
		break;
		case 12:
			fu = sMonitorTemp.sTx.fU12;
		break;
		case 13:
			fu = sMonitorTemp.sTx.fU13;
		break;
		case 14:
			fu = sMonitorTemp.sTx.fU14;
		break;
		case 15:
			fu = sMonitorTemp.sTx.fU15;
		break;
		case 16:
			fu = sMonitorTemp.sTx.fU16;
		break;
		default:
			return -1;
	}

	snprintf(strfu, strlen, "fu%d: %f\r\n", uindex, fu);

	return 0;
}

