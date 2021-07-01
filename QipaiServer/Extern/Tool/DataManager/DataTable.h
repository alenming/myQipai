#ifndef DataTable_h__
#define DataTable_h__
#include "Core.h"

#include "Tea/TEACrypt.h"
#ifdef RunningInServer
#include "Common/Data_.h"
#endif

class RowData;

class DataTable
{
public:

	DataTable();
	~DataTable();
public:

	//初始化
	void init(const char* tableName, TEACrypt* tea);
	//加载数据表
	void loadFile();
	//通过id查询数据
	RowData* searchDataById(int id);
	//通过id查询所有与id匹配的数据
	std::vector<RowData*> searchAllDataById(int id);
	//获取表头字段索引
	int getFieldIndex(std::string field);
	//获取整表数据
	std::multimap<int, RowData>& getTableData();
private:
	//设置表头
	void setTableHead(std::vector<std::string>& vecHead);
	//添加数据
	void addRowData(std::vector<std::string>& data);
	//去掉字符串首尾空格
	std::string& strim(std::string &s);
#ifdef RunningInServer
	Data_ getStringFromFile(const std::string &path);
#endif
private:
	//表数据, 通过id字段索引
	std::multimap<int, RowData> mTableData;
	//表头, 通过表头字符串获取表头索引
	std::map<std::string, int> mTableHead;
	//是否已加载
	bool mIsLoaded;
	//数据表名称
	std::string mTableName;
	//Tea解密算法类
	TEACrypt* mTea;
};


class RowData
{
public:
	RowData(DataTable* dataTable);
	~RowData();
public:
	//添加数据
	void setData(std::vector<std::string>& data);
	//获取整型数据
	int getIntData(std::string field);
	//获取浮点数据
	float getFloatData(std::string field);
	//获取字符串数据
	std::string getStringData(std::string field);
	//获取长整型数据
	long long getLongData(std::string field);
private:
	//行数据
	std::vector<std::string> mData;
	//表数据，用以获取表头
	DataTable* mDataTable;
};

#endif