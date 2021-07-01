/*
* ʵ��TCPЭ��Socket���䵥Ԫ
* ��Ҫ���TCP���ݴ��������
* ��֤�����ܹ��������ط���
* 
*  2015-05-04 By ��ү
*
*/
#ifndef __TCPUNIT_H__
#define __TCPUNIT_H__

#include "Core.h"
#include "CommInterfaces.h"
#include "Sock.h"
#include "BufferList.h"


class TCPUnit : public IComm
{
public:
    TCPUnit();
    virtual ~TCPUnit();

    // ��ʼ��
	virtual bool init();

	virtual bool init(SOCK_VERSION ipv);
    // ��������
    virtual int sendData(const char* buffer, unsigned int len);
    // ��������
    virtual int recvData(char* buffer, unsigned int len);
    // �رո�ͨѶ����
    virtual void close() { m_Socket->close(); }

    // ���յ�����ʱ�����Ļص�����IKxCommPoller����
    virtual int onRecv();
    // ���ݿɱ�����ʱ�����Ļص�����IKxCommPoller����
    virtual int onSend();

    // ��ȡͨѶID��ͨ����socket fd��
    virtual COMMID getCommId() { return m_Socket->getSockFd(); }
    // ��ȡSock
    inline Sock* getSock() { return m_Socket; }

protected:
    Sock* m_Socket;						// Socket����
	char m_ServerName[16];				// ����������
    char* m_SendBuffer;                 // ���ڷ��͵Ļ�����
    char* m_RecvBuffer;                 // ���ڽ��յĻ�����
    unsigned int m_SendBufferLen;       // ���ڷ��͵Ļ�������С
    unsigned int m_RecvBufferLen;       // ���ڽ��յĻ�������С
    unsigned int m_SendBufferOffset;    // ���ڷ��͵Ļ�����ƫ��
    unsigned int m_RecvBufferOffset;    // ���ڽ��յĻ�����ƫ��
    BufferList m_BufferList;          // �����͵Ļ������б�
    static char* s_RecvBuffer;          // ȫ�ֽ��ջ�����
};


#endif