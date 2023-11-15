
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <sys/syslog.h>
#include <pthread.h>

#include "log.h"

static log_level_t s_log_lvl = LL_NONE;

static const int map_to_log_type[LL_NONE + 1] = {LOG_NOTICE, LOG_NOTICE, LOG_INFO, LOG_DEBUG, LOG_ERR, LOG_NOTICE, LOG_WARNING};
static pthread_mutex_t mutex;

void log_set_lvl(log_level_t lvl)
{
    s_log_lvl = lvl;
}

log_level_t log_get_lvl_num(void)
{
    return s_log_lvl;
}

const char * log_get_lvl_str(void)
{
    const char *lvl = NULL;

    switch (s_log_lvl)
    {
    case LL_ANY:
        lvl = "ANY";
        break;
    case LL_VERB:
        lvl = "VERB";
        break;
    case LL_INFO:
        lvl = "INFO";
        break;
    case LL_DEBUG:
        lvl = "DEBUG";
        break;
    case LL_ERROR:
        lvl = "ERROR";
        break;
    case LL_PRINTF:
        lvl = "PRINTF";
        break;
    default:
        lvl = "NONE";
        break;
    }

    return lvl;
}

void log_record(log_level_t lvl, char *fmt, ...)
{
    char log_buf[128] = {0};

    if (lvl >= s_log_lvl )
    {
        return;
    }

    va_list args;
    va_start(args, fmt);
    vsnprintf(log_buf, sizeof(log_buf), fmt, args);
    va_end(args);

	pthread_mutex_lock(&mutex);
	openlog("linux-app", LOG_CONS | LOG_PID | LOG_PERROR, LOG_SYSLOG);
	syslog(map_to_log_type[s_log_lvl], "%s", log_buf);
	closelog();
	pthread_mutex_unlock(&mutex);
}

void log_dump(uint32_t base_addr, uint8_t *buf, uint32_t len)
{
    int i = 0;
	char *p;
	int size;

    if ((NULL == buf) || (0 == len))
    {
        return;
    }

	/* 12345678: xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx\r\n */
	size = 8 + 1 + 16*3 + 2 + 1;
	p = (unsigned char *)malloc(size);
	if (NULL == p)
	{
		return ;
	}

	memset(p, 0, size);

    for (i = 0; i < len; i++)
    {
        if (i % 16 == 0)
        {
            sprintf(&p[strlen(p)], "%08x:", (base_addr + i));
        }

        if ((i + 1) % 16 == 0)
        {
            sprintf(&p[strlen(p)], " %02x\r\n", buf[i]);

			pthread_mutex_lock(&mutex);
			openlog("linux-app", LOG_CONS | LOG_PID | LOG_PERROR, LOG_SYSLOG);
			syslog(LOG_WARNING, "%s", p);
			closelog();
			pthread_mutex_unlock(&mutex);

			memset(p, 0, size);
        }
        else
        {
            sprintf(&p[strlen(p)], " %02x", buf[i]);
        }
    }

	if ((i  % 16) != 0)
	{
	    sprintf(&p[strlen(p)],  "\r\n");

		pthread_mutex_lock(&mutex);
		openlog("linux-app", LOG_CONS | LOG_PID | LOG_PERROR, LOG_SYSLOG);
		syslog(LOG_WARNING, "%s", p);
		closelog();
		pthread_mutex_unlock(&mutex);
	}

	free(p);
	p = NULL;
}

void log_for_rtos(char *log)
{
	pthread_mutex_lock(&mutex);
	openlog("rtos-app", LOG_CONS | LOG_PID, LOG_SYSLOG);
	syslog(LOG_ALERT, "%s", log);
	closelog();
	pthread_mutex_unlock(&mutex);
}


void log_init( void )
{
	int ret;
	pthread_mutex_init(&mutex, NULL);

	ret = system("syslogd -L -R 192.168.235.88");
	if (0 == ret)
	{
		log_record(LL_INFO, "log start");
	}
	else
	{
		printf("start syslogd error:%d\n", ret);
	}
}

