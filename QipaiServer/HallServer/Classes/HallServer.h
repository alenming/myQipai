#ifndef __HALL_SERVER_H__
#define __HALL_SERVER_H__
#include "BaseServer.h"
#include "ServerConfig.h"

class HallServer : public BaseServer
{
private:
	HallServer(void);
	virtual ~HallServer(void);
public:
	static HallServer* getInstance();
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
	static HallServer*		m_Instance;		
	ServerData				m_ServerData;
	ServerConfig			m_ServerConfig;	
};
#endif //__HALL_SERVER_H__