#include "UserManager.h"
#include "DBManager.h"
#include "RedisStorer.h"
#include "BaseServer.h"
#include "LogManager.h"
#include "ConfGameSetting.h"


using namespace std;
UserManager * UserManager::m_pInstance = nullptr;

UserManager::UserManager()
{
	m_VectServerModel.clear();
	m_DelUserList.clear();
	m_MapDelUserList.clear();
}

UserManager::~UserManager()
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

UserManager *UserManager::getInstance()
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new UserManager;
	}
	return m_pInstance;
}

void UserManager::destroy()
{
	if (nullptr != m_pInstance)
	{
		delete m_pInstance;
	}
}

bool UserManager::init(TimerManager *pTimerManager)
{
	m_TimeManager = pTimerManager;
	if (m_TimeManager == nullptr)
	{
		return false;
	}
	if (!m_TimeManager->addTimer(this, 300, KXREPEAT_FOREVER))
	{
		return false;
	}
	ConfManager::getInstance()->init();
	return true;
}

void UserManager::addModelType(ModelType Type)
{
	m_VectServerModel.push_back(Type);
}

void UserManager::onTimer(const TimeVal& now)
{
	int nCurTime = (int)time(nullptr);

	for (std::list<SDelayDelData>::iterator ator = m_DelUserList.begin(); ator != m_DelUserList.end();)
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

GameUser* UserManager::getGameUser(int uId)
{
	std::map<int, GameUser *>::iterator iter = m_GameUsers.find(uId);
	if (iter != m_GameUsers.end())
	{
		return iter->second;
	}
	return nullptr;
}

GameUser* UserManager::getGameUser(int uId, std::string userName, std::string passWord, bool noNull)
{
	std::map<int, GameUser *>::iterator iter = m_GameUsers.find(uId);
	if (iter != m_GameUsers.end())
	{
		std::string pass = iter->second->getPassWord();
		std::string username = iter->second->getUserName();
		if ((pass == passWord) && (username ==userName))
		{
			return iter->second;
		}
	}
	//数据没有数据的话直接给他创建一个新的
	if (noNull)
	{
		return initGameUser(uId, userName, passWord);
	}
	return nullptr;
}
//假如getGameUser没有这个玩家说明服务器中可能没有这个玩家的.先去数据库中找一下.没有再用newGameUser
GameUser* UserManager::initGameUser(int uid, std::string userName, std::string passWord)
{
	std::map<int, GameUser *>::iterator iter = m_GameUsers.find(uid);
	if (iter == m_GameUsers.end())
	{
		GameUser *pGameUser = new GameUser;
		if (!pGameUser->initModels(uid))
		{
			delete pGameUser;
			return NULL;
		}
		pGameUser->setUserName(userName);
		pGameUser->setPassWord(passWord);
		pGameUser->setUid(uid);

		m_GameUsers[uid] = pGameUser;
		return pGameUser;
	}

	return NULL;
}

GameUser *UserManager::newGameUser(int uId, std::string userName, std::string passWord)
{
	// 新用户
	GameUser *pGameUser = new GameUser;

	// 创建每个模型
	UserModel *pUserModel = new UserModel;
	pUserModel->init(uId);

	// 用户信息模型初始化
	pGameUser->setModel(MODELTYPE_USER, pUserModel);
	pGameUser->setUid(uId);
	pGameUser->setUserName(userName);
	pGameUser->setPassWord(passWord);
	// 添加到玩家管理器
	addGameUser(uId, pGameUser);

	// 用户基本信息
	std::map<int, int> attrs;
	for (int i = USR_FD_USERID; i < USR_FD_END; i++)
		attrs[i] = 0;		// 所有属性默认为0

	ConfNewPlayerSetting* conf = (ConfNewPlayerSetting*)ConfManager::getInstance()->getConf(CONF_NEW_PLAYER);
	NewPlayerItem* newItem = (NewPlayerItem*)conf->getData(CONF_NEW_PLAYER);
	attrs[USR_FD_USERID] = uId;
	attrs[USR_FD_PERMISSION] = 0;
	attrs[USR_FD_USERLV] = newItem->UserLv;
	attrs[USR_FD_EXP] = newItem->UserExp;
	attrs[USR_FD_GOLD] = newItem->Gold;
	attrs[USR_FD_DIAMOID] = newItem->Diamond;
	int nCreateTime = BaseServer::getInstance()->getCurrentTime();
	attrs[USR_FD_CREATETIME] = nCreateTime;
	//写数据进数据库
	if (!pUserModel->writeNewUserData(uId, userName, passWord, attrs))
	{
		// 用户数据初始化失败
		return nullptr;
	}

	return pGameUser;
}


void UserManager::addGameUser(int uId, GameUser* gameUsr)
{
	std::map<int, GameUser *>::iterator iter = m_GameUsers.find(uId);
	if (iter == m_GameUsers.end())
	{
		m_GameUsers[uId] = gameUsr;
	}
}

//检测用户在该服务器是否存在
bool UserManager::checkUserIsExist(int uid)
{
	std::map<int, GameUser *>::iterator iter = m_GameUsers.find(uid);
	if (iter == m_GameUsers.end())
	{
		return false;
	}
	return true;
}

void UserManager::removeGameUser(int uid)
{
	if (m_TimeManager == nullptr)
	{
		return;
	}

	std::list<SDelayDelData>::iterator ator;
	SDelayDelData DelData;

	DelData.nDelayTime = (int)time(nullptr) + 3600;
	DelData.nUid = uid;
	ator = m_DelUserList.insert(m_DelUserList.end(), DelData);
	m_MapDelUserList[uid] = ator;
}

//删除移除用户数据
void UserManager::donotDeleteUser(int uid)
{
	std::map<int, std::list<SDelayDelData>::iterator>::iterator ator = m_MapDelUserList.find(uid);
	if (ator != m_MapDelUserList.end())
	{
		std::list<SDelayDelData>::iterator iter = ator->second;
		m_DelUserList.erase(iter);
		m_MapDelUserList.erase(ator);
	}
}

//真正删除用户
void UserManager::RealremoveGameUser(int uid)
{
	std::map<int, GameUser *>::iterator iter = m_GameUsers.find(uid);
	if (iter != m_GameUsers.end())
	{
		delete iter->second;
		m_GameUsers.erase(iter);
	}
}

