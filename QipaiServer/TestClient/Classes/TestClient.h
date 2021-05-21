#ifndef __SESSIONSERVER_H__
#define __SESSIONSERVER_H__

#include "BaseServer.h"

// ���ӷ���������
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
										
	//������ģ���ʼ��
    virtual bool onServerInit();		

	bool login();

private:
	static TestClient*	m_Instance;		//����
	ConnectServerConf*  m_ServerCon;
};

#endif //__SESSIONSERVER_H__
