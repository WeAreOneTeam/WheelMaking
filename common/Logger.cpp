//
//  Logger.cpp
//  WheelMaking
//
//  Created by housir on 2019/5/17.
//  Copyright © 2019年 housir. All rights reserved.
//


#include "Logger.h"
#include "time.h"

char* szLogLevelStr[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

Logger* Logger::m_pInstance = NULL;

Logger::Logger()
{
    m_szLogBuff[0] = 0;
    m_enOutType = OutType_CMD;
    printf("construct Logger.\n");
}

Logger::~Logger()
{
    printf("destruct Logger.\n");
}

Logger* Logger::getInstance()
{
    if(NULL == m_pInstance)
    {
        pthread_mutex_lock(&mutex);
        if(NULL == m_pInstance)
        {
            m_pInstance = new Logger();
        }
        pthread_mutex_unlock(&mutex);
    }
    return m_pInstance;
}

void Logger::getCurLocalTime(char* pszCurTime)
{
    time_t tt = time(NULL);//这句返回的只是一个时间cuo
    tm* t= localtime(&tt);
    snprintf(pszCurTime, MAX_TIME_LEN, "%d-%02d-%02d %02d:%02d:%02d",
             t->tm_year + 1900,
             t->tm_mon + 1,
             t->tm_mday,
             t->tm_hour,
             t->tm_min,
             t->tm_sec);
}

// date level
// 日期、时间、日志级别、代码位置、日志内容、错误码等信息
const char szFormat[] = "%s %s %s\n";
__INT32 Logger::genLogStr(const char *msg, LogLevel eLevel)
{
    char szCurTime[MAX_TIME_LEN] = {0};
    getCurLocalTime(szCurTime);
    /* return the number of characters that would have been printed
     * if the size were unlimited (not including the final `\0');
     * return a negative value if an error occurs
     */
    __INT32 dwStrLen = snprintf(m_szLogBuff, MAX_LOG_BUFF_LEN, szFormat,
                                szCurTime, szLogLevelStr[eLevel], msg);
    dwStrLen = MIN(dwStrLen, MAX_LOG_BUFF_LEN - 1);
    return dwStrLen;
}

void Logger::log(const char *msg, LogLevel eLevel)
{
    __INT32 dwStrLen = genLogStr(msg, eLevel);
    if(dwStrLen <= 0)
    {
        return ;
    }
    
    switch(m_enOutType)
    {
        case OutType_CMD:
            printf("%s", m_szLogBuff);
            break;
        case OutType_FILE:
            //write_to_file(m_szLogBuff);
            break;
        default: // can not happen
            break;
    }
}

void Logger::trace(const char* msg)
{
    log(msg, LOG_LEVEL_TRACE);
}

void Logger::debug(const char* msg)
{
    log(msg, LOG_LEVEL_DEBUG);
}

void Logger::info(const char* msg)
{
    log(msg, LOG_LEVEL_INFO);
}

void Logger::warn(const char* msg)
{
    log(msg, LOG_LEVEL_WARN);
}

void Logger::error(const char* msg)
{
    log(msg, LOG_LEVEL_ERROR);
}

void Logger::fatal(const char* msg)
{
    log(msg, LOG_LEVEL_FATAL);
}
