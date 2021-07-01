#ifndef __LOGIN_SERVICE_H__
#define __LOGIN_SERVICE_H__

#include "CommInterfaces.h"

class LoginService
{
public:

	static void processServiceC2S(int subcmd, int uid, char *buffer, int len, IComm *commun);
	static void processServiceS2S(int subcmd, int uid, char *buffer, int len, IComm *commun);

	static void CMD_C2S_LOGIN(int uid, char *buffer, int len, IComm *commun);
	static void CMD_S2C_LOGIN(int uid, int UserNameID);
	static void CMD_S2C_NEW_USER_LOGIN(int uid, int userId);
	
	static void SERVER_SUB_OFFLINE(int uid, char *buffer, int len, IComm *commun);

private:
	static void pullUserData(int uId);
};

#endif //__LOGIN_SERVICE_H__
