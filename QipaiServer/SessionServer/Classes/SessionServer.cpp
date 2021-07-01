#include "SessionServer.h"

#include "SessionListener.h"
#include "ModuleClient.h"
#include "ModuleConnect.h"
#include "SessionConnect.h"
#include "NetManager.h"
#include "SelectPoller.h"
#include "EventConnect.h"
#include "LogManager.h"

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
        m_Instance = new SessionServer();
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
	BaseServer::onServerInit();
	LOGDEBUG("======================onServerInit============================");
	//³õÊ¼»¯ÂÖÑ¯Æ÷
#if(KX_TARGET_PLATFORM == KX_PLATFORM_LINUX)
	m_Poller = new Epoller();
#else
	m_Poller = new SelectPoller();
#endif

    SessionListener* listener = new SessionListener();
	if (!listener->init())      return false;
	if (!listener->listen(m_ServerData.port, (char*)m_ServerData.ip.c_str()))
		return false;

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
		pConnector->setServerName(iter->second.name);
		if (!pConnector->init() || !pConnector->connect((char *)iter->second.ip.c_str(), iter->second.port, iter->second.serverId, true))
		{
			LOGERROR("%s Connect to %s: IP=%s, Port=%d Failed!", m_ServerData.name.c_str(), iter->second.name.c_str(), iter->second.ip.c_str(), iter->second.port);
			return false;
		}
		pConnector->setModule(pConnectModule);
		m_Poller->addCommObject(pConnector, pConnector->getPollType());
		NetManager::getInstance()->addServer(iter->second.serverId, pConnector);
		KXSAFE_RELEASE(pConnector);
	}
	KXSAFE_RELEASE(pConnectModule);
	KXSAFE_RELEASE(listener);
	KXSAFE_RELEASE(pClientModel);

	LOGDEBUG("%s Launching IP=%s Port=%d!", m_ServerData.name.c_str(), m_ServerData.ip.c_str(), m_ServerData.port);
	return true;
}

bool SessionServer::initServerInfo()
{
	if (!m_ServerConfig.create())	// ·þÎñÆ÷ÅäÖÃ
		return false;
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


