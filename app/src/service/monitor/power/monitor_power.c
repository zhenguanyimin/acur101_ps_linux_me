
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

#include "monitor_power.h"
#include "../monitor.h"
#include "../../storage/storage.h"
#include "../../sysport/sysport.h"
#include "../../../hal/chc2442_hal/chc2442_config.h"

typedef struct monitor_power_code
{
	float	fBoard[3];
	float	fPs[3];
	float	fPl[3];

	float	fVcc_3v3_mcu[4];
	float	fTx_2v1_A[4];
	float	fTx_2v1_B[4];
	float	fTx_1v9_A[4];
	float	fTx_1v9_B[4];
	float	fRx_1v9[4];
	float	fVcc_src_5v3[4];
	float	fVcc_src_3v3[4];
	float	fVcc_fpga_v58[4];
	float	fVcc_fpga_0v9[4];
	float	fVcc_fpga_1v8[4];
	float	fVcc_2v5[4];
	float	fVcc_fpga_1v2[4];
	float	fVcc_fpga_3v3[4];
	float	fVcc_2nd_1v8[4];
	float	fDdr_vtt_0v6[4];
}monitor_power_code_t;

typedef struct monitor_power_source
{
	float	fVco;
}monitor_power_source_t;

typedef struct monitor_power_tx
{
	float	fU8[8];
	float	fU9[8];
	float	fU10[8];
	float	fU11[8];
	float	fU12[8];
	float	fU13[8];
	float	fU14[8];
	float	fU15[8];
	float	fU16[8];
}monitor_power_tx_t;

typedef struct monitor_power_rx
{
	float	fU17[8];
	float	fU18[8];
	float	fU19[8];
	float	fU20[8];
	float	fU21[8];
	float	fU22[8];
	float	fU23[8];
	float	fU24[8];
}monitor_power_rx_t;

typedef struct monitor_power
{
	struct monitor_power_code 	sCode;
	struct monitor_power_tx		sTx;
	struct monitor_power_rx		sRx;
	struct monitor_power_source	sSource;

	void*	pvModuleTask;
	void*	pvModuleTimer;
}monitor_power_t;

static struct monitor_power sMonitorPower = { 0 };

static const char strPowerLogName[] = "power";

#define monitor_power_log( strFormat, ...) \
    do{ \
        storage_log( strPowerLogName, LOG_LEVEL_NOTICE, strFormat, ##__VA_ARGS__); \
    }while(0)

#define monitor_power_debug( strFormat, ...) \
    do{ \
        storage_log( strPowerLogName, LOG_LEVEL_DEBUG, strFormat, ##__VA_ARGS__); \
    }while(0)

static void monitor_power_code_iic_write_reg( uint8_t cbAddr, uint8_t cbReg, uint16_t wData )
{
	int fd = open( "/dev/i2c-3", O_RDWR );
    if( 0 > fd )
	{
    	monitor_power_log( "iic open err %s", strerror(errno) );
    }
	else
	{
		if( 0 > ioctl(fd,I2C_SLAVE,cbAddr) )
		{
        	monitor_power_log( "acquire bus access err %s", strerror(errno) );
    	}
		else
		{
    		char buf[3];
			buf[0] = cbReg;
			buf[1] = 0xFF & (wData >> 8);
			buf[2] = 0xFF & (wData >> 0);
			if( 0 > write(fd,buf,sizeof(buf)) )
			{
		        monitor_power_log( "write reg addr err %x %s", cbAddr, strerror(errno) );
			}
		}
		close( fd );
	}
}

static void monitor_power_code_iic_read_reg( uint8_t cbAddr, uint8_t cbReg, uint16_t* pwData )
{
	int fd = open( "/dev/i2c-3", O_RDWR );
    if( 0 > fd )
	{
    	monitor_power_log( "iic open err %s", strerror(errno) );
    }
	else
	{
		if( 0 > ioctl(fd,I2C_SLAVE,cbAddr) )
		{
        	monitor_power_log( "acquire bus access err %s", strerror(errno) );
    	}
		else
		{
			int reg = cbReg;
    		char buf[2] = { 0x00, 0x00 };
			if( 0 > write(fd,&reg,1) )
			{
		        monitor_power_log( "write reg addr err %x %s", cbAddr, strerror(errno) );
		    }

		    if( 0 > read(fd,buf,2) )
			{
		        monitor_power_log( "read device %x err", cbAddr, strerror(errno) );
		    }
			else
			{
				pwData[0] = (int16_t)((buf[0]<<8) | buf[1]);
			}
		}
		close( fd );
	}
}

typedef struct monitor_power_code_sgm832_cfg
{
	uint8_t		cbAddr;
	uint16_t	wCalibration;
	float		fCurrent_LSB;
	float		fPower_LSB;
}monitor_power_code_sgm832_cfg_t;

const struct monitor_power_code_sgm832_cfg sSgm832_VCC_3V3_MCU	= { 0x40,	1280,	0.00004,	0.001 };
const struct monitor_power_code_sgm832_cfg sSgm832_TX_2V1A		= { 0x41,	6400,	0.0008,		0.02 };
const struct monitor_power_code_sgm832_cfg sSgm832_TX_2V1B 		= { 0x42,	6400,	0.0008,		0.02 };
const struct monitor_power_code_sgm832_cfg sSgm832_TX_1V9A		= { 0x43,	6400,	0.0008,		0.02 };
const struct monitor_power_code_sgm832_cfg sSgm832_TX_1V9B		= { 0x44,	6400, 	0.0008,		0.02 };
const struct monitor_power_code_sgm832_cfg sSgm832_RX_1V9		= { 0x45,	12800,	0.0004, 	0.01 };
const struct monitor_power_code_sgm832_cfg sSgm832_VCC_SRC_5V3	= { 0x46,	1280, 	0.00013333, 0.003333333 };
const struct monitor_power_code_sgm832_cfg sSgm832_VCC_SRC_3V3 	= { 0x47,	2560, 	0.00002, 	0.0005 };
const struct monitor_power_code_sgm832_cfg sSgm832_VCC_FPGA_V85	= { 0x48,	2560, 	0.002, 		0.05 };
const struct monitor_power_code_sgm832_cfg sSgm832_VCC_FPGA_0V9 = { 0x49,	1280, 	0.0004, 	0.01 };
const struct monitor_power_code_sgm832_cfg sSgm832_VCC_FPGA_1V8 = { 0x4A,	12800, 	0.0004, 	0.01 };
const struct monitor_power_code_sgm832_cfg sSgm832_VCC_2V5		= { 0x4B,	2560, 	0.0002, 	0.005 };
const struct monitor_power_code_sgm832_cfg sSgm832_VCC_FPGA_1V2 = { 0x4C,	1280, 	0.0004, 	0.01 };
const struct monitor_power_code_sgm832_cfg sSgm832_VCC_FPGA_3V3 = { 0x4D,	2560, 	0.0002, 	0.005 };
const struct monitor_power_code_sgm832_cfg sSgm832_VCC_2ND_1V8 	= { 0x4E,	2560, 	0.0002, 	0.005 };
const struct monitor_power_code_sgm832_cfg sSgm832_DDR_VTT_0V6 	= { 0x4F,	2560, 	0.0002, 	0.005 };

static void monitor_power_code_sgm832_config( const struct monitor_power_code_sgm832_cfg* psCfg )
{
	monitor_power_code_iic_write_reg( psCfg->cbAddr, 0x00, 0x4527 );
	monitor_power_code_iic_write_reg( psCfg->cbAddr, 0x05, psCfg->wCalibration );
}

static void monitor_power_code_sgm832_read_power( const struct monitor_power_code_sgm832_cfg* psCfg, float* pfPower )
{
	uint16_t wTemp;
	monitor_power_code_iic_read_reg( psCfg->cbAddr, 0x03, &wTemp );
	pfPower[0] = psCfg->fPower_LSB * wTemp;
}

static void monitor_power_code_sgm832_read_voltage( const struct monitor_power_code_sgm832_cfg* psCfg, float* pfVoltage )
{
	uint16_t wTemp;
	monitor_power_code_iic_read_reg( psCfg->cbAddr, 0x02, &wTemp );
	pfVoltage[0] = 0.00125 * wTemp;
}

static void monitor_power_code_sgm832_read_current( const struct monitor_power_code_sgm832_cfg* psCfg, float* pfCurrent )
{
	uint16_t wTemp;
	monitor_power_code_iic_read_reg( psCfg->cbAddr, 0x04, &wTemp );
	pfCurrent[0] = psCfg->fCurrent_LSB * wTemp;
}

static void monitor_power_code_sgm832_read( const struct monitor_power_code_sgm832_cfg* psCfg, float* pfOut )
{
	monitor_power_code_sgm832_read_power( psCfg, pfOut );
	monitor_power_code_sgm832_read_voltage( psCfg, pfOut + 1 );
	monitor_power_code_sgm832_read_current( psCfg, pfOut + 2 );
}

static void monitor_power_code_init( void )
{
	monitor_power_code_sgm832_config( &sSgm832_VCC_3V3_MCU );
	monitor_power_code_sgm832_config( &sSgm832_TX_2V1A );
	monitor_power_code_sgm832_config( &sSgm832_TX_2V1B );
	monitor_power_code_sgm832_config( &sSgm832_TX_1V9A );
	monitor_power_code_sgm832_config( &sSgm832_TX_1V9B );
	monitor_power_code_sgm832_config( &sSgm832_RX_1V9 );
	monitor_power_code_sgm832_config( &sSgm832_VCC_SRC_5V3 );
	monitor_power_code_sgm832_config( &sSgm832_VCC_SRC_3V3 );
	monitor_power_code_sgm832_config( &sSgm832_VCC_FPGA_V85 );
	monitor_power_code_sgm832_config( &sSgm832_VCC_FPGA_0V9 );
	monitor_power_code_sgm832_config( &sSgm832_VCC_FPGA_1V8 );
	monitor_power_code_sgm832_config( &sSgm832_VCC_2V5 );
	monitor_power_code_sgm832_config( &sSgm832_VCC_FPGA_1V2 );
	monitor_power_code_sgm832_config( &sSgm832_VCC_FPGA_3V3 );
	monitor_power_code_sgm832_config( &sSgm832_VCC_2ND_1V8 );
	monitor_power_code_sgm832_config( &sSgm832_DDR_VTT_0V6 );
}

static void monitor_power_code_read( void )
{
	int fd = open( "/dev/i2c-0", O_RDWR );
    if( 0 > fd )
	{
    	monitor_power_log( "iic open err %s", strerror(errno) );
    }
	else
	{
		if( 0 > ioctl(fd,I2C_SLAVE,0x49) )
		{
        	monitor_power_log( "acquire bus access err %s", strerror(errno) );
    	}
		else
		{
    		char buf[3] = { 0x01, 0x72, 0xE3 };
		    if( 0 > write(fd,buf,3) )
			{
		        monitor_power_log( "write device err", strerror(errno) );
		    }
		}

		int32_t iTemp = 0;
		for( int i = 0; 8 > i; ++i )
		{
			usleep( 1000 );
			if( 0 > ioctl(fd,I2C_SLAVE,0x49) )
			{
	        	monitor_power_log( "acquire bus access err %s", strerror(errno) );
	    	}
			else
			{
				int reg = 0x00;
	    		char buf[2] = { 0x42, 0xE3 };
				if( 0 > write(fd,&reg,1) )
				{
			        monitor_power_log( "write reg addr err %s", strerror(errno) );
			    }

			    if( 0 > read(fd,buf,2) )
				{
			        monitor_power_log( "read device err", strerror(errno) );
			    }
				else
				{
					iTemp += (int16_t)((buf[0]<<8) | buf[1]);
				}
			}
		}
		sMonitorPower.sCode.fBoard[1] = (108.2/8.2)*(4.096*(iTemp/8.0)/32767.0);
		close( fd );
	}

	monitor_power_code_sgm832_read( &sSgm832_VCC_3V3_MCU,	sMonitorPower.sCode.fVcc_3v3_mcu );
	monitor_power_code_sgm832_read( &sSgm832_TX_2V1A,		sMonitorPower.sCode.fTx_2v1_A );
	monitor_power_code_sgm832_read( &sSgm832_TX_2V1B,		sMonitorPower.sCode.fTx_2v1_B );
	monitor_power_code_sgm832_read( &sSgm832_TX_1V9A,		sMonitorPower.sCode.fTx_1v9_A );
	monitor_power_code_sgm832_read( &sSgm832_TX_1V9B,		sMonitorPower.sCode.fTx_1v9_B );
	monitor_power_code_sgm832_read( &sSgm832_RX_1V9,		sMonitorPower.sCode.fRx_1v9 );
	monitor_power_code_sgm832_read( &sSgm832_VCC_SRC_5V3,	sMonitorPower.sCode.fVcc_src_5v3 );
	monitor_power_code_sgm832_read( &sSgm832_VCC_SRC_3V3,	sMonitorPower.sCode.fVcc_src_3v3 );
	monitor_power_code_sgm832_read( &sSgm832_VCC_FPGA_V85,	sMonitorPower.sCode.fVcc_fpga_v58 );
	monitor_power_code_sgm832_read( &sSgm832_VCC_FPGA_0V9,	sMonitorPower.sCode.fVcc_fpga_0v9 );
	monitor_power_code_sgm832_read( &sSgm832_VCC_FPGA_1V8,	sMonitorPower.sCode.fVcc_fpga_1v8 );
	monitor_power_code_sgm832_read( &sSgm832_VCC_2V5,		sMonitorPower.sCode.fVcc_2v5 );
	monitor_power_code_sgm832_read( &sSgm832_VCC_FPGA_1V2,	sMonitorPower.sCode.fVcc_fpga_1v2 );
	monitor_power_code_sgm832_read( &sSgm832_VCC_FPGA_3V3,	sMonitorPower.sCode.fVcc_fpga_3v3 );
	monitor_power_code_sgm832_read( &sSgm832_VCC_2ND_1V8,	sMonitorPower.sCode.fVcc_2nd_1v8 );
	monitor_power_code_sgm832_read( &sSgm832_DDR_VTT_0V6,	sMonitorPower.sCode.fDdr_vtt_0v6 );
}

static void monitor_power_source_read( void )
{
	CHC2442_MUX_mode_setting( 2 );

	usleep( 10000 );
	
	int fd = open( "/dev/i2c-0", O_RDWR );
    if( 0 > fd )
	{
    	monitor_power_log( "iic open err %s", strerror(errno) );
    }
	else
	{
		if( 0 > ioctl(fd,I2C_SLAVE,0x49) )
		{
        	monitor_power_log( "acquire bus access err %s", strerror(errno) );
    	}
		else
		{
    		char buf[3] = { 0x01, 0x42, 0xE3 };
		    if( 0 > write(fd,buf,3) )
			{
		        monitor_power_log( "write device err", strerror(errno) );
		    }
		}

		int32_t iTemp = 0;
		for( int i = 0; 8 > i; ++i )
		{
			usleep( 1000 );
			if( 0 > ioctl(fd,I2C_SLAVE,0x49) )
			{
	        	monitor_power_log( "acquire bus access err %s", strerror(errno) );
	    	}
			else
			{
				int reg = 0x00;
	    		char buf[2] = { 0x42, 0xE3 };
				if( 0 > write(fd,&reg,1) )
				{
			        monitor_power_log( "write reg addr err %s", strerror(errno) );
			    }

			    if( 0 > read(fd,buf,2) )
				{
			        monitor_power_log( "read device err", strerror(errno) );
			    }
				else
				{
					iTemp += (int16_t)((buf[0]<<8) | buf[1]);
				}
			}
		}
		sMonitorPower.sSource.fVco = 10*log10((4.096*(iTemp/8.0)/32767.0)/0.3756)+5;
		close( fd );
	}
}

static void monitor_temp_read_handler( void* pvArg, void* pvInfo, uint32_t uSize )
{
	monitor_power_code_read();
	monitor_power_source_read();

	monitor_power_log( 
					"\r\nCODE: BOARD P=%f V=%f C=%f\nVCC_3V3_MCU P=%f V=%f C=%f\nTX_2V1_A P=%f V=%f C=%f\nTX_2V1_B P=%f V=%f C=%f\nTX_1V9_A P=%f V=%f C=%f\nTX_1V9_B P=%f V=%f C=%f\nRX_1V9 P=%f V=%f C=%f\nVCC_SRC_5V3 P=%f V=%f C=%f\nVCC_SRC_3V3 P=%f V=%f C=%f\nVCC_FPGA_V58 P=%f V=%f C=%f\nVCC_FPGA_0V9 P=%f V=%f C=%f\nVCC_FPGA_1V8 P=%f V=%f C=%f\nVCC_2V5 P=%f V=%f C=%f\nVCC_FPGA_1V2 P=%f V=%f C=%f\nVCC_FPGA_3V3 P=%f V=%f C=%f\nVCC_2ND_1V8 P=%f V=%f C=%f\nVCC_VTT_0V6 P=%f V=%f C=%f\r\nSOURCE: VCO %f\r\nTX: U8 %f %f %f %f %f %f %f %f\nU9 %f %f %f %f %f %f %f %f\nU10 %f %f %f %f %f %f %f %f\nU11 %f %f %f %f %f %f %f %f\nU12 %f %f %f %f %f %f %f %f\nU13 %f %f %f %f %f %f %f %f\nU14 %f %f %f %f %f %f %f %f\nU15 %f %f %f %f %f %f %f %f\nU16 %f %f %f %f %f %f %f %f\r\nRX: U17 %f %f %f %f %f %f %f %f\nU18 %f %f %f %f %f %f %f %f\nU19 %f %f %f %f %f %f %f %f\nU20 %f %f %f %f %f %f %f %f\nU21 %f %f %f %f %f %f %f %f\nU22 %f %f %f %f %f %f %f %f\nU23 %f %f %f %f %f %f %f %f\nU24 %f %f %f %f %f %f %f %f\r\n",
					
					sMonitorPower.sCode.fBoard[0], sMonitorPower.sCode.fBoard[1], sMonitorPower.sCode.fBoard[2],
					sMonitorPower.sCode.fVcc_3v3_mcu[0], sMonitorPower.sCode.fVcc_3v3_mcu[1], sMonitorPower.sCode.fVcc_3v3_mcu[2],
					sMonitorPower.sCode.fTx_2v1_A[0], sMonitorPower.sCode.fTx_2v1_A[1], sMonitorPower.sCode.fTx_2v1_A[2],
					sMonitorPower.sCode.fTx_2v1_B[0], sMonitorPower.sCode.fTx_2v1_B[1], sMonitorPower.sCode.fTx_2v1_B[2],
					sMonitorPower.sCode.fTx_1v9_A[0], sMonitorPower.sCode.fTx_1v9_A[1], sMonitorPower.sCode.fTx_1v9_A[2],
					sMonitorPower.sCode.fTx_1v9_B[0], sMonitorPower.sCode.fTx_1v9_B[1], sMonitorPower.sCode.fTx_1v9_B[2],
					sMonitorPower.sCode.fRx_1v9[0], sMonitorPower.sCode.fRx_1v9[1], sMonitorPower.sCode.fRx_1v9[2],
					sMonitorPower.sCode.fVcc_src_5v3[0], sMonitorPower.sCode.fVcc_src_5v3[1], sMonitorPower.sCode.fVcc_src_5v3[2],
					sMonitorPower.sCode.fVcc_src_3v3[0], sMonitorPower.sCode.fVcc_src_3v3[1], sMonitorPower.sCode.fVcc_src_3v3[2],
					sMonitorPower.sCode.fVcc_fpga_v58[0], sMonitorPower.sCode.fVcc_fpga_v58[1], sMonitorPower.sCode.fVcc_fpga_v58[2],
					sMonitorPower.sCode.fVcc_fpga_0v9[0], sMonitorPower.sCode.fVcc_fpga_0v9[1], sMonitorPower.sCode.fVcc_fpga_0v9[2],
					sMonitorPower.sCode.fVcc_fpga_1v8[0], sMonitorPower.sCode.fVcc_fpga_1v8[1], sMonitorPower.sCode.fVcc_fpga_1v8[2],
					sMonitorPower.sCode.fVcc_2v5[0], sMonitorPower.sCode.fVcc_2v5[1], sMonitorPower.sCode.fVcc_2v5[2],
					sMonitorPower.sCode.fVcc_fpga_1v2[0], sMonitorPower.sCode.fVcc_fpga_1v2[1], sMonitorPower.sCode.fVcc_fpga_1v2[2],
					sMonitorPower.sCode.fVcc_fpga_3v3[0], sMonitorPower.sCode.fVcc_fpga_3v3[1], sMonitorPower.sCode.fVcc_fpga_3v3[2],
					sMonitorPower.sCode.fVcc_2nd_1v8[0], sMonitorPower.sCode.fVcc_2nd_1v8[1], sMonitorPower.sCode.fVcc_2nd_1v8[2],
					sMonitorPower.sCode.fDdr_vtt_0v6[0], sMonitorPower.sCode.fDdr_vtt_0v6[1], sMonitorPower.sCode.fDdr_vtt_0v6[2],
	
					sMonitorPower.sSource.fVco,

					sMonitorPower.sTx.fU8[0], sMonitorPower.sTx.fU8[1], sMonitorPower.sTx.fU8[2], sMonitorPower.sTx.fU8[3], sMonitorPower.sTx.fU8[4], sMonitorPower.sTx.fU8[5], sMonitorPower.sTx.fU8[6], sMonitorPower.sTx.fU8[7],
					sMonitorPower.sTx.fU9[0], sMonitorPower.sTx.fU9[1], sMonitorPower.sTx.fU9[2], sMonitorPower.sTx.fU9[3], sMonitorPower.sTx.fU9[4], sMonitorPower.sTx.fU9[5], sMonitorPower.sTx.fU9[6], sMonitorPower.sTx.fU9[7],
					sMonitorPower.sTx.fU10[0], sMonitorPower.sTx.fU10[1], sMonitorPower.sTx.fU10[2], sMonitorPower.sTx.fU10[3], sMonitorPower.sTx.fU10[4], sMonitorPower.sTx.fU10[5], sMonitorPower.sTx.fU10[6], sMonitorPower.sTx.fU10[7],
					sMonitorPower.sTx.fU11[0], sMonitorPower.sTx.fU11[1], sMonitorPower.sTx.fU11[2], sMonitorPower.sTx.fU11[3], sMonitorPower.sTx.fU11[4], sMonitorPower.sTx.fU11[5], sMonitorPower.sTx.fU11[6], sMonitorPower.sTx.fU11[7],
					sMonitorPower.sTx.fU12[0], sMonitorPower.sTx.fU12[1], sMonitorPower.sTx.fU12[2], sMonitorPower.sTx.fU12[3], sMonitorPower.sTx.fU12[4], sMonitorPower.sTx.fU12[5], sMonitorPower.sTx.fU12[6], sMonitorPower.sTx.fU12[7],
					sMonitorPower.sTx.fU13[0], sMonitorPower.sTx.fU13[1], sMonitorPower.sTx.fU13[2], sMonitorPower.sTx.fU13[3], sMonitorPower.sTx.fU13[4], sMonitorPower.sTx.fU13[5], sMonitorPower.sTx.fU13[6], sMonitorPower.sTx.fU13[7],
					sMonitorPower.sTx.fU14[0], sMonitorPower.sTx.fU14[1], sMonitorPower.sTx.fU14[2], sMonitorPower.sTx.fU14[3], sMonitorPower.sTx.fU14[4], sMonitorPower.sTx.fU14[5], sMonitorPower.sTx.fU14[6], sMonitorPower.sTx.fU14[7],
					sMonitorPower.sTx.fU15[0], sMonitorPower.sTx.fU15[1], sMonitorPower.sTx.fU15[2], sMonitorPower.sTx.fU15[3], sMonitorPower.sTx.fU15[4], sMonitorPower.sTx.fU15[5], sMonitorPower.sTx.fU15[6], sMonitorPower.sTx.fU15[7],
					sMonitorPower.sTx.fU16[0], sMonitorPower.sTx.fU16[1], sMonitorPower.sTx.fU16[2], sMonitorPower.sTx.fU16[3], sMonitorPower.sTx.fU16[4], sMonitorPower.sTx.fU16[5], sMonitorPower.sTx.fU16[6], sMonitorPower.sTx.fU16[7],
					
					sMonitorPower.sRx.fU17[0], sMonitorPower.sRx.fU17[1], sMonitorPower.sRx.fU17[2], sMonitorPower.sRx.fU17[3], sMonitorPower.sRx.fU17[4], sMonitorPower.sRx.fU17[5], sMonitorPower.sRx.fU17[6], sMonitorPower.sRx.fU17[7],
					sMonitorPower.sRx.fU18[0], sMonitorPower.sRx.fU18[1], sMonitorPower.sRx.fU18[2], sMonitorPower.sRx.fU18[3], sMonitorPower.sRx.fU18[4], sMonitorPower.sRx.fU18[5], sMonitorPower.sRx.fU18[6], sMonitorPower.sRx.fU18[7],
					sMonitorPower.sRx.fU19[0], sMonitorPower.sRx.fU19[1], sMonitorPower.sRx.fU19[2], sMonitorPower.sRx.fU19[3], sMonitorPower.sRx.fU19[4], sMonitorPower.sRx.fU19[5], sMonitorPower.sRx.fU19[6], sMonitorPower.sRx.fU19[7],
					sMonitorPower.sRx.fU20[0], sMonitorPower.sRx.fU20[1], sMonitorPower.sRx.fU20[2], sMonitorPower.sRx.fU20[3], sMonitorPower.sRx.fU20[4], sMonitorPower.sRx.fU20[5], sMonitorPower.sRx.fU20[6], sMonitorPower.sRx.fU20[7],
					sMonitorPower.sRx.fU21[0], sMonitorPower.sRx.fU21[1], sMonitorPower.sRx.fU21[2], sMonitorPower.sRx.fU21[3], sMonitorPower.sRx.fU21[4], sMonitorPower.sRx.fU21[5], sMonitorPower.sRx.fU21[6], sMonitorPower.sRx.fU21[7],
					sMonitorPower.sRx.fU22[0], sMonitorPower.sRx.fU22[1], sMonitorPower.sRx.fU22[2], sMonitorPower.sRx.fU22[3], sMonitorPower.sRx.fU22[4], sMonitorPower.sRx.fU22[5], sMonitorPower.sRx.fU22[6], sMonitorPower.sRx.fU22[7],
					sMonitorPower.sRx.fU23[0], sMonitorPower.sRx.fU23[1], sMonitorPower.sRx.fU23[2], sMonitorPower.sRx.fU23[3], sMonitorPower.sRx.fU23[4], sMonitorPower.sRx.fU23[5], sMonitorPower.sRx.fU23[6], sMonitorPower.sRx.fU23[7],
					sMonitorPower.sRx.fU24[0], sMonitorPower.sRx.fU24[1], sMonitorPower.sRx.fU24[2], sMonitorPower.sRx.fU24[3], sMonitorPower.sRx.fU24[4], sMonitorPower.sRx.fU24[5], sMonitorPower.sRx.fU24[6], sMonitorPower.sRx.fU24[7]
					);
}

static void monitor_power_read( void* pvArg )
{
	struct monitor_power* psModule = (struct monitor_power*)(&sMonitorPower);

	void* pvInfo = sysport_task_malloc( psModule->pvModuleTask, 0 );
	if( pvInfo )
	{
		sysport_task_func( psModule->pvModuleTask, monitor_temp_read_handler, pvInfo );
	}
}

int monitor_power_init( void* pvModuleTask )
{
	int eRet = 0;

	monitor_power_code_init();

	sMonitorPower.pvModuleTask = pvModuleTask;
	sMonitorPower.pvModuleTimer = sysport_timer_create( "/monitor_power_timer", MONITOR_POWER_FREQ_TIME, 0, &sMonitorPower, monitor_power_read );
		
	return eRet;
}

static float pdet_adc_cal_out_power(float x1)
{
	float result = 0;
#define P1 (35.7)
#define P2 (958.6)
#define P3 (-2280)
#define Q1 (109.7)
#define Q2 (219.2)
	
	result =  (P1*x1*x1 + P2*x1 + P3)/(x1*x1 + Q1*x1 + Q2);
	printf("x1 = %f, result = %f\n", x1, result);	

	return result;
}

static void monitor_power_txrx_updata_handler( void* pvArg, void* pvInfo, uint32_t uSize )
{
	float *piPower = (float*)pvInfo;

	for( int i = 0; 8 > i; ++i )
		sMonitorPower.sTx.fU9[i]	= pdet_adc_cal_out_power(piPower[0*8+i]);
	for( int i = 0; 8 > i; ++i )
		sMonitorPower.sTx.fU10[i]	= pdet_adc_cal_out_power(piPower[1*8+i]);
	for( int i = 0; 8 > i; ++i )
		sMonitorPower.sTx.fU11[i]	= pdet_adc_cal_out_power(piPower[2*8+i]);
	for( int i = 0; 8 > i; ++i )
		sMonitorPower.sTx.fU12[i]	= pdet_adc_cal_out_power(piPower[3*8+i]);
	for( int i = 0; 8 > i; ++i )
		sMonitorPower.sTx.fU13[i]	= pdet_adc_cal_out_power(piPower[4*8+i]);
	for( int i = 0; 8 > i; ++i )
		sMonitorPower.sTx.fU14[i]	= pdet_adc_cal_out_power(piPower[5*8+i]);
	for( int i = 0; 8 > i; ++i )
		sMonitorPower.sTx.fU15[i]	= pdet_adc_cal_out_power(piPower[6*8+i]);
	for( int i = 0; 8 > i; ++i )
		sMonitorPower.sTx.fU16[i]	= pdet_adc_cal_out_power(piPower[7*8+i]);

	for( int i = 0; 8 > i; ++i )
		sMonitorPower.sRx.fU17[i]	= pdet_adc_cal_out_power(piPower[8*8+i]);
	for( int i = 0; 8 > i; ++i )
		sMonitorPower.sRx.fU18[i]	= pdet_adc_cal_out_power(piPower[19*8+i]);
	for( int i = 0; 8 > i; ++i )
		sMonitorPower.sRx.fU19[i]	= pdet_adc_cal_out_power(piPower[10*8+i]);
	for( int i = 0; 8 > i; ++i )
		sMonitorPower.sRx.fU20[i]	= pdet_adc_cal_out_power(piPower[11*8+i]);
	for( int i = 0; 8 > i; ++i )
		sMonitorPower.sRx.fU21[i]	= pdet_adc_cal_out_power(piPower[12*8+i]);
	for( int i = 0; 8 > i; ++i )
		sMonitorPower.sRx.fU22[i]	= pdet_adc_cal_out_power(piPower[13*8+i]);
	for( int i = 0; 8 > i; ++i )
		sMonitorPower.sRx.fU23[i]	= pdet_adc_cal_out_power(piPower[14*8+i]);
	for( int i = 0; 8 > i; ++i )
		sMonitorPower.sRx.fU24[i]	= pdet_adc_cal_out_power(piPower[15*8+i]);
	for( int i = 0; 8 > i; ++i )
		sMonitorPower.sTx.fU8[i]	= pdet_adc_cal_out_power(piPower[16*8+i]);
}

void monitor_power_updata_txrx( void* pvData )
{
	struct monitor_power* psModule = (struct monitor_power*)(&sMonitorPower);

	void* pvInfo = sysport_task_malloc( psModule->pvModuleTask, 17*8*sizeof(float) );
	memcpy( pvInfo, pvData, 17*8*sizeof(float) );
	if( pvInfo )
	{
		sysport_task_func( psModule->pvModuleTask, monitor_power_txrx_updata_handler, pvInfo );
	}
}

int get_power_vco(float *vco)
{
	*vco = sMonitorPower.sSource.fVco;

	return 0;
}

int get_power_code(char *rbuf, int rbuf_len)
{
	snprintf(rbuf, rbuf_len,\ 
		"fBoard:%f,%f,%f\r\n"
		"fPs:%f,%f,%f\r\n"
		"fPs:%f,%f,%f\r\n"
		"fVcc_3v3_mcu:%f,%f,%f,%f\r\n"
		"fTx_2v1_A:%f,%f,%f,%f\r\n"
		"fTx_2v1_B:%f,%f,%f,%f\r\n"
		"fTx_1v9_A:%f,%f,%f,%f\r\n"
		"fTx_1v9_B:%f,%f,%f,%f\r\n"
		"fRx_1v9:%f,%f,%f,%f\r\n"
		"fVcc_src_5v3:%f,%f,%f,%f\r\n"
		"fvcc_src_3v3:%f,%f,%f,%f\r\n"
		"fVcc_fpga_v58:%f,%f,%f,%f\r\n"
		"fVcc_fpga_0v9:%f,%f,%f,%f\r\n"
		"fVcc_fpga_1v8:%f,%f,%f,%f\r\n"
		"fVcc_2v5:%f,%f,%f,%f\r\n"
		"fVcc_fpga_1v2:%f,%f,%f,%f\r\n"
		"fVcc_fpga_3v3:%f,%f,%f,%f\r\n"
		"fVcc_2nd_1v8:%f,%f,%f,%f\r\n"
		"fDdr_vtt_0v6:%f,%f,%f,%f\r\n",\
		sMonitorPower.sCode.fBoard[0], sMonitorPower.sCode.fBoard[1] , sMonitorPower.sCode.fBoard[2],\
		sMonitorPower.sCode.fPs[0], sMonitorPower.sCode.fPs[1], sMonitorPower.sCode.fPs[2],\
		sMonitorPower.sCode.fPl[0], sMonitorPower.sCode.fPs[1], sMonitorPower.sCode.fPs[2],\
		sMonitorPower.sCode.fVcc_3v3_mcu[0], sMonitorPower.sCode.fVcc_3v3_mcu[1], sMonitorPower.sCode.fVcc_3v3_mcu[2], sMonitorPower.sCode.fVcc_3v3_mcu[3],\
		sMonitorPower.sCode.fTx_2v1_A[0], sMonitorPower.sCode.fTx_2v1_A[1], sMonitorPower.sCode.fTx_2v1_A[2], sMonitorPower.sCode.fTx_2v1_A[3],\
		sMonitorPower.sCode.fTx_2v1_B[0], sMonitorPower.sCode.fTx_2v1_B[1], sMonitorPower.sCode.fTx_2v1_B[2], sMonitorPower.sCode.fTx_2v1_B[3],\
		sMonitorPower.sCode.fTx_1v9_A[0], sMonitorPower.sCode.fTx_1v9_A[1], sMonitorPower.sCode.fTx_1v9_A[2], sMonitorPower.sCode.fTx_1v9_A[3],\
		sMonitorPower.sCode.fTx_1v9_B[0], sMonitorPower.sCode.fTx_1v9_B[1], sMonitorPower.sCode.fTx_1v9_B[2], sMonitorPower.sCode.fTx_1v9_B[3],\
		sMonitorPower.sCode.fRx_1v9[0], sMonitorPower.sCode.fRx_1v9[1], sMonitorPower.sCode.fRx_1v9[2], sMonitorPower.sCode.fRx_1v9[3],\
		sMonitorPower.sCode.fVcc_src_5v3[0], sMonitorPower.sCode.fVcc_src_5v3[1], sMonitorPower.sCode.fVcc_src_5v3[2], sMonitorPower.sCode.fVcc_src_5v3[3],\
		sMonitorPower.sCode.fVcc_src_3v3[0], sMonitorPower.sCode.fVcc_src_3v3[1], sMonitorPower.sCode.fVcc_src_3v3[2], sMonitorPower.sCode.fVcc_src_3v3[3],\
		sMonitorPower.sCode.fVcc_fpga_v58[0], sMonitorPower.sCode.fVcc_fpga_v58[1], sMonitorPower.sCode.fVcc_fpga_v58[2], sMonitorPower.sCode.fVcc_fpga_v58[3],\
		sMonitorPower.sCode.fVcc_fpga_0v9[0], sMonitorPower.sCode.fVcc_fpga_0v9[1], sMonitorPower.sCode.fVcc_fpga_0v9[2], sMonitorPower.sCode.fVcc_fpga_0v9[3],\
		sMonitorPower.sCode.fVcc_fpga_1v8[0], sMonitorPower.sCode.fVcc_fpga_1v8[1], sMonitorPower.sCode.fVcc_fpga_1v8[2], sMonitorPower.sCode.fVcc_fpga_1v8[3],\
		sMonitorPower.sCode.fVcc_2v5[0], sMonitorPower.sCode.fVcc_2v5[1], sMonitorPower.sCode.fVcc_2v5[2], sMonitorPower.sCode.fVcc_2v5[3],\
		sMonitorPower.sCode.fVcc_fpga_1v2[0], sMonitorPower.sCode.fVcc_fpga_1v2[1], sMonitorPower.sCode.fVcc_fpga_1v2[2], sMonitorPower.sCode.fVcc_fpga_1v2[3],\
		sMonitorPower.sCode.fVcc_fpga_3v3[0], sMonitorPower.sCode.fVcc_fpga_3v3[1], sMonitorPower.sCode.fVcc_fpga_3v3[2], sMonitorPower.sCode.fVcc_fpga_3v3[3],\
		sMonitorPower.sCode.fVcc_2nd_1v8[0], sMonitorPower.sCode.fVcc_2nd_1v8[1], sMonitorPower.sCode.fVcc_2nd_1v8[2], sMonitorPower.sCode.fVcc_2nd_1v8[3],\
		sMonitorPower.sCode.fDdr_vtt_0v6[0], sMonitorPower.sCode.fDdr_vtt_0v6[1], sMonitorPower.sCode.fDdr_vtt_0v6[2], sMonitorPower.sCode.fDdr_vtt_0v6[3]\
		);

	return 0;
}

int get_power_tx_fu(int uindex, char *strfu, int strlen)
{
	int ret = -1;
	float *fu = NULL;

	if(uindex < 8 || uindex > 16) {
		return ret;	
	}

	switch(uindex) {
		case 8:
			fu = sMonitorPower.sTx.fU8;
		break;
		case 9:
			fu = sMonitorPower.sTx.fU9;
		break;
		case 10:
			fu = sMonitorPower.sTx.fU10;
		break;
		case 11:
			fu = sMonitorPower.sTx.fU11;
		break;
		case 12:
			fu = sMonitorPower.sTx.fU12;
		break;
		case 13:
			fu = sMonitorPower.sTx.fU13;
		break;
		case 14:
			fu = sMonitorPower.sTx.fU14;
		break;
		case 15:
			fu = sMonitorPower.sTx.fU15;
		break;
		case 16:
			fu = sMonitorPower.sTx.fU16;
		break;
		default:
			return -1;
	}

	snprintf(strfu, strlen, "%f,%f,%f,%f,%f,%f,%f,%f\r\n",\
			fu[0], fu[1], fu[2], fu[3], fu[4], fu[5], fu[6], fu[7]);

	return 0;
}

