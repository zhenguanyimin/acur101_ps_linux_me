#ifndef _GPS_DEV_H_
#define _GPS_DEV_H_

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "../../inc/radar_error.h"

#ifdef  __cplusplus
extern "C" {
#endif


/**********************************************************************
* @Macro defines for module parameter config
**********************************************************************/

#define MAX_MESSAGE_OFF_PACKAGE		8

#define RMC_DEFINE "RMC"

#define VTG_DEFINE "VTG"

#define GGA_DEFINE "GGA"

#define GSA_DEFINE "GSA"

#define GSV_DEFINE "GSV"

#define GLL_DEFINE "GLL"

/**********************************************************************
*uart trans package struct
**********************************************************************/


typedef struct{
	uint32_t year;
	uint32_t month;
	uint32_t day;
	uint32_t hour;
	uint32_t minute;
	uint32_t second;
	uint32_t millisecond;
}DATE_TIME;

typedef struct{
	DATE_TIME D;
	double latitude; //γ��
	double longitude; //����
	int latitude_Degree; //��
	int latitude_Cent; //��
	int latitude_Second; //��
	int longitude_Degree; //��
	int longitude_Cent; //��
	int longitude_Second; //��
	float speed; //Speed over ground (Knots)
	float direction; //Course over ground
	char NS;
	char EW;
	char Post_sys_status;//Positioning system status.
	char Mode; //Mode indicator.
	char navgtn_status; //Navigational status.
	char reserve[3];
}RMC_INFO;


typedef struct{
	float direction; //Course over ground, in true north course direction.
	float knots_speed; //knots
	float kilometers_speed; //kilometers per hour
	char Mode; //Mode indicator.
	char reserve[3];
}VTG_INFO;


typedef struct{
	DATE_TIME D;
	float latitude; //γ��
	float longitude; //����
	int latitude_Degree; //��
	int latitude_Cent; //��
	int latitude_Second; //��
	int longitude_Degree; //��
	int longitude_Cent; //��
	int longitude_Second; //��
	float Hor_pre;//Horizontal dilution of precision.
	float Altitude;//Altitude above mean-sea-level
	float Geoid_Altitude;//the mean-sea-level (geoid)	surface defined by the reference datum used in the position	solution).
	char quality;//GPS quality indicator.
	char num_of_sate;//Number of satellites in use.
	char NS;
	char EW;
	char reserve[2];
}GGA_INFO;

typedef struct{
	RMC_INFO RMC ;
	VTG_INFO VTG ;
	GGA_INFO GGA ;
}GPS_INFO;

/***********************************************************************
* Function declaration
**********************************************************************/
extern void GPS_enable_full_on_mode();
extern void GPS_enable_standby_mode();
extern void GPS_enable_backup_mode();
extern void GPS_reset();
extern void gps_analysis();
ret_code_t getGpsInfo(GPS_INFO *gpsInfo);

#ifdef __cplusplus
    }
#endif
#endif /* _GPS_DEV_H_ */

