/*
 * TCP Socket ������
 * Client Module ���ڸ��ͻ������ö�Ӧ�Ĵ���ģ��
 * Process Module ���ڶԿͻ��˵����ӽ��д����������ǽ��֤��IP���˵ȵ�
 *
 */
#ifndef __TCPLISTENER_H__
#define __TCPLISTENER_H__

#include "core/KxCore.h"
#include "commnication/KxCommInterfaces.h"
#include "commnication/KxTCPClienter.h"
#include "commnication/KxSock.h"

class KxTCPListener : public IKxComm
{
public:
	KxTCPListener();
	virtual ~KxTCPListener();

    // ��ʼ��Sock
    bool init();
    // �󶨵��˿ڲ�����
    bool listen(int port, char* addr = NULL, bool nonblock = true, int listenQ = 128);
    // Acceptʱ�ص�������KXCOMMID����KxTCPClienter
    virtual KxTCPClienter* onAccept(KXCOMMID client);

    // ���յ�����ʱ�����Ļص�����IKxCommPoller����
    virtual int onRecv();
    // ���ݿɱ�����ʱ�����Ļص�����IKxCommPoller����
    virtual int onSend();

    // ��ȡͨѶID��ͨ����socket fd��
    virtual KXCOMMID getCommId() { return m_Socket->getSockFd(); }
    // �رո�ͨѶ����
    virtual void close() { m_Socket->close(); }

    // ���úͻ�ȡ�ͻ��˴���ģ��
    void setClientModule(IKxModule* module);
    inline IKxModule* getClientModule()	{ return m_ClientModule; }

    // ��ȡSock
    inline KxSock* getSock() { return m_Socket; }

protected:
    // ��������
    virtual int sendData(const char* buffer, unsigned int len);
    // ��������
    virtual int recvData(char* buffer, unsigned int len);

protected:
	KxSock* m_Socket;
	IKxModule* m_ClientModule;
};

#endif
