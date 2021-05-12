#include "GameUser.h"
#include "GameUserManager.h"

GameUser::GameUser()
: m_nUid(0)
, m_nAccountId(0)
{
}

GameUser::~GameUser()
{
}

bool GameUser::initModels(int uid, int accoundID)
{
	std::vector<ModelType> VectModelType = GameUserManager::getInstance()->getModelType();
	for (std::vector<ModelType>::iterator iter = VectModelType.begin(); iter != VectModelType.end(); ++iter)
	{
		int type = *iter;
		IDBModel *model = NULL;
		switch (type)
		{
		case MODELTYPE_USER:
			model = new UserModel;
			break;
		default:
			break;
		}

		if (!model->init(accoundID))
        {
			//KX_LOGERROR("accoundID=%d init model fail ! modelType=%d", accoundID, type);
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
	m_nAccountId = accoundID;
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
	return NULL;
}

void GameUser::setModel(int modelType, IDBModel *model)
{
	if (m_mapModels.find(modelType) != m_mapModels.end())
	{
		delete m_mapModels[modelType];
	}

	m_mapModels[modelType] = model;
}