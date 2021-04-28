#include "GameModule.h"
#include "GameNetworkNode.h"
#include "server/Head.h"

GameModule::GameModule(void)
{
}

GameModule::~GameModule(void)
{
}

void GameModule::processLogic(char* buffer, unsigned int len, IKxComm *target)
{
	Head* head = reinterpret_cast<Head*>(buffer);
	int nMainCmd = head->MainCommand();
	int nSubCmd = head->SubCommand();
	int id = head->uid;
	//LOGIN_DATA* a = reinterpret_cast<LOGIN_DATA*>(head->data());
}

void GameModule::processError(IKxComm *target)
{

}

void GameModule::processEvent(int eventId, IKxComm* target)
{

}
