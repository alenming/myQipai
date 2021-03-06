#ifndef __SESSIONSERVER_H__
#define __SESSIONSERVER_H__

#include "BaseServer.h"
#include "ServerConfig.h"


class SessionServer : public BaseServer
{
private:
    SessionServer(void);
    virtual ~SessionServer(void);

public:
    static SessionServer* getInstance();

    static void destroy();

	//服务器信息初始化
	virtual bool initServerInfo();
	//服务器模块初始化
    virtual bool onServerInit();
	//设置服务器立刻关闭 
	void setSessionServerClose();							
	//设置服务器平滑开始标志
	void setSessionServerClosing();							
	//得到平滑关闭标志位
	bool getClosingFlag(){ return m_IsClosing; }

	ServerConfig * getServerCoinfig() { return &m_ServerConfig; }
private:
	ServerData				m_ServerData;
	bool					m_IsClosing;	//开启平滑关闭时的标志
	ServerConfig			m_ServerConfig;	//服务器数读取配置表
	static SessionServer*	m_Instance;		//单例
};

#endif //__SESSIONSERVER_H__
