#include "SessionHelper.h"
#include "NetworkManager.h"
#include "SessionClient.h"
#include "BufferData.h"
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

void SessionHelper::updateUserPermission(int uId, char* buffer1)
{
	Head* head1 = reinterpret_cast<Head*>(buffer1);
	LOGIN_DATA *loginData = reinterpret_cast<LOGIN_DATA*>(head1->data());
	int userNameId = atoi(loginData->userName);

	SessionClient *pSessionClient = dynamic_cast<SessionClient *>(NetWorkManager::getInstance()->getGuest(uId));
	pSessionClient->setUserId(userNameId);
	pSessionClient->setPermission(1);
	LOGDEBUG("切换");
	NetWorkManager::getInstance()->changeGuestToUser(pSessionClient, userNameId);	// 验证成功，guest转user

	LOGIN_DATA loginSC;
	BufferData* buffer = newBufferData(CMD_MAIN::CMD_LOGIN_SERVER, LOGIN_SUB_CMD::CMD_S2C_LOGIN);
	memcpy(loginSC.userName, loginData->userName, sizeof(loginSC.userName));
	memcpy(loginSC.passWord, loginData->passWord, sizeof(loginSC.passWord));
	buffer->writeData(loginSC);
	//重新写入数据长度
	char* bu = buffer->getBuffer();
	Head* head = reinterpret_cast<Head*>(bu);
	head->id = userNameId;
	head->length = buffer->getDataLength();

	//发送用户数据
	if (pSessionClient)
		pSessionClient->sendData(reinterpret_cast<char*>(&head), sizeof(head));
}
void SessionHelper::ServerSubInit(int uId, int userNameId)
{
	NetWorkManager *pNetWorkManager = NetWorkManager::getInstance();
	SessionClient *pSessionClient = dynamic_cast<SessionClient *>(pNetWorkManager->getGuest(uId));
	// 连接断开/或者连接不存在
	if (pSessionClient == nullptr)	return;
	//验证逻辑
	bool isSuccessful = true;
	BufferData* buffer1 = newBufferData(CMD_MAIN::CMD_LOGIN_SERVER, LOGIN_SUB_CMD::CMD_S2C_PERMISSION);
	char* bu = buffer1->getBuffer();
	Head* head = reinterpret_cast<Head*>(bu);
	//head->id = userId;//还是guesId
	head->length = buffer1->getDataLength();
	if (!isSuccessful)		// 验证失败
	{
		LOGDEBUG("验证失败!");
		head->id = uId;
		pSessionClient->sendData(reinterpret_cast<char*>(&head), sizeof(head));
		pSessionClient->clean();
	}
	else
	{
		head->id = userNameId;
		pSessionClient->sendData(reinterpret_cast<char*>(&head), sizeof(head));
		pSessionClient->setUserId(userNameId);
		pSessionClient->setPermission(1);
		LOGDEBUG("验证成功!,切换游客为正式玩家");
		// 验证成功，guest转user
		pNetWorkManager->changeGuestToUser(pSessionClient, userNameId);
	}
}


