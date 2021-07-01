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

	// 直接转发给游戏服务器
	Head* head = reinterpret_cast<Head*>(buffer);
	int nMainCmd = head->MainCommand();
	int nSubCmd = head->SubCommand();

	if (nMainCmd == MAIN_CMD::CMD_HEARTBEAT && nSubCmd == MAIN_CMD::CMD_HEARTBEAT)
	{
		LOGDEBUG("======================= 收到心跳包====================");
		Head head;
		head.MakeCommand(MAIN_CMD::CMD_HEARTBEAT, MAIN_CMD::CMD_HEARTBEAT);
		head.length = sizeof(head);
		head.id = -1;
		pClient->sendData(reinterpret_cast<char*>(&head), sizeof(head));
		return;
	}
	//第一次消息来,帐号没验证.用的是guesId,登录成功后会转为userId
	if (pClient->getPermission() == 0)
		head->id = pClient->getGuestId();
	else if (pClient->getPermission() == 1)
		head->id = pClient->getUserId();

	if (pClient->sendDataToServer(nMainCmd, nSubCmd, buffer, len))
		LOGDEBUG("转发成功!");
	else
		LOGDEBUG("转发失败!");
}

void SessionHelper::userDisconnect(IComm *target)
{
	// 1. 告诉所有服务器，玩家XXX掉线了  2. 从ClientManager中移除该玩家
	SessionClient *pClient = dynamic_cast<SessionClient*>(target);
	if (pClient == nullptr)
		return;
	// 设置下线消息
	Head head;
	head.MakeCommand(SERVER_MAIN_CMD::SERVER_MAIN, SERVER_SUB_CMD::SERVER_SUB_OFFLINE);
	head.length = sizeof(head);

	if (pClient->getPermission() == 0)
		head.id = pClient->getGuestId();
	else if (pClient->getPermission() == 1)
		head.id = pClient->getUserId();

	// 发送数据到后端告知角色下线 发送下线消息给后端指定的服务器
	pClient->sendDataToAllServer(reinterpret_cast<char*>(&head), sizeof(head));
	// 关闭socket、从NetWorkManager中和clean移除
	LOGDEBUG("玩家掉线了!:%d", head.id);
	pClient->clean();
}


/*-----------------------------------------------------------------------------------------------------*/

void SessionHelper::ProcessServerToSession(char* buffer, unsigned int len, IComm *target)
{
	// 发给指定的前端
	Head* head = reinterpret_cast<Head*>(buffer);
	int nMainCmd = head->MainCommand();
	int nSubCmd = head->SubCommand();
	int uId = head->id;

	switch (nMainCmd)
	{
		case MAIN_CMD::CMD_LOGIN_SERVER: //转发消息给前端
		{
			SessionClient* pClient = reinterpret_cast<SessionClient*>(NetManager::getInstance()->getUser(uId));
			if (!pClient)		return;
			LOGDEBUG("接收到服务器消息,转发给前端!ConnectModule::processLogic uId:%d", uId);
			pClient->sendData(buffer, head->length);
			break;
		}
		case SERVER_MAIN_CMD::SERVER_MAIN: //处理服务器来的消息
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
		LOGDEBUG("找不到玩家%d", uId);
		return;
	}
	pSessionClient->setUserId(userNameId);
	pSessionClient->setPermission(1);
	LOGDEBUG("切换权限");
	NetManager::getInstance()->changeGuestToUser(pSessionClient, userNameId);	// 验证成功，guest转user


	BufferData* buffer1 = newBufferData(MAIN_CMD::CMD_LOGIN_SERVER, SUB_CMD_LOGIN::CMD_S2C_LOGIN);
	LOGIN_DATA da = LOGIN_DATA();
	memcpy(da.userName, loginData->userName, sizeof(da.userName));
	memcpy(da.passWord, loginData->passWord, sizeof(da.passWord));
	buffer1->writeData(da);
	char* bu = buffer1->getBuffer();
	Head* head = reinterpret_cast<Head*>(bu);
	head->length = buffer1->getDataLength();
	head->id = userNameId;
	//发送用户数据
	if (pSessionClient != NULL)
		pSessionClient->sendData(bu, head->length);
}

void SessionHelper::ServerSubInit(int uId, int userNameId)
{
	NetManager *pNetWorkManager = NetManager::getInstance();
	SessionClient *pSessionClient = dynamic_cast<SessionClient *>(pNetWorkManager->getGuest(uId));
	// 连接断开/或者连接不存在
	if (!pSessionClient)
	{
		LOGDEBUG("找不到玩家%d", uId);
		return;
	}
	//验证逻辑
	bool isSuccessful = true;
	BufferData* buffer1 = newBufferData(MAIN_CMD::CMD_LOGIN_SERVER, SUB_CMD_LOGIN::CMD_S2C_PERMISSION);
	char* bu = buffer1->getBuffer();
	Head* head = reinterpret_cast<Head*>(bu);
	head->length = buffer1->getDataLength();

	if (!isSuccessful)		// 验证失败
	{
		LOGDEBUG("验证失败!");
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
		LOGDEBUG("验证成功!,切换游客为正式玩家");
		pNetWorkManager->changeGuestToUser(pSessionClient, userNameId);		// 验证成功，guest转user
	}
}


