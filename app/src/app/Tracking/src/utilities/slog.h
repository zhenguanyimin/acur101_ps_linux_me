/*
 * The MIT License (MIT)
 *  
 *  Copyleft (C) 2015 - 2018  Sun Dro (a.k.a. kala13x)
 *  Copyleft (C) 2017  George G. Gkasdrogkas (a.k.a. GeorgeGkas)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE
 */

#ifndef __SLOG_H__
#define __SLOG_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <pthread.h>
//#include <thread>
#include <stdarg.h>
#include <limits.h>
#include <errno.h>
#include <time.h>


#include "common_struct.h"

/* For include header in CPP code */
//#ifdef __cplusplus
//extern "C" {
#include <mutex>
//#elif
//#include <pthread.h>
//#endif
#ifdef WIN32
#include <windows.h>
#endif

using namespace std;

/* Definations for version info */
#define SLOGVERSION_MAJOR  1
#define SLOGVERSION_MINOR  6
#define SLOGBUILD_NUM      2

/* Loging flags */
#define SLOG_FLAG_NONE   0
#define SLOG_FLAG_LIVE   1
#define SLOG_FLAG_INFO   2
#define SLOG_FLAG_WARN   3
#define SLOG_FLAG_DEBUG  4
#define SLOG_FLAG_ERROR  5
#define SLOG_FLAG_FATAL  6
#define SLOG_FLAG_PANIC  7

/* Supported colors */
#define CLR_NORMAL   "\x1B[0m"
#define CLR_RED      "\x1B[31m"
#define CLR_GREEN    "\x1B[32m"
#define CLR_YELLOW   "\x1B[33m"
#define CLR_BLUE     "\x1B[34m"
#define CLR_NAGENTA  "\x1B[35m"
#define CLR_CYAN     "\x1B[36m"
#define CLR_WHITE    "\x1B[37m"
#define CLR_RESET    "\033[0m"

#define LVL1(x) #x
#define LVL2(x) LVL1(x)
#define SOURCE_THROW_LOCATION "<" __FILE__ ":" LVL2(__LINE__) "> -- "

#ifdef PATH_MAX
#undef PATH_MAX
#endif
#define PATH_MAX 255

/* 
 * Define macros to allow us get further informations 
 * on the corresponding erros. These macros used as wrappers 
 * for slog() function.
 */
#define SLOG_NONE(...) \
    slog(SLOG_FLAG_NONE, __VA_ARGS__);

#define SLOG_LIVE(...) \
    slog(SLOG_FLAG_LIVE, __VA_ARGS__);

#define SLOG_INFO(...) \
    slog(SLOG_FLAG_INFO, __VA_ARGS__);

#define SLOG_WARN(...) \
    slog(SLOG_FLAG_WARN, __VA_ARGS__);

#define SLOG_DEBUG(...) \
    slog(SLOG_FLAG_DEBUG, __VA_ARGS__);
    //slog(LEVEL, SLOG_FLAG_DEBUG, SOURCE_THROW_LOCATION __VA_ARGS__);

#define SLOG_ERROR(...) \
    slog(SLOG_FLAG_ERROR, __VA_ARGS__);
    //slog(LEVEL, SLOG_FLAG_ERROR, SOURCE_THROW_LOCATION __VA_ARGS__);

#define SLOG_FATAL(...) \
    slog(SLOG_FLAG_FATAL, __VA_ARGS__);
    //slog(LEVEL, SLOG_FLAG_FATAL, SOURCE_THROW_LOCATION __VA_ARGS__);

#define SLOG_PANIC(...) \
    slog(SLOG_FLAG_PANIC, __VA_ARGS__);
    //slog(LEVEL, SLOG_FLAG_PANIC, SOURCE_THROW_LOCATION __VA_ARGS__);

/* Flags */
typedef struct {
    char sFileName[PATH_MAX];
    unsigned short nFileLevel;
    unsigned short nLogLevel;
    unsigned short nFileStamp:1;
    unsigned short nToFile:1;
    unsigned short nPretty:1;
    unsigned short nTdSafe:1;
    unsigned short nErrLog:1;
    unsigned short nSilent:1;
    unsigned short nSync:1;
    //pthread_mutex_t slogLock;
	//std::mutex slogMutex;
} SlogConfig;

/* Date variables */
typedef struct {
    int year; 
    int mon; 
    int day;
    int hour;
    int min;
    int sec;
    int usec;
} SlogDate;

typedef struct {
    int nType;
    const char* pDesc;
    const char* pColor;
} SlogTag;


const char* slog_version(int nMin);

void slog_config_get(SlogConfig *pCfg);
void slog_config_set(SlogConfig *pCfg);

void slog_init(const char* pName, const char* pConf, int nLogLevel, int nTdSafe);
void slog(int flag, const char *pMsg, ...);
void slog_free();

/* For include header in CPP code */
//#ifdef __cplusplus
//}
//#endif

#endif /* __SLOG_H__ */
