#include "DataManager.h"


DataManager* DataManager::mInstance = nullptr;

//tea√‹‘ø
UnInt32_t DataManager::mKey[] = {0x01, 0x02, 0x03, 0x04};



#define GET_ARRAY_LEN(array,len) {len = (sizeof(array) / sizeof(array[0]) - 1 );} 

template <class T>
unsigned int getArrayLen(T& array)
{
	return (sizeof(array) / sizeof(array[0]));
}

DataManager::DataManager()
{
	mTea.setKey(mKey);
	initData();
}

DataManager::~DataManager()
{
}

DataManager* DataManager::getInstance()
{
	if(!mInstance)
	{
		mInstance = new DataManager();
	}
	return mInstance;
}

void DataManager::destroyInstance()
{
	if(mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void DataManager::initData()
{
	for (size_t i = 0; i < getArrayLen(DataManager_AllData); i++)
	{
		
		m_DataTable[DataManager_AllData[i]] = DataTable();

		bindTable(m_DataTable[DataManager_AllData[i]], DataManager_AllData[i]);
	}

	//bindTable(mXXTable, DATA_XXTABLE);
	//bindTable(mXXTableCsv, DATA_XXTABLE_CSV);

}

void DataManager::bindTable(DataTable& dataTable, const char* filename)
{
	dataTable.init(filename, &mTea);
}



