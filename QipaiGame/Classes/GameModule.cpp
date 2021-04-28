#include "GameModule.h"
#include "server/Head.h"
#include "GameNetworkNode.h"
#include "GameService.h"


GameModule::GameModule(void)
{
}

GameModule::~GameModule(void)
{
}

void GameModule::processLogic(char* buffer, unsigned int len, IKxComm *target)
{
    // 收到数据, 交由gameservice处理
    Head *head = reinterpret_cast<Head*>(buffer);
    int maincmd = head->MainCommand();
	int subcmd = head->SubCommand();

	GameService::processMessgFromServer(maincmd, subcmd, buffer);
}

void GameModule::processError(IKxComm *target)
{

}

void GameModule::processEvent(int eventId, IKxComm* target)
{
    GameNetworkNode* net = GameNetworkNode::getInstance();
    ServerConn* pServerConn = net->getServerConn();
    if (nullptr == pServerConn)
    {
        return;
    }

	bool success = eventId != KXEVENT_CONNECT_FAILE;
	if (pServerConn->ConnectCallback != nullptr)
	{
		pServerConn->ConnectCallback(success);
		pServerConn->ConnectCallback = nullptr;
	}
}
