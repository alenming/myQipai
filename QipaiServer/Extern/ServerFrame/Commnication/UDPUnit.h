/*
 * ʵ��UDPЭ��Socket���䵥Ԫ
 */
#ifndef __UDPUNIT_H__
#define __UDPUNIT_H__

#include "Core.h"
#include "CommInterfaces.h"
#include "Sock.h"

class UDPUnit : public IComm
{
public:
	UDPUnit();
	virtual ~UDPUnit();

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
    virtual COMMID getCommId() { return m_Socket->getSockFd(); }

private:
	Sock*     m_Socket;
    char*       m_RecvBuffer;
};


#endif
