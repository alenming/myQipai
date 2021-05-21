#include "SessionHelper.h"
#include "NetworkManager.h"
#include "SessionClient.h"
#include "protocol/ServerProtocol.h"
#include "protocol/LoginProtocol.h"
#include "log/LogManager.h"

SessionHelper::SessionHelper()
{
}

SessionHelper::~SessionHelper()
{
}

void SessionHelper::ServerProcess(int subCmd, char* buffer)
{
	Head* head = reinterpret_cast<Head*>(buffer);
	int nMainCmd = head->MainCommand();
	int nSubCmd = head->SubCommand();
	int userId = head->id;
	switch (subCmd)
	{
		case SERVER_SUB_CMD::SERVER_SUB_NEW_USER:
		{
			ServerSubInit(userId);
			break;
		}
		case SERVER_SUB_CMD::SERVER_SUB_UPDATE_PER:
		{
			break;
		}
		default:
			break;
	}
}

void SessionHelper::ServerSubInit( int userId)
{

	NetWorkManager *pNetWorkManager = NetWorkManager::getInstance();

	unsigned int offset = sizeof(Head);
	SessionClient *pSessionClient = dynamic_cast<SessionClient *>(pNetWorkManager->getGuest(userId));
	// ���ӶϿ�/�������Ӳ�����
	if (pSessionClient == NULL)
	{
		return;
	}
	//��֤�߼�
	bool isSuccessful = true;
	Head head;
	head.MakeCommand(CMD_MAIN::CMD_LOGIN_SERVER, LOGIN_SUB_CMD::CMD_C2S_RESIGIT);
	head.length = sizeof(head);
	head.id = pSessionClient->getUserId();

	if (!isSuccessful)
	{
		// ��֤ʧ��
		pSessionClient->sendData(reinterpret_cast<char*>(&head), sizeof(head));
		pSessionClient->clean();
	}
	else
	{
		pSessionClient->sendData(reinterpret_cast<char*>(&head), sizeof(head));
		pSessionClient->setUserId(userId);
		pSessionClient->setPermission(1);
		KX_LOGDEBUG("��֤�ɹ�!,�л��ο�Ϊ��ʽ���");
		// ��֤�ɹ���guestתuser
		pNetWorkManager->changeGuestToUser(pSessionClient, userId);

	}
}


