#ifndef __IDBMODEL_H__
#define __IDBMODEL_H__

enum ModelType
{
	MODELTYPE_NONE,					//��
	MODELTYPE_USER,					//�������ģ��

};

class IDBModel
{
public:
	IDBModel();
	virtual ~IDBModel();

public:

	virtual bool init(int uid) { return true; }
	virtual bool Refresh() { return true; }
	virtual bool updateData(int feild, int value, int isWrite=false){ return true; }
	virtual bool updateData(){ return true; }
};

#endif //__IDBMODEL_H__