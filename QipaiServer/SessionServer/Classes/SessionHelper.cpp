#include "SessionHelper.h"
#include "NetManager.h"
#include "SessionClient.h"
#include "BufferData.h"
#include "ServerProtocol.h"
#include "LoginProtocol.h"
#include "LogManager.h"
#include "SessionServer.h"


SessionHelper::SessionHelper()
{
}

SessionHelper::~SessionHelper()
{
}

void SessionHelper::ProcessClientToServer(char* buffer, unsigned int len, IComm *target)
{
	SessionClient* pClient = dynamic_cast<SessionClient*>(target);
	ServerConfig * pServerConfig = SessionServer::getInstance()->getServerCoinfig();

	// ֱ��ת������Ϸ������
	Head* head = reinterpret_cast<Head*>(buffer);
	int nMainCmd = head->MainCommand();
	int nSubCmd = head->SubCommand();

	if (nMainCmd == MAIN_CMD::CMD_HEARTBEAT && nSubCmd == MAIN_CMD::CMD_HEARTBEAT)
	{
		LOGDEBUG("======================= �յ�������====================");
		Head head;
		head.MakeCommand(MAIN_CMD::CMD_HEARTBEAT, MAIN_CMD::CMD_HEARTBEAT);
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
		LOGDEBUG("ת���ɹ�!");
	else
		LOGDEBUG("ת��ʧ��!");
}

void SessionHelper::userDisconnect(IComm *target)
{
	// 1. �������з����������XXX������  2. ��ClientManager���Ƴ������
	SessionClient *pClient = dynamic_cast<SessionClient*>(target);
	if (pClient == nullptr)
		return;
	// ����������Ϣ
	Head head;
	head.MakeCommand(SERVER_MAIN_CMD::SERVER_MAIN, SERVER_SUB_CMD::SERVER_SUB_OFFLINE);
	head.length = sizeof(head);

	if (pClient->getPermission() == 0)
		head.id = pClient->getGuestId();
	else if (pClient->getPermission() == 1)
		head.id = pClient->getUserId();

	// �������ݵ���˸�֪��ɫ���� ����������Ϣ�����ָ���ķ�����
	pClient->sendDataToAllServer(reinterpret_cast<char*>(&head), sizeof(head));
	// �ر�socket����NetWorkManager�к�clean�Ƴ�
	LOGDEBUG("��ҵ�����!:%d", head.id);
	pClient->clean();
}


/*-----------------------------------------------------------------------------------------------------*/

void SessionHelper::ProcessServerToSession(char* buffer, unsigned int len, IComm *target)
{
	// ����ָ����ǰ��
	Head* head = reinterpret_cast<Head*>(buffer);
	int nMainCmd = head->MainCommand();
	int nSubCmd = head->SubCommand();
	int uId = head->id;

	switch (nMainCmd)
	{
		case MAIN_CMD::CMD_LOGIN_SERVER: //ת����Ϣ��ǰ��
		{
			SessionClient* pClient = reinterpret_cast<SessionClient*>(NetManager::getInstance()->getUser(uId));
			if (!pClient)		return;
			LOGDEBUG("���յ���������Ϣ,ת����ǰ��!ConnectModule::processLogic uId:%d", uId);
			pClient->sendData(buffer, head->length);
			break;
		}
		case SERVER_MAIN_CMD::SERVER_MAIN: //���������������Ϣ
		{
			ServerProcess(nSubCmd, buffer);
			break;
		}
		default:
			break;
	}
}


void SessionHelper::ServerProcess(int subCmd, char* buffer)
{
	Head* head = reinterpret_cast<Head*>(buffer);
	int nSubCmd = head->SubCommand();
	int uId = head->id;

	switch (subCmd)
	{
		case SERVER_SUB_CMD::SERVER_SUB_NEW_USER:
		{
			LOGIN_DATA *loginCS = reinterpret_cast<LOGIN_DATA*>(head->data());
			std::string temp = loginCS->userName;
			int userNameId = atoi(loginCS->userName);
			ServerSubInit(uId, userNameId);
			break;
		}
		case SERVER_SUB_CMD::SERVER_SUB_UPDATE_PER:
		{
			updateUserPermission(uId, buffer);
			break;
		}
		default:
			break;
	}
}

void SessionHelper::updateUserPermission(int uId, char* buffer)
{
	Head* head1 = reinterpret_cast<Head*>(buffer);
	LOGIN_DATA *loginData = reinterpret_cast<LOGIN_DATA*>(head1->data());
	int userNameId = atoi(loginData->userName);

	SessionClient *pSessionClient = dynamic_cast<SessionClient *>(NetManager::getInstance()->getGuest(uId));
	if (!pSessionClient)
	{
		LOGDEBUG("�Ҳ������%d", uId);
		return;
	}
	pSessionClient->setUserId(userNameId);
	pSessionClient->setPermission(1);
	LOGDEBUG("�л�Ȩ��");
	NetManager::getInstance()->changeGuestToUser(pSessionClient, userNameId);	// ��֤�ɹ���guestתuser


	BufferData* buffer1 = newBufferData(MAIN_CMD::CMD_LOGIN_SERVER, SUB_CMD_LOGIN::CMD_S2C_LOGIN);
	LOGIN_DATA da = LOGIN_DATA();
	memcpy(da.userName, loginData->userName, sizeof(da.userName));
	memcpy(da.passWord, loginData->passWord, sizeof(da.passWord));
	buffer1->writeData(da);
	char* bu = buffer1->getBuffer();
	Head* head = reinterpret_cast<Head*>(bu);
	head->length = buffer1->getDataLength();
	head->id = userNameId;
	//�����û�����
	if (pSessionClient != NULL)
		pSessionClient->sendData(bu, head->length);
}

void SessionHelper::ServerSubInit(int uId, int userNameId)
{
	NetManager *pNetWorkManager = NetManager::getInstance();
	SessionClient *pSessionClient = dynamic_cast<SessionClient *>(pNetWorkManager->getGuest(uId));
	// ���ӶϿ�/�������Ӳ�����
	if (!pSessionClient)
	{
		LOGDEBUG("�Ҳ������%d", uId);
		return;
	}
	//��֤�߼�
	bool isSuccessful = true;
	BufferData* buffer1 = newBufferData(MAIN_CMD::CMD_LOGIN_SERVER, SUB_CMD_LOGIN::CMD_S2C_PERMISSION);
	char* bu = buffer1->getBuffer();
	Head* head = reinterpret_cast<Head*>(bu);
	head->length = buffer1->getDataLength();

	if (!isSuccessful)		// ��֤ʧ��
	{
		LOGDEBUG("��֤ʧ��!");
		head->id = uId;
		pSessionClient->sendData(bu, head->length);
		pSessionClient->clean();
	}
	else
	{
		head->id = userNameId;
		pSessionClient->sendData(bu, head->length);
		pSessionClient->setUserId(userNameId);
		pSessionClient->setPermission(1);
		LOGDEBUG("��֤�ɹ�!,�л��ο�Ϊ��ʽ���");
		pNetWorkManager->changeGuestToUser(pSessionClient, userNameId);		// ��֤�ɹ���guestתuser
	}
}


