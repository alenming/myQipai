#include "LoginService.h"
#include "userdata/UserManager.h"
#include "GateManager.h"
#include "helper/BufferData.h"
#include "server/Head.h"
#include "protocol/ServerProtocol.h"
#include "protocol/LoginProtocol.h"


using namespace std;

// ����ͻ��˵���Ϣ
void LoginService::processServiceC2S(int subcmd, int uid, char *buffer, int len, IKxComm *commun)
{
	switch (subcmd)
	{
	case LOGIN_SUB_CMD::CMD_C2S_LOGIN:
		CMD_C2S_LOGIN(uid, buffer, len, commun);
		break;
	default:
		break;
	}
}


void LoginService::CMD_C2S_LOGIN(int uid, char *buffer, int len, IKxComm *commun)
{
	Head* head = reinterpret_cast<Head*>(buffer);

	if (head->id != uid || head->id < 0) return;

	LOGIN_DATA *loginCS = reinterpret_cast<LOGIN_DATA*>(head->data());

	// ���CGameUser
	GameUser* pGameUser = UserManager::getInstance()->getGameUser(loginCS->userId);
	if (NULL == pGameUser)
	{
		//���û�
		KX_LOGDEBUG("���û�!");
		pGameUser = UserManager::getInstance()->newGameUser(loginCS->userId, loginCS->passWord);
		CMD_S2C_NEW_USER_LOGIN(uid, loginCS->userId);
	}
	else
	{
		// ����������û������ߺ����һ��ʱ�����Զ��Ƴ�
		// �÷������޳��Ƴ��б����ݣ��������Զ��ͷţ���Ϊ�Һ������ֻ�����
		KX_LOGDEBUG("���û�!");
		UserManager::getInstance()->donotDeleteUser(uid);
		pGameUser->refreshModel(MODELTYPE_USER);
		CMD_S2C_LOGIN(uid, loginCS->userId);
	}
}

void LoginService::CMD_S2C_LOGIN(int uid, int accountId)
{
	// ��ʼ�·�����
	LOGIN_DATA loginSC;

	BufferData* buffer = newBufferData(CMD_MAIN::CMD_LOGIN_SERVER, LOGIN_SUB_CMD::CMD_S2C_LOGIN);

	GameUser* pGameUser = UserManager::getInstance()->getGameUser(uid);

	loginSC.userId = accountId;
	buffer->writeData(loginSC);

	//����д�����ݳ���
	char* bu = buffer->getBuffer();
	Head* head = reinterpret_cast<Head*>(bu);
	head->id = uid;
	head->length = buffer->getDataLength();

	//�����û�����
	GateManager::getInstance()->Send(buffer->getBuffer(), head->length);
	KX_LOGDEBUG("��¼�ɹ�! uid = %d, accounld=%d", head->id, loginSC.userId);
}

void LoginService::CMD_S2C_NEW_USER_LOGIN(int uid, int accountId)
{
	// ��ʼ�·�����
	LOGIN_DATA loginSC;

	BufferData* buffer = newBufferData(SERVER_MAIN_CMD::SERVER_MAIN, SERVER_SUB_CMD::SERVER_SUB_NEW_USER);

	GameUser* pGameUser = UserManager::getInstance()->getGameUser(uid);

	loginSC.userId = accountId;
	buffer->writeData(loginSC);

	//����д�����ݳ���
	char* bu = buffer->getBuffer();
	Head* head = reinterpret_cast<Head*>(bu);
	head->id = uid;
	head->length = buffer->getDataLength();

	//�����û�����
	GateManager::getInstance()->Send(buffer->getBuffer(), head->length);
	KX_LOGDEBUG("���û���֤�ɹ� uid = %d, accounld=%d", head->id, loginSC.userId);
}

// ����ͻ��˵���Ϣ
void LoginService::processServiceS2S(int subcmd, int uid, char *buffer, int len, IKxComm *commun)
{
	switch (subcmd)
	{
	case SERVER_SUB_CMD::SERVER_SUB_OFFLINE:
		SERVER_SUB_OFFLINE(uid, buffer, len, commun);
		break;
	default:
		break;
	}
}

void LoginService::SERVER_SUB_OFFLINE(int uid, char *buffer, int len, IKxComm *commun)
{
	//��Ҷ��ߴ��� ��Session����
	GameUser* pGameUser = UserManager::getInstance()->getGameUser(uid);
	if (!pGameUser)
	{
		return;
	}
	KX_LOGDEBUG("�������! uid=%d, accounld=%d", uid, pGameUser->getUid());
	UserModel* pUserModel = dynamic_cast<UserModel*>(pGameUser->getModel(MODELTYPE_USER));
	int curTime  = KxBaseServer::getInstance()->getTimerManager()->getTimestamp();
	pUserModel->SetUserFieldVal(USR_FD_LOGINOUTTIME, curTime);
	//GameUserManager::getInstance()->RealremoveGameUser(uid);
}


