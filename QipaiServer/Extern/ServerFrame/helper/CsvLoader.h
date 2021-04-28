
#ifndef __CSV_LOADER_H__
#define __CSV_LOADER_H__

#include <string>
#include <vector>

#if (KX_TARGET_PLATFORM == KX_PLATFORM_LINUX)
	#include<stdlib.h>
	#include<stdio.h>
	#include<time.h>
	#include<string.h>
#endif



class CsvLoader
{
public:
	CsvLoader();
	virtual ~CsvLoader();

	void Release();

	// ����csv�ļ����������·��������ʧ�ܻ᷵��ʧ��
	// ���سɹ����Զ���ת����һ��
	bool LoadCSV(const char* fileName);

	// NextLine����ת����һ��
	// ����false��ʾ�Ѿ�����β��������true��ʾ�������µ�һ������
	bool NextLine();

	// ����ǰ����һ���ֶν���Ϊfloat������
	// �������ʧ�ܷ���0.0f
	float NextFloat();

	// ����ǰ����һ���ֶν���Ϊint������
	// �������ʧ�ܷ���0
	int NextInt();

	// ����ǰ����һ���ֶν���Ϊstring������
	// �������ʧ�ܷ���""
	std::string NextStr();
	// ��������ȥ������
	std::string Trim(std::string str);
	// ���ص�ǰ��ʣ����ַ���
	std::string LeftStr();

	bool isCurrentLineNull();	//��������(��ʱֻ��������һ��Ϊ�յ��ֶ�)

	int ColCount();

	// ��һ���ַ����Էָ��� sep �ָ�����ת��Ϊ�ַ���������vector������
	bool SplitStrToVector(const std::string &str, char sep, std::vector<std::string>& out);

	// ��һ���ַ����Էָ��� sep �ָ�����ת��Ϊfloat������vector������
	bool SplitFloatToVector(const std::string &str, char sep, std::vector<float>& out);

	// ��һ���ַ����Էָ��� sep �ָ�����ת��Ϊint������vector������
	bool SplitIntToVector(const std::string &str, char sep, std::vector<int>& out);

private:

	std::string getStringFromFile(const std::string &path);

private:

	std::string m_CurLine;          //��ǰ�ڽ�������
	std::string m_Document;         //CSV�ĵ�����
	std::string::size_type m_nDocOffset;
	std::string::size_type m_nDocStep;        //�ĵ��Ľ�ȡ�±�
	std::string::size_type m_nLineOffset;     //�е��±�ƫ��ֵ
	std::string::size_type m_nLineStep;       //�еĽ�ȡ�±�
};

#endif