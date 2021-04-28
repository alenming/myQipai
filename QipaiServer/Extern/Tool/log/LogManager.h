/*
*   ��־��¼��
*   ��ΪKxLogger�Լ�IKxLogHandler������
*   KxLogger������־�Ĺ��ˣ����ݿ����Լ����ã���������IKxLogHandler
*   IKxLogHandler����ʵ����־��Ϣ�Ĵ���������д�ļ������������̨������������ʾ��
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


// ��־���󣨵�����
// ������־�Լ�IKxLogHandler���
class LogManager
{
private:
    LogManager();
    virtual ~LogManager();

public:
    static LogManager* getInstance();
    static void destroy();

    // ���һ����־������
    bool addHandler(int handleId, ILogHandler* handler);
    // ����handler�����handleId���һ����־������
    bool addHandler(ILogHandler* handler);
    // �Ƴ�һ����־������
    bool removeHandler(int handleId);
    // ����handleId����ȡһ����־������
    ILogHandler* getHandler(int id);
    // ������е���־������
    void clearHandler();

    // д��һ����־��������־���������д���
    void writelog(int tag, int level, const char* file, const char* func, int line, const char* log, ...);
    // �����־��ֻ�ǵ�����־��������onClean����
    void clearLog();

    // ������־���Լ���
    inline void setLogLevel(int lv) { m_LogLevel = lv; };
    inline int getLogLevel() { return m_LogLevel; }

    // �������ȡ��־����ʾ��Ϣ
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
