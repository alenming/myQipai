#include "ClientModule.h"
#include "SessionClienter.h"
#include "SessionServer.h"
#include "server/ServerProtocol.h"


ClientModule::ClientModule(void)
{
}

ClientModule::~ClientModule(void)
{
}

void ClientModule::processLogic(char* buffer, unsigned int len, IKxComm *target)
{
    SessionClienter* pClient = dynamic_cast<SessionClienter*>(target);
	ServerConfig * pServerConfig = SessionServer::getInstance()->getServerCoinfig();
	
	// ֱ��ת������Ϸ������
	Head* head = reinterpret_cast<Head*>(buffer);
	int nMainCmd = head->MainCommand();
	int nSubCmd = head->SubCommand();
	head->uid = pClient->getGuestId();//������֮��ͨѶ�����ID

	if (nMainCmd == CMD_MAIN::CMD_HEARTBEAT && nSubCmd == CMD_MAIN::CMD_HEARTBEAT)
	{
		KX_LOGDEBUG("======================= �յ�������====================");
		Head head;
		head.MakeCommand(CMD_MAIN::CMD_HEARTBEAT, CMD_MAIN::CMD_HEARTBEAT);
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
    SessionClienter *pClient = dynamic_cast<SessionClienter*>(target);
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
	KX_LOGDEBUG("��ҵ�����!:%d", pClient->getGuestId());
	pClient->clean();
	
}
