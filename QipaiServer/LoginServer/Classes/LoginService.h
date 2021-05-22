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

	//�����û���¼���ύ��־��Ϣ
	static void processUserLogService(int uid, char *buffer, int len, IComm *commun);
	//�����û���¼���ύ������־��Ϣ
	static void processGuideLogService(int uid, char *buffer, int len, IComm *commun);
	//�����û�����������ʱ���ˢ����������
	static void processLoginFreshService(int uid, char *buffer, int len, IComm *commun);
};

#endif //__LOGIN_SERVICE_H__
