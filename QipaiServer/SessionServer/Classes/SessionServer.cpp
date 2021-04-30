#include "SessionServer.h"

#include "C2S/SessionListener.h"
#include "C2S/ClientModule.h"
//#include "SessionClienter.h"

#include "S2C/ConnectModule.h"
#include "S2C/SessionConnector.h"

#include "NetworkManager.h"

#include "core/KxPlatform.h"
#include "log/LogManager.h"

#include "C2S/ClienterEvent.h"
#include "S2C/ConnectEvent.h"





using namespace std;

SessionServer* SessionServer::m_Instance = NULL;
SessionServer::SessionServer(void)
:m_IsClosing(false)
{

}

SessionServer::~SessionServer(void)
{
	KXSAFE_RELEASE(m_Poller);
	m_ServerConfig.clean();
}

SessionServer* SessionServer::getInstance()
{
    if (NULL == m_Instance)
    {
        m_Instance = new SessionServer();
    }
    return m_Instance;
}

void SessionServer::destroy()
{
    if (NULL != m_Instance)
    {
        delete m_Instance;
        m_Instance = NULL;
    }
}

bool SessionServer::onServerInit()
{
	KxBaseServer::onServerInit();
	KX_LOGDEBUG("==================================================");

	//1.³õÊ¼»¯ÂÖÑ¯Æ÷
#if(KX_TARGET_PLATFORM == KX_PLATFORM_LINUX)
	m_Poller = new KxEpoller();
#else
	m_Poller = new KxSelectPoller();
#endif

    SessionListener* listener = new SessionListener();
	if (!listener->init())
    {
        return false;
    }
	
	//m_ServerData.ip = "192.168.235.1";
	//m_ServerData.port = 12345;
	char *ip = NULL;
	if (m_ServerData.ip != "0" && m_ServerData.ip != "")
	{
		ip = (char *)m_ServerData.ip.c_str();
	}
	if (!listener->listen(m_ServerData.port, ip))
	{
		return false;
	}

	ClientModule *pClientModel = new ClientModule();
	listener->setClientModule(pClientModel);
    m_Poller->addCommObject(listener, listener->getPollType());

	KX_LOGDEBUG("SessionServer Launching IP=%s Port=%d!", m_ServerData.ip.c_str(), m_ServerData.port);

	ConnectModule *pConnectModule = new ConnectModule();
	ConnectEvent *pGameEvent = new ConnectEvent();
	pConnectModule->init(pGameEvent);

	std::map<int, ServerData> alldaata = m_ServerConfig.getServerData();
	for (std::map<int, ServerData>::iterator iter = alldaata.begin(); iter != alldaata.end(); ++iter)
	{
		if (iter->second.name == this->getServerName())
		{
			continue;
		}
		SessionConnector *pConnector = new SessionConnector();
		if (!pConnector->init() || !pConnector->connect((char *)iter->second.ip.c_str(), iter->second.port, iter->second.serverId, true))
		{
			KX_LOGERROR("SessionServer Connect to %s: IP=%s, Port=%d Failed!", iter->second.name.c_str(), iter->second.ip.c_str(), iter->second.port);
			return false;
		}
		else
		{
			KX_LOGDEBUG("SessionServer Connect to %s: IP=%s, Port=%d Susessful!", iter->second.name.c_str(), iter->second.ip.c_str(), iter->second.port);
		}
		
		pConnector->setModule(pConnectModule);
		m_Poller->addCommObject(pConnector, pConnector->getPollType());
		NetWorkManager::getInstance()->addServer(iter->second.serverId, pConnector);
		KXSAFE_RELEASE(pConnector);
	}

	KXSAFE_RELEASE(pConnectModule);
	KXSAFE_RELEASE(listener);
	KXSAFE_RELEASE(pClientModel);

	return true;
}

bool SessionServer::initServerInfo()
{
	// ·þÎñÆ÷ÅäÖÃ
	if (!m_ServerConfig.create())
	{
		return false;
	}
	this->setServerName(SERVER_NAME_SESSION);
	m_ServerData = m_ServerConfig.getServerDataByName(this->getServerName());

	return true;
}

void SessionServer::setSessionServerClose()
{
	m_IsRunning = false;
}

void SessionServer::setSessionServerClosing()
{
	m_IsClosing = true;
}


