#include <assert.h>
#include "gps_dev.h"

static uint32_t g_XTime_PPS = 0x00;
static char g_gps_org_data[1024];

/******** GPS information, global variable *********/
GPS_INFO g_GPS;

/*******************************************************************************
 * Local Functions - Implementation
 ******************************************************************************/
static void gps_time_calibration()
{

//	XTime tCur = 0x00;
	uint32_t tCur_dif = 0x00, l_day = 0x00;

	if (g_XTime_PPS == 0x00)
	{
		return;
	}
//	XTime_GetTime(&tCur);
//	tCur_dif = (uint32_t)((tCur - g_XTime_PPS) * 1000 / (COUNTS_PER_SECOND));

	if (tCur_dif >= 1000)
	{
		return;
	}

/******************************************************************/
	g_GPS.RMC.D.millisecond = g_GPS.RMC.D.millisecond + tCur_dif;

	if (g_GPS.RMC.D.millisecond < 1000)
	{
		return;
	}
	g_GPS.RMC.D.millisecond = g_GPS.RMC.D.millisecond % 1000;

/******************************************************************/
	g_GPS.RMC.D.second++;
	if (g_GPS.RMC.D.second < 60)
	{
		return;
	}
	g_GPS.RMC.D.second = g_GPS.RMC.D.second % 60;

/******************************************************************/
	g_GPS.RMC.D.minute++;
	if (g_GPS.RMC.D.minute < 60)
	{
		return;
	}
	g_GPS.RMC.D.minute = g_GPS.RMC.D.minute % 60;

/******************************************************************/
	g_GPS.RMC.D.hour++;
	if (g_GPS.RMC.D.hour < 24)
	{
		return;
	}
	g_GPS.RMC.D.hour = g_GPS.RMC.D.hour % 24;
	g_GPS.RMC.D.day++;

/******************************************************************/
	if ((g_GPS.RMC.D.month == 1)
		|| (g_GPS.RMC.D.month == 3)
		|| (g_GPS.RMC.D.month == 5)
		|| (g_GPS.RMC.D.month == 7)
		|| (g_GPS.RMC.D.month == 8)
		|| (g_GPS.RMC.D.month == 10)
		|| (g_GPS.RMC.D.month == 12))
	{
		l_day = 31;
	}
	else if (g_GPS.RMC.D.month == 2)
	{
		if (((g_GPS.RMC.D.year + 2000) % 400) == 0x00)
		{
			l_day = 29;
		}
		else
		{
			if ((((g_GPS.RMC.D.year + 2000) % 4) == 0x00) && ((g_GPS.RMC.D.year + 2000) % 100) != 0x00)
			{
				l_day = 29;
			}
			else
			{
				l_day = 28;
			}
		}
	}
	else
	{
		l_day = 30;
	}

	if (g_GPS.RMC.D.day <= l_day)
	{
		return;
	}
	g_GPS.RMC.D.day = g_GPS.RMC.D.day - l_day;
	g_GPS.RMC.D.month++;

/******************************************************************/
	if (g_GPS.RMC.D.month <= 12)
	{
		return;
	}
	g_GPS.RMC.D.month = g_GPS.RMC.D.month - 12;
	g_GPS.RMC.D.year++;

}

/***********************************************************************
 * @brief        uart inner transmit receive.
 *
 * @param[in]    recvp : UART_RECV_ST struct type pointer
 *
 * @return       None
 *
 * @note         None
 *
 *********************************************************************/
 /*
static void uart_GPS_isr(UART_TYPEDEF *uart_devp)
{	
	char rec_data;
	XTime tCur;
	uint32_t time_tmp;
	static uint32_t s_sys_time = 0x00, cnt = 0x00;

	while (FALSE == uart_rx_fifo_empty(uart_devp))						//锟斤拷锟斤拷锟斤拷锟紽IFO锟角匡拷
	{
		uart_receive_no_block(uart_devp, &rec_data, 1);	//锟斤拷锟斤拷1Byte锟斤拷锟斤拷

		XTime_GetTime(&tCur);
		time_tmp = (tCur * 1000) / (COUNTS_PER_SECOND);

		if ((time_tmp - s_sys_time) > 200)
		{
			cnt = 0x00;
		}
		if ((cnt == 0x00) && (rec_data != '$'))
		{
			return;
		}

		s_sys_time = time_tmp;
		g_gps_org_data[cnt++] = rec_data;
	}
}
*/

static uint32_t GetComma(uint32_t num, char* buf)
{
	uint32_t i = 0x00, cnt = 0x00;

	for (i = 0x00; buf[i] != '\n'; i++)
	{
		if (buf[i] == ',')
		{
			cnt++;
			if (cnt == num)
			{
				return (i + 1);
			}
		}

	}
	return 0;
}

static float StringToInt(uint32_t num, char* buf)
{
	uint32_t index;

	index = GetComma(num, buf);

	return (buf[index] != ',') ? atoi(&buf[index]) : 0x00;

}

static float StringToChar(uint32_t num, char* buf)
{
	uint32_t index;

	index = GetComma(num, buf);

	return (buf[index] != ',') ? buf[index] : 0x00;

}

static float StringToFloat(uint32_t num, char* buf)
{
	uint32_t index;

	index = GetComma(num, buf);

	return (buf[index] != ',') ? atof(&buf[index]) : 0.0;

}

static float StringToDouble(uint32_t num, char* buf)
{
	uint32_t index;

	index = GetComma(num, buf);

	return (buf[index] != ',') ? atof(&buf[index]) : 0.0;

}

static uint32_t NMEA_RMC_Analysis(char *line, RMC_INFO *GPS)
{
	float float_data;
	uint32_t uint_data;
	char *buf = NULL;
	const char *tail = "\r\n";
	const char *message_id_string = RMC_DEFINE;

	buf = strstr(line, message_id_string);

	if (buf == NULL)
	{
		return 0;
	}
	if (strstr(buf, tail) == NULL)
	{
		return 0;
	}

	GPS->Post_sys_status = StringToChar(2, buf);
	if (GPS->Post_sys_status == 'A') //Data valid
	{

		//get UTC
		float_data = StringToFloat(1, buf);

		GPS->D.hour = (uint32_t)(float_data / 10000);
		GPS->D.minute = (uint32_t)((float_data - GPS->D.hour * 10000) / 100);
		GPS->D.second = (uint32_t)(float_data - GPS->D.hour * 10000 - GPS->D.minute * 100);
		GPS->D.millisecond = (uint32_t)((float_data - GPS->D.hour * 10000 - GPS->D.minute * 100 - GPS->D.second) * 1000);

		//get DATE
		uint_data = StringToInt(9, buf);
		GPS->D.day = (uint32_t)(uint_data / 10000);
		GPS->D.month = (uint32_t)((uint_data - GPS->D.day * 10000) / 100);
		GPS->D.year = (uint32_t)(uint_data - GPS->D.day * 10000 - GPS->D.month * 100);

		GPS->NS = StringToChar(4, buf);
		GPS->EW = StringToChar(6, buf);

		GPS->latitude = StringToDouble(3, buf);
		GPS->longitude = StringToDouble(5, buf);

		GPS->latitude_Degree = (int32_t)(GPS->latitude / 100);
		GPS->latitude_Cent = (int32_t)(GPS->latitude - GPS->latitude_Degree * 100);
		GPS->latitude_Second = (int32_t)((GPS->latitude - GPS->latitude_Degree * 100 - GPS->latitude_Cent) * 60);

		GPS->longitude_Degree = (int32_t)(GPS->longitude / 100);
		GPS->longitude_Cent = (int32_t)(GPS->longitude - GPS->longitude_Degree * 100);
		GPS->longitude_Second = (int32_t)((GPS->longitude - GPS->longitude_Degree * 100 - GPS->longitude_Cent) * 60);

		GPS->speed = StringToFloat(7, buf);
		GPS->direction = StringToFloat(8, buf);

		GPS->Mode = StringToChar(12, buf);
		GPS->navgtn_status = StringToChar(13, buf);

	}
	return 1;

}

static uint32_t NMEA_VTG_Analysis(char *line, VTG_INFO *GPS)
{

	char *buf = NULL;
	const char *tail = "\r\n";
	const char *message_id_string = VTG_DEFINE;

	buf = strstr(line, message_id_string);
	if (buf == NULL)
	{
		return 0;
	}
	if (strstr(buf, tail) == NULL)
	{
		return 0;
	}

	GPS->direction = StringToFloat(1, buf);
	GPS->knots_speed = StringToFloat(5, buf);
	GPS->kilometers_speed = StringToFloat(7, buf);
	GPS->Mode = StringToChar(9, buf);

	return 1;

}

static uint32_t NMEA_GGA_Analysis(char *line, GGA_INFO *GPS)
{

	float float_data;
	char *buf = NULL;
	const char *tail = "\r\n";
	const char *message_id_string = GGA_DEFINE;

	buf = strstr(line, message_id_string);

	if (buf == NULL)
	{
		return 0;
	}
	if (strstr(buf, tail) == NULL)
	{
		return 0;
	}
	//get UTC
	float_data = StringToFloat(1, buf);

	GPS->D.hour = (uint32_t)(float_data / 10000);
	GPS->D.minute = (uint32_t)((float_data - GPS->D.hour * 10000) / 100);
	GPS->D.second = (uint32_t)(float_data - GPS->D.hour * 10000 - GPS->D.minute * 100);
	GPS->D.millisecond = (uint32_t)((float_data - GPS->D.hour * 10000 - GPS->D.minute * 100 - GPS->D.second) * 1000);

	GPS->NS = StringToChar(3, buf);
	GPS->EW = StringToChar(5, buf);

	GPS->latitude = StringToDouble(2, buf);
	GPS->longitude = StringToDouble(4, buf);

	GPS->latitude_Degree = (int32_t)(GPS->latitude / 100);
	GPS->latitude_Cent = (int32_t)(GPS->latitude - GPS->latitude_Degree * 100);
	GPS->latitude_Second = (int32_t)((GPS->latitude - GPS->latitude_Degree * 100 - GPS->latitude_Cent) * 60);

	GPS->longitude_Degree = (int32_t)(GPS->longitude / 100);
	GPS->longitude_Cent = (int32_t)(GPS->longitude - GPS->longitude_Degree * 100);
	GPS->longitude_Second = (int32_t)((GPS->longitude - GPS->longitude_Degree * 100 - GPS->longitude_Cent) * 60);

	GPS->quality = StringToChar(6, buf);
	GPS->num_of_sate = (uint8_t)StringToInt(7, buf);
	GPS->Hor_pre = StringToFloat(8, buf);
	GPS->Altitude = StringToFloat(9, buf);
	GPS->Geoid_Altitude = StringToFloat(11, buf);

	return 1;

}

/*******************************************************************************
 * API - Implementation
 ******************************************************************************/
/***********************************************************************
 * @brief        GPS_Setup_Intr_System
 *
 * @param[in]    GicInstancePtr
 *
 * @param[in]    InstancePtr
 *
 * @param[in]    IntrId
 *
 * @note         None
 *
 *********************************************************************/
 /*
void GPS_Setup_Intr_System(XScuGic *GicInstancePtr)
{
    XScuGic_SetPriorityTriggerType(GicInstancePtr, UARTLITE_IRPT_GPS, 0xA1, 0x3);

	XScuGic_Connect(GicInstancePtr, UARTLITE_IRPT_GPS,	(Xil_ExceptionHandler)uart_GPS_isr,	(void *)UARTLITE_GPS);

	uart_tx_fifo_clear(UARTLITE_GPS);
	uart_rx_fifo_clear(UARTLITE_GPS);

	XScuGic_Enable(GicInstancePtr, UARTLITE_IRPT_GPS);		// enable the interrupt for GPIO at GIC

    uart_interrupt_control(UARTLITE_GPS, ENABLE);
}
*/

/***********************************************************************
 * @brief        enable_full_on_mode
 *
 * @note         None
 *
 *********************************************************************/
void GPS_enable_full_on_mode()
{
	/*
	Gpio_WritePin(TX_GPS_RESET_PIN,	   0);
	Gpio_WritePin(TX_GPS_STANDBY_PIN,  1);
	Gpio_WritePin(TX_GPS_FORCE_ON_PIN, 1);
	*/
}
/***********************************************************************
 * @brief        enable_standby_mode
 *
 * @note         None
 *
 *********************************************************************/
void GPS_enable_standby_mode()
{
	/*
	Gpio_WritePin(TX_GPS_STANDBY_PIN, 0);
	*/
}
/***********************************************************************
 * @brief        enable_backup_mode
 *
 * @note         None
 *
 *********************************************************************/
void GPS_enable_backup_mode()
{
	/*
	Gpio_WritePin(	TX_GPS_FORCE_ON_PIN, 0);
	*/
}
/***********************************************************************
 * @brief        pgs_reset
 *
 * @note         None
 *
 *********************************************************************/
void GPS_reset()
{
//	Gpio_WritePin(TX_GPS_RESET_PIN, 1);
}


void gps_pps_isr()
{

	//XTime_GetTime(&g_XTime_PPS);

}

void gps_analysis(GPS_INFO *info_out)
{
	//9us
	char *ptr;
	uint32_t status = 0x00;

	ptr = &g_gps_org_data[0];
	if (ptr[0] == '$')
	{

		status = NMEA_VTG_Analysis(ptr, &g_GPS.VTG);
		if (status == 0x00)
		{
			return;
		}
		status = NMEA_GGA_Analysis(ptr, &g_GPS.GGA);
		if (status == 0x00)
		{
			return;
		}
		status = NMEA_RMC_Analysis(ptr, &g_GPS.RMC);
		if (status == 0x00)
		{
			return;
		}

		memset(ptr, 0x00, sizeof(g_gps_org_data));
		gps_time_calibration();

	}

	if (info_out)
	{
		memcpy(info_out, &g_GPS, sizeof(g_GPS));
	}
}


ret_code_t getGpsInfo(GPS_INFO *gpsInfo)
{
	ret_code_t ret = RET_OK;

	memcpy(gpsInfo, &g_GPS, sizeof(GPS_INFO));

	return ret;
}

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

void *gps_dev_task( void )
{
	
}

int gps_dev_init( void )
{
	
}

