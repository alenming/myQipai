#include "ModuleConnect.h"
#include "SessionHelper.h"
#include "SessionConnect.h"
#include "LogManager.h"
ModuleConnect::ModuleConnect(void)
: m_pSink(nullptr)
{
}

ModuleConnect::~ModuleConnect(void)
{
    KXSAFE_DELETE(m_pSink);
}

bool ModuleConnect::init(IGameEvent *pSink)
{
	m_pSink = pSink;
	return true;
}

void ModuleConnect::processLogic(char* buffer, unsigned int len, IComm *target)
{
	// 发给指定的前端
	Head* head = reinterpret_cast<Head*>(buffer);
	if (len < sizeof(Head))
		return;
	SessionHelper::ProcessServerToSession(buffer, len, target);
}

void ModuleConnect::processError(IComm *target)
{
	// 后端连接断开
	LOGDEBUG("Socket Connect To Server Failed");
}

void ModuleConnect::processEvent(int eventId, IComm* target)
{
    if (eventId == EVENT_CONNECT_FAILE)
    {
		LOGDEBUG("Connect Server Failed");
    }
    else if (eventId == EVENT_CONNECT_SUCCESS)
    {
		LOGDEBUG("Connect Server Success");
    }
}

bool ModuleConnect::processUserEvent(int nType, IComm* target)
{
	if (m_pSink != nullptr)
	{
        m_pSink->onUserEvent(nType, target);
	}
	return true;
}
