#include "LoginServer.h"
#include <string>
#include "KxPlatform.h"
#include "SessionModule.h"
#include "LoginListener.h"
#include "UserData/UserManager.h"
#include "DBDriver/DBManager.h"

using namespace std;

LoginServer* LoginServer::m_Instance = nullptr;
LoginServer::LoginServer(void)
:m_IsClosing(false)
{

}

LoginServer::~LoginServer(void)
{
	KXSAFE_RELEASE(m_Poller);
}

LoginServer* LoginServer::getInstance()
{
    if (nullptr == m_Instance)
    {
        m_Instance = new LoginServer();
    }
    return m_Instance;
}

void LoginServer::destroy()
{
	DBManager::getInstance()->destroy();
	LogManager::getInstance()->destroy();

    if (nullptr != m_Instance)
    {
        delete m_Instance;
        m_Instance = nullptr;
    }
}

bool LoginServer::onServerInit()
{
	this->setServerName("LoginServer");
	KxBaseServer::onServerInit();
	
	KX_LOGDEBUG("==================================================");

	//1.��ʼ����ѯ��
#if(KX_TARGET_PLATFORM == KX_PLATFORM_LINUX)
	m_Poller = new KxEpoller();
#else
	m_Poller = new KxSelectPoller();
#endif

	LoginListener* listener = new LoginListener();
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
		KX_LOGDEBUG("LoginServer listen IP=%s Port=%d Failed!", m_ServerData.ip.c_str(), m_ServerData.port);
		return false;
	}
	else
	{
		KX_LOGDEBUG("LoginServer listen IP=%s Port=%d Successful!", m_ServerData.ip.c_str(), m_ServerData.port);
	}

	SessionModule *pClientModel = new SessionModule();
	listener->setClientModule(pClientModel);
    m_Poller->addCommObject(listener, listener->getPollType());
	
	KXSAFE_RELEASE(listener);
	KXSAFE_RELEASE(pClientModel);
	
	// ��ʼ�����ݿ�
	//��������,һ��Ҫ��redis���ܰ� ��Ȼ�������Ķ���֪��
	if (!DBManager::getInstance()->InitWithXML(STORAGE_XML_FILE))
	{
		return false;
	}

	AddModelType();

	return true;
}

void LoginServer::AddModelType()
{
	//��ʼ��
	UserManager::getInstance()->init(getTimerManager());
	//���ظ÷�������������ģ��
	UserManager::getInstance()->addModelType(MODELTYPE_USER);
}

void LoginServer::setSessionServerClose()
{
	m_IsRunning = false;
}

void LoginServer::setSessionServerClosing()
{
	m_IsClosing = true;
}


bool LoginServer::initServerInfo()
{
	// ����������
	if (!m_ServerConfig.create())
	{
		return false;
	}
	this->setServerName(SERVER_NAME_LOGIN);
	m_ServerData = m_ServerConfig.getServerDataByName(this->getServerName());
	return true;
}
