#include "UserManager.h"
#include "DBManager.h"
#include "RedisStorer.h"
#include "BaseServer.h"

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
	for (std::map<string, GameUser *>::iterator iter = m_GameUsers.begin(); iter != m_GameUsers.end(); ++iter)
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

GameUser* UserManager::getGameUser(int uid, char* passWord, bool noNull)
{
	std::map<std::string, GameUser *>::iterator iter = m_GameUsers.find(to_string(uid));
	if(iter != m_GameUsers.end())
	{
		char* pass = iter->second->getPassWord();
		if (strcmp(pass, passWord))
		{
			return iter->second;
		}
	}

	if (noNull)
	{
		return initGameUser(uid);
	}
	return nullptr;
}
GameUser* UserManager::getGameUser(int uid, bool noNull)
{
	std::map<std::string, GameUser *>::iterator iter = m_GameUsers.find(to_string(uid));
	if (iter != m_GameUsers.end())
	{
		return iter->second;
	}
	if (noNull)
	{
		return initGameUser(uid);
	}
	return nullptr;
}

GameUser* UserManager::initGameUser(int uid)
{
	std::map<string, GameUser *>::iterator iter = m_GameUsers.find(to_string(uid));
	if (iter == m_GameUsers.end())
	{
		GameUser *pGameUser = new GameUser;
		if (!pGameUser->initModels(uid))
		{
			delete pGameUser;
			return nullptr;
		}

		m_GameUsers[to_string(uid)] = pGameUser;
		return pGameUser;
	}

	return nullptr;
}

GameUser *UserManager::newGameUser(int uid, char* passWord)
{
	// ���û�
	GameUser *pGameUser = new GameUser;
	int nCreateTime = BaseServer::getInstance()->getTimerManager()->getTimestamp();
	// ����ÿ��ģ��
	UserModel *pUserModel = new UserModel;
	pUserModel->init(uid);

	pGameUser->setModel(MODELTYPE_USER, pUserModel);
	pGameUser->setUid(uid);


	// ��ӵ���ҹ�����
	addGameUser(uid, pGameUser);

	// �û���Ϣģ�ͳ�ʼ��
	std::string name = "Name-" + std::to_string(uid);
	pGameUser->setUserName(name);

	char password[16] = {};
	memcpy(password, passWord, sizeof(password));
	pGameUser->setPassWord(password);

	// �û�������Ϣ
	std::map<int, int> attrs;
	for (int i = USR_FD_USERID; i < USR_FD_END; i++)
		attrs[i] = 0;		// ��������Ĭ��Ϊ0


	attrs[USR_FD_USERID] = uid;
	attrs[USR_FD_PERMISSION] = 0;
	attrs[USR_FD_USERLV] = 1;
	attrs[USR_FD_EXP] = 1;
	attrs[USR_FD_GOLD] = 100;
	attrs[USR_FD_DIAMOID] = 1000;
	attrs[USR_FD_CREATETIME] = nCreateTime;

	if (!pUserModel->NewUser(uid, name, password,attrs))
	{
		// �û����ݳ�ʼ��ʧ��
		return nullptr;
	}
	return pGameUser;
}

//�����û���Ҫ���õ�����
void UserManager::reSetGameUserData(int uid, bool bLogin)
{
	GameUser *pGameUser = getGameUser(uid);
	if (pGameUser == nullptr)
	{
		return;
	}
    updateGameUserData(pGameUser, bLogin);
}

void UserManager::updateGameUserData(GameUser* gameUsr, bool bLogin)
{
    if (gameUsr == nullptr)
    {
        return;
    }
    int uid = gameUsr->getUid();

}

void UserManager::addGameUser(int uid, GameUser* gameUsr)
{
	std::map<string, GameUser *>::iterator iter = m_GameUsers.find(to_string(uid));
	if (iter == m_GameUsers.end())
	{
		m_GameUsers[to_string(uid)] = gameUsr;
	}
}

//����û��ڸ÷������Ƿ����
bool UserManager::checkUserIsExist(int uid)
{
	std::map<string, GameUser *>::iterator iter = m_GameUsers.find(to_string(uid));
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

//ɾ���Ƴ��û�����
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

//����ɾ���û�
void UserManager::RealremoveGameUser(int uid)
{
	std::map<string, GameUser *>::iterator iter = m_GameUsers.find(to_string(uid));
	if (iter != m_GameUsers.end())
	{
		delete iter->second;
		m_GameUsers.erase(iter);
	}
}
