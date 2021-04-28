#ifndef DataManager_h__
#define DataManager_h__

#include "DataTable.h"
#include "Tea/TEACrypt.h"


#define DATA_XXTABLE_D  "XXTable.d"
#define DATA_XXTABLE_CSV "XXTable.csv"

static const char* DataManager_AllData[] =
{
	DATA_XXTABLE_D,
	DATA_XXTABLE_CSV,
};


class DataManager
{

public:

	DataManager();
	~DataManager();

public:

	//singleton
	static DataManager* getInstance();
	static void destroyInstance();

	//获取XX整表数据
	DataTable* DataManager::getXXTable()
	{
		return &mXXTable;
	}

	//查询获取xx某个数据
	RowData* DataManager::searchXXTableById(int id)
	{
		return mXXTable.searchDataById(id);
	}

	//获取XX整表数据
	DataTable* DataManager::getTableByName(std::string name)
	{
		return &m_DataTable[name];
	}
	//查询获取xx某个数据
	RowData* DataManager::searchTableById(std::string name,int id)
	{
		return m_DataTable[name].searchDataById(id);
	}
private:

	//初始化数据
	void initData();

	//绑定数据表
	void bindTable(DataTable& dataTable, const char* filepath);
	
private:

	static DataManager* mInstance;

	//Tea加解密密钥
	static UnInt32_t mKey[];

	//Tea解密算法类
	TEACrypt mTea;

	//XX表
	DataTable mXXTable;
	DataTable mXXTableCsv;

	std::map<std::string, DataTable> m_DataTable;

};



#endif