#include "SessionServer.h"

#include "SessionListener.h"
#include "ModuleClient.h"
#include "ModuleConnect.h"
#include "SessionConnect.h"
#include "NetworkManager.h"
#include "core/KxPlatform.h"
#include "log/LogManager.h"

#include "EventConnect.h"


using namespace std;

SessionServer* SessionServer::m_Instance = nullptr;
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
	if (nullptr == m_Instance)
    {
        m_Instance = new SessionServer();
    }
    return m_Instance;
}

void SessionServer::destroy()
{
	if (nullptr != m_Instance)
    {
        delete m_Instance;
		m_Instance = nullptr;
    }
}

bool SessionServer::onServerInit()
{
	KxBaseServer::onServerInit();
	KX_LOGDEBUG("======================onServerInit============================");

	//1.³õÊ¼»¯ÂÖÑ¯Æ÷
#if(KX_TARGET_PLATFORM == KX_PLATFORM_LINUX)
	m_Poller = new KxEpoller();
#else
	m_Poller = new KxSelectPoller();
#endif

    SessionListener* listener = new SessionListener();
	if (!listener->init())      return false;

	char *ip = nullptr;
	if (m_ServerData.ip != "0" && m_ServerData.ip != "")
		ip = (char *)m_ServerData.ip.c_str();

	if (!listener->listen(m_ServerData.port, ip))	
		return false;

	KX_LOGDEBUG("SessionServer Launching IP=%s Port=%d!", m_ServerData.ip.c_str(), m_ServerData.port);

	ModuleClient *pClientModel = new ModuleClient();
	listener->setClientModule(pClientModel);
    m_Poller->addCommObject(listener, listener->getPollType());


	ModuleConnect *pConnectModule = new ModuleConnect();
	EventConnect *pGameEvent = new EventConnect();
	pConnectModule->init(pGameEvent);

	std::map<int, ServerData> alldata = m_ServerConfig.getServerData();
	for (std::map<int, ServerData>::iterator iter = alldata.begin(); iter != alldata.end(); ++iter)
	{
		if (iter->second.name == this->getServerName())
			continue;

		SessionConnect *pConnector = new SessionConnect();
		if (!pConnector->init() || !pConnector->connect((char *)iter->second.ip.c_str(), iter->second.port, iter->second.serverId, true))
		{
			KX_LOGERROR("SessionServer Connect to %s: IP=%s, Port=%d Failed!", iter->second.name.c_str(), iter->second.ip.c_str(), iter->second.port);
			return false;
		}
		else
			KX_LOGDEBUG("SessionServer Connect to %s: IP=%s, Port=%d Susessful!", iter->second.name.c_str(), iter->second.ip.c_str(), iter->second.port);

		
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


