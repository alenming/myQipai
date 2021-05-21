/*************************************************
数据库存储器基类
************************************************/
#ifndef __ISTORER_H__
#define __ISTORER_H__

#include<string>

enum STORE_TYPE
{
	STORE_NONE,
	STORE_REDIS,
	STORE_MYSQL,
	STORE_OTHER
};

// 具体model相关的storageId
enum DBID
{
	// all of redis
	DB_NONE = 0,			   //无
	DB_USER_MODEL = 1,
};

class IStorer;

struct DBRule
{
	std::string ip;
	int port;
	std::string password;
	int range_minvalue;          // id最小值
	int range_maxvalue;          // id最大值
	STORE_TYPE store_type;		 //
	DBID db_id;					 // modeltype
	IStorer * storer;		     // 数据库实体
};

class IStorer
{
public:
	IStorer();
	virtual ~IStorer();
};

#endif //__ISTORER_H__
