#include <vector>
#include <iostream>
#include "TestClient.h"
#include "GameNetworkNode.h"
using namespace std;


int main()
{
	TestClient::getInstance()->onServerInit();
	TestClient::getInstance()->login();
	auto clientPoll = CGameNetworkNode::getInstance()->getPoller();
	while (true)
	{
		clientPoll->poll();
	}

	LogManager::destroy();
	TestClient::destroy();
	return 0;
}
