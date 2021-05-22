#ifndef __USER_MANAGER_H__
#define __USER_MANAGER_H__
/*
	玩家模型管理器
*/
#include <map>
#include <string>
#include "GameUser.h"
#include "TimerManager.h"

//延迟处理结构
struct SDelayDelData
{
	int nDelayTime;
	int nUid;
};



class UserManager : public TimerObject
{
private:
	UserManager();
	~UserManager();

public:

	static UserManager *getInstance();
	static void destroy();

public:

	bool init(TimerManager *pTimerManager);

	void addModelType(ModelType Type);

	void onTimer(const TimeVal& now);

	// 获得已有用户，当createWhenNull为true时，获取不到会自动从数据库中初始化它
	GameUser* getGameUser(int uid, std::string userName, std::string passWord, bool createWhenNull = true);
	GameUser* getGameUser(int uid);
	GameUser* newGameUser(int uid, std::string userName, std::string passWord);
	GameUser* initGameUser(int uid, std::string userName, std::string passWord);

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

	const std::map<int, GameUser *>& getGameUsers() { return m_GameUsers; }

	//真正删除用户
	void RealremoveGameUser(int uid);

private:
	static UserManager *								m_pInstance;
	std::map<int, GameUser*>							m_GameUsers;
	std::vector<ModelType>								m_VectServerModel;					//服务器所需求服务器模型
	std::map<int, std::list<SDelayDelData>::iterator>	m_MapDelUserList;					//玩家对应的向量迭代器
	std::list<SDelayDelData>							m_DelUserList;						//延迟玩家下线列表
	TimerManager *										m_TimeManager;
};

#endif //__USER_MANAGER_H__
