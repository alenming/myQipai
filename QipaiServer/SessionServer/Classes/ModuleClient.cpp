#include "ModuleClient.h"
#include "SessionClient.h"
#include "SessionServer.h"
#include "protocol/ServerProtocol.h"


ModuleClient::ModuleClient(void)
{
}

ModuleClient::~ModuleClient(void)
{
}

void ModuleClient::processLogic(char* buffer, unsigned int len, IKxComm *target)
{
    SessionClient* pClient = dynamic_cast<SessionClient*>(target);
	ServerConfig * pServerConfig = SessionServer::getInstance()->getServerCoinfig();
	
	// ֱ��ת������Ϸ������
	Head* head = reinterpret_cast<Head*>(buffer);
	int nMainCmd = head->MainCommand();
	int nSubCmd = head->SubCommand();

	if (nMainCmd == CMD_MAIN::CMD_HEARTBEAT && nSubCmd == CMD_MAIN::CMD_HEARTBEAT)
	{
		KX_LOGDEBUG("======================= �յ�������====================");
		Head head;
		head.MakeCommand(CMD_MAIN::CMD_HEARTBEAT, CMD_MAIN::CMD_HEARTBEAT);
		head.length = sizeof(head);
		head.id = -1;
		pClient->sendData(reinterpret_cast<char*>(&head), sizeof(head));
		return;
	}
	//��һ����Ϣ��,�ʺ�û��֤.�õ���guesId,��¼�ɹ����תΪuserId
	if (pClient->getPermission() == 0)
		head->id = pClient->getGuestId();
	else if (pClient->getPermission() == 1)
		head->id = pClient->getUserId();

	if (pClient->sendDataToServer(nMainCmd, nSubCmd, buffer, len))
		KX_LOGDEBUG("ת���ɹ�!");
	else
		KX_LOGDEBUG("ת��ʧ��!");
}

void ModuleClient::processError(IKxComm *target)
{
	this->userDisconnect(target);    // ��Ҷ���
}

// 1. �������з����������XXX������
// 2. ��ClientManager���Ƴ������
void ModuleClient::userDisconnect(IKxComm *target)
{
    SessionClient *pClient = dynamic_cast<SessionClient*>(target);
    if (pClient == NULL)
        return;

    // ����������Ϣ
    Head head;
	head.MakeCommand(SERVER_MAIN_CMD::SERVER_MAIN, SERVER_SUB_CMD::SERVER_SUB_OFFLINE);
    head.length = sizeof(head);

	if (pClient->getPermission() == 0)
		head.id = pClient->getGuestId();
	else if (pClient->getPermission() == 1)
		head.id = pClient->getUserId();

    // �������ݵ���˸�֪��ɫ����
    // ����������Ϣ�����ָ���ķ�����
    pClient->sendDataToAllServer(reinterpret_cast<char*>(&head), sizeof(head));
    // �ر�socket����NetWorkManager�к�clean�Ƴ�
	KX_LOGDEBUG("��ҵ�����!:%d", pClient->getUserId());
	pClient->clean();
	
}
