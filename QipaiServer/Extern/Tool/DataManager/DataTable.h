#ifndef DataTable_h__
#define DataTable_h__

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

	//��ʼ��
	void init(const char* tableName, TEACrypt* tea);

	//�������ݱ�
	void loadFile();

	//ͨ��id��ѯ����
	RowData* searchDataById(int id);

	//ͨ��id��ѯ������idƥ�������
	std::vector<RowData*> searchAllDataById(int id);

	//��ȡ��ͷ�ֶ�����
	int getFieldIndex(std::string field);

	//��ȡ��������
	std::multimap<int, RowData>& getTableData();

private:

	//���ñ�ͷ
	void setTableHead(std::vector<std::string>& vecHead);

	//�������
	void addRowData(std::vector<std::string>& data);

	//ȥ���ַ�����β�ո�
	std::string& strim(std::string &s);

#ifdef RunningInServer
	Data_ getStringFromFile(const std::string &path);
#endif

private:

	//������, ͨ��id�ֶ�����
	std::multimap<int, RowData> mTableData;

	//��ͷ, ͨ����ͷ�ַ�����ȡ��ͷ����
	std::map<std::string, int> mTableHead;

	//�Ƿ��Ѽ���
	bool mIsLoaded;

	//���ݱ�����
	std::string mTableName;

	//Tea�����㷨��
	TEACrypt* mTea;

};


class RowData
{

public:

	RowData(DataTable* dataTable);
	~RowData();

public:

	//�������
	void setData(std::vector<std::string>& data);

	//��ȡ��������
	int getIntData(std::string field);

	//��ȡ��������
	float getFloatData(std::string field);

	//��ȡ�ַ�������
	std::string getStringData(std::string field);

	//��ȡ����������
	long long getLongData(std::string field);

private:

	//������
	std::vector<std::string> mData;

	//�����ݣ����Ի�ȡ��ͷ
	DataTable* mDataTable;
};


#endif