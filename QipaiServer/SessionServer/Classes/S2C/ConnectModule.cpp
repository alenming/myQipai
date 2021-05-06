#include "ConnectModule.h"
#include "NetworkManager.h"
#include "SessionClienter.h"



ConnectModule::ConnectModule(void)
: m_pSink(NULL)
{
}

ConnectModule::~ConnectModule(void)
{
    KXSAFE_DELETE(m_pSink);
}

bool ConnectModule::init(IGameEvent *pSink)
{
	m_pSink = pSink;
	return true;
}

void ConnectModule::processLogic(char* buffer, unsigned int len, IKxComm *target)
{
    // 发给指定的前端
	Head* head = reinterpret_cast<Head*>(buffer);
	int nMainCmd = head->MainCommand();
	int nSubCmd = head->SubCommand();
	int guesID = head->uid;

	SessionClienter* pClient = reinterpret_cast<SessionClienter*>(NetWorkManager::getInstance()->getUser(guesID));

	if (!pClient)
	{
		return;
	}
	KX_LOGDEBUG("接收到服务器消息,转发给前端!ConnectModule::processLogic");
	pClient->sendData(buffer, head->length);
}

void ConnectModule::processError(IKxComm *target)
{
	// 后端连接断开
	KX_LOGDEBUG("Socket Connect To Server Failed");
}

void ConnectModule::processEvent(int eventId, IKxComm* target)
{
    if (eventId == KXEVENT_CONNECT_FAILE)
    {
		KX_LOGDEBUG("Connect Server Failed");
    }
    else if (eventId == KXEVENT_CONNECT_SUCCESS)
    {
		//KX_LOGDEBUG("Connect Server Success");
    }
}

bool ConnectModule::processUserEvent(int nType, IKxComm* target)
{
	if (m_pSink != NULL)
	{
        m_pSink->onUserEvent(nType, target);
	}
	return true;
}
