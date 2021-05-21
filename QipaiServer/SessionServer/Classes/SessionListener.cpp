#include "SessionListener.h"
#include "SessionClient.h"
#include "SessionServer.h"
#include "NetworkManager.h"
#include "log/LogManager.h"


SessionListener::SessionListener()
{
}

SessionListener::~SessionListener(void)
{
}

KxTCPClienter* SessionListener::onAccept(KXCOMMID client)
{
	NetWorkManager *pNetWorkManager = NetWorkManager::getInstance();
    SessionClient* tcpClient = new SessionClient();
    if (tcpClient->init(client))
    {//刚建立连接,C端还没有发登录消息来验证帐号数据等
		unsigned int guesId = pNetWorkManager->genGuestId();
		tcpClient->setGuestId(guesId);
		if (!pNetWorkManager->addGuest(guesId, tcpClient))
        {
			KX_LOGERROR("add guesId error id %d", guesId);
            tcpClient->release();
            return nullptr;
        }
		KX_LOGDEBUG("客户端连接成功!->SessionListener::onAccept");
        SessionServer::getInstance()->getMainPoller()->addCommObject(tcpClient, tcpClient->getPollType());
        return tcpClient;
    }
    else
    {
        tcpClient->release();
        return nullptr;
    }
}
