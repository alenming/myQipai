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
	//LogManager::getInstance()->setShowTime(false);
	//LogManager::getInstance()->setShowDate(false);
	//LogManager::getInstance()->addHandler(1, new LogConsoleHandler());
	//LogFileHandler* pFileHandle = new LogFileHandler();
	//pFileHandle->setFilePath("../bin/");
	//pFileHandle->setNewFile(true);
	//pFileHandle->setFileName("TestClientOut");
	//pFileHandle->setFastModel(false);
	//LogManager::getInstance()->addHandler(2, pFileHandle);

	//regiestLuaFunction();
	//LuaStack_::getInstance()->executeScriptFile("../LuaSupport/Test/Test.lua");
	LogManager::destroy();
	TestClient::destroy();
	return 0;
}