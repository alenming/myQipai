#include "BaseServer.h"
#include <signal.h>
#include "TimerManager.h"
#include "LogManager.h"


#if KX_TARGET_PLATFORM == KX_PLATFORM_WIN32
    bool ctrlhandler(DWORD ev)
    {
        switch (ev)
        {
        case CTRL_C_EVENT:          // - 当用户按下了CTRL + C, 或者由GenerateConsoleCtrlEvent API发出.
        case CTRL_BREAK_EVENT:      // - 用户按下CTRL + BREAK, 或者由GenerateConsoleCtrlEvent API发出.
        case CTRL_CLOSE_EVENT:      // - 当试图关闭控制台程序，系统发送关闭消息。
        case CTRL_LOGOFF_EVENT:     // - 用户退出时，但是不能决定是哪个用户.
        case CTRL_SHUTDOWN_EVENT:   // - 当系统被关闭时.
            BaseServer::getInstance()->stopServer();
        default:
            break;
        }
        return true;
    }
#endif

void processSingal(int nSingal)
{
	switch (nSingal)
    {
#if KX_TARGET_PLATFORM == KX_PLATFORM_LINUX
	case SIGQUIT:
    case SIGKILL:
#endif
    case SIGINT:
    case SIGABRT:
	case SIGTERM:
	{
		BaseServer::getInstance()->stopServer();
	}
	break;
	default:
		break;
	}
}

BaseServer* BaseServer::m_Server = nullptr;

BaseServer::BaseServer()
: m_Poller(nullptr)
, m_TimerMgr(nullptr)
, m_IsRunning(false)
, m_ServerName("KxBaseServer")
{
    m_Server = this;
	m_Tick = 0;
}

BaseServer::~BaseServer()
{
}

void BaseServer::startServer()
{
#if KX_TARGET_PLATFORM == KX_PLATFORM_LINUX
	signal(SIGPIPE, SIG_IGN);
	signal(SIGQUIT, processSingal);
	signal(SIGKILL, processSingal);
#endif
#if KX_TARGET_PLATFORM == KX_PLATFORM_WIN32
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)ctrlhandler, true);
#endif
    signal(SIGINT, processSingal);
    signal(SIGABRT, processSingal);
    signal(SIGTERM, processSingal);

	if (!onServerInit())
		return;

	runServer();
	onServerUninit();
}

//server run
void BaseServer::runServer()
{
    m_IsRunning = true;
	while(m_IsRunning)
	{
		if (nullptr != m_Poller)
		{
			int nRet = m_Poller->poll();
			if (nRet == 0)
			{
				m_Tick++;
				if (m_Tick >= 100)
				{
#if(KX_TARGET_PLATFORM == KX_PLATFORM_LINUX)
					usleep(1000);
#else
					Sleep(10);
#endif
				}
			}
			else
			{
				m_Tick = 1;
			}
		}
        if (nullptr != m_TimerMgr)
            m_TimerMgr->updateTimers();
	}
}

void BaseServer::onServerUpdate()
{

}

bool BaseServer::onServerInit()
{
    if (nullptr == m_TimerMgr)
        m_TimerMgr = new TimerManager();

	LogManager::getInstance()->setShowDate(true);
	LogManager::getInstance()->setShowTime(true);
	LogManager::getInstance()->setShowFunc(true);
	LogManager::getInstance()->setShowLine(true);
	LogManager::getInstance()->setShowFile(false);
	LogConsoleHandler* pConsoleHandle = new LogConsoleHandler();
	LogManager::getInstance()->addHandler(1, pConsoleHandle);
	LogFileHandler* pFileHandle = new LogFileHandler();
	pFileHandle->setFilePath("../bin/");
	pFileHandle->setFileName(m_ServerName);
//	pFileHandle->setFastModel(false);
	LogManager::getInstance()->addHandler(2, pFileHandle);
	return true;
}

//server uninit
void BaseServer::onServerUninit()
{
	LOGDEBUG("Server Stop !!");
	m_IsRunning = false;
    KXSAFE_RELEASE(m_Poller);
    KXSAFE_RELEASE(m_TimerMgr);

	LogManager::destroy();
}

int BaseServer::getCurrentTime()
{
	return static_cast<int>(time(nullptr));
}
