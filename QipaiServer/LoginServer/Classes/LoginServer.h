#ifndef __LOGINSERVER_H__
#define __LOGINSERVER_H__
#include "BaseServer.h"
#include "ServerConfig.h"

class LoginServer : public BaseServer
{
private:
	LoginServer(void);
	virtual ~LoginServer(void);
public:
	static LoginServer* getInstance();
	static void destroy();

	bool dynamicUpdate();

	virtual bool initServerInfo();

	virtual bool onServerInit();

	void setSessionServerClose();

	void setSessionServerClosing();

	bool getClosingFlag(){ return m_IsClosing; }
private:
	void AddModelType();
private:
	bool					m_IsClosing;	
	static LoginServer*		m_Instance;		
	ServerData				m_ServerData;
	ServerConfig			m_ServerConfig;	
};
#endif //__LOGINSERVER_H__