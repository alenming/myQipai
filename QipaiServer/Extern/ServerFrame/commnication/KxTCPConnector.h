/*
 * BufferList �������б�
 * ���ڻ������ݿ�
 *
 */
#ifndef __TCPCONNECTOR_H__
#define __TCPCONNECTOR_H__

#include "core/KxCore.h"
#include "KxTCPUnit.h"

enum KXEVENT_CONNECTER
{
    KXEVENT_CONNECT_SUCCESS,
    KXEVENT_CONNECT_FAILE,
};

class KxTCPConnector : public KxTCPUnit
{
public:
	KxTCPConnector();
	virtual ~KxTCPConnector();

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
