#ifndef __SESSIONSERVER_H__
#define __SESSIONSERVER_H__

#include "KXServer.h"
#include "ServerConfig.h"


class LoginServer : public KxBaseServer
{
private:
    LoginServer(void);
    virtual ~LoginServer(void);

public:
    static LoginServer* getInstance();
    static void destroy();

	//��̬���·�����
	bool dynamicUpdate();											
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

private:
	void AddModelType();

private:
	bool					m_IsClosing;	//����ƽ���ر�ʱ�ı�־
	static LoginServer*		m_Instance;		//����

	ServerData				m_ServerData;
	ServerConfig			m_ServerConfig;	//����������ȡ���ñ�
};

#endif //__SESSIONSERVER_H__
