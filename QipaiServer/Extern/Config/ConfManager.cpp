#include "ConfManager.h"
#include "ConfGameSetting.h"
#include "log/LogManager.h"

#define CHECK(success) if(!success) return false;

using namespace std;

ConfManager* ConfManager::m_Instance = NULL;

ConfBase::~ConfBase()
{
	for (map<int, void*>::iterator iter = m_Datas.begin();
		iter != m_Datas.end(); ++iter)
	{
		delete iter->second;
	}
}

ConfManager* ConfManager::getInstance()
{
	if (NULL == m_Instance)
	{
		m_Instance = new ConfManager();
	}
	return m_Instance;
}

void ConfManager::destroy()
{
	if (NULL != m_Instance)
	{
		delete m_Instance;
		m_Instance = NULL;
	}
}

ConfManager::ConfManager()
{
}

ConfManager::~ConfManager()
{
	for (map<int, ConfBase*>::iterator iter = m_Confs.begin();
		iter != m_Confs.end(); ++iter)
	{
		delete iter->second;
	}
}

bool ConfManager::init()
{
	do
	{
		CHECK(addConf(CONF_NEW_PLAYER, "..//GameConfig//NewPlayerSetting.csv", new ConfNewPlayerSetting()));

	} while (0);

	return true;
}

bool ConfManager::addConf(int confId, const std::string& csvFile, ConfBase* conf)
{
	// 如果重复，则直接更新
	if (m_Confs.find(confId) != m_Confs.end())
	{
		delete m_Confs[confId];
	}

	if (NULL == conf || !conf->LoadCSV(csvFile))
	{
		LOGDEBUG("Load %s Error", csvFile.c_str());
		return false;
	}

	m_Confs[confId] = conf;
	return true;
}
