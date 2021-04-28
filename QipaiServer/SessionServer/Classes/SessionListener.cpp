#include "SessionListener.h"
#include "SessionClient.h"
#include "SessionServer.h"
#include "NetworkManager.h"
#include "LogManager.h"


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
    {
        // 连接时置为游客，并为其生成游客ID
        unsigned int guestId = pNetWorkManager->genGuestId();
        tcpClient->setGuestId(guestId);
        if (!pNetWorkManager->addGuest(guestId, tcpClient))
        {
            KX_LOGERROR("add guest error id %d", guestId);
            tcpClient->release();
            return NULL;
        }
		KX_LOGDEBUG("client connect to SessionServer successful!");
        SessionServer::getInstance()->getMainPoller()->addCommObject(tcpClient, tcpClient->getPollType());
        return tcpClient;
    }
    else
    {
        tcpClient->release();
        return NULL;
    }
}
