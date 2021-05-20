#include "ConnectModule.h"
#include "NetworkManager.h"
#include "SessionClienter.h"
#include "protocol/ServerProtocol.h"
#include "SessionHelper.h"
#include "protocol/LoginProtocol.h"

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
    // ����ָ����ǰ��
	Head* head = reinterpret_cast<Head*>(buffer);
	int nMainCmd = head->MainCommand();
	//int nSubCmd = head->SubCommand();
	int guesID = head->id;

	switch (nMainCmd)
	{
	case CMD_MAIN::CMD_LOGIN_SERVER:
	{
		SessionClienter* pClient = reinterpret_cast<SessionClienter*>(NetWorkManager::getInstance()->getUser(guesID));
		if (!pClient)		return;
		KX_LOGDEBUG("���յ���������Ϣ,ת����ǰ��!ConnectModule::processLogic");
		pClient->sendData(buffer, head->length);
		break;
	}
	case SERVER_MAIN_CMD::SERVER_MAIN:
	{
		LOGIN_DATA *loginCS = reinterpret_cast<LOGIN_DATA*>(head->data());
		int userId = loginCS->userId;
		SessionHelper::ServerSubInit(guesID, userId);
		break;
	}
	default:
		break;
	}

}

void ConnectModule::processError(IKxComm *target)
{
	// ������ӶϿ�
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
