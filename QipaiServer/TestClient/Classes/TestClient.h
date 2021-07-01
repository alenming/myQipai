#ifndef __SESSIONSERVER_H__
#define __SESSIONSERVER_H__

#include "BaseServer.h"

// 连接服务器配置
struct ConnectServerConf
{
	int serverGroup;
	int port;
	std::string host;
};

class TestClient
{
private:
    TestClient(void);
    virtual ~TestClient(void);

public:
    static TestClient* getInstance();
    static void destroy();
										
	//服务器模块初始化
    virtual bool onServerInit();		

	bool login();

private:
	static TestClient*	m_Instance;		//单例
	ConnectServerConf*  m_ServerCon;
};

#endif //__SESSIONSERVER_H__
