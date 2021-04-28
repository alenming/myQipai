#include "UserModel.h"

#include "helper/ModelDef.h"

#include "DBDriver/DBManager.h"
#include "DBDriver/RedisStorer.h"

UserModel::UserModel()
	: m_nUid(0)
	, m_pStorer(NULL)
{
}

UserModel::~UserModel()
{
}

bool UserModel::init(int uid)
{
	m_nUid = uid;
	m_pStorer = DBManager::getInstance()->GetStorer(DB_USER_MODEL)->storer;
	RedisStorer *pStorer = reinterpret_cast<RedisStorer*>(m_pStorer);
	if (NULL != pStorer)
	{
		m_strUsrKey = ModelKey::UsrKey(m_nUid);
		if (SUCCESS != pStorer->ExistKey(m_strUsrKey))
		{
			return false;
		}
	}

	Refresh();
	return true;
}

bool UserModel::Refresh()
{
	RedisStorer *pStorer = reinterpret_cast<RedisStorer*>(m_pStorer);
	if (NULL != pStorer)
	{
		for (int i = USR_FD_ACCOUNTID; i < USR_FD_END; i++)
		{
			m_mapUserInfo[i] = 0;
		}

		if (SUCCESS != pStorer->GetHashByField(m_strUsrKey, m_mapUserInfo))
		{
			// 没有用户信息
			return false;
		}

		if (SUCCESS != pStorer->GetHashByField(m_strUsrKey, USR_FD_USERNAME, m_strUserName))
		{
			// 用户不存在
			return false;
		}

		return true;
	}

	return false;
}

bool UserModel::NewUser(int uid, std::string name, std::map<int, int> &info)
{
	m_nUid = uid;
	m_mapUserInfo = info;
	m_pStorer = DBManager::getInstance()->GetStorer(DB_USER_MODEL)->storer;
	RedisStorer *pStorer = reinterpret_cast<RedisStorer*>(m_pStorer);
	if (NULL != pStorer)
	{
		m_strUsrKey = ModelKey::UsrKey(m_nUid);

		if (SUCCESS != pStorer->SetHash(m_strUsrKey, m_mapUserInfo))
		{
			return false;
		}

		m_strUserName = name;
		return SUCCESS == pStorer->SetHashByField(m_strUsrKey, USR_FD_USERNAME, name);
	}
	return false;
}

std::string& UserModel::GetName()
{
	return m_strUserName;
}

std::map<int, int>& UserModel::GetUserInfo()
{
	return m_mapUserInfo;
}

