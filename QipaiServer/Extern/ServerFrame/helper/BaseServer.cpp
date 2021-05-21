#include "BaseServer.h"
#include <signal.h>

#include "TimerManager.h"
#include "Platform.h"
#include "log/LogManager.h"
#include "log/LogFileHandler.h"
#include "log/LogConsoleHandler.h"

#if KX_TARGET_PLATFORM == KX_PLATFORM_WIN32
    bool ctrlhandler(DWORD ev)
    {
        switch (ev)
        {
        case CTRL_C_EVENT:          // - ���û�������CTRL + C, ������GenerateConsoleCtrlEvent API����.
        case CTRL_BREAK_EVENT:      // - �û�����CTRL + BREAK, ������GenerateConsoleCtrlEvent API����.
        case CTRL_CLOSE_EVENT:      // - ����ͼ�رտ���̨����ϵͳ���͹ر���Ϣ��
        case CTRL_LOGOFF_EVENT:     // - �û��˳�ʱ�����ǲ��ܾ������ĸ��û�.
        case CTRL_SHUTDOWN_EVENT:   // - ��ϵͳ���ر�ʱ.
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

        //onServerUpdate();
	}
}

void BaseServer::onServerUpdate()
{

}

bool BaseServer::onServerInit()
{
    if (nullptr == m_TimerMgr)
    {
        m_TimerMgr = new TimerManager();
    }

	LogManager::getInstance()->setShowTime(false);
	LogManager::getInstance()->setShowDate(false);
	LogManager::getInstance()->setShowFunc(false);

	LogConsoleHandler* pConsoleHandle = new LogConsoleHandler();
	LogManager::getInstance()->addHandler(1, pConsoleHandle);

	LogFileHandler* pFileHandle = new LogFileHandler();
	pFileHandle->setFilePath("../bin/");
	pFileHandle->setFileName(m_ServerName);
	pFileHandle->setFastModel(false);
	LogManager::getInstance()->addHandler(2, pFileHandle);

	return true;
}

//server uninit
void BaseServer::onServerUninit()
{
	KX_LOGDEBUG("Server Stop !!");
	m_IsRunning = false;
    KXSAFE_RELEASE(m_Poller);
    KXSAFE_RELEASE(m_TimerMgr);

	LogManager::destroy();
}
