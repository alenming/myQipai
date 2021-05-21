#ifndef SESSION_CONNECT_H__
#define SESSION_CONNECT_H__

#include <map>
#include "KxTCPConnector.h"
#include "KxTimerCallback.h"

#define RECONNECT_TIME_INTERVAL		5			// �������5��
#define MAX_RECONECT_TIME			0xFFFFFFFF	// ����������

class SessionConnect : public KxTCPConnector
{
public:
	SessionConnect();
	virtual ~SessionConnect();

	// ����ָ����IP�Ͷ˿�
	virtual bool connect(const char* addr, int port, int serverId = 0, bool nonblock = true);
	// ���ӳɹ�ʧ�ܻص�
	virtual void onConnected(bool success);
	// ��������ʱ�ص�����IKxCommPoller����
	virtual int onError();

	// ���ö�ʱ����
	virtual void setTimer(int nSec);
	// ��ʱ����
	virtual void onTimer();

	int getServerId() { return m_ServerId; }

private:
	// ����
	bool reconnect();

private:
	int m_ServerId;			// ��������ID
	int m_Port;			    // �������˿�
	char m_strIP[16];	    // ������IP
	KxTimerCallback<SessionConnect>* m_TimerCallBack;
};


#endif //SESSION_CONNECT_H__
