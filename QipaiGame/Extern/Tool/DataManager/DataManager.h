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

	//��ȡXX��������
	DataTable* DataManager::getXXTable()
	{
		return &mXXTable;
	}

	//��ѯ��ȡxxĳ������
	RowData* DataManager::searchXXTableById(int id)
	{
		return mXXTable.searchDataById(id);
	}

	//��ȡXX��������
	DataTable* DataManager::getTableByName(std::string name)
	{
		return &m_DataTable[name];
	}
	//��ѯ��ȡxxĳ������
	RowData* DataManager::searchTableById(std::string name,int id)
	{
		return m_DataTable[name].searchDataById(id);
	}
private:

	//��ʼ������
	void initData();

	//�����ݱ�
	void bindTable(DataTable& dataTable, const char* filepath);
	
private:

	static DataManager* mInstance;

	//Tea�ӽ�����Կ
	static UnInt32_t mKey[];

	//Tea�����㷨��
	TEACrypt mTea;

	//XX��
	DataTable mXXTable;
	DataTable mXXTableCsv;

	std::map<std::string, DataTable> m_DataTable;

};



#endif