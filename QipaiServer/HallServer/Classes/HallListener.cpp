#include "HallListener.h"
#include "HallServer.h"
#include "GateManager.h"


HallListener::HallListener()
{
}

HallListener::~HallListener(void)
{
}

TCPClienter* HallListener::onAccept(COMMID client)
{
	TCPClienter* tcpClient = new TCPClienter();
	if (tcpClient->init(client))
	{
		HallServer::getInstance()->getMainPoller()->addCommObject(tcpClient, tcpClient->getPollType());
		//Ŀǰ����ֻ����һ��
		GateManager::getInstance()->SetGate(tcpClient);
		return tcpClient;
	}
	else
	{
		tcpClient->release();
		return nullptr;
	}
}
