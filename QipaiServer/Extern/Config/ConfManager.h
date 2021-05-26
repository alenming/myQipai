#ifndef __CONF_MANAGER_H__
#define __CONF_MANAGER_H__

#include "CsvLoader/CsvLoader.h"


// 配置文件的类型
enum ConfType
{
	CONF_BASE,                  // 配置基础偏移值，无实际意义
	CONF_NEW_PLAYER,            // 新玩家配置信息
};

// 配置文件的基类
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

	// 加载csv文件,跳过几行(-1)
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

		// 注释备注,跳过
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

// 配置文件管理单例

class ConfManager
{
private:
	ConfManager();
	virtual ~ConfManager();

public:
	static ConfManager* getInstance();

	static void destroy();

	// 批量加载所有的配置文件
	// 加载成功返回true，加载失败返回false并打印日志
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
