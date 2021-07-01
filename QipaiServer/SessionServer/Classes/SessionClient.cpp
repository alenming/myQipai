#include "SessionClient.h"
#include "NetManager.h"
#include "SessionServer.h"
#include "TCPConnect.h"
#include "Log/LogManager.h"

using namespace std;

#define HEARTBEATTIME                   (24*60*60)       //心跳时间 s

SessionClient::SessionClient()
	: m_Permission(0),
	  m_UserId(0),
	  m_GuestId(0)
{
    // 设置客户端心跳超时
    m_TimerCallBack = new TimerCallback<SessionClient>();
    setTimer();
}

SessionClient::~SessionClient(void)
{
    m_TimerCallBack->stop();
    KXSAFE_RELEASE(m_TimerCallBack);
}

bool SessionClient::setConServer(int nKey, int nValue)
{
	if (nKey == 0)
	{
		return false;
	}
	m_MapConKeyValue[nKey] = nValue;
	return true;
}

bool SessionClient::sendDataToServer(int mainCmd, int subCmd, char *pszContext, int nLen)
{
    TCPConnect* pTcpConnector = nullptr;
	pTcpConnector = static_cast<TCPConnect*>(NetManager::getInstance()->getServer(mainCmd));
    if (pTcpConnector == nullptr)
    {
        return false;
    }
	LOGDEBUG("收到客户端消息,转发给指定服务器!mainCmd=%d,subCmd=%d,SessionClienter::sendDataToServer!", mainCmd, subCmd);
    return pTcpConnector->sendData(pszContext, nLen) >= 0;
}

bool SessionClient::sendDataToGroupServer(int nGroupID, char *pszContext, int nLen)
{
    vector<IComm*>* pVectConnector = NetManager::getInstance()->getGroupServer(nGroupID);
	if (pVectConnector == nullptr)
	{
		return false;
	}
	LOGDEBUG("收到客户端消息,转发给指定服务器组!nGroupID=%d,SessionClienter::sendDataToGroupServer!", nGroupID);
    for (vector<IComm*>::iterator ator = pVectConnector->begin();
        ator != pVectConnector->end(); ++ator)
    {
        (*ator)->sendData(pszContext, nLen);
    }
    return true;
}

bool SessionClient::sendDataToAllServer(char *pszContext, int nLen)
{
    map<int, IComm*>& allServer = NetManager::getInstance()->getAllServer();
    for (map<int,IComm* >::iterator ator = allServer.begin(); ator != allServer.end(); ++ator)
    {
        // ps. 如果send失败触发onError，在onError中从NetWorkManager中移除，会导致崩溃
		ator->second->sendData(pszContext, nLen);
    }
	LOGDEBUG("收到客户端消息,转发给所有服务器组!SessionClienter::sendDataToAllServer!");
    return true;
}

int SessionClient::getRouteValue(int nKey)
{
	map<int, int>::iterator ator = m_MapConKeyValue.find(nKey);
	if (ator == m_MapConKeyValue.end())
	{
		return 0;
	}
	return ator->second;
}

int SessionClient::onRecv()
{
    m_TimerCallBack->stop(); 
    SessionServer::getInstance()->getTimerManager()->addTimer( m_TimerCallBack, HEARTBEATTIME, 0);
    return TCPClienter::onRecv();
}

void SessionClient::setTimer()
{
	m_TimerCallBack->setCallback(this, &SessionClient::onTimer);
	SessionServer::getInstance()->getTimerManager()->addTimer(m_TimerCallBack, HEARTBEATTIME, 0);
	LOGDEBUG("CSessionClient::setTimer()");
}

void SessionClient::onTimer()
{
    // 时间到了
    clean();
	LOGDEBUG("SessionClienter::onTimer()");
}

void SessionClient::clean()
{
	NetManager::getInstance()->removeUser(m_UserId);

    m_TimerCallBack->stop();
    m_TimerCallBack->clean();
    if (m_Poller != nullptr)
    {
        m_Poller->removeCommObject(this);
    }
}
