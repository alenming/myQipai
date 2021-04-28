#include "GameNetworkNode.h"
#include "GameModule.h"
#include <string.h>

using namespace std;

CGameNetworkNode::CGameNetworkNode(void)
: m_bUserClose(false)
, m_bEnabledHb(false)
, m_fMaxHbTime(5.0f)
, m_fTickNet(0.0f)
, m_pPoller(NULL)
, m_pGameModule(NULL)
{
}


CGameNetworkNode* CGameNetworkNode::create()
{ 
	CGameNetworkNode *pRet = new(std::nothrow) CGameNetworkNode();
	if (pRet && pRet->init())
	{
		pRet->retain();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

CGameNetworkNode::~CGameNetworkNode(void)
{
    KXSAFE_RELEASE(m_pPoller);
}

CGameNetworkNode *CGameNetworkNode::m_pInstance = NULL;

CGameNetworkNode *CGameNetworkNode::getInstance()
{
    if (m_pInstance == NULL)
    {
        m_pInstance = new CGameNetworkNode;
        if (!m_pInstance->init())
        {
            delete m_pInstance;
            m_pInstance = NULL;
        }
    }

    return m_pInstance;
}

void CGameNetworkNode::destroy()
{
    if (NULL != m_pInstance)
    {
        m_pInstance->release();
    }
}

bool CGameNetworkNode::init()
{
    m_pPoller = new KxSelectPoller();
	m_pGameModule = new GameModule();
    return true;
}

void CGameNetworkNode::onEnter()
{
	//CCNode::onEnter();
}

void CGameNetworkNode::onExit()
{
	//CCNode::onExit();
}

void CGameNetworkNode::update(float dt)
{
    if (m_bEnabledHb)
    {
        // 心跳发送, 如果n秒内没有数据交互, 这时才发送心跳包
        m_fTickNet += dt;
        if (m_fTickNet >= m_fMaxHbTime)
        {
            heartbeatScheduler(dt);
        }
    }
}

//void CGameNetworkNode::visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags)
//{
//    Node::visit(renderer, parentTransform, parentFlags);
//   if (NULL != m_pPoller)
//   {
//       // 轮询网络
//       m_pPoller->poll();
//   }
//}

bool CGameNetworkNode::connectToServer(const char *ip, int port, EServerConnType connType, KXSOCK_VERSION version /*= KXV_IPV4*/, std::function<void(bool)> callBack /*= nullptr*/)
{
    map<int, ServerConn>::iterator iter = m_mapServerConns.find(connType);
    if (iter != m_mapServerConns.end())
    {
        return false;
    }

    KxTCPConnector *pConnector = new KxTCPConnector();
	if (!pConnector->init(version))
    {
        KX_LOGDEBUG("error: m_pConnector init faile %d", pConnector->getCommId());
		pConnector->release();
        return false;
    }

	if (SERVER_CONN_SESSION == connType)
	{
		pConnector->setModule(m_pGameModule);
	}

    ServerConn conn;
    conn.SockVersion = version;
    conn.Connector = pConnector;
    conn.ConnectCallback = callBack;
    conn.ConnIP = ip;
    conn.Port = port;

    m_mapServerConns[connType] = conn;

    if (!pConnector->isConnecting()&& !pConnector->connect(ip, port))
    {
		KX_LOGDEBUG("error: m_pConnector connect faile %d", pConnector->getCommId());
		pConnector->release();
        return false;
    }

    m_pPoller->addCommObject(pConnector, pConnector->getPollType());
    
    m_bUserClose = false;
    return true;
}

int CGameNetworkNode::sendData(char* buffer, unsigned int len, EServerConnType connType /*= SERVER_CONN_SESSION*/)
{
    KxTCPConnector*pConnector = getConnector(connType);
    if (nullptr != pConnector && !pConnector->isConnecting())
    {
        return pConnector->sendData(buffer, len);
    }
    return -1;
}

bool CGameNetworkNode::reconnectToServer(EServerConnType connType, std::function<void(bool)> callBack /*= nullptr*/)
{
    map<int, ServerConn>::iterator iter = m_mapServerConns.find(connType);
    if (iter == m_mapServerConns.end())
    {
        return false;
    }

    iter->second.ConnectCallback = callBack;
    KxTCPConnector *pConnector = iter->second.Connector;
    if (nullptr != pConnector)
    {
        iter->second.ConnectingTimes += 1;
        // 正在重连中，别催我
        if (pConnector->isConnecting() && iter->second.ConnectingTimes < 8)
        {
			KX_LOGDEBUG("reconnectToServer But Connector isConnecting");
            return true;
        }
    }
    iter->second.ConnectingTimes = 0;

	KxTCPConnector* pNewConnector = new KxTCPConnector();
    if (!pNewConnector->init(iter->second.SockVersion))
    {
		pConnector->release();
        return false;
    }

	if (SERVER_CONN_SESSION == connType)
	{
		pNewConnector->setModule(m_pGameModule);
	}

    // 先移除,再赋值
    if (nullptr != pConnector)
    {
        if (pConnector->getPoller() != nullptr)
        {
            pConnector->getPoller()->removeCommObject(pConnector);
        }
        pConnector->close();
		pConnector->release();
    }

    if (!pNewConnector->connect(iter->second.ConnIP.c_str(), iter->second.Port))
    {
		pConnector->release();
        return false;
    }

    m_pPoller->addCommObject(pNewConnector, pNewConnector->getPollType());
    iter->second.Connector = pNewConnector;

    m_bUserClose = false;
	return true;
}

void CGameNetworkNode::setIsHeartbeat(bool bEnabled) 
{
    m_bEnabledHb = bEnabled; 
}

void CGameNetworkNode::setHeartbeatTime(float fHbTime)
{
    m_fMaxHbTime = fHbTime;
}

void CGameNetworkNode::closeConnect()
{
    m_bUserClose = true;
    map<int, ServerConn>::iterator iter = m_mapServerConns.begin();
    for (; iter != m_mapServerConns.end(); ++iter)
    {
        KxTCPConnector* pConnector = iter->second.Connector;
        if (nullptr != pConnector)
        {
            if (pConnector->getPoller() != nullptr)
            {
                pConnector->getPoller()->removeCommObject(pConnector);
            }
            pConnector->close();
			pConnector->release();
        }
    }

    m_mapServerConns.clear();
}

KxTCPConnector *CGameNetworkNode::getConnector(EServerConnType connType /*= SERVER_CONN_SESSION*/)
{
    map<int, ServerConn>::iterator iter = m_mapServerConns.find(connType);
    if (iter != m_mapServerConns.end())
    {
        return iter->second.Connector;
    }

    return nullptr;
}


ServerConn* CGameNetworkNode::getServerConn(EServerConnType connType /*= SERVER_CONN_SESSION*/)
{
    map<int, ServerConn>::iterator iter = m_mapServerConns.find(connType);
    if (iter != m_mapServerConns.end())
    {
        return &(iter->second);
    }

    return nullptr;
}

void CGameNetworkNode::heartbeatScheduler(float dt)
{
    map<int, ServerConn>::iterator iter = m_mapServerConns.begin();
    for (; iter != m_mapServerConns.end(); ++iter)
    {
        if (nullptr != iter->second.Connector)
        {
            // 发送心跳包
            char buffer[32] = { 0 };
            int len = 32;

            iter->second.Connector->sendData(buffer, len);
        }
    }
}

void CGameNetworkNode::registeNetworkCommunicate(void *data)
{
    m_fTickNet = 0;
}
