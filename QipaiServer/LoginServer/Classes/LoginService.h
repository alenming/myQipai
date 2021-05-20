#ifndef __LOGIN_SERVICE_H__
#define __LOGIN_SERVICE_H__

#include "KxCommInterfaces.h"

class LoginService
{
public:

	static void processServiceC2S(int subcmd, int uid, char *buffer, int len, IKxComm *commun);
	static void processServiceS2S(int subcmd, int uid, char *buffer, int len, IKxComm *commun);

	static void CMD_C2S_LOGIN(int uid, char *buffer, int len, IKxComm *commun);
	static void CMD_S2C_LOGIN(int uid, int accountId);
	static void CMD_S2C_NEW_USER_LOGIN(int uid, int accountId);
	
	static void SERVER_SUB_OFFLINE(int uid, char *buffer, int len, IKxComm *commun);

	//处理用户登录后提交日志信息
	static void processUserLogService(int uid, char *buffer, int len, IKxComm *commun);
	//处理用户登录后提交引导日志信息
	static void processGuideLogService(int uid, char *buffer, int len, IKxComm *commun);
	//处理用户零点或者其他时间点刷新数据请求
	static void processLoginFreshService(int uid, char *buffer, int len, IKxComm *commun);
};

#endif //__LOGIN_SERVICE_H__
