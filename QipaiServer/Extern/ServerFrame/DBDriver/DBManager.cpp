#include "DBManager.h"
#include "log/LogManager.h"
#include "MysqlStorer.h"
#include "RedisStorer.h"
#include "XML/Markup.h"

DBManager::DBManager()
{    
}

DBManager::~DBManager()
{
    //清除Storage对象数据
	for (std::map<std::string, DBRule>::iterator iter = m_mapStorers.begin(); iter != m_mapStorers.end(); ++iter)
    {
        if(&iter->second != nullptr)
        {
			delete iter->second.storer;
			iter->second.storer = nullptr;
        }
    }
	m_mapStorers.clear();
	m_mapStorers_id.clear();
}

DBManager* DBManager::m_pInstance = nullptr;
DBManager * DBManager::getInstance()
{
    if(m_pInstance == nullptr)
    {
        m_pInstance = new DBManager;
    }
    return m_pInstance;
}

void DBManager::destroy()
{
    if(m_pInstance != nullptr)
    {
        delete m_pInstance;
        m_pInstance = nullptr;
    }
}

bool DBManager::InitWithXML(std::string xmlFile)
{
	//XML conf;
	CMarkup conf;
	if (conf.Load(xmlFile.c_str()))
	{
		if (!conf.FindElem("module"))
		{
			return false;
		}

		conf.IntoElem();
		while (conf.FindElem("server"))
		{
			// 依次读取配置
			std::string dbtype = conf.GetAttrib("type");

			if (dbtype == "redis")
			{
				DBRule rule;
				rule.range_minvalue = atoi(conf.GetAttrib("rangemin").c_str());
				rule.range_maxvalue = atoi(conf.GetAttrib("rangemax").c_str());

				rule.ip = conf.GetAttrib("ip");
				rule.port = atoi(conf.GetAttrib("port").c_str());
				rule.password = conf.GetAttrib("passward");
				rule.db_id = static_cast<DBID>(atoi(conf.GetAttrib("modeltype").c_str()));
				rule.store_type = STORE_TYPE::STORE_REDIS;

				DBRule *getRule = dynamic_cast<DBRule *>(GetStorer(rule));

				if (nullptr == getRule)
				{
					RedisStorer* redisStorer = new RedisStorer();
					if (SUCCESS == redisStorer->Connect(rule.ip, rule.port, rule.password))
					{
						LOGDEBUG("connect to redis ip=%s, port=%d success!", rule.ip.c_str(), rule.port);
					}
					else
					{
						LOGDEBUG("connect to redis ip=%s, port=%d failed!", rule.ip.c_str(), rule.port);
						delete redisStorer;
						continue;
					}
					rule.storer = redisStorer;
					InsertStorer(rule);
					LOGDEBUG("insert redis storer storageId=%d", rule.db_id);
				}
			}
			else if (dbtype == "mysql")
			{
				DBRule rule;
				//初始化MYSQL
				rule.ip = conf.GetAttrib("ip");
				rule.password = conf.GetAttrib("password");
				rule.port = atoi(conf.GetAttrib("port").c_str());
				rule.db_id = static_cast<DBID>(atoi(conf.GetAttrib("dataBaseId").c_str()));
				rule.range_minvalue = atoi(conf.GetAttrib("rangemin").c_str());
				rule.range_maxvalue = atoi(conf.GetAttrib("rangemax").c_str());
				rule.store_type = STORE_TYPE::STORE_MYSQL;

				std::string username = conf.GetAttrib("username");
				std::string dbname = conf.GetAttrib("dbname");
				DBRule *getRule = dynamic_cast<DBRule *>(GetStorer(rule));
				if (nullptr == getRule)
				{
					MysqlStorer* dbStorer = new MysqlStorer();
					dbStorer->SetDbName(dbname);
					dbStorer->SetDbUser(username);
					dbStorer->SetDbPwd(rule.password);
					dbStorer->SetDbAddress(rule.ip);
					dbStorer->SetDbPort(rule.port);
					if (dbStorer->Connect())
					{
						LOGDEBUG("connect to mysql ip=%s, port=%d success!", rule.ip.c_str(), rule.port);
					}
					else
					{
						LOGDEBUG("connect to mysql ip=%s, port=%d failed!", rule.ip.c_str(), rule.port);
						delete dbStorer;
						continue;
					}
					rule.storer = dbStorer;
					InsertStorer(rule);
					LOGDEBUG("insert mysql storer storageId=%d", rule.db_id);
				}
			}
			else if (dbtype == "other")
			{
				// 暂未用
			}
		}
		return true;
	}
	return false;
}


DBRule *DBManager::GetStorer(DBRule& rule)
{
	char szPort[32] = {};
	char szModelType[32] = {};
	sprintf_s(szPort, sizeof(szPort), "%d", rule.port);
	sprintf_s(szModelType, sizeof(szModelType), "%d", rule.db_id);
	std::string ip = rule.ip + szPort + szModelType;

	if (m_mapStorers.find(ip) != m_mapStorers.end())
	{
		return &m_mapStorers[ip];
	}
	return nullptr;
}

DBRule *DBManager::GetStorer(DBID id)
{
	if (m_mapStorers_id.find(id) != m_mapStorers_id.end())
	{
		return m_mapStorers_id[id];
	}
	return nullptr;
}

bool DBManager::InsertStorer(DBRule rule)
{
	if (nullptr == rule.storer)
    { 
        return false;
    }

	char szPort[32] = {};
	char szModelType[32] = {};

	sprintf_s(szPort, sizeof(szPort), "%d", rule.port);
	sprintf_s(szModelType, sizeof(szModelType), "%d", rule.db_id);

	std::string ip = rule.ip + szPort + szModelType;

	if (m_mapStorers.find(ip) == m_mapStorers.end())
	{
		m_mapStorers[ip] = rule;
	}
	if (m_mapStorers_id.find(rule.db_id) == m_mapStorers_id.end())
	{
		m_mapStorers_id[rule.db_id] = &m_mapStorers[ip];
	}

	return true;
}


