//
//  Logger.h
//  WheelMaking
//
//  Created by housir on 2019/5/17.
//  Copyright © 2019年 housir. All rights reserved.
//

#ifndef Logger_h
#define Logger_h

#include <stdio.h>
#include "commondefine.h"
#include <pthread.h>

#define MAX_TIME_LEN 64
#define MAX_LOG_BUFF_LEN 1024

enum LogLevel
{
    LOG_LEVEL_TRACE,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
};

enum OutType
{
    OutType_CMD,
    OutType_FILE
};

class Logger
{
public:
    Logger();
    ~Logger();
    
    static Logger* getInstance();
    
    void setOutType(OutType type) { m_enOutType = type; }
    
    void getCurLocalTime(char* pszCurTime);
    // return the length of genStr
    __INT32 genLogStr(const char* msg, LogLevel eLevel);
    
    void log(const char* msg, LogLevel eLevel);
    
    void trace(const char* msg);
    void debug(const char* msg);
    void info(const char* msg);
    void warn(const char* msg);
    void error(const char* msg);
    void fatal(const char* msg);
    
protected:
    
private:
    static Logger* m_pInstance;
    
    char m_szLogBuff[MAX_LOG_BUFF_LEN];
    OutType m_enOutType;
};

#endif /* Logger_h */
