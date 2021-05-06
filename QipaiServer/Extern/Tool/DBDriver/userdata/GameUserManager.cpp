#include "GameUserManager.h"
#include "DBDriver/DBManager.h"
#include "DBDriver/RedisStorer.h"

GameUserManager * GameUserManager::m_pInstance = NULL;

GameUserManager::GameUserManager()
{
	m_VectServerModel.clear();
	m_DelUserList.clear();
	m_MapDelUserList.clear();
}

GameUserManager::~GameUserManager()
{
	for (std::map<int, GameUser *>::iterator iter = m_GameUsers.begin();
		iter != m_GameUsers.end(); ++iter)
	{
		delete iter->second;
	}

	m_VectServerModel.clear();
	m_DelUserList.clear();
	m_MapDelUserList.clear();
}



GameUserManager *GameUserManager::getInstance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new GameUserManager;
	}
	return m_pInstance;
}

void GameUserManager::destroy()
{
	if (NULL != m_pInstance)
	{
		delete m_pInstance;
	}
}

bool GameUserManager::init(KxTimerManager *pTimerManager)
{
	m_TimeManager = pTimerManager;

	if (m_TimeManager == NULL)
	{
		return false;
	}

	if (!m_TimeManager->addTimer(this, 300, KXREPEAT_FOREVER))
	{
		return false;
	}

	return true;
}

void GameUserManager::addModelType(ModelType Type)
{
	m_VectServerModel.push_back(Type);
}

void GameUserManager::onTimer(const kxTimeVal& now)
{
	int nCurTime = (int)time(NULL);

	for (std::list<SDelayDelData>::iterator ator = m_DelUserList.begin();
        ator != m_DelUserList.end();)
	{
		if (nCurTime > ator->nDelayTime)
		{
			RealremoveGameUser(ator->nUid);
			m_MapDelUserList.erase(ator->nUid);
			m_DelUserList.erase(ator++);
		}
		else
		{
			break;
		}
	}
}

GameUser* GameUserManager::getGameUser(int uid, int accountId, bool noNull)
{
	std::map<int, GameUser *>::iterator iter = m_GameUsers.find(uid);
	if(iter != m_GameUsers.end())
	{
		return iter->second;
	}

	if (noNull)
	{
		return initGameUser(uid, accountId);
	}
	return NULL;
}

GameUser* GameUserManager::getGameUser(int uid)
{
	std::map<int, GameUser *>::iterator iter = m_GameUsers.find(uid);
	if (iter != m_GameUsers.end())
	{
		return iter->second;
	}
	return NULL;
}
GameUser* GameUserManager::initGameUser(int uid, int accountId)
{
	std::map<int, GameUser *>::iterator iter = m_GameUsers.find(uid);
	if (iter == m_GameUsers.end())
	{
		GameUser *pGameUser = new GameUser;
		if (!pGameUser->initModels(uid, accountId))
		{
			delete pGameUser;
			return NULL;
		}

		m_GameUsers[uid] = pGameUser;
		initUserData(pGameUser);
		return pGameUser;
	}

	return NULL;
}

GameUser *GameUserManager::newGameUser(int uid, int accountId)
{
	// 新用户
	GameUser *pGameUser = new GameUser;
	int nCreateTime = KxBaseServer::getInstance()->getTimerManager()->getTimestamp();
	// 创建每个模型
	UserModel *pUserModel = new UserModel;
	pUserModel->init(accountId);

	pGameUser->setModel(MODELTYPE_USER, pUserModel);
	pGameUser->setUid(uid);
	pGameUser->setAccountId(accountId);

	// 添加到玩家管理器
	addGameUser(uid, pGameUser);

	// 用户信息模型初始化
	char name[32] = {};
	std::string randName = "Name-" + std::to_string(accountId);
	sprintf_s(name, "%s", randName);
	pGameUser->setUserName(name);

	// 用户基本信息
	std::map<int, int> attrs;
	for (int i = USR_ACCOUNDID; i < USR_FD_END; i++)
	{
		// 所有属性默认为0
		attrs[i] = 0;
	}

	attrs[USR_ACCOUNDID] = accountId;
	attrs[USR_FD_USERLV] = 1;
	attrs[USR_FD_EXP] = 0;
	attrs[USR_FD_GOLD] = 1;
	attrs[USR_FD_CREATETIME] = nCreateTime;

	if (!pUserModel->NewUser(accountId, name, attrs))
	{
		// 用户数据初始化失败
		return NULL;
	}

	initUserData(pGameUser);
	return pGameUser;
}

//更新用户需要重置的数据
void GameUserManager::reSetGameUserData(int uid, bool bLogin)
{
	GameUser *pGameUser = getGameUser(uid);
	if (pGameUser == NULL)
	{
		return;
	}
    initUserData(pGameUser);
    updateGameUserData(pGameUser, bLogin);
}

void GameUserManager::updateGameUserData(GameUser* gameUsr, bool bLogin)
{
    if (gameUsr == NULL)
    {
        return;
    }
    int uid = gameUsr->getUid();

}

void GameUserManager::addGameUser(int uid, GameUser* gameUsr)
{
	std::map<int, GameUser *>::iterator iter = m_GameUsers.find(uid);
	if (iter == m_GameUsers.end())
	{
		m_GameUsers[uid] = gameUsr;
	}
}

//检测用户在该服务器是否存在
bool GameUserManager::checkUserIsExist(int uid)
{
	std::map<int, GameUser *>::iterator iter = m_GameUsers.find(uid);
	if (iter == m_GameUsers.end())
	{
		return false;
	}
	return true;
}

void GameUserManager::removeGameUser(int uid)
{
	if (m_TimeManager == NULL)
	{
		return;
	}

	std::list<SDelayDelData>::iterator ator;
	SDelayDelData DelData;

	DelData.nDelayTime = (int)time(NULL) + 3600;
	DelData.nUid = uid;
	ator = m_DelUserList.insert(m_DelUserList.end(), DelData);
	m_MapDelUserList[uid] = ator;
}

//删除移除用户数据
void GameUserManager::donotDeleteUser(int uid)
{
	std::map<int, std::list<SDelayDelData>::iterator>::iterator ator = m_MapDelUserList.find(uid);
	if (ator != m_MapDelUserList.end())
	{
		std::list<SDelayDelData>::iterator iter = ator->second;
		m_DelUserList.erase(iter);
		m_MapDelUserList.erase(ator);
	}
}

bool GameUserManager::initUserData(GameUser* gameUsr)
{
	if (gameUsr == NULL)
	{
		return false;
	}

	return true;
}

//真正删除用户
void GameUserManager::RealremoveGameUser(int uid)
{
	std::map<int, GameUser *>::iterator iter = m_GameUsers.find(uid);
	if (iter != m_GameUsers.end())
	{
		delete iter->second;
		m_GameUsers.erase(iter);
	}
}
