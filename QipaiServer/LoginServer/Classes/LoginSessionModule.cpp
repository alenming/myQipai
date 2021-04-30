#include "LoginSessionModule.h"
#include "GateManager.h"
#include "LoginService.h"
#include "helper/BufferData.h"
#include "server/Head.h"

#include "server/ServerProtocol.h"
#include "protocol/LoginProtocol.h"

LoginSessionModule::LoginSessionModule(void)
{
}

LoginSessionModule::~LoginSessionModule(void)
{
}

void LoginSessionModule::processLogic(char* buffer, unsigned int len, IKxComm *target)
{
	Head* head = reinterpret_cast<Head*>(buffer);
	int nMainCmd = head->MainCommand();
	int nSubCmd = head->SubCommand();
	int uid = head->uid;
	int length = head->length;

	switch (nMainCmd)
	{
	case CMD_MAIN::CMD_LOGIN_SERVER:
		{
			LoginService::processServiceC2S(nSubCmd, uid, buffer, length, target);
			break;
		}
		case SERVER_MAIN_CMD::SERVER_MAIN:
		{
			LoginService::processServiceS2S(nSubCmd, uid, buffer, length, target);
			break;
		}
		default:
			break;
	}

}

int LoginSessionModule::processLength(char* buffer, unsigned int len)
{
	if (len < sizeof(Head))
	{
		return sizeof(Head);
	}

	// 返回一个包的长度, 由包头解析, 解析的长度包括包头
	return reinterpret_cast<Head*>(buffer)->length;
}

void LoginSessionModule::processError(IKxComm *target)
{

}
