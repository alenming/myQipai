/*
 * TCP Socket ������
 * Client Module ���ڸ��ͻ������ö�Ӧ�Ĵ���ģ��
 * Process Module ���ڶԿͻ��˵����ӽ��д������������ǽ��֤��IP���˵ȵ�
 *
 */
#ifndef __TCPLISTENER_H__
#define __TCPLISTENER_H__

#include "Core.h"
#include "TCPClienter.h"
#include "Sock.h"

class TCPListener : public IComm
{
public:
	TCPListener();
	virtual ~TCPListener();

    // ��ʼ��Sock
    bool init();
    // �󶨵��˿ڲ�����
    bool listen(int port, char* addr = nullptr, bool nonblock = true, int listenQ = 128);
    // Acceptʱ�ص�������KXCOMMID����KxTCPClienter
    virtual TCPClienter* onAccept(COMMID client);

    // ���յ�����ʱ�����Ļص�����IKxCommPoller����
    virtual int onRecv();
    // ���ݿɱ�����ʱ�����Ļص�����IKxCommPoller����
    virtual int onSend();

    // ��ȡͨѶID��ͨ����socket fd��
    virtual COMMID getCommId() { return m_Socket->getSockFd(); }
    // �رո�ͨѶ����
    virtual void close() { m_Socket->close(); }

    // ���úͻ�ȡ�ͻ��˴���ģ��
    void setClientModule(IModule* module);
    inline IModule* getClientModule()	{ return m_ClientModule; }

    // ��ȡSock
    inline Sock* getSock() { return m_Socket; }

protected:
    // ��������
    virtual int sendData(const char* buffer, unsigned int len);
    // ��������
    virtual int recvData(char* buffer, unsigned int len);

protected:
	Sock* m_Socket;
	IModule* m_ClientModule;
};

#endif