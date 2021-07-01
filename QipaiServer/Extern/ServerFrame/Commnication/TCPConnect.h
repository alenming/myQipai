/*
 * ������֮�����ӵĶ���
 */
#ifndef __TCPCONNECTOR_H__
#define __TCPCONNECTOR_H__

#include "Core.h"
#include "TCPUnit.h"

enum EVENT_CONNECTER
{
    EVENT_CONNECT_SUCCESS,
    EVENT_CONNECT_FAILE,
};

class TCPConnect : public TCPUnit
{
public:
	TCPConnect();
	virtual ~TCPConnect();

    // ����ָ����IP�Ͷ˿�
	virtual bool connect(const char* addr, int port, bool nonblock = true);

    // ���յ�����ʱ�����Ļص�����IKxCommPoller����
    virtual int onSend();
    virtual int onRecv();
    
    virtual void onConnected(bool success);
	bool isConnecting() { return m_IsConnecting; }
protected:
    bool m_IsConnecting;    // �Ƿ���������
};

#endif
