#include "ClientModule.h"
#include "SessionClient.h"
#include "SessionServer.h"
#include "NetworkManager.h"
#include "protocol/MainProtocol.h"
#include "protocol/LoginProtocol.h"

ClientModule::ClientModule(void)
{
}

ClientModule::~ClientModule(void)
{
}

void ClientModule::processLogic(char* buffer, unsigned int len, IKxComm *target)
{
    SessionClient* pClient = dynamic_cast<SessionClient*>(target);
	ServerConfig * pServerConfig = SessionServer::getInstance()->getServerCoinfig();

	// 直接转发给游戏服务器
	Head* head = reinterpret_cast<Head*>(buffer);
	int nMainCmd = head->MainCommand();
	int nSubCmd = head->SubCommand();
	head->uid = pClient->getGuestId();//服务器之间通讯用玩家ID

	KX_LOGDEBUG("ClientModule onRecy Message!");
	if (nMainCmd ==  MAIN_CMD::CMD_HEARTBEART && nSubCmd == MAIN_CMD::CMD_HEARTBEART)
	{
		KX_LOGDEBUG("======================= heartbeart!====================");
		Head head;
		head.MakeCommand(MAIN_CMD::CMD_HEARTBEART, MAIN_CMD::CMD_HEARTBEART);
		head.length = sizeof(head);
		head.uid = -1;

		pClient->sendData(reinterpret_cast<char*>(&head), sizeof(head));
		return;
	}

	pClient->sendDataToServer(nMainCmd, nSubCmd, buffer, len);
}

void ClientModule::processError(IKxComm *target)
{
    // 玩家断线
    this->userDisconnect(target);
}

// 1. 告诉所有服务器，玩家XXX掉线了
// 2. 从ClientManager中移除该玩家
void ClientModule::userDisconnect(IKxComm *target)
{
    SessionClient *pClient = dynamic_cast<SessionClient*>(target);
    if (pClient == NULL)
    {
        return;
    }

    // 设置下线消息
    Head head;
	head.MakeCommand(SERVER_MAIN_CMD::SERVER_MAIN, SERVER_SUB_CMD::SERVER_SUB_OFFLINE);
    head.length = sizeof(head);
    head.uid = pClient->getGuestId();

    // 发送数据到后端告知角色下线
    // 发送下线消息给后端指定的服务器
    pClient->sendDataToAllServer(reinterpret_cast<char*>(&head), sizeof(head));
    // 关闭socket、从NetWorkManager中和clean移除
	KX_LOGDEBUG("client is loginout:%d", pClient->getGuestId());
	pClient->clean();
	
}
