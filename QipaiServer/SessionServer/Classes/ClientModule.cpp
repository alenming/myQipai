#include "ClientModule.h"
#include "SessionClient.h"
#include "SessionServer.h"
#include "NetworkManager.h"
#include "protocol/MainProtocol.h"
#include "protocol/LoginProtocol.h"

ClientModule::ClientModule(void)
{
}

ClientModule::~ClientModule(void)
{
}

void ClientModule::processLogic(char* buffer, unsigned int len, IKxComm *target)
{
    SessionClient* pClient = dynamic_cast<SessionClient*>(target);
	ServerConfig * pServerConfig = SessionServer::getInstance()->getServerCoinfig();

	// ֱ��ת������Ϸ������
	Head* head = reinterpret_cast<Head*>(buffer);
	int nMainCmd = head->MainCommand();
	int nSubCmd = head->SubCommand();
	head->uid = pClient->getGuestId();//������֮��ͨѶ�����ID

	KX_LOGDEBUG("ClientModule onRecy Message!");
	if (nMainCmd ==  MAIN_CMD::CMD_HEARTBEART && nSubCmd == MAIN_CMD::CMD_HEARTBEART)
	{
		KX_LOGDEBUG("======================= heartbeart!====================");
		Head head;
		head.MakeCommand(MAIN_CMD::CMD_HEARTBEART, MAIN_CMD::CMD_HEARTBEART);
		head.length = sizeof(head);
		head.uid = -1;

		pClient->sendData(reinterpret_cast<char*>(&head), sizeof(head));
		return;
	}

	pClient->sendDataToServer(nMainCmd, nSubCmd, buffer, len);
}

void ClientModule::processError(IKxComm *target)
{
    // ��Ҷ���
    this->userDisconnect(target);
}

// 1. �������з����������XXX������
// 2. ��ClientManager���Ƴ������
void ClientModule::userDisconnect(IKxComm *target)
{
    SessionClient *pClient = dynamic_cast<SessionClient*>(target);
    if (pClient == NULL)
    {
        return;
    }

    // ����������Ϣ
    Head head;
	head.MakeCommand(SERVER_MAIN_CMD::SERVER_MAIN, SERVER_SUB_CMD::SERVER_SUB_OFFLINE);
    head.length = sizeof(head);
    head.uid = pClient->getGuestId();

    // �������ݵ���˸�֪��ɫ����
    // ����������Ϣ�����ָ���ķ�����
    pClient->sendDataToAllServer(reinterpret_cast<char*>(&head), sizeof(head));
    // �ر�socket����NetWorkManager�к�clean�Ƴ�
	KX_LOGDEBUG("client is loginout:%d", pClient->getGuestId());
	pClient->clean();
	
}
