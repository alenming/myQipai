#include "UserModel.h"

#include "helper/ModelDef.h"

#include "DBDriver/DBManager.h"
#include "DBDriver/RedisStorer.h"
#include "UserModel.h"
#include "RedisStorer.h"
#include "log/LogManager.h"


UserModel::UserModel()
	: m_uId(0)
	, m_pStorageDB(NULL)
{
}

UserModel::~UserModel()
{
}

bool UserModel::init(int uId)
{
	m_uId = uId;
	m_pStorageDB = DBManager::getInstance()->GetStorer(DB_USER_MODEL);
	RedisStorer *pStorer = reinterpret_cast<RedisStorer*>(m_pStorageDB->storer);
	if (NULL != pStorer)
	{
		m_strUsrKey = ModelKey::UsrKey(uId);
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
	RedisStorer *pStorer = reinterpret_cast<RedisStorer*>(m_pStorageDB->storer);
	if (NULL != pStorer)
	{
		for (int i = USR_ACCOUNDID; i < USR_FD_END; i++)
		{
			m_mapUserInfo[i] = 0;
		}

		if (SUCCESS != pStorer->GetHashByField(m_strUsrKey, m_mapUserInfo))
		{
			// 没有用户信息
			KX_LOGDEBUG("没有用户信息!");
			return false;
		}

		if (SUCCESS != pStorer->GetHashByField(m_strUsrKey, USR_FD_USERNAME, m_strUserName))
		{
			KX_LOGDEBUG("用户不存在!");
			// 用户不存在
			return false;
		}

		return true;
	}

	return false;
}

bool UserModel::NewUser(int accoundId, std::string name, std::map<int, int> &info)
{
	m_uId = accoundId;
	m_mapUserInfo = info;

	RedisStorer *pStorer = reinterpret_cast<RedisStorer*>(m_pStorageDB->storer);

	if (NULL != pStorer)
	{
		m_strUsrKey = ModelKey::UsrKey(m_uId);

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

bool UserModel::GetUserFieldVal(int field, int &value, bool bNew)
{
	if (bNew)
	{
		if (!GetRealDataFromDB(field, value))
		{
			return false;
		}
		m_mapUserInfo[field] = value;
	}
	else
	{
		std::map<int, int>::iterator iter = m_mapUserInfo.find(field);
		if (iter == m_mapUserInfo.end())
		{
			return false;
		}
		value = iter->second;
	}
	return true;
}

bool UserModel::GetUserFieldVal(std::map<int, int> &kvs, bool bNew)
{
	RedisStorer *pStorer = reinterpret_cast<RedisStorer*>(m_pStorageDB->storer);
	if (NULL != pStorer)
	{
		if (bNew)
		{
			if (SUCCESS != pStorer->GetHash(m_strUsrKey, m_mapUserInfo))
			{
				return false;
			}
		}

		for (std::map<int, int>::iterator iter = kvs.begin();
			iter != kvs.end(); ++iter)
		{
			if (m_mapUserInfo.find(iter->first) != m_mapUserInfo.end())
			{
				kvs[iter->first] = m_mapUserInfo[iter->first];
			}
		}
		return true;
	}
	return false;
}

bool UserModel::SetUserFieldVal(int field, int value)
{
	RedisStorer *pStorer = reinterpret_cast<RedisStorer*>(m_pStorageDB->storer);
	if (NULL != pStorer)
	{
		if (SUCCESS == pStorer->SetHashByField(m_strUsrKey, field, value))
		{
			m_mapUserInfo[field] = value;
			return true;
		}
	}
	return false;
}

bool UserModel::AddUserFieldVal(int field, int value)
{
	RedisStorer *pStorer = reinterpret_cast<RedisStorer*>(m_pStorageDB->storer);
	if (NULL != pStorer)
	{
		m_mapUserInfo[field] += value;
		if (SUCCESS == pStorer->SetHashByField(m_strUsrKey, field, m_mapUserInfo[field]))
		{
			return true;
		}
	}
	return false;
}

int UserModel::IncreaseFieldVal(int field, int increase)
{
	int value = 0;
	GetUserFieldVal(field, value);
	value += increase;
	if (!SetUserFieldVal(field, value))
	{
		return 0;
	}
	return value;
}

bool UserModel::SetUserFieldVal(std::map<int, int> &kvs)
{
	RedisStorer *pStorer = reinterpret_cast<RedisStorer*>(m_pStorageDB->storer);
	if (NULL != pStorer)
	{
		if (SUCCESS == pStorer->SetHash(m_strUsrKey, kvs))
		{
			std::map<int, int>::iterator iter = kvs.begin();
			for (; iter != kvs.end(); ++iter)
			{
				m_mapUserInfo[iter->first] = iter->second;
			}

			return true;
		}
	}
	return false;
}

bool UserModel::ModUserName(std::string name)
{
	RedisStorer *pStorer = reinterpret_cast<RedisStorer*>(m_pStorageDB->storer);
	if (NULL != pStorer)
	{
		if (SUCCESS == pStorer->SetHashByField(m_strUsrKey, USR_FD_USERNAME, name))
		{
			m_strUserName = name;
			return true;
		}
	}
	return false;
}

bool UserModel::AlterUserFieldVal(int field, int value)
{
	std::map<int, int>::iterator iter = m_mapUserInfo.find(field);
	if (iter != m_mapUserInfo.end())
	{
		iter->second = value;
		return true;
	}

	return false;
}

bool UserModel::DeleteUser()
{
	RedisStorer *pStorer = reinterpret_cast<RedisStorer*>(m_pStorageDB->storer);
	if (NULL != pStorer)
	{
		return SUCCESS == pStorer->DelKey(m_strUsrKey);
	}
	return false;
}

bool UserModel::SetUserInfo(std::map<int, int> &userInfo)
{
	RedisStorer *pStorer = reinterpret_cast<RedisStorer*>(m_pStorageDB->storer);
	if (NULL != pStorer)
	{
		if (SUCCESS == pStorer->SetHash(m_strUsrKey, userInfo))
		{
			m_mapUserInfo = userInfo;
			return true;
		}
	}
	return false;
}

bool UserModel::GetRealDataFromDB(int field, int &value)
{
	RedisStorer *pStorer = reinterpret_cast<RedisStorer*>(m_pStorageDB->storer);
	if (NULL == pStorer)
	{
		return false;
	}
	if (SUCCESS != pStorer->GetHashByField(m_strUsrKey, field, value))
	{
		return false;
	}

	return true;
}
