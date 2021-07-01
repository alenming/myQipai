#include "SessionListener.h"
#include "SessionClient.h"
#include "SessionServer.h"
#include "NetManager.h"
#include "log/LogManager.h"


SessionListener::SessionListener()
{
}

SessionListener::~SessionListener(void)
{
}

TCPClienter* SessionListener::onAccept(COMMID client)
{
	NetManager *pNetWorkManager = NetManager::getInstance();
    SessionClient* tcpClient = new SessionClient();
    if (tcpClient->init(client))
    {//�ս�������,C�˻�û�з���¼��Ϣ����֤�ʺ����ݵ�
		unsigned int guesId = pNetWorkManager->genGuestId();
		tcpClient->setGuestId(guesId);
		if (!pNetWorkManager->addGuest(guesId, tcpClient))
        {
			LOGERROR("add guesId error id %d", guesId);
            tcpClient->release();
            return nullptr;
        }
		LOGDEBUG("�ͻ������ӳɹ�!->SessionListener::onAccept");
        SessionServer::getInstance()->getMainPoller()->addCommObject(tcpClient, tcpClient->getPollType());
        return tcpClient;
    }
    else
    {
        tcpClient->release();
        return nullptr;
    }
}
