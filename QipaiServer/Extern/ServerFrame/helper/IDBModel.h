#ifndef __IDBMODEL_H__
#define __IDBMODEL_H__

enum ModelType
{
	MODELTYPE_NONE,					//无
	MODELTYPE_USER,					//玩家数据模型

};

class IDBModel
{
public:
	IDBModel();
	virtual ~IDBModel();

public:

	virtual bool init(int uid) { return true; }
	virtual bool Refresh() { return true; }
};

#endif //__IDBMODEL_H__