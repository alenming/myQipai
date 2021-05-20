#include "LoginService.h"
#include "userdata/UserManager.h"
#include "GateManager.h"
#include "helper/BufferData.h"
#include "server/Head.h"
#include "protocol/ServerProtocol.h"
#include "protocol/LoginProtocol.h"


using namespace std;

// 处理客户端的消息
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

	// 获得CGameUser
	GameUser* pGameUser = UserManager::getInstance()->getGameUser(loginCS->userId);
	if (NULL == pGameUser)
	{
		//新用户
		KX_LOGDEBUG("新用户!");
		pGameUser = UserManager::getInstance()->newGameUser(loginCS->userId, loginCS->passWord);
		CMD_S2C_NEW_USER_LOGIN(uid, loginCS->userId);
	}
	else
	{
		// 如果不是新用户，断线后会在一段时间内自动移除
		// 该方法会剔除移除列表数据，不让它自动释放，因为我胡汉三又回来了
		KX_LOGDEBUG("老用户!");
		UserManager::getInstance()->donotDeleteUser(uid);
		pGameUser->refreshModel(MODELTYPE_USER);
		CMD_S2C_LOGIN(uid, loginCS->userId);
	}
}

void LoginService::CMD_S2C_LOGIN(int uid, int accountId)
{
	// 开始下发数据
	LOGIN_DATA loginSC;

	BufferData* buffer = newBufferData(CMD_MAIN::CMD_LOGIN_SERVER, LOGIN_SUB_CMD::CMD_S2C_LOGIN);

	GameUser* pGameUser = UserManager::getInstance()->getGameUser(uid);

	loginSC.userId = accountId;
	buffer->writeData(loginSC);

	//重新写入数据长度
	char* bu = buffer->getBuffer();
	Head* head = reinterpret_cast<Head*>(bu);
	head->id = uid;
	head->length = buffer->getDataLength();

	//发送用户数据
	GateManager::getInstance()->Send(buffer->getBuffer(), head->length);
	KX_LOGDEBUG("登录成功! uid = %d, accounld=%d", head->id, loginSC.userId);
}

void LoginService::CMD_S2C_NEW_USER_LOGIN(int uid, int accountId)
{
	// 开始下发数据
	LOGIN_DATA loginSC;

	BufferData* buffer = newBufferData(SERVER_MAIN_CMD::SERVER_MAIN, SERVER_SUB_CMD::SERVER_SUB_NEW_USER);

	GameUser* pGameUser = UserManager::getInstance()->getGameUser(uid);

	loginSC.userId = accountId;
	buffer->writeData(loginSC);

	//重新写入数据长度
	char* bu = buffer->getBuffer();
	Head* head = reinterpret_cast<Head*>(bu);
	head->id = uid;
	head->length = buffer->getDataLength();

	//发送用户数据
	GateManager::getInstance()->Send(buffer->getBuffer(), head->length);
	KX_LOGDEBUG("新用户验证成功 uid = %d, accounld=%d", head->id, loginSC.userId);
}

// 处理客户端的消息
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
	//玩家断线处理 由Session触发
	GameUser* pGameUser = UserManager::getInstance()->getGameUser(uid);
	if (!pGameUser)
	{
		return;
	}
	KX_LOGDEBUG("玩家离线! uid=%d, accounld=%d", uid, pGameUser->getUid());
	UserModel* pUserModel = dynamic_cast<UserModel*>(pGameUser->getModel(MODELTYPE_USER));
	int curTime  = KxBaseServer::getInstance()->getTimerManager()->getTimestamp();
	pUserModel->SetUserFieldVal(USR_FD_LOGINOUTTIME, curTime);
	//GameUserManager::getInstance()->RealremoveGameUser(uid);
}


