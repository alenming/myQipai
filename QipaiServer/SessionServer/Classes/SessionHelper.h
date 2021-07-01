#ifndef __SessionHelper_H__
#define __SessionHelper_H__

#include "ModuleConnect.h"

class SessionHelper
{
public:
	SessionHelper();
	~SessionHelper();

	//C->S
	static void ProcessClientToServer(char* buffer, unsigned int len, IComm *target);
	static void userDisconnect(IComm *target);
	//S->Session  S->Client
	static void ProcessServerToSession(char* buffer, unsigned int len, IComm *target);

private:
	static void ServerProcess(int subCmd, char* buffer);
	static void ServerSubInit(int uId, int userNameId);
	static void updateUserPermission(int uId, char* buffer);

};

#endif