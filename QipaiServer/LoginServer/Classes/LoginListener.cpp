#include "LoginListener.h"
#include "LoginServer.h"
#include "GateManager.h"


LoginListener::LoginListener()
{
}

LoginListener::~LoginListener(void)
{
}

KxTCPClienter* LoginListener::onAccept(KXCOMMID client)
{
	KxTCPClienter* tcpClient = new KxTCPClienter();
	if (tcpClient->init(client))
	{

		LoginServer::getInstance()->getMainPoller(
			)->addCommObject(tcpClient, tcpClient->getPollType());

		//Ŀǰ����ֻ����һ��
		GateManager::getInstance()->SetGate(tcpClient);

		return tcpClient;
	}
	else
	{
		tcpClient->release();
		return NULL;
	}
}
