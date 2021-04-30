#include "SessionConnector.h"
#include "SessionServer.h"




SessionConnector::SessionConnector()
: m_ServerId(0)
, m_Port(0)
, m_TimerCallBack(NULL)
{
    memset(m_strIP, 0, sizeof(m_strIP));
}

SessionConnector::~SessionConnector()
{
    if (m_TimerCallBack != NULL)
    {
        m_TimerCallBack->stop();
        KXSAFE_RELEASE(m_TimerCallBack);
    }
}

// 连接指定的IP和端口
bool SessionConnector::connect(const char* addr, int port, int serverId, bool nonblock)
{
    if (addr == NULL)
    {
        return false;
    }

    memcpy(m_strIP, addr, sizeof(m_strIP));
    m_Port = port;
	m_ServerId = serverId;
	return KxTCPConnector::connect(m_strIP, m_Port, nonblock);
}

void SessionConnector::onConnected(bool success)
{
    KxTCPConnector::onConnected(success);
    if (success)
    {
		KX_LOGDEBUG("SessionConnect::onConnected() success IP:%s,port:%d", m_strIP, m_Port);
    }
    else
    {
		KX_LOGDEBUG("SessionConnect::onConnected() faile IP:%s,port:%d", m_strIP, m_Port);
    }
}

// 发生错误时回调，由IKxCommPoller调用
int SessionConnector::onError()
{
    int nError = KxTCPConnector::onError();
	KX_LOGDEBUG("SessionConnect::onError() IP:%s,port:%d", m_strIP, m_Port);
    setTimer(RECONNECT_TIME_INTERVAL);
    return nError;
}

// 设置定时重连	
void SessionConnector::setTimer(int nSec)
{
    if (m_TimerCallBack == NULL)
    {
        m_TimerCallBack = new KxTimerCallback<SessionConnector>();
    }
    m_TimerCallBack->setCallback(this, &SessionConnector::onTimer);
    SessionServer::getInstance()->getTimerManager()->addTimer(
        m_TimerCallBack, nSec, 0);
}

// 定时触发
void SessionConnector::onTimer()
{
    reconnect();
    KXSAFE_RELEASE(m_TimerCallBack);
}

// 重连
bool SessionConnector::reconnect()
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
