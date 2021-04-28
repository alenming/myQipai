/*
	游戏协议处理
*/
#ifndef __LOGIN_SERVICE_H__
#define __LOGIN_SERVICE_H__

#include "commnication/KxCommInterfaces.h"

class GameService
{
public:

	static void sendMessgToServer(int mainCmd, int subcmd);
	static void processMessgFromServer(int mainCmd, int subcmd, char* buffer);
	

	static void CMD_C2S_LOGIN();
	static void CMD_C2S_REGISTER();


	static void CMD_S2C_LOGIN(int subCmd, char* buffer);
	static void CMD_S2C_REGISTER(int subCmd, char* buffer);

};

#endif //__LOGIN_SERVICE_H__
