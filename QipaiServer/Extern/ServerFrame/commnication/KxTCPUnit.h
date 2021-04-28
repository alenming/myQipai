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

#include "core/KxCore.h"
#include "commnication/KxCommInterfaces.h"
#include "commnication/KxSock.h"
#include "commnication/KxBufferList.h"


class KxTCPUnit : public IKxComm
{
public:
    KxTCPUnit();
    virtual ~KxTCPUnit();

    // ��ʼ��
    virtual bool init();

	virtual bool init(KXSOCK_VERSION ipv);

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
    virtual KXCOMMID getCommId() { return m_Socket->getSockFd(); }
    // ��ȡSock
    inline KxSock* getSock() { return m_Socket; }

protected:
    KxSock* m_Socket;                   // Socket����
    char* m_SendBuffer;                 // ���ڷ��͵Ļ�����
    char* m_RecvBuffer;                 // ���ڽ��յĻ�����
    unsigned int m_SendBufferLen;       // ���ڷ��͵Ļ�������С
    unsigned int m_RecvBufferLen;       // ���ڽ��յĻ�������С
    unsigned int m_SendBufferOffset;    // ���ڷ��͵Ļ�����ƫ��
    unsigned int m_RecvBufferOffset;    // ���ڽ��յĻ�����ƫ��
    KxBufferList m_BufferList;          // �����͵Ļ������б�
    static char* s_RecvBuffer;          // ȫ�ֽ��ջ�����
};


#endif
