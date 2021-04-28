#include <string.h>
#include "GameNetworkNode.h"
#include "GameModule.h"
#include "helper/BufferData.h"
#include "protocol/MainProtocol.h"
#include "server/Head.h"


using namespace std;

USING_NS_CC;

GameNetworkNode::GameNetworkNode(void)
: m_bUserClose(false)
, m_bEnabledHb(false)
, m_fMaxHbTime(5.0f)
, m_fTickNet(0.0f)
, m_pPoller(NULL)
, m_pGameModule(NULL)
, m_ServerCon(NULL)
{
}


GameNetworkNode* GameNetworkNode::create()
{
	GameNetworkNode *pRet = new(std::nothrow) GameNetworkNode();
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

GameNetworkNode::~GameNetworkNode(void)
{
	KXSAFE_RELEASE(m_pPoller);
	KXSAFE_RELEASE(m_pGameModule);
	delete m_ServerCon;
}

GameNetworkNode *GameNetworkNode::m_pInstance = NULL;

GameNetworkNode *GameNetworkNode::getInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new GameNetworkNode;
		if (!m_pInstance->init())
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

	return m_pInstance;
}

void GameNetworkNode::destroy()
{
	if (NULL != m_pInstance)
	{
		m_pInstance->release();
	}
}

bool GameNetworkNode::init()
{
	m_pPoller = new KxSelectPoller();
	m_pGameModule = new GameModule();
	m_ServerCon = new ConnectServerConf();
	return onServerInit();
}

void GameNetworkNode::onEnter()
{
	//CCNode::onEnter();
}

void GameNetworkNode::onExit()
{
	//CCNode::onExit();
}

void GameNetworkNode::update(float dt)
{
	if (m_bEnabledHb)
	{
		// 心跳发送, 如果n秒内没有数据交互, 这时才发送心跳包
		m_fTickNet += 0.05f;
		if (m_fTickNet >= m_fMaxHbTime)
		{
			heartbeatScheduler();
		}
	}
}
void GameNetworkNode::visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
	Node::visit(renderer, parentTransform, parentFlags);
	if (NULL != m_pPoller)
	{
		// 轮询网络
		m_pPoller->poll();
	}
}

bool GameNetworkNode::onServerInit()
{
	LogManager::getInstance()->setShowTime(false);
	LogManager::getInstance()->setShowDate(false);
	LogManager::getInstance()->setShowFunc(false);
	LogManager::getInstance()->addHandler(1, new LogConsoleHandler());
	LogFileHandler* pFileHandle = new LogFileHandler();
	pFileHandle->setFilePath("../bin/");
	pFileHandle->setFileName("TestClientOut");
	pFileHandle->setFastModel(false);
	LogManager::getInstance()->addHandler(2, pFileHandle);

	m_ServerCon->serverGroup = 1;
	m_ServerCon->host = "127.0.0.1";
	m_ServerCon->port = 8888;

#if KX_PLATFORM_WIN32 != KX_TARGET_PLATFORM
	// 域名解析成ip
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	struct hostent *host = gethostbyname(m_ServerCon->host.c_str());
	if (!host)
	{
		puts("Get IP address error!");
		system("pause");
		exit(0);
	}
	//别名
	for (int i = 0; host->h_aliases[i]; i++)
	{
		printf("Aliases %d: %s\n", i + 1, host->h_aliases[i]);
	}
	//地址类型
	printf("Address type: %s\n", (host->h_addrtype == AF_INET) ? "AF_INET" : "AF_INET6");
	//IP地址
	for (int i = 0; host->h_addr_list[i]; i++)
	{
		printf("IP addr %d: %s\n", i + 1, inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
	}
#endif

	if (connectToServer((char *)m_ServerCon->host.c_str(), m_ServerCon->port))
	{
		KX_LOGDEBUG("ClientTest Connect to IP=%s Port=%d successful", m_ServerCon->host.c_str(), m_ServerCon->port);
	}
	else
		KX_LOGDEBUG("ClientTest Connect to IP=%s Port=%d failed", m_ServerCon->host.c_str(), m_ServerCon->port);

	return true;
}

bool GameNetworkNode::connectToServer(const char *ip, int port, EServerConnType connType, KXSOCK_VERSION version /*= KXV_IPV4*/, std::function<void(bool)> callBack /*= nullptr*/)
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

	if (!pConnector->isConnecting() && !pConnector->connect(ip, port))
	{
		KX_LOGDEBUG("error: m_pConnector connect faile %d", pConnector->getCommId());
		pConnector->release();
		return false;
	}

	m_pPoller->addCommObject(pConnector, pConnector->getPollType());

	m_bUserClose = false;
	return true;
}

bool GameNetworkNode::reconnectToServer(EServerConnType connType, std::function<void(bool)> callBack /*= nullptr*/)
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

int GameNetworkNode::sendData(char* buffer, unsigned int len, EServerConnType connType /*= SERVER_CONN_SESSION*/)
{
	KxTCPConnector*pConnector = getConnector(connType);
	if (nullptr != pConnector && !pConnector->isConnecting())
	{
		return pConnector->sendData(buffer, len);
	}
	return -1;
}
void GameNetworkNode::setIsHeartbeat(bool bEnabled)
{
	m_bEnabledHb = bEnabled;
}

void GameNetworkNode::setHeartbeatTime(float fHbTime)
{
	m_fMaxHbTime = fHbTime;
}

void GameNetworkNode::closeConnect()
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

KxTCPConnector *GameNetworkNode::getConnector(EServerConnType connType /*= SERVER_CONN_SESSION*/)
{
	map<int, ServerConn>::iterator iter = m_mapServerConns.find(connType);
	if (iter != m_mapServerConns.end())
	{
		return iter->second.Connector;
	}

	return nullptr;
}

ServerConn* GameNetworkNode::getServerConn(EServerConnType connType /*= SERVER_CONN_SESSION*/)
{
	map<int, ServerConn>::iterator iter = m_mapServerConns.find(connType);
	if (iter != m_mapServerConns.end())
	{
		return &(iter->second);
	}

	return nullptr;
}

void GameNetworkNode::heartbeatScheduler()
{
	BufferData* buffer = newBufferData(MAIN_CMD::CMD_HEARTBEART, MAIN_CMD::CMD_HEARTBEART);

	//重新写入数据长度
	char* bu = buffer->getBuffer();
	Head* head = reinterpret_cast<Head*>(bu);
	head->length = buffer->getDataLength();
	head->uid = -1;//服务器用ID

	if (GameNetworkNode::getInstance()->sendData(buffer->getBuffer(), buffer->getDataLength()) > 0)
	{
		KX_LOGDEBUG("发送心跳包成功");
	}

	deleteBufferData(buffer);
}

void GameNetworkNode::registeNetworkCommunicate(void *data)
{
	m_fTickNet = 0;
}

