/*
 * ʵ��UDPЭ��Socket���䵥Ԫ
 */
#ifndef __UDPUNIT_H__
#define __UDPUNIT_H__

#include "core/KxCore.h"
#include "commnication/KxCommInterfaces.h"
#include "commnication/KxSock.h"

class KxUDPUnit : public IKxComm
{
public:
	KxUDPUnit();
	virtual ~KxUDPUnit();

    // ��ʼ��
    virtual bool init();

    // ��������
    virtual int sendData(const char* buffer, unsigned int len);
    virtual int sendData(const char* buffer, unsigned int len, char* ip, int port);
    // ��������
    virtual int recvData(char* buffer, unsigned int len);
    // �رո�ͨѶ����
    virtual void close();

    // ���յ�����ʱ�����Ļص�����IKxCommPoller����
    virtual int onRecv();
    // ���ݿɱ�����ʱ�����Ļص�����IKxCommPoller����
    virtual int onSend();

    // ��Socket
	int bind(char* ip, int port);
    // ���÷���Ŀ��IP�Ͷ˿�
    int setSendToAddr(char* ip, int port);

    // ��ȡͨѶID��ͨ����socket fd��
    virtual KXCOMMID getCommId() { return m_Socket->getSockFd(); }

private:
	KxSock*     m_Socket;
    char*       m_RecvBuffer;
};


#endif
