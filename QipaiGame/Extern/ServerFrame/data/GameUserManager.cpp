#include "GameUserManager.h"
#include "DBDriver/DBManager.h"
#include "DBDriver/RedisStorer.h"

CGameUserManager * CGameUserManager::m_pInstance = NULL;

CGameUserManager::CGameUserManager()
{
	m_VectServerModel.clear();
	m_DelUserList.clear();
	m_MapDelUserList.clear();
}

CGameUserManager::~CGameUserManager()
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



CGameUserManager *CGameUserManager::getInstance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CGameUserManager;
	}
	return m_pInstance;
}

void CGameUserManager::destroy()
{
	if (NULL != m_pInstance)
	{
		delete m_pInstance;
	}
}

bool CGameUserManager::init(KxTimerManager *pTimerManager)
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

void CGameUserManager::addModelType(ModelType Type)
{
	m_VectServerModel.push_back(Type);
}

void CGameUserManager::onTimer(const kxTimeVal& now)
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

GameUser* CGameUserManager::getGameUser(int uid, bool noNull)
{
	std::map<int, GameUser *>::iterator iter = m_GameUsers.find(uid);
	if (iter != m_GameUsers.end())
	{
		return iter->second;
	}

	if (noNull)
	{
		return initGameUser(uid);
	}
	return NULL;
}

GameUser* CGameUserManager::initGameUser(int uid)
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

		m_GameUsers[uid] = pGameUser;
		initUserData(pGameUser);
		return pGameUser;
	}

	return NULL;
}

GameUser *CGameUserManager::newGameUser(int uid, int accountId)
{
	// ���û�
	GameUser *pGameUser = new GameUser;
	int nCreateTime = KxBaseServer::getInstance()->getTimerManager()->getTimestamp();
	// ����ÿ��ģ��
	UserModel *pUserModel = new UserModel;

	pGameUser->setModel(MODELTYPE_USER, pUserModel);

	pGameUser->setUid(uid);
	pGameUser->setAccountId(accountId);

	// ��ӵ���ҹ�����
	addGameUser(uid, pGameUser);

	// �û���Ϣģ�ͳ�ʼ��
	char name[32] = {};

	std::string randName = "DefaultName-" + std::to_string(uid);
	sprintf_s(name, "%s", randName);
	pGameUser->setUserName(name);

	// �û�������Ϣ
	std::map<int, int> attrs;
	for (int i = USR_FD_ACCOUNTID; i < USR_FD_END; i++)
	{
		// ��������Ĭ��Ϊ0
		attrs[i] = 0;
	}

	attrs[USR_FD_ACCOUNTID] = uid;
	attrs[USR_FD_USERLV] = 1;
	attrs[USR_FD_EXP] = 0;
	attrs[USR_FD_GOLD] = 1;
	attrs[USR_FD_DIAMOND] = 1;
	attrs[USR_FD_CREATETIME] = nCreateTime;

	if (!pUserModel->NewUser(uid, name, attrs))
	{
		// �û����ݳ�ʼ��ʧ��
		return NULL;
	}

	initUserData(pGameUser);
	return pGameUser;
}

//�����û���Ҫ���õ�����
void CGameUserManager::reSetGameUserData(int uid, bool bLogin)
{
	GameUser *pGameUser = getGameUser(uid);
	if (pGameUser == NULL)
	{
		return;
	}
    initUserData(pGameUser);
    updateGameUserData(pGameUser, bLogin);
}

void CGameUserManager::updateGameUserData(GameUser* gameUsr, bool bLogin)
{
    if (gameUsr == NULL)
    {
        return;
    }
    int uid = gameUsr->getUid();

}

void CGameUserManager::addGameUser(int uid, GameUser* gameUsr)
{
	std::map<int, GameUser *>::iterator iter = m_GameUsers.find(uid);
	if (iter == m_GameUsers.end())
	{
		m_GameUsers[uid] = gameUsr;
	}
}

//����û��ڸ÷������Ƿ����
bool CGameUserManager::checkUserIsExist(int uid)
{
	std::map<int, GameUser *>::iterator iter = m_GameUsers.find(uid);
	if (iter == m_GameUsers.end())
	{
		return false;
	}
	return true;
}

void CGameUserManager::removeGameUser(int uid)
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

//ɾ���Ƴ��û�����
void CGameUserManager::donotDeleteUser(int uid)
{
	std::map<int, std::list<SDelayDelData>::iterator>::iterator ator = m_MapDelUserList.find(uid);
	if (ator != m_MapDelUserList.end())
	{
		std::list<SDelayDelData>::iterator iter = ator->second;
		m_DelUserList.erase(iter);
		m_MapDelUserList.erase(ator);
	}
}

bool CGameUserManager::initUserData(GameUser* gameUsr)
{
	if (gameUsr == NULL)
	{
		return false;
	}

	return true;
}

//����ɾ���û�
void CGameUserManager::RealremoveGameUser(int uid)
{
	std::map<int, GameUser *>::iterator iter = m_GameUsers.find(uid);
	if (iter != m_GameUsers.end())
	{
		delete iter->second;
		m_GameUsers.erase(iter);
	}
}
