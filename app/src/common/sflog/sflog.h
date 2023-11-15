
#ifndef __SFLOG_H__
#define __SFLOG_H__

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <syslog.h>
#include <time.h>
#include <sys/syscall.h>

/*******************************************************************************
@description:	sflog
*******************************************************************************/
#define SFLOG_LEVEL_EMERG		(LOG_EMERG)		//紧急情况
#define SFLOG_LEVEL_ALERT		(LOG_ALERT)		//高优先级故障，例如数据库崩溃
#define SFLOG_LEVEL_CRIT		(LOG_CRIT)		//严重错误，例如硬件故障
#define SFLOG_LEVEL_ERR			(LOG_ERR)		//错误
#define SFLOG_LEVEL_WARNING		(LOG_WARNING)	//警告
#define SFLOG_LEVEL_NOTICE		(LOG_NOTICE)	//需要注意的特殊情况
#define SFLOG_LEVEL_INFO		(LOG_INFO)		//一般信息
#define SFLOG_LEVEL_DEBUG		(LOG_DEBUG)		//调试信息

#define SFLOG_OUTPUT_FUNC		(0)
#define SFLOG_OUTPUT_TIME		(1)

#if (SFLOG_OUTPUT_FUNC)
	#if (0 == SFLOG_OUTPUT_TIME)
		#define sflog(eLevel, strFormat, ...) \
		do { \
	        syslog(eLevel, "[%d:%s:%d] " strFormat, (int)syscall(SYS_gettid), __FILE__, __LINE__, ##__VA_ARGS__); \
	    } while (0)
	#elif (1 == SFLOG_OUTPUT_TIME)
		#define sflog(eLevel, strFormat, ...) \
		do { \
			time_t current_time;	\
    		time(&current_time);	\
	        syslog(eLevel, "[%ld][%d:%s:%d] " strFormat, current_time, (int)syscall(SYS_gettid), __FILE__, __LINE__, ##__VA_ARGS__); \
	    } while (0)
	#else
		#define sflog(eLevel, strFormat, ...) \
		do { \
			struct timespec ts; \
			clock_gettime(CLOCK_MONOTONIC, &ts);	\
			syslog(eLevel, "[%ld:%ld][%d:%s:%d] " strFormat, ts.tv_sec, ts.tv_nsec, (int)syscall(SYS_gettid), __FILE__, __LINE__, ##__VA_ARGS__); \
		} while (0)
	#endif
#else
	#if (0 == SFLOG_OUTPUT_TIME)
		#define sflog(eLevel, strFormat, ...) \
		do { \
	        syslog(eLevel, strFormat, ##__VA_ARGS__); \
	    } while (0)
	#elif (1 == SFLOG_OUTPUT_TIME)
		#define sflog(eLevel, strFormat, ...) \
		do { \
			time_t current_time;	\
    		time(&current_time);	\
	        syslog(eLevel, "[%ld] " strFormat, current_time, ##__VA_ARGS__); \
	    } while (0)
	#else
		#define sflog(eLevel, strFormat, ...) \
		do { \
			struct timespec ts; \
			clock_gettime(CLOCK_MONOTONIC, &ts);	\
			syslog(eLevel, "[%ld:%ld] " strFormat, ts.tv_sec, ts.tv_nsec, ##__VA_ARGS__); \
		} while (0)
	#endif
#endif

#endif /* __SFLOG_H__ */

