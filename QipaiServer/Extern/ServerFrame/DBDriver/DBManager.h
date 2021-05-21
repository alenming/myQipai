/*
 * storagemanager�� ����baseframe�ڣ� 
 * �������baseframe��Ŀ���ý���ʹ��
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
	//ͨ�����ñ��ʼ��
	bool InitWithXML(std::string xmlFile);

    //��ô洢����
	//IStorer* GetStorer(int dataBaseId, int uid);
	//ͨ��IP
	DBRule* GetStorer(DBRule& rule);
	DBRule* GetStorer(DBID id);
    //����洢���ݿ����
	bool InsertStorer(DBRule rule);
    
private:
    static DBManager *				m_pInstance;

	std::map<std::string, DBRule>		m_mapStorers;	// <ip:port:model_type, storer>
	std::map<DBID, DBRule*>				m_mapStorers_id;	// <ip:port:model_type, storer>
};


#endif //_STORAGE_MANAGER_H__
