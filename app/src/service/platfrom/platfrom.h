
#ifndef __PLATFROM_H__
#define __PLATFROM_H__

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

void platfrom_init( void );

/* gps */
typedef struct platfrom_gps_fix
{
	double	longitude;	//经度（单位：度）
	double	latitude;	//纬度（单位：度）
	double	altitude;	//海拔高度（单位：米）
	double	speed;		//速度（单位：米/秒）
	double	track;		//航向角（单位：度）
	double	climb;		//爬升率（单位：米/分钟）
	double	time;		//GPS时间戳，表示定位数据的时间信息
} platfrom_gps_fix_t;

void platfrom_gps_get_fix( struct platfrom_gps_fix* psFix );

/* attitude */
typedef struct platfrom_attitude_angle
{
	int32_t yawing;		//偏航角
	int32_t pitching;	//俯仰角
	int32_t rolling;	//翻滚角
} platfrom_attitude_angle_t;

void platfrom_attitude_get_angle( struct platfrom_attitude_angle* psAngle );

#endif /* __PLATFROM_H__ */

