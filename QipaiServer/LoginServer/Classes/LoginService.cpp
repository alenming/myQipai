#include "LoginService.h"
#include "UserManager.h"
#include "GateManager.h"
#include "BufferData.h"
#include "BaseServer.h"
#include "ServerProtocol.h"
#include "LoginProtocol.h"
#include "log/LogManager.h"


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
	GameUser* pGameUser = UserManager::getInstance()->getGameUser(loginCS->userId, loginCS->passWord);
	if (nullptr == pGameUser)
	{
		//���û�
		KX_LOGDEBUG("���û�!");
		pGameUser = UserManager::getInstance()->newGameUser(loginCS->userId, loginCS->passWord);
		CMD_S2C_NEW_USER_LOGIN(uid);
	}
	else
	{
		// ����������û������ߺ����һ��ʱ�����Զ��Ƴ�
		// �÷������޳��Ƴ��б����ݣ��������Զ��ͷţ���Ϊ�Һ������ֻ�����
		KX_LOGDEBUG("���û�!");
		UserManager::getInstance()->donotDeleteUser(loginCS->userId);
		pGameUser->refreshModel(MODELTYPE_USER);
		CMD_S2C_LOGIN(loginCS->userId);
	}
}

void LoginService::CMD_S2C_LOGIN(int uid)
{
	// ��ʼ�·�����
	LOGIN_DATA loginSC;

	BufferData* buffer = newBufferData(CMD_MAIN::CMD_LOGIN_SERVER, LOGIN_SUB_CMD::CMD_S2C_LOGIN);

	GameUser* pGameUser = UserManager::getInstance()->getGameUser(uid);

	string passWord = pGameUser->getPassWord();
	memcpy(loginSC.passWord, passWord.c_str(),passWord.size());
	buffer->writeData(loginSC);

	//����д�����ݳ���
	char* bu = buffer->getBuffer();
	Head* head = reinterpret_cast<Head*>(bu);
	head->id = uid;
	head->length = buffer->getDataLength();


	//�����û�����
	GateManager::getInstance()->Send(buffer->getBuffer(), head->length);
	KX_LOGDEBUG("��¼�ɹ�! uid = %d", head->id);
}

void LoginService::CMD_S2C_NEW_USER_LOGIN(int uid)
{
	// ��ʼ�·�����
	LOGIN_DATA loginSC;

	BufferData* buffer = newBufferData(SERVER_MAIN_CMD::SERVER_MAIN, SERVER_SUB_CMD::SERVER_SUB_NEW_USER);


	GameUser* pGameUser = UserManager::getInstance()->getGameUser(uid);

	loginSC.userId = pGameUser->getUid();
	char* pass = pGameUser->getPassWord();
	memcpy(loginSC.passWord, pass, sizeof(loginSC.passWord));


	buffer->writeData(loginSC);


	//����д�����ݳ���
	char* bu = buffer->getBuffer();
	Head* head = reinterpret_cast<Head*>(bu);
	head->id = uid;
	head->length = buffer->getDataLength();

	//�����û�����
	GateManager::getInstance()->Send(buffer->getBuffer(), head->length);
	KX_LOGDEBUG("���û���֤�ɹ� uid = %d", head->id);
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
	int curTime  = BaseServer::getInstance()->getTimerManager()->getTimestamp();
	pUserModel->SetUserFieldVal(USR_FD_LOGINOUTTIME, curTime);
	//GameUserManager::getInstance()->RealremoveGameUser(uid);

}



void sendUserModel(int uid, UserModel *model)
{
	int len = sizeof(LoginUserModelInfo);
	LoginUserModelInfo *userInfo = reinterpret_cast<LoginUserModelInfo*>(new char[len]);
	int nValue = 0;
	memset(userInfo->name, 0, sizeof(userInfo->name));
	strcpy(userInfo->name, model->GetName().c_str());
	model->GetUserFieldVal(USR_FD_USERID, userInfo->userId);
	model->GetUserFieldVal(USR_FD_USERLV, nValue);
	userInfo->cuserLv = nValue;
	model->GetUserFieldVal(USR_FD_EXP, userInfo->userExp);
	model->GetUserFieldVal(USR_FD_GOLD, userInfo->gold);
	model->GetUserFieldVal(USR_FD_DIAMOID, userInfo->diamond);

	//GateManager::getInstance()->sendData(uid, CMD_LOGIN, CMD_LOGIN_USERMODEL_SC,reinterpret_cast<char *>(userInfo), len);
	delete[](char*)userInfo;

}
