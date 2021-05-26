#ifndef __CONF_MANAGER_H__
#define __CONF_MANAGER_H__

#include "CsvLoader/CsvLoader.h"


// �����ļ�������
enum ConfType
{
	CONF_BASE,                  // ���û���ƫ��ֵ����ʵ������
	CONF_NEW_PLAYER,            // �����������Ϣ
};

// �����ļ��Ļ���
class ConfBase
{
public:
	virtual ~ConfBase();

	virtual bool LoadCSV(const std::string& str)
	{
		return false;
	}

	inline void* getData(int dataId)
	{
		std::map<int, void*>::iterator iter = m_Datas.find(dataId);
		if (iter != m_Datas.end())
		{
			return iter->second;
		}

		return NULL;
	}

	void addData(int dataId, void* pData)
	{
		void* data = getData(dataId);
		if (!data)
		{
			m_Datas[dataId] = pData;
		}
	}

	inline std::map<int, void*>& getDatas()
	{
		return m_Datas;
	}

	// ����csv�ļ�,��������(-1)
	bool loadCsv(CsvLoader& csvLoader, const std::string& csvFile, int skipRow)
	{
		if (skipRow <= 0 || "" == csvFile)
		{
			return false;
		}

		if (!csvLoader.LoadCSV(csvFile.c_str()))
		{
			return false;
		}

		// ע�ͱ�ע,����
		for (int i = 0; i < skipRow; i++)
		{
			if (!csvLoader.NextLine())
			{
				return false;
			}
		}

		return true;
	}

protected:
	std::map<int, void*> m_Datas;
};

// �����ļ�������

class ConfManager
{
private:
	ConfManager();
	virtual ~ConfManager();

public:
	static ConfManager* getInstance();

	static void destroy();

	// �����������е������ļ�
	// ���سɹ�����true������ʧ�ܷ���false����ӡ��־
	bool init();

	inline ConfBase* getConf(int confId)
	{
		std::map<int, ConfBase*>::iterator iter = m_Confs.find(confId);
		if (iter != m_Confs.end())
		{
			return iter->second;
		}
		return NULL;
	}

	bool addConf(int confId, const std::string& csvFile, ConfBase* conf);

private:
	static ConfManager* m_Instance;
	std::string m_szConfRoot;
	std::map<int, ConfBase*> m_Confs;
};

#endif
