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
        // ����ʱ��Ϊ�οͣ���Ϊ�������ο�ID
        unsigned int guestId = pNetWorkManager->genGuestId();
        tcpClient->setGuestId(guestId);
        if (!pNetWorkManager->addGuest(guestId, tcpClient))
        {
            KX_LOGERROR("add guest error id %d", guestId);
            tcpClient->release();
            return NULL;
        }
		KX_LOGDEBUG("�ͻ������ӳɹ�!->SessionListener::onAccept");
        SessionServer::getInstance()->getMainPoller()->addCommObject(tcpClient, tcpClient->getPollType());
        return tcpClient;
    }
    else
    {
        tcpClient->release();
        return NULL;
    }
}
