/*
*   日志记录类
*   分为KxLogger以及IKxLogHandler两个类
*   KxLogger负责日志的过滤，内容控制以及设置，并管理多个IKxLogHandler
*   IKxLogHandler负责实现日志信息的处理，可以是写文件，输出到控制台，甚至弹出提示框
*/
#ifndef _KX_LOGMANAGER_H__
#define _KX_LOGMANAGER_H__
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include "ILogHandler.h"

#include "LogFileHandler.h"
#include "LogConsoleHandler.h"

#if (KX_TARGET_PLATFORM == KX_PLATFORM_LINUX)
	#include<stdlib.h>
	#include<stdio.h>
	#include<time.h>
	#include<string.h>
	#include <stdarg.h>
#endif


// 日志对象（单例）
// 管理日志以及IKxLogHandler句柄
class LogManager
{
private:
    LogManager();
    virtual ~LogManager();

public:
    static LogManager* getInstance();
    static void destroy();

    // 添加一个日志处理句柄
    bool addHandler(int handleId, ILogHandler* handler);
    // 根据handler自身的handleId添加一个日志处理句柄
    bool addHandler(ILogHandler* handler);
    // 移除一个日志处理句柄
    bool removeHandler(int handleId);
    // 根据handleId，获取一个日志处理句柄
    ILogHandler* getHandler(int id);
    // 清除所有的日志处理句柄
    void clearHandler();

    // 写入一行日志，将由日志处理句柄进行处理
    void writelog(int tag, int level, const char* file, const char* func, int line, const char* log, ...);
    // 清除日志，只是调用日志处理句柄的onClean方法
    void clearLog();

    // 设置日志调试级别
    inline void setLogLevel(int lv) { m_LogLevel = lv; };
    inline int getLogLevel() { return m_LogLevel; }

    // 设置与获取日志的显示信息
    inline bool isShowLevel() { return m_LevelOption; }
    inline bool isShowTime() { return m_TimeOption; }
    inline bool isShowDate() { return m_DateOption; }
    inline bool isShowFile() { return m_FileOption; }
    inline bool isShowFunc() { return m_FuncOption; }
    inline bool isShowLine() { return m_LineOption; }
    inline void setShowLevel(bool b) { m_LevelOption = b; }
    inline void setShowTime(bool b) { m_TimeOption = b; }
    inline void setShowDate(bool b) { m_DateOption = b; }
    inline void setShowFile(bool b) { m_FileOption = b; }
    inline void setShowFunc(bool b) { m_FuncOption = b; }
    inline void setShowLine(bool b) { m_LineOption = b; }

	inline void HexDumpImp(const void *pdata, unsigned int len);

private:
    void updateFormat(int level, const char* log, const char* file, const char* func, int line);

private:
    int m_LogLevel;
    bool m_LevelOption;
    bool m_TimeOption;
    bool m_DateOption;
    bool m_FileOption;
    bool m_FuncOption;
    bool m_LineOption;
    std::string m_StringFormat;
    std::map<int, ILogHandler*> m_Handlers;
    static LogManager* m_Instance;
};

#define KX_LOG(tag, level, log, ...) LogManager::getInstance()->writelog(tag, level, __FILE__, __FUNCTION__, __LINE__, log, ##__VA_ARGS__)
#define KX_LOGDEBUG(log, ...) LogManager::getInstance()->writelog(1, KX_LOG_LEVEL::KX_LOG_DEBUG, __FILE__, __FUNCTION__, __LINE__, log, ##__VA_ARGS__)
#define KX_LOGINFO(log, ...) LogManager::getInstance()->writelog(1, KX_LOG_LEVEL::KX_LOG_INFO, __FILE__, __FUNCTION__, __LINE__, log, ##__VA_ARGS__)
#define KX_LOGERROR(log, ...) LogManager::getInstance()->writelog(1, KX_LOG_LEVEL::KX_LOG_ERROR, __FILE__, __FUNCTION__, __LINE__, log, ##__VA_ARGS__)
//#define LOGBATTLE(...)       do {} while (0)

#endif //_KX_LOGMANAGER_H__
