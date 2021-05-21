/*
 *  
 *  管理服务器网络结构, 通过NetworkManager可以对指定服务器发送消息
 *  
 */
#ifndef __NETWORKMANAGER_H__
#define __NETWORKMANAGER_H__

#include <map>
#include<vector>

#include "TimerManager.h"
#include "SessionClient.h"

class NetWorkManager : public TimerObject
{
private:
    NetWorkManager(void);
    virtual ~NetWorkManager(void);

public:
    static NetWorkManager* getInstance();
    static void destroy();

    //主动连后端服务器组ID
	bool addServer(int serverId, IComm* obj);
	//添加备份后端服务器组ID
	bool addBakServer(int nGroupID, IComm* obj);
	//清除某个备份服务器组ID
	bool clearBakServer(int nGroupID);
	//通过类型获取服务器,Key对应的Value值来取模
	IComm* getServer(int serverId, int nValue = 0);
    //获取某个服务器组
	std::vector<IComm*>* getGroupServer(int nGroupID);
	//获取某个服务器组备份
	std::vector<IComm*>* getBakGroupServer(int nGroupID);
    //获取所有服务器
	std::map<int, IComm*>& getAllServer() { return m_ServerList; }

	//将连接添加到待验证连接列表中
	bool addGuest(unsigned int guestId, IComm* obj);
	//获取待验证连接客户端
	IComm* getGuest(unsigned int guestId);
	bool removeGuest(unsigned int guestId);
	//将连接转为连接OK的客户端
	bool changeGuestToUser(SessionClient* guest, unsigned int userId);

	bool addUser(int guestId, IComm* obj);
	IComm* getUser(int guestId);
	bool removeUser(int guestId);

    //数据广播给所有玩家
    bool broadCastData(char *pszContext, int nLen);

	bool sendDataToClient(SessionClient* pClient, int nMainCmd, int nSubCmd, char *pszContext, int nLen);

    //获取当前连接玩家数
    unsigned int getCurClientNum();
	//活动动态唯一ID
	unsigned int genGuestId();

	void closeAllServer();

	void onTimer(const TimeVal& now);

	void setChangeFlag(bool bChange) { m_bChanging = bChange; }
	bool getChangeFlag() { return m_bChanging; }

private:
    static NetWorkManager* m_Instance;

	unsigned int m_GuestId;											// 未验证客户端唯一ID
	bool		m_bChanging;										//是否在切换中
	std::map<int, std::vector<IComm*> >	m_ServerMapByGroup;		// 服务端连接管理
	std::map<int, std::vector<IComm*> >	m_BakServerMapByGroup;	// 备份连接管理
	std::map<unsigned int, IComm*> m_UserMap;       				// 已经验证过的客户端连接管理
	std::map<unsigned int, IComm*> m_GuestMap;					// 未验证的客户端连接管理
	std::map<int, IComm* >	m_ServerList;						// 服务端连接管理
};

#endif
