#ifndef __SessionHelper_H__
#define __SessionHelper_H__

#include "ModuleConnect.h"

class SessionHelper
{
public:
	SessionHelper();
	~SessionHelper();
	static void ServerProcess(int subCmd, char* buffer);

	static void ServerSubInit(int userId);

private:

};

#endif