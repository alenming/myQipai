#include "GameUser.h"
#include "UserManager.h"

GameUser::GameUser()
: m_nUid(0)
{
}

GameUser::~GameUser()
{
}

bool GameUser::initModels(int uid)
{
	std::vector<ModelType> VectModelType = UserManager::getInstance()->getModelType();
	for (std::vector<ModelType>::iterator iter = VectModelType.begin(); iter != VectModelType.end(); ++iter)
	{
		int type = *iter;
		IDBModel *model = nullptr;
		switch (type)
		{
		case MODELTYPE_USER:
			model = new UserModel;
			break;
		default:
			break;
		}

		if (!model->init(uid))
        {
            if (MODELTYPE_USER == type)
            {
                delete model;
                return false;
            }
            else
            {
                delete model;
                continue;
            }
		}
		m_mapModels[type] = model;
	}
	m_nUid = uid;
	return true;
}

void GameUser::refreshModels()
{
	for (std::map<int, IDBModel*>::iterator iter = m_mapModels.begin();
		iter != m_mapModels.end(); ++iter)
	{
		iter->second->Refresh();
	}
}

void GameUser::refreshModel(int modelType)
{
	if (m_mapModels.find(modelType) != m_mapModels.end())
	{
		m_mapModels[modelType]->Refresh();
	}
}

IDBModel* GameUser::getModel(int modelType)
{
	if (m_mapModels.find(modelType) != m_mapModels.end())
	{
		return m_mapModels[modelType];
	}
	return nullptr;
}

void GameUser::setModel(int modelType, IDBModel *model)
{
	if (m_mapModels.find(modelType) != m_mapModels.end())
	{
		delete m_mapModels[modelType];
	}

	m_mapModels[modelType] = model;
}

bool GameUser::updateData(int modelType, int feild, int value, bool isWrite)
{
	std::map<int, IDBModel*>::iterator iter = m_mapModels.find(modelType);
	if (m_mapModels.find(modelType) != m_mapModels.end())
	{
		return iter->second->updateData(feild, value, isWrite);
	}
	return false;
}

bool GameUser::updateData(int modelType)
{
	std::map<int, IDBModel*>::iterator iter = m_mapModels.find(modelType);
	if (m_mapModels.find(modelType) != m_mapModels.end())
	{
		return iter->second->updateData();
	}
	return false;
}
