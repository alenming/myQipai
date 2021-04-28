/*************************************************
���ݿ�洢������
************************************************/
#ifndef __ISTORER_H__
#define __ISTORER_H__

#include<string>

enum STORE_TYPE
{
	STORE_NONE,
	STORE_TMEM,
	STORE_MYSQL,
	STORE_CACHE,
	STORE_REDIS,
	STORE_OTHER
};

// ����model��ص�storageId
enum DBID
{
	// all of redis
	DB_NONE = 0,			   //��
	DB_USER_MODEL = 1,
};

class IStorer;

struct DBRule
{
	std::string ip;
	int port;
	std::string password;
	int range_minvalue;          // id��Сֵ
	int range_maxvalue;          // id���ֵ
	STORE_TYPE store_type;		 //
	DBID db_id;			 // modeltype
	IStorer * storer;		     // ���ݿ�ʵ��
};

class IStorer
{
public:
	IStorer();
	virtual ~IStorer();
};

#endif //__ISTORER_H__
