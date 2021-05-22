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
void LoginService::processServiceC2S(int subcmd, int uid, char *buffer, int len, IComm *commun)
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


void LoginService::CMD_C2S_LOGIN(int uid, char *buffer, int len, IComm *commun)
{
	Head* head = reinterpret_cast<Head*>(buffer);

	if (head->id != uid || head->id < 0) return;

	LOGIN_DATA *loginCS = reinterpret_cast<LOGIN_DATA*>(head->data());

	const std::string temp = loginCS->userName;

	int userNameId = atoi(loginCS->userName);
	// ���CGameUser
	GameUser* pGameUser = UserManager::getInstance()->getGameUser(userNameId, loginCS->userName, loginCS->passWord);
	if (nullptr == pGameUser)
	{
		//���û�
		LOGDEBUG("���û�!");
		pGameUser = UserManager::getInstance()->newGameUser(userNameId, loginCS->userName, loginCS->passWord);
		if (pGameUser)
			CMD_S2C_NEW_USER_LOGIN(uid, userNameId);
	}
	else
	{
		// ����������û������ߺ����һ��ʱ�����Զ��Ƴ�
		// �÷������޳��Ƴ��б����ݣ��������Զ��ͷţ���Ϊ�Һ������ֻ�����
		LOGDEBUG("���û�!");
		UserManager::getInstance()->donotDeleteUser(userNameId);
		pGameUser->refreshModel(MODELTYPE_USER);
		CMD_S2C_LOGIN(userNameId);
	}

	//���µ�¼ʱ��
	int nCurTime = BaseServer::getInstance()->getCurrentTime();
	pGameUser->updateData(MODELTYPE_USER, USR_FD_LOGINTIME, nCurTime);
}

void LoginService::CMD_S2C_NEW_USER_LOGIN(int uid, int userId)
{
	// ��ʼ�·�����
	LOGIN_DATA loginSC;

	BufferData* buffer = newBufferData(SERVER_MAIN_CMD::SERVER_MAIN, SERVER_SUB_CMD::SERVER_SUB_NEW_USER);

	GameUser* pGameUser = UserManager::getInstance()->getGameUser(userId);
	if (!pGameUser) return;

	std::string nameTmep = pGameUser->getUserName();
	memcpy(loginSC.userName, nameTmep.c_str(), sizeof(loginSC.userName));

	std::string passTemp = pGameUser->getPassWord();
	memcpy(loginSC.passWord, passTemp.c_str(), sizeof(loginSC.passWord));

	buffer->writeData(loginSC);

	//����д�����ݳ���
	char* bu = buffer->getBuffer();
	Head* head = reinterpret_cast<Head*>(bu);
	head->id = uid;//����guesId
	head->length = buffer->getDataLength();

	//�����û�����
	GateManager::getInstance()->Send(buffer->getBuffer(), head->length);
	LOGDEBUG("���û���֤�ɹ� uid = %d", head->id);
}

void LoginService::CMD_S2C_LOGIN(int uid)
{
	// ��ʼ�·�����
	LOGIN_DATA loginSC;

	BufferData* buffer = newBufferData(SERVER_MAIN_CMD::SERVER_MAIN, SERVER_SUB_CMD::SERVER_SUB_UPDATE_PER);

	GameUser* pGameUser = UserManager::getInstance()->getGameUser(uid);
	if (!pGameUser) return;

	string userName = pGameUser->getUserName();
	string passWord = pGameUser->getPassWord();
	memcpy(loginSC.userName, userName.c_str(), sizeof(loginSC.userName));
	memcpy(loginSC.passWord, passWord.c_str(), sizeof(loginSC.passWord));

	buffer->writeData(loginSC);

	//����д�����ݳ���
	char* bu = buffer->getBuffer();
	Head* head = reinterpret_cast<Head*>(bu);
	head->id = uid;
	head->length = buffer->getDataLength();

	//�����û�����
	GateManager::getInstance()->Send(buffer->getBuffer(), head->length);
	LOGDEBUG("��¼�ɹ�! uid = %d", head->id);
}









// �������˵���Ϣ
void LoginService::processServiceS2S(int subcmd, int uid, char *buffer, int len, IComm *commun)
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

void LoginService::SERVER_SUB_OFFLINE(int uid, char *buffer, int len, IComm *commun)
{
	//��Ҷ��ߴ��� ��Session����
	GameUser* pGameUser = UserManager::getInstance()->getGameUser(uid);
	if (!pGameUser)
	{
		return;
	}
	LOGDEBUG("�������! uid=%d", uid, pGameUser->getUid());

	int curTime = BaseServer::getInstance()->getCurrentTime();
	pGameUser->updateData(MODELTYPE_USER, USR_FD_LOGINOUTTIME, curTime);

	UserManager::getInstance()->RealremoveGameUser(uid);
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
