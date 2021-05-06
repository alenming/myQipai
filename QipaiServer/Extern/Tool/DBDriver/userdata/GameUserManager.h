#ifndef __GAMEUSER_MANAGER_H__
#define __GAMEUSER_MANAGER_H__
/*
	玩家模型管理器
*/
#include <map>
#include "GameUser.h"
#include "KXServer.h"


//延迟处理结构
struct SDelayDelData
{
	int nDelayTime;
	int nUid;
};



class GameUserManager : public KxTimerObject
{
private:
	GameUserManager();
	~GameUserManager();

public:

	static GameUserManager *getInstance();
	static void destroy();

public:

	bool init(KxTimerManager *pTimerManager);

	void addModelType(ModelType Type);

	void onTimer(const kxTimeVal& now);
	// 获得已有用户，当createWhenNull为true时，获取不到会自动从数据库中初始化它
	GameUser* getGameUser(int uid, int accountId, bool createWhenNull = true);
	GameUser* getGameUser(int uid);
	// 初始化用户
	GameUser* initGameUser(int uid, int accountId);
	GameUser* newGameUser (int uid, int accountId);
	//更新用户需要重置的数据
	void reSetGameUserData(int uid, bool bLogin = false);
    // 刷新用户数据
    void updateGameUserData(GameUser* gameUsr, bool bLogin = false);
	// 添加用户, 注册的时候添加
	void addGameUser(int uid, GameUser* gameUsr);
	//检测用户在该服务器是否存在
	bool checkUserIsExist(int uid);
	// 移除用户
	void removeGameUser(int uid);
	// 删除移除用户数据
	void donotDeleteUser(int uid);


	// 获得所有模型类型
	std::vector<ModelType>& getModelType() { return m_VectServerModel; }

	bool initUserData(GameUser* gameUsr);

	const std::map<int, GameUser *>& getGameUsers() { return m_GameUsers; }

	//真正删除用户
	void RealremoveGameUser(int uid);

private:

	static GameUserManager *							m_pInstance;
	std::map<int, GameUser *>							m_GameUsers;
	std::vector<ModelType>								m_VectServerModel;					//服务器所需求服务器模型
	std::map<int, std::list<SDelayDelData>::iterator>	m_MapDelUserList;					//玩家对应的向量迭代器
	std::list<SDelayDelData>							m_DelUserList;						//延迟玩家下线列表
	KxTimerManager *							m_TimeManager;
};

#endif //__GAMEUSER_MANAGER_H__
