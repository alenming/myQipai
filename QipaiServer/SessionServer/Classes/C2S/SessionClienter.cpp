#include "SessionClienter.h"
#include "NetworkManager.h"
#include "SessionServer.h"


using namespace std;

#define HEARTBEATTIME                   (24*60*60)       //����ʱ�� s

SessionClienter::SessionClienter()
: m_Permission(0)
, m_GuestId(0)
, m_UserId(0)
{
    // ���ÿͻ���������ʱ
    m_TimerCallBack = new KxTimerCallback<SessionClienter>();
    setTimer();
}

SessionClienter::~SessionClienter(void)
{
    m_TimerCallBack->stop();
    KXSAFE_RELEASE(m_TimerCallBack);
}

bool SessionClienter::setConServer(int nKey, int nValue)
{
	if (nKey == 0)
	{
		return false;
	}
	m_MapConKeyValue[nKey] = nValue;
	return true;
}

bool SessionClienter::sendDataToServer(int mainCmd, int subCmd, char *pszContext, int nLen)
{
    KxTCPConnector* pTcpConnector = NULL;
	pTcpConnector = static_cast<KxTCPConnector*>(NetWorkManager::getInstance()->getServer(mainCmd));
    if (pTcpConnector == NULL)
    {
        return false;
    }
	KX_LOGDEBUG("�յ��ͻ�����Ϣ,ת����ָ��������!mainCmd=%d,subCmd=%d,SessionClient::sendDataToServer!", mainCmd, subCmd);
    return pTcpConnector->sendData(pszContext, nLen) >= 0;
}

bool SessionClienter::sendDataToGroupServer(int nGroupID, char *pszContext, int nLen)
{
    vector<IKxComm*>* pVectConnector = NetWorkManager::getInstance()->getGroupServer(nGroupID);
	if (pVectConnector == NULL)
	{
		return false;
	}
	KX_LOGDEBUG("�յ��ͻ�����Ϣ,ת����ָ����������!nGroupID=%d,SessionClient::sendDataToGroupServer!", nGroupID);
    for (vector<IKxComm*>::iterator ator = pVectConnector->begin();
        ator != pVectConnector->end(); ++ator)
    {
        (*ator)->sendData(pszContext, nLen);
    }
    return true;
}

bool SessionClienter::sendDataToAllServer(char *pszContext, int nLen)
{
    map<int, IKxComm*>& allServer = NetWorkManager::getInstance()->getAllServer();
    for (map<int,IKxComm* >::iterator ator = allServer.begin(); ator != allServer.end(); ++ator)
    {
        // ps. ���sendʧ�ܴ���onError����onError�д�NetWorkManager���Ƴ����ᵼ�±���
		ator->second->sendData(pszContext, nLen);
    }
	KX_LOGDEBUG("�յ��ͻ�����Ϣ,ת�������з�������!SessionClient::sendDataToAllServer!");
    return true;
}

int SessionClienter::getRouteValue(int nKey)
{
	map<int, int>::iterator ator = m_MapConKeyValue.find(nKey);
	if (ator == m_MapConKeyValue.end())
	{
		return 0;
	}
	return ator->second;
}

int SessionClienter::onRecv()
{
    m_TimerCallBack->stop(); 
    SessionServer::getInstance()->getTimerManager()->addTimer( m_TimerCallBack, HEARTBEATTIME, 0);
    return KxTCPClienter::onRecv();
}

void SessionClienter::setTimer()
{
	m_TimerCallBack->setCallback(this, &SessionClienter::onTimer);
	SessionServer::getInstance()->getTimerManager()->addTimer(m_TimerCallBack, HEARTBEATTIME, 0);
	KX_LOGDEBUG("CSessionClient::setTimer()");
}

void SessionClienter::onTimer()
{
    // ʱ�䵽��
    clean();
	KX_LOGDEBUG("CSessionClient::onTimer()");
}

void SessionClienter::clean()
{
	if (m_Permission == 0)
	{
		NetWorkManager::getInstance()->removeGuest(m_GuestId);
	}
	else
	{
		NetWorkManager::getInstance()->removeUser(m_UserId);
	}

    m_TimerCallBack->stop();
    m_TimerCallBack->clean();
    if (m_Poller != NULL)
    {
        m_Poller->removeCommObject(this);
    }
}
