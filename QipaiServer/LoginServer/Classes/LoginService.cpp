#include "LoginService.h"
#include "userdata/GameUserManager.h"
#include "GateManager.h"

#include "helper/BufferData.h"
#include "server/Head.h"

#include "server/ServerProtocol.h"
#include "protocol/LoginProtocol.h"


using namespace std;

// 处理客户端的消息
void LoginService::processServiceC2S(int subcmd, int uid, char *buffer, int len, IKxComm *commun)
{
	switch (subcmd)
	{
	case LOGIN_SUB_CMD::CMD_C2S_REGISTER:
	{
		CMD_C2S_REGISTER(uid, buffer, len, commun);
	}
	case LOGIN_SUB_CMD::CMD_C2S_LOGIN:
		CMD_C2S_LOGIN(uid, buffer, len, commun);
		break;
	default:
		break;
	}
}

void LoginService::CMD_C2S_REGISTER(int uid, char *buffer, int len, IKxComm *commun)
{
	Head* head = reinterpret_cast<Head*>(buffer);

	if (head->uid != uid || head->uid < 0)
	{
		return;
	}

	REGISTER_DATA *loginCS = reinterpret_cast<REGISTER_DATA*>(head->data());
	GameUser* pGameUser = GameUserManager::getInstance()->newGameUser(uid, loginCS->accountId);

	CMD_S2C_REGISTER(uid);
}

void LoginService::CMD_S2C_REGISTER(int uid)
{


}

void LoginService::CMD_C2S_LOGIN(int uid, char *buffer, int len, IKxComm *commun)
{
	Head* head = reinterpret_cast<Head*>(buffer);

	if (head->uid != uid || head->uid < 0)
	{
		return;
	}

	LOGIN_DATA *loginCS = reinterpret_cast<LOGIN_DATA*>(head->data());

	// 获得CGameUser
	GameUser* pGameUser = GameUserManager::getInstance()->getGameUser(uid);
	if (NULL == pGameUser)
	{
		//新用户
		pGameUser = GameUserManager::getInstance()->newGameUser(uid, loginCS->accountId);
	}
	else
	{
		// 如果不是新用户，断线后会在一段时间内自动移除
		// 该方法会剔除移除列表数据，不让它自动释放，因为我胡汉三又回来了
		GameUserManager::getInstance()->donotDeleteUser(uid);
		pGameUser->refreshModel(MODELTYPE_USER);
	}

	int nCurTime = KxBaseServer::getInstance()->getTimerManager()->getTimestamp();
	//UserModel *pUserModel = dynamic_cast<UserModel*>(pGameUser->getModel(MODELTYPE_USER));


	CMD_S2C_LOGIN(uid);
}

void LoginService::CMD_S2C_LOGIN(int uid)
{
	// 开始下发数据
	LOGIN_DATA loginSC;

	BufferData* buffer = newBufferData(CMD_MAIN::CMD_LOGIN_SERVER, LOGIN_SUB_CMD::CMD_S2C_LOGIN);

	GameUser* pGameUser = GameUserManager::getInstance()->getGameUser(uid);
	UserModel* pUserModel = dynamic_cast<UserModel*>(pGameUser->getModel(MODELTYPE_USER));

	loginSC.accountId = pGameUser->getAccountId();
	buffer->writeData(loginSC);

	//重新写入数据长度
	char* bu = buffer->getBuffer();
	Head* head = reinterpret_cast<Head*>(bu);
	head->uid = uid;
	head->length = buffer->getDataLength();

	//发送用户数据
	GateManager::getInstance()->Send(buffer->getBuffer(), head->length);
	KX_LOGDEBUG("LoginFinish uid = %d", head->uid);
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
	KX_LOGDEBUG("SERVER_SUB_OFFLINE uid=%d", uid);

}

void LoginService::processUserReconect(int uid, char *buffer, int len, IKxComm *commun)
{
	// 获得CGameUser
	GameUser* pGameUser = GameUserManager::getInstance()->getGameUser(uid);
	if (NULL == pGameUser)
	{
		return;
	}

	//重置数据
	GameUserManager::getInstance()->reSetGameUserData(uid);
	// 如果不是新用户，断线后会在一段时间内自动移除
	// 改方法会剔除移除列表数据，不让它自动释放，因为我胡汉三又回来了
	GameUserManager::getInstance()->donotDeleteUser(uid);
}


