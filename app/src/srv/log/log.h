
#ifndef LOG_H
#define LOG_H

#include <stdint.h>

typedef enum log_level {
    LL_ANY = 0,
    LL_VERB = 1,
    LL_INFO = 2,
    LL_DEBUG = 3,
    LL_ERROR = 4,
    LL_PRINTF = 5,
    LL_NONE
} log_level_t;

void log_set_lvl(log_level_t lvl);
log_level_t log_get_lvl_num(void);
const char * log_get_lvl_str(void);
void log_record(log_level_t lvl, char *fmt, ...);
void log_dump(uint32_t base_addr, uint8_t *buf, uint32_t len);
void log_for_rtos(char *log);


#define CONFIG_APP_LOG_OUTPUT

#define APP_LOG_PRINTF(format, args...) log_record(LL_PRINTF, format, ##args)
#ifdef MODULE_NAME
#define APP_LOG_ERROR(format, args...) log_record(LL_ERROR, "%s: ""[LOG-E] "format, MODULE_NAME, ##args)
#define APP_LOG_DEBUG(format, args...) log_record(LL_DEBUG, "%s: ""[LOG-D] "format, MODULE_NAME, ##args)
#define APP_LOG_INFO(format, args...) log_record(LL_INFO, "%s: ""[LOG-I] "format, MODULE_NAME, ##args)
#define APP_LOG_VERBOSE(format, args...) log_record(LL_VERB, "%s: ""[LOG-V] "format, MODULE_NAME, ##args)
#else
	#ifdef	CONFIG_APP_LOG_OUTPUT
		#define APP_LOG_ERROR(format, args...) log_record(LL_ERROR, "[LOG-E] "format, ##args)
		#define APP_LOG_DEBUG(format, args...) log_record(LL_DEBUG, "[LOG-D] "format, ##args)
		#define APP_LOG_INFO(format, args...) log_record(LL_INFO, "[LOG-I] "format, ##args)
		#define APP_LOG_VERBOSE(format, args...) log_record(LL_VERB, "[LOG-V] "format, ##args)
	#else
		#define APP_LOG_ERROR(format, args...)		((void)0)
		#define APP_LOG_DEBUG(format, args...)		((void)0)
		#define APP_LOG_INFO(format, args...)		((void)0)
		#define APP_LOG_VERBOSE(format, args...)	((void)0)
	#endif
#endif

#define APP_LOG_TRACE_POINT APP_LOG_DEBUG("[%s:%d]\r\n", __FUNCTION__, __LINE__)

void log_init( void );

#endif /* LOG_H */
