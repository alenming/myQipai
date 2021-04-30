#ifndef __SESSIONSERVER_H__
#define __SESSIONSERVER_H__

#include "KXServer.h"
#include "ServerConfig.h"


class SessionServer : public KxBaseServer
{
private:
    SessionServer(void);
    virtual ~SessionServer(void);

public:
    static SessionServer* getInstance();

    static void destroy();

	//��������Ϣ��ʼ��
	virtual bool initServerInfo();
	//������ģ���ʼ��
    virtual bool onServerInit();
	//���÷��������̹ر� 
	void setSessionServerClose();							
	//���÷�����ƽ����ʼ��־
	void setSessionServerClosing();							
	//�õ�ƽ���رձ�־λ
	bool getClosingFlag(){ return m_IsClosing; }			
	ServerConfig * getServerCoinfig() { return &m_ServerConfig; }


private:
	ServerData				m_ServerData;
	bool					m_IsClosing;	//����ƽ���ر�ʱ�ı�־
	ServerConfig			m_ServerConfig;	//����������ȡ���ñ�
	static SessionServer*	m_Instance;		//����
};

#endif //__SESSIONSERVER_H__
