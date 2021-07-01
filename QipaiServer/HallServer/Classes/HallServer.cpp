#include "HallServer.h"
#include "SessionModule.h"
#include "HallListener.h"
#include "UserManager.h"
#include "DBManager.h"
#include "SelectPoller.h"
#include "LogManager.h"
using namespace std;

HallServer* HallServer::m_Instance = nullptr;
HallServer::HallServer(void)
:m_IsClosing(false)
{

}

HallServer::~HallServer(void)
{
	KXSAFE_RELEASE(m_Poller);
}

HallServer* HallServer::getInstance()
{
    if (nullptr == m_Instance)
    {
        m_Instance = new HallServer();
    }
    return m_Instance;
}

void HallServer::destroy()
{
	DBManager::getInstance()->destroy();
	LogManager::getInstance()->destroy();

    if (nullptr != m_Instance)
    {
        delete m_Instance;
        m_Instance = nullptr;
    }
}

bool HallServer::onServerInit()
{
	BaseServer::onServerInit();
	
	LOGDEBUG("==================================================");

	//1.初始化轮询器
#if(KX_TARGET_PLATFORM == KX_PLATFORM_LINUX)
	m_Poller = new Epoller();
#else
	m_Poller = new SelectPoller();
#endif

	HallListener* listener = new HallListener();
	if (!listener->init())
    {
        return false;
    }
	m_ServerData = m_ServerConfig.getServerDataByName(this->getServerName());
	char *ip = nullptr;
	if (m_ServerData.ip != "0" && m_ServerData.ip != "")
	{
		ip = (char *)m_ServerData.ip.c_str();
	}
	if (!listener->listen(m_ServerData.port, ip))
	{
		LOGDEBUG("%s listen IP=%s Port=%d Failed!", m_ServerData.name.c_str(), m_ServerData.ip.c_str(), m_ServerData.port);
		return false;
	}
	else
	{
		LOGDEBUG("%s listen IP=%s Port=%d Successful!", m_ServerData.name.c_str(), m_ServerData.ip.c_str(), m_ServerData.port);
	}

	SessionModule *pClientModel = new SessionModule();
	listener->setClientModule(pClientModel);
    m_Poller->addCommObject(listener, listener->getPollType());
	
	KXSAFE_RELEASE(listener);
	KXSAFE_RELEASE(pClientModel);
	
	// 初始化数据库
	//这里好奇怪,一定要开redis再跑啊 不然错误在哪都不知道
	if (!DBManager::getInstance()->InitWithXML(STORAGE_XML_FILE))
	{
		return false;
	}

	AddModelType();

	return true;
}

void HallServer::AddModelType()
{
	//初始化
	UserManager::getInstance()->init(getTimerManager());
	//加载该服务器创建所需模型
	UserManager::getInstance()->addModelType(MODELTYPE_USER);
}

void HallServer::setSessionServerClose()
{
	m_IsRunning = false;
}

void HallServer::setSessionServerClosing()
{
	m_IsClosing = true;
}


bool HallServer::initServerInfo()
{
	// 服务器配置
	if (!m_ServerConfig.create())
	{
		return false;
	}
	this->setServerName(SERVER_NAME_HALL);
	m_ServerData = m_ServerConfig.getServerDataByName(this->getServerName());
	return true;
}
