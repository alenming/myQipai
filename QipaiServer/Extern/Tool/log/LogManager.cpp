#include "LogManager.h"


#define MAX_BUF 1024

using namespace std;

LogManager* LogManager::m_Instance = nullptr;

LogManager::LogManager()
:m_LogLevel(0),
m_LevelOption(true),
m_TimeOption(true),
m_DateOption(true),
m_FileOption(false),
m_FuncOption(false),
m_LineOption(false)
{

}

LogManager::~LogManager()
{
    clearHandler();
}

LogManager* LogManager::getInstance()
{
    if (nullptr == m_Instance)
    {
        m_Instance = new LogManager();
    }

    return m_Instance;
}

void LogManager::destroy()
{
    if (nullptr != m_Instance)
    {
        delete m_Instance;
        m_Instance = nullptr;
    }
}

bool LogManager::addHandler(int handleId, ILogHandler* handler)
{
    if (nullptr == handler)
    {
        return false;
    }

    map<int, ILogHandler*>::iterator iter = m_Handlers.find(handleId);
    if (iter != m_Handlers.end())
    {
        return false;
    }

    handler->setHandleId(handleId);
    m_Handlers[handleId] = handler;
    return true;
}

bool LogManager::addHandler(ILogHandler* handler)
{
    if (nullptr == handler)
    {
        return false;
    }

    map<int, ILogHandler*>::iterator iter = m_Handlers.find(handler->getHandleId());
    if (iter != m_Handlers.end())
    {
        return false;
    }

    m_Handlers[handler->getHandleId()] = handler;
    return true;
}

bool LogManager::removeHandler(int handleId)
{
    map<int, ILogHandler*>::iterator iter = m_Handlers.find(handleId);
    if (iter != m_Handlers.end())
    {
        iter->second->onExit();
        delete iter->second;
        m_Handlers.erase(iter);
        return true;
    }

    return false;
}

ILogHandler* LogManager::getHandler(int id)
{
    map<int, ILogHandler*>::iterator iter = m_Handlers.find(id);
    if (iter != m_Handlers.end())
    {
        return iter->second;
    }

    return nullptr;
}

void LogManager::clearHandler()
{
    for (map<int, ILogHandler*>::iterator iter = m_Handlers.begin();
        iter != m_Handlers.end(); ++iter)
    {
        iter->second->onExit();
        delete iter->second;
    }

    m_Handlers.clear();
}

void LogManager::writelog(int tag, int level, const char* file, const char* func, int line, const char* log, ...)
{

#ifdef _DEBUG
    if (level >= m_LogLevel)
    {
        char szBuf[MAX_BUF];
        va_list ap;
        va_start(ap, log);
        vsnprintf_s(szBuf, MAX_BUF, log, ap);
        va_end(ap);

        updateFormat(level, szBuf, file, func, line);
        map<int, ILogHandler*>::iterator iter = m_Handlers.begin();
        for (; iter != m_Handlers.end(); ++iter)
        {
            // 与为ture的才执行
            if (iter->second->getTag() & tag)
            {
                if (1 == tag)
                {
                    iter->second->onLog(level, m_StringFormat);
                }
                else
                {
                    // tag 如果不是默认值1的话, 自己处理前缀格式
                    iter->second->onLog(level, szBuf);
                }
            }            
        }
    }
#endif
}

void LogManager::clearLog()
{
    map<int, ILogHandler*>::iterator iter = m_Handlers.begin();
    for (; iter != m_Handlers.end(); ++iter)
    {
        iter->second->onClean();
    }
}

void LogManager::updateFormat(int level, const char* log, const char* file, const char* func, int line)
{
    char szBuf[MAX_BUF];
    m_StringFormat = "";

    if (m_LevelOption)
    {
        switch (level)
        {
		case KX_LOG_DEBUG:
            m_StringFormat += "[Debug]";
            break;
		case KX_LOG_INFO:
            m_StringFormat += "[Info]";
            break;
		case KX_LOG_ERROR:
            m_StringFormat += "[Error]";
            break;

        default:
            break;
        }
    }

    if (m_DateOption || m_TimeOption)
    {
        time_t t = time(nullptr);
		tm tmnow;
		localtime_s(&tmnow, &t);

        if (m_DateOption)
        {
            sprintf_s(szBuf, sizeof(szBuf), "[%04d-%02d-%02d]",
                tmnow.tm_year + 1900, tmnow.tm_mon, tmnow.tm_mday);
            m_StringFormat += szBuf;
        }

        if (m_TimeOption)
        {
            sprintf_s(szBuf, sizeof(szBuf), "[%02d:%02d:%02d]",
                tmnow.tm_hour, tmnow.tm_min, tmnow.tm_sec);
            m_StringFormat += szBuf;
        }
    }

    if (m_FileOption && nullptr != file)
    {
		m_StringFormat += "[file]->[" + string(file) + "] ";
    }

    if (m_FuncOption && nullptr != func)
    {
        m_StringFormat += "[func]->[" + string(func) + "] ";
    }

    if (m_LineOption)
    {
        sprintf_s(szBuf, sizeof(szBuf), "[line]->[%d] ", line);
        m_StringFormat += szBuf;
    }

    m_StringFormat += "	";
    m_StringFormat += log;
}

void LogManager::HexDumpImp(const void *pdata, unsigned int len)
{
	{
		if (pdata == 0 || len == 0)
		{
			return;
		}

		int cnt = 0;
		int n = 0;
		int cnt2 = 0;
		const char *data = (const char *)pdata;
		ILogHandler* pHandle = getHandler(2);
		if (pHandle == nullptr)
		{
			return;
		}
		std::string NetStatMem;
		NetStatMem = pHandle->getFileName() + "NetMem.log";
		ofstream fileos(NetStatMem.c_str(), ios::app);
		if (!fileos)return;
		fileos << "Address               Hexadecimal values                  Printable\n";
		cout << "Address               Hexadecimal values                  Printable\n";
		fileos << "-------  -----------------------------------------------  -------------\n";
		cout << "-------  -----------------------------------------------  -------------\n";
		fileos << "\n";
		//cout << "\n";
		unsigned char buffer[20];
		unsigned int rpos = 0;

		while (1)
		{
			if (len <= rpos)
			{
				break;
			}
			if (len >= rpos + 16)
			{
				memcpy(buffer, data + rpos, 16);
				rpos += 16;
				cnt = 16;
			}
			else
			{
				memcpy(buffer, data + rpos, len - rpos);
				cnt = len - rpos;
				rpos = len;
			}
			if (cnt <= 0)
			{
				return;
			}

			fileos << setw(7) << (int)rpos << "  ";
			cout << setw(7) << (int)rpos << "  ";

			cnt2 = 0;
			for (n = 0; n < 16; n++)
			{
				cnt2 = cnt2 + 1;
				if (cnt2 <= cnt)
				{
					fileos << hex << setw(2) << setfill('0') << (unsigned int)buffer[n];
					cout << hex << setw(2) << setfill('0') << (unsigned int)buffer[n];
				}
				else
				{
					fileos << "  ";
					cout << "  ";
				}
				fileos << "  ";
				cout << " ";
			}

			fileos << setfill(' ');
			cout << setfill(' ');

			fileos << " ";
			cout << " ";
			cnt2 = 0;
			for (n = 0; n < 16; n++)
			{
				cnt2 = cnt2 + 1;
				if (cnt2 <= cnt)
				{
					if (buffer[n] < 32 || 126 < buffer[n])
					{
						fileos << '.';
						cout << '.';
					}
					else
					{
						fileos << buffer[n];
						cout << buffer[n];
					}
				}
			}
			fileos << "\n";
			cout << "\n";
			fileos << dec;
			cout << dec;
		}
		fileos.close();
		return;
	}

}
