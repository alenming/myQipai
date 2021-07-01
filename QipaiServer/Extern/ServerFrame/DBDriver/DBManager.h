/*
 * storagemanager， 基于baseframe内， 
 * 建议根据baseframe项目配置进行使用
 * 2014-02-10
 */

#ifndef _STORAGE_MANAGER_H__
#define _STORAGE_MANAGER_H__
#include <map>
#include "IStorer.h"

#define STORAGE_XML_FILE  "..//GameConfig//module_config.xml"


class DBManager
{
private:
    DBManager();
    ~DBManager();

public:
    static DBManager* getInstance();
    static void destroy();

public:
	//通过配置表初始化
	bool InitWithXML(std::string xmlFile);

    //获得存储对象
	//IStorer* GetStorer(int dataBaseId, int uid);
	//通过IP
	DBRule* GetStorer(DBRule& rule);
	DBRule* GetStorer(DBID id);
    //插入存储数据库对象
	bool InsertStorer(DBRule rule);
    
private:
    static DBManager *				m_pInstance;

	std::map<std::string, DBRule>		m_mapStorers;	// <ip:port:model_type, storer>
	std::map<DBID, DBRule*>				m_mapStorers_id;	// <ip:port:model_type, storer>
};


#endif //_STORAGE_MANAGER_H__
