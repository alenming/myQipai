#include "SessionHelper.h"
#include "NetworkManager.h"
#include "SessionClienter.h"
#include "protocol/ServerProtocol.h"
#include "protocol/LoginProtocol.h"
#include "log/LogManager.h"

SessionHelper::SessionHelper()
{
}

SessionHelper::~SessionHelper()
{
}


void SessionHelper::ServerSubInit(int guesId, int userId)
{

	NetWorkManager *pNetWorkManager = NetWorkManager::getInstance();

	unsigned int offset = sizeof(Head);
	SessionClienter *pSessionClient = dynamic_cast<SessionClienter *>(pNetWorkManager->getGuest(guesId));
	// 连接断开/或者连接不存在
	if (pSessionClient == NULL)
	{
		return;
	}
	//验证逻辑
	bool isSuccessful = true;
	Head head;
	head.MakeCommand(CMD_MAIN::CMD_LOGIN_SERVER, LOGIN_SUB_CMD::CMD_C2S_RESIGIT);
	head.length = sizeof(head);
	head.id = pSessionClient->getUserId();

	if (!isSuccessful)
	{
		// 验证失败
		pSessionClient->sendData(reinterpret_cast<char*>(&head), sizeof(head));
		pSessionClient->clean();
	}
	else
	{
		pSessionClient->sendData(reinterpret_cast<char*>(&head), sizeof(head));
		pSessionClient->setUserId(userId);
		pSessionClient->setPermission(USER_PERMISSION::PERMISSION_USER);
		KX_LOGDEBUG("验证成功!,切换游客为正式玩家");
		// 验证成功，guest转user
		pNetWorkManager->changeGuestToUser(pSessionClient, userId);

	}
}