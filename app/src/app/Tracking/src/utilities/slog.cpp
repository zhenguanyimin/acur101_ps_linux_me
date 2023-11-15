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

#include "slog.h"

/* Max size of string */
#define MAXMSG 8196
#define BIGSTR 4098

static SlogTag g_SlogTags[] =
{
    { 0, "NONE", NULL },
    { 1, "LIVE", NULL },
    { 2, "INFO", NULL },
    { 3, "WARN", NULL },
    { 4, "DEBUG", NULL },
    { 5, "ERROR", NULL},
    { 6, "FATAL", NULL },
    { 7, "PANIC", NULL }
    //{ 0, "NONE", NULL },
    //{ 1, "LIVE", CLR_NORMAL },
    //{ 2, "INFO", CLR_GREEN },
    //{ 3, "WARN", CLR_YELLOW },
    //{ 4, "DEBUG", CLR_BLUE },
    //{ 5, "ERROR", CLR_RED},
    //{ 6, "FATAL", CLR_RED },
    //{ 7, "PANIC", CLR_WHITE }
};

std::mutex slogMutex;
static FILE *slog_file;
char slogFileName[PATH_MAX];
SlogConfig g_slogCfg;

void slog_sync_lock()
{
    if (g_slogCfg.nTdSafe)
    {
		//g_slogCfg.slogMutex.lock();
		slogMutex.lock();
        //if (pthread_mutex_lock(&g_slogCfg.slogLock))
        //{
        //    printf("[ERROR] Slog can not lock mutex: %d\n", errno);
        //    exit(EXIT_FAILURE);
        //}
    }
}

void slog_sync_unlock()
{
    if (g_slogCfg.nTdSafe) 
    {
		//g_slogCfg.slogMutex.unlock();
		slogMutex.unlock();
        //if (pthread_mutex_unlock(&g_slogCfg.slogLock))
        //{
        //    printf("[ERROR] Slog can not unlock mutex: %d\n", errno);
        //    exit(EXIT_FAILURE);
        //}
    }
}

void slog_sync_init()
{
    if (g_slogCfg.nTdSafe)
    {
        /* Init mutex attribute */
        //pthread_mutexattr_t m_attr;
        //if (pthread_mutexattr_init(&m_attr) ||
        //    pthread_mutexattr_settype(&m_attr, PTHREAD_MUTEX_RECURSIVE) ||
        //    pthread_mutex_init(&g_slogCfg.slogLock, &m_attr) ||
        //    pthread_mutexattr_destroy(&m_attr))
        //{
        //    printf("[ERROR] Slog can not initialize mutex: %d\n", errno);
        //    g_slogCfg.nTdSafe = 0;
        //    g_slogCfg.nSync = 0;
        //}
        g_slogCfg.nSync = 1;
    }
	else {
		g_slogCfg.nSync = 0;
	}
}

void slog_sync_destroy()
{
    //if (pthread_mutex_destroy(&g_slogCfg.slogLock))
    //{
    //    printf("[ERROR] Can not deinitialize mutex: %d\n", errno);
    //    exit(EXIT_FAILURE);
    //}
    g_slogCfg.nSync = 0;
    g_slogCfg.nTdSafe = 0;
}

//#ifdef __cplusplus
//static inline int clock_gettime(int clock_id, struct timespec *ts)
//{
//    struct timeval tv;
//
//    if (clock_id != CLOCK_REALTIME) 
//    {
//        errno = EINVAL;
//        return -1;
//    }
//    if (gettimeofday(&tv, NULL) < 0) 
//    {
//        return -1;
//    }
//    ts->tv_sec = tv.tv_sec;
//    ts->tv_nsec = tv.tv_usec * 1000;
//    return 0;
//}
//#endif /* __cplusplus */

void slog_get_date(SlogDate *pDate)
{
    struct tm *timeinfo;
	timeinfo = (struct tm *)malloc(sizeof(struct tm));
    time_t rawtime = time(NULL);
    timeinfo = localtime_r(&rawtime, timeinfo);
    //localtime_s(timeinfo, &rawtime);

    /* Get System Date */
    pDate->year = timeinfo->tm_year+1900;
    pDate->mon = timeinfo->tm_mon+1;
    pDate->day = timeinfo->tm_mday;
    pDate->hour = timeinfo->tm_hour;
    pDate->min = timeinfo->tm_min;
    pDate->sec = timeinfo->tm_sec;

#ifdef WIN32
	SYSTEMTIME sys;
	GetLocalTime(&sys);
    ///* Get micro seconds */
    //struct timespec now;
    //clock_gettime(CLOCK_MONOTONIC, &now);
    //pDate->usec = now.tv_nsec / 10000000;
    pDate->usec = sys.wMilliseconds;
#endif
	free(timeinfo);
}

const char* slog_version(int nMin)
{
    static char sVersion[128];

    /* Version short */
    if (nMin) sprintf(sVersion, "%d.%d.%d", 
        SLOGVERSION_MAJOR, SLOGVERSION_MINOR, SLOGBUILD_NUM);

    /* Version long */
    else sprintf(sVersion, "%d.%d build %d (%s)", 
        SLOGVERSION_MAJOR, SLOGVERSION_MINOR, SLOGBUILD_NUM, __DATE__);

    return sVersion;
}

void slog_prepare_output(const char* pStr, const SlogDate *pDate, int nType, int nColor, char* pOut, int nSize)
{
    char sDate[32];
    snprintf(sDate, sizeof(sDate), "%02d.%02d.%02d-%02d:%02d:%02d.%02d", 
                    pDate->year, pDate->mon, pDate->day, pDate->hour, 
                    pDate->min, pDate->sec, pDate->usec);

    /* Walk throu */
    int i;
    for (i = 0;; i++)
    //for (int i = 0;; i++)
    {
        if ((nType == SLOG_FLAG_NONE) && !g_SlogTags[i].nType && (g_SlogTags[i].pDesc == NULL))
        {
            snprintf(pOut, nSize, "%s - %s", sDate, pStr);
            return;
        }

        if ((nType != SLOG_FLAG_NONE) && (g_SlogTags[i].nType == nType) && (g_SlogTags[i].pDesc != NULL))
        {
            if (nColor)
                snprintf(pOut, nSize, "%s - [%s%s%s] %s", sDate, g_SlogTags[i].pColor, g_SlogTags[i].pDesc, CLR_RESET, pStr);
            else
                snprintf(pOut, nSize, "%s - [%s] %s", sDate, g_SlogTags[i].pDesc, pStr);

            return;
        }
    }
}

void slog_to_file(char *pStr, const char *pFile, SlogDate *pDate)
{
	//fopen_s(&slog_file, slogFileName, "a+");
    if (slog_file == NULL) return;
    fprintf(slog_file, "%s\n", pStr);
	//fclose(slog_file);
}

int slog_parse_config(const char *pConfig)
{
    if (pConfig == NULL) return 0;

	std::fstream pFile(pConfig);
	stringstream sstr;
	string str;
	string sName;
	int value = 0;
	if (pFile.is_open()) {
		while (!pFile.eof()) {
			str.clear();
			sstr.clear();
			sstr.str("");
			getline(pFile, str, '\n');
			sstr << str;
			sstr >> sName >> value;
			if ((sName.length() > 0) && (sName[0] == '#'))
			{
				/* Skip comment */
				continue;
			}
			else if (strcmp(sName.c_str(), "LOGLEVEL") == 0)
			{
				//g_slogCfg.nLogLevel = atoi(sArg);
				g_slogCfg.nLogLevel = value;
			}
			else if (strcmp(sName.c_str(), "LOGFILELEVEL") == 0)
			{
				//g_slogCfg.nFileLevel = atoi(sArg);
				g_slogCfg.nFileLevel = value;
			}
			else if (strcmp(sName.c_str(), "LOGTOFILE") == 0)
			{
				//g_slogCfg.nToFile = atoi(sArg);
				g_slogCfg.nToFile = value;
			}
			else if (strcmp(sName.c_str(), "ERRORLOG") == 0)
			{
				//g_slogCfg.nErrLog = atoi(sArg);
				g_slogCfg.nErrLog = value;
			}
			else if (strcmp(sName.c_str(), "PRETTYLOG") == 0)
			{
				//g_slogCfg.nPretty = atoi(sArg);
				g_slogCfg.nPretty = value;
			}
			else if (strcmp(sName.c_str(), "FILESTAMP") == 0)
			{
				//g_slogCfg.nFileStamp = atoi(sArg);
				g_slogCfg.nFileStamp = value;
			}
			else if (strcmp(sName.c_str(), "SILENT") == 0)
			{
				g_slogCfg.nSilent = value;
			}
		}
	}
	pFile.close();
    return 1;
}

void slog(int nFlag, const char *pMsg, ...)
{
    slog_sync_lock();

    if (g_slogCfg.nSilent && 
        (nFlag == SLOG_FLAG_DEBUG || 
        nFlag == SLOG_FLAG_LIVE)) 
    {
        slog_sync_unlock();
        return;
    }

    char sInput[MAXMSG];
    memset(sInput, 0, sizeof(sInput));

    va_list args;
    va_start(args, pMsg);
    vsprintf(sInput, pMsg, args);
    va_end(args);

    /* Check logging levels */
	int level = g_SlogTags[nFlag].nType;
    //if(!nLevel || nLevel <= g_slogCfg.nLogLevel || nLevel <= g_slogCfg.nFileLevel)
    if(level <= g_slogCfg.nLogLevel || level <= g_slogCfg.nFileLevel)
    {
        SlogDate date;
        slog_get_date(&date);

        char sMessage[MAXMSG];
        memset(sMessage, 0, sizeof(sMessage));

        slog_prepare_output(sInput, &date, nFlag, 0, sMessage, sizeof(sMessage));
		//std::cout << "test point2, level: " << int(level) << ", nlogLevel: " << \
			g_slogCfg.nLogLevel << std::endl;
        if (level <= g_slogCfg.nLogLevel) printf("%s\n", sMessage);

        /* Save log in the file */
        if ((g_slogCfg.nToFile && level <= g_slogCfg.nFileLevel) || 
            (g_slogCfg.nErrLog && (nFlag == (SLOG_FLAG_ERROR | SLOG_FLAG_PANIC | \
				SLOG_FLAG_FATAL))))
        {
            if (g_slogCfg.nPretty)
            {
                memset(sMessage, 0, sizeof(sMessage));
                slog_prepare_output(sInput, &date, nFlag, 0, sMessage, sizeof(sMessage));
            }

            slog_to_file(sMessage, g_slogCfg.sFileName, &date);
        }
    }

    slog_sync_unlock();
}

void slog_config_get(SlogConfig *pCfg)
{
    slog_sync_lock();
    memset(pCfg->sFileName, 0, sizeof(pCfg->sFileName));
    strcpy(pCfg->sFileName, g_slogCfg.sFileName);

    pCfg->nFileStamp = g_slogCfg.nFileStamp;
    pCfg->nFileLevel = g_slogCfg.nFileLevel;
    pCfg->nLogLevel = g_slogCfg.nLogLevel;
    pCfg->nToFile = g_slogCfg.nToFile;
    pCfg->nPretty = g_slogCfg.nPretty;
    pCfg->nTdSafe = g_slogCfg.nTdSafe;
    pCfg->nErrLog = g_slogCfg.nErrLog;
    pCfg->nSilent = g_slogCfg.nSilent;
    slog_sync_unlock();
}

void slog_config_set(SlogConfig *pCfg)
{
    slog_sync_lock();
    memset(g_slogCfg.sFileName, 0, sizeof(g_slogCfg.sFileName));
    strcpy(g_slogCfg.sFileName, pCfg->sFileName);

    g_slogCfg.nFileStamp = pCfg->nFileStamp;
    g_slogCfg.nFileLevel = pCfg->nFileLevel;
    g_slogCfg.nLogLevel = pCfg->nLogLevel;
    g_slogCfg.nToFile = pCfg->nToFile;
    g_slogCfg.nPretty = pCfg->nPretty;
    g_slogCfg.nTdSafe = pCfg->nTdSafe;
    g_slogCfg.nErrLog = pCfg->nErrLog;
    g_slogCfg.nSilent = pCfg->nSilent;

    if (g_slogCfg.nTdSafe && !g_slogCfg.nSync)
    {
        //slog_sync_init();
        slog_sync_lock();
    }
    else if (!g_slogCfg.nTdSafe && g_slogCfg.nSync)
    {
        g_slogCfg.nTdSafe = 1;
        slog_sync_unlock();
        slog_sync_destroy();
    }

    slog_sync_unlock();
}

void slog_init(const char* pName, const char* pConf, int nLogLevel, int nTdSafe)
{
    /* Set up default values */
    memset(g_slogCfg.sFileName, 0, sizeof(g_slogCfg.sFileName));
    strcpy(g_slogCfg.sFileName, pName);

    g_slogCfg.nLogLevel = nLogLevel;
    g_slogCfg.nTdSafe = nTdSafe;
    g_slogCfg.nFileStamp = 1;
    g_slogCfg.nFileLevel = 0;
    g_slogCfg.nErrLog = 0;
    g_slogCfg.nSilent = 0;
    g_slogCfg.nToFile = 0;
    g_slogCfg.nPretty = 0;
	if (g_slogCfg.nTdSafe) {
		g_slogCfg.nSync = 1;
	}
	else {
		g_slogCfg.nSync = 0;
	}
    /* Parse config file */
    slog_parse_config(pConf);

	SlogDate date;
	slog_get_date(&date);

    memset(slogFileName, 0, sizeof(slogFileName));
    if (g_slogCfg.nFileStamp) 
        snprintf(slogFileName, sizeof(slogFileName), "%s-%02d-%02d-%02d.log", \
			g_slogCfg.sFileName, date.year, date.mon, date.day);
    else 
        snprintf(slogFileName, sizeof(slogFileName), "%s.log", g_slogCfg.sFileName);

#if 0
	fopen(&slog_file, slogFileName, "w+");
	//fopen_s(&slog_file, slogFileName, "w+");
	if (slog_file == NULL) {
		printf("Open Log File Fail!!!!");
		return;
	}
	////char pStr[100];
	////snprintf(pStr, sizeof(pStr), "\n\n\n\n***************** New Log **************");
    ////fprintf(slog_file, "%s\n", pStr);
	fclose(slog_file);
#endif
}
void slog_free()
{
	fclose(slog_file);
}
