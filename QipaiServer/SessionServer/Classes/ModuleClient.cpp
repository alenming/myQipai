#include "ModuleClient.h"
#include "SessionHelper.h"


ModuleClient::ModuleClient(void)
{
}

ModuleClient::~ModuleClient(void)
{
}

void ModuleClient::processLogic(char* buffer, unsigned int len, IComm *target)
{
	SessionHelper::ProcessClientToServer(buffer, len, target); //转发前端消息
}

void ModuleClient::processError(IComm *target)
{
	SessionHelper::userDisconnect(target);    // 玩家断线
}
