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
	SessionHelper::ProcessClientToServer(buffer, len, target); //ת��ǰ����Ϣ
}

void ModuleClient::processError(IComm *target)
{
	SessionHelper::userDisconnect(target);    // ��Ҷ���
}
