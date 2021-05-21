#include "ModuleConnect.h"
#include "NetworkManager.h"
#include "SessionClient.h"
#include "protocol/ServerProtocol.h"
#include "SessionHelper.h"
#include "protocol/LoginProtocol.h"
#include "log/LogManager.h"
#include "KxTCPConnector.h"
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

void ModuleConnect::processLogic(char* buffer, unsigned int len, IKxComm *target)
{
    // 发给指定的前端
	Head* head = reinterpret_cast<Head*>(buffer);
	int nMainCmd = head->MainCommand();
	int nSubCmd = head->SubCommand();
	int guesID = head->id;

	switch (nMainCmd)
	{
	case CMD_MAIN::CMD_LOGIN_SERVER:
	{
		SessionClient* pClient = reinterpret_cast<SessionClient*>(NetWorkManager::getInstance()->getUser(guesID));
		if (!pClient)		return;
		KX_LOGDEBUG("接收到服务器消息,转发给前端!ConnectModule::processLogic");
		pClient->sendData(buffer, head->length);
		break;
	}
	case SERVER_MAIN_CMD::SERVER_MAIN:
	{
		SessionHelper::ServerProcess(nSubCmd, buffer);
		break;
	}
	default:
		break;
	}

}

void ModuleConnect::processError(IKxComm *target)
{
	// 后端连接断开
	KX_LOGDEBUG("Socket Connect To Server Failed");
}

void ModuleConnect::processEvent(int eventId, IKxComm* target)
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

bool ModuleConnect::processUserEvent(int nType, IKxComm* target)
{
	if (m_pSink != nullptr)
	{
        m_pSink->onUserEvent(nType, target);
	}
	return true;
}
