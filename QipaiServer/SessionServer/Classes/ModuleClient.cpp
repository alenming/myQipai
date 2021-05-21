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
	
	// 直接转发给游戏服务器
	Head* head = reinterpret_cast<Head*>(buffer);
	int nMainCmd = head->MainCommand();
	int nSubCmd = head->SubCommand();

	if (nMainCmd == CMD_MAIN::CMD_HEARTBEAT && nSubCmd == CMD_MAIN::CMD_HEARTBEAT)
	{
		KX_LOGDEBUG("======================= 收到心跳包====================");
		Head head;
		head.MakeCommand(CMD_MAIN::CMD_HEARTBEAT, CMD_MAIN::CMD_HEARTBEAT);
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
		KX_LOGDEBUG("转发成功!");
	else
		KX_LOGDEBUG("转发失败!");
}

void ModuleClient::processError(IKxComm *target)
{
	this->userDisconnect(target);    // 玩家断线
}

// 1. 告诉所有服务器，玩家XXX掉线了
// 2. 从ClientManager中移除该玩家
void ModuleClient::userDisconnect(IKxComm *target)
{
    SessionClient *pClient = dynamic_cast<SessionClient*>(target);
    if (pClient == NULL)
        return;

    // 设置下线消息
    Head head;
	head.MakeCommand(SERVER_MAIN_CMD::SERVER_MAIN, SERVER_SUB_CMD::SERVER_SUB_OFFLINE);
    head.length = sizeof(head);

	if (pClient->getPermission() == 0)
		head.id = pClient->getGuestId();
	else if (pClient->getPermission() == 1)
		head.id = pClient->getUserId();

    // 发送数据到后端告知角色下线
    // 发送下线消息给后端指定的服务器
    pClient->sendDataToAllServer(reinterpret_cast<char*>(&head), sizeof(head));
    // 关闭socket、从NetWorkManager中和clean移除
	KX_LOGDEBUG("玩家掉线了!:%d", pClient->getUserId());
	pClient->clean();
	
}
