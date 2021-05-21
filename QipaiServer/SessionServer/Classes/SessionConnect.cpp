#include "SessionConnect.h"
#include "SessionServer.h"
#include "log/LogManager.h"



SessionConnect::SessionConnect()
: m_ServerId(0)
, m_Port(0)
, m_TimerCallBack(nullptr)
{
    memset(m_strIP, 0, sizeof(m_strIP));
}

SessionConnect::~SessionConnect()
{
    if (m_TimerCallBack != nullptr)
    {
        m_TimerCallBack->stop();
        KXSAFE_RELEASE(m_TimerCallBack);
    }
}

// ����ָ����IP�Ͷ˿�
bool SessionConnect::connect(const char* addr, int port, int serverId, bool nonblock)
{
    if (addr == nullptr)
    {
        return false;
    }

    memcpy(m_strIP, addr, sizeof(m_strIP));
    m_Port = port;
	m_ServerId = serverId;
	return TCPConnect::connect(m_strIP, m_Port, nonblock);
}

void SessionConnect::onConnected(bool success)
{
    TCPConnect::onConnected(success);
    if (success)
    {
		KX_LOGDEBUG("SessionConnect::onConnected() success IP:%s,port:%d", m_strIP, m_Port);
    }
    else
    {
		KX_LOGDEBUG("SessionConnect::onConnected() faile IP:%s,port:%d", m_strIP, m_Port);
    }
}

// ��������ʱ�ص�����IKxCommPoller����
int SessionConnect::onError()
{
    int nError = TCPConnect::onError();
	KX_LOGDEBUG("SessionConnect::onError() IP:%s,port:%d", m_strIP, m_Port);
    setTimer(RECONNECT_TIME_INTERVAL);
    return nError;
}

// ���ö�ʱ����	
void SessionConnect::setTimer(int nSec)
{
    if (m_TimerCallBack == nullptr)
    {
        m_TimerCallBack = new KxTimerCallback<SessionConnect>();
    }
    m_TimerCallBack->setCallback(this, &SessionConnect::onTimer);
    SessionServer::getInstance()->getTimerManager()->addTimer(
        m_TimerCallBack, nSec, 0);
}

// ��ʱ����
void SessionConnect::onTimer()
{
    reconnect();
    KXSAFE_RELEASE(m_TimerCallBack);
}

// ����
bool SessionConnect::reconnect()
{
    close();
    if (!init() || !connect(m_strIP, m_Port))
    {
        return false;
    }

    IKxCommPoller* pPoller = SessionServer::getInstance()->getMainPoller();
    pPoller->addCommObject(this, getPollType());
    return true;
}
