#include "SessionListener.h"
#include "SessionClienter.h"
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
    SessionClienter* tcpClient = new SessionClienter();
    if (tcpClient->init(client))
    {
		KXCOMMID guesID =tcpClient->getSock()->getSockFd();
		tcpClient->setUserId(guesID);
		if (!pNetWorkManager->addUser(guesID, tcpClient))
        {
			KX_LOGERROR("add guest error id %d", guesID);
            tcpClient->release();
            return NULL;
        }
		KX_LOGDEBUG("客户端连接成功!->SessionListener::onAccept");
        SessionServer::getInstance()->getMainPoller()->addCommObject(tcpClient, tcpClient->getPollType());
        return tcpClient;
    }
    else
    {
        tcpClient->release();
        return NULL;
    }
}
