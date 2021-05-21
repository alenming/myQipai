#include "GameModule.h"
#include "GameNetworkNode.h"
#include "Head.h"

GameModule::GameModule(void)
{
}

GameModule::~GameModule(void)
{
}

void GameModule::processLogic(char* buffer, unsigned int len, IComm *target)
{
	Head* head = reinterpret_cast<Head*>(buffer);
	int nMainCmd = head->MainCommand();
	int nSubCmd = head->SubCommand();
	int id = head->id;
	//LOGIN_DATA* a = reinterpret_cast<LOGIN_DATA*>(head->data());
}

void GameModule::processError(IComm *target)
{

}

void GameModule::processEvent(int eventId, IComm* target)
{

}
