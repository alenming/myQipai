#include "DataTable.h"
#include "CSVParser.h"
#include "StringFormat/StringFormat.h"

#ifndef RunningInServer
	#include "cocos2d.h"
#endif


DataTable::DataTable()
	: mIsLoaded(false)
	, mTea(nullptr)
{

}

DataTable::~DataTable()
{
}

void DataTable::init(const char* tableName, TEACrypt* tea)
{
	mTableName = std::string(tableName);
	mTea = tea;
}

#ifdef RunningInServer
Data_ DataTable::getStringFromFile(const std::string &path)
{
	Data_ data;

	//FILE *fp = fopen(path.c_str(), "rb");
	FILE *fp = fopen(path.c_str(), "rt");
	if (NULL == fp)
	{
		return data;
	}

	int size = 0;
	int readsize = 0;

	fseek(fp, 0, SEEK_END);
	size = ftell(fp) + 1;
	fseek(fp, 0, SEEK_SET);

	data.setSize(size);
	readsize = fread(data.getBytes(), sizeof(unsigned char), size, fp);
	fclose(fp);
	data.setSize(readsize);

	return data;
}
#endif

void DataTable::loadFile()
{
	//�ж����ݱ��Ƿ��Ѽ���
	if (mIsLoaded)
	{
		return;
	}

	//��ȡ�������ļ�

	std::istringstream iss;

	std::vector<std::string> strSplit;
	StringFormat::split(strSplit, mTableName, ".");
	if (strSplit.at(1).size() > 0 && strSplit.at(1) == "csv")
	{
		std::string path = "..Resources/DataCsv/" + mTableName;

#ifndef RunningInServer
		cocos2d::Data data = cocos2d::FileUtils::getInstance()->getDataFromFile(path);
#else
		Data_ data =  getStringFromFile(path);
#endif
		
		const char *buffer = (char*)data.getBytes();

		iss.str(buffer);
	}
	else
	{
		std::string path = "Data/" + mTableName;
#ifndef RunningInServer
		cocos2d::Data data = cocos2d::FileUtils::getInstance()->getDataFromFile(path);
#else
		Data_ data = getStringFromFile(path);
#endif
	
		if (data.isNull())
		{
			printf("!!error, data null");
			return;
		}

		const char *buffer = (char*)data.getBytes();
		size_t size = data.getSize();
		mTea->decrypt(buffer, size);

		//��ȡ��������
		iss.str(mTea->getBuffer());
	}

	bool isReadHead = false;
	std::vector<std::string> head;
	std::vector<std::string> record;
	std::string line;

	while(getline(iss, line)  && iss.good())
	{
		if (!isReadHead)
		{
			//��ȡ��ͷ
			csvline_populate(head, line, ',');
			this->setTableHead(head);
			isReadHead = true;
		}
		else
		{
			//��ȡ����
			csvline_populate(record, line, ',');
			this->addRowData(record);
		}
	}

	mTea->clear();

	mIsLoaded = true;
}

void DataTable::setTableHead(std::vector<std::string>& vecHead)
{
	this->mTableHead = mTableHead;

	for (size_t i = 0; i < vecHead.size(); i++)
	{
		mTableHead.insert(std::pair<std::string, int>(strim(vecHead[i]), i));
	}
}

void DataTable::addRowData(std::vector<std::string>& data)
{
	RowData rowData(this);
	rowData.setData(data);
	int id = rowData.getIntData("id");
	if (id >= 0)
	{
		mTableData.insert(std::pair<int, RowData>(id, rowData));
	}
}

RowData* DataTable::searchDataById(int id)
{
	loadFile();
	auto iter = mTableData.find(id);
	if (iter != mTableData.end())
	{
		return &((*iter).second);
	}
	return nullptr;
}

std::vector<RowData*> DataTable::searchAllDataById(int id)
{
	loadFile();
	std::vector<RowData*> vecData;
	for (auto iter = mTableData.begin(); iter != mTableData.end(); iter++)
	{
		int tempId = (*iter).first;
		if (tempId == id)
		{
			vecData.push_back(&((*iter).second));
		}
	}
	return vecData;
}

//��ȡ��������
std::multimap<int, RowData>& DataTable::getTableData()
{
	loadFile();
	return mTableData;
}

int DataTable::getFieldIndex(std::string field)
{
	auto iter = mTableHead.find(field);
	if (iter != mTableHead.end())
	{
		return (*iter).second;
	}
	return -1;

}

std::string& DataTable::strim(std::string &s)
{  
	if (s.empty())   
	{  
		return s;
	}  
	s.erase(0,s.find_first_not_of(" "));  
	s.erase(s.find_last_not_of(" ") + 1);  
	return s;  
}  


///////////////////////////////�������ķָ���////////////////////////////////////////////


RowData::RowData(DataTable* dataTable)
{
	this->mDataTable = dataTable;
};

RowData::~RowData()
{

}

//�������
void RowData::setData(std::vector<std::string>& data)
{
	this->mData = data;
}

//��ȡ��������
int RowData::getIntData(std::string field)
{
	unsigned int fieldIndex = mDataTable->getFieldIndex(field);
	if (fieldIndex >= 0 && fieldIndex < mData.size())
	{
		std::string str = mData.at(fieldIndex);
		if (!str.empty())
		{
			return atoi(str.c_str());
		}
		else
		{
			return -1;
		}
	}
	else
	{
		printf("Not Found Field %s", field.c_str());
		return -1;
	}
}

//��ȡ��������
float RowData::getFloatData(std::string field)
{
	unsigned int fieldIndex = mDataTable->getFieldIndex(field);
	if (fieldIndex >= 0 && fieldIndex < mData.size())
	{
		std::string str = mData.at(fieldIndex);
		if (!str.empty())
		{
			return (float)atof(str.c_str());
		}
		else
		{
			return -1;
		}
	}
	else
	{
		printf("Not Found Field %s", field.c_str());
		return -1.0f;
	}
}

//��ȡ�ַ�������
std::string RowData::getStringData(std::string field)
{
	unsigned int fieldIndex = mDataTable->getFieldIndex(field);
	if (fieldIndex >= 0 && fieldIndex < mData.size())
	{
		return mData.at(fieldIndex);
	}
	else
	{
		printf("Not Found Field %s", field.c_str());
		return "";
	}
}

long long RowData::getLongData(std::string field)
{
	unsigned int fieldIndex = mDataTable->getFieldIndex(field);
	if (fieldIndex >= 0 && fieldIndex < mData.size())
	{
		std::string str = mData.at(fieldIndex);
		if (!str.empty())
		{
			return (long long)atof(str.c_str());
		}
		else
		{
			return -1;
		}
	}
	else
	{
		printf("Not Found Field %s", field.c_str());
		return (long long)(- 1.0f);
	}
}


