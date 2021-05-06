#ifndef __GAME_USER_H__
#define __GAME_USER_H__
/*
	玩家模型
	DataModel 中是玩家的各类依赖数据模型, 类似RPG中公会模型,背包模型.卡牌模型
	UserModel 玩家的各位账号重要数据等
*/
#include "UserModel.h"

#include <vector>
#include <map>

class GameUser
{
public:
	GameUser();
	~GameUser();

public:

	bool initModels(int uid);
	
	void refreshModels();

	void refreshModel(int modelType);

	void setModel(int modelType, IDBModel *model);

	int	getUid() { return m_nUid; }
	void setUid(int uid) { m_nUid = uid; }

	int	getAccountId() { return m_nAccountId; }
	void setAccountId(int accountId) { m_nAccountId = accountId; }

	std::string	getUserName() { return m_sUserName; }
	void setUserName(std::string userName) { m_sUserName = userName; }

	IDBModel* getModel(int modelType);

private:
	int							m_nUid;
	int							m_nAccountId;
	std::string					m_sUserName;
	std::map<int, IDBModel*>	m_mapModels;
};

#endif //__GAME_USER_H__
