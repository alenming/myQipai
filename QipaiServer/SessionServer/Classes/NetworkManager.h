/*
 *  
 *  管理服务器网络结构, 通过NetworkManager可以对指定服务器发送消息
 *  
 */
#ifndef __NETWORKMANAGER_H__
#define __NETWORKMANAGER_H__

#include <map>
#include<vector>

#include "KXServer.h"
#include "SessionClient.h"
#include "KXServer.h"

class NetWorkManager : public KxTimerObject
{
private:
    NetWorkManager(void);
    virtual ~NetWorkManager(void);

public:
    static NetWorkManager* getInstance();
    static void destroy();

    //主动连后端服务器组ID
	bool addServer(int serverId, IKxComm* obj);
	//添加备份后端服务器组ID
	bool addBakServer(int nGroupID, IKxComm* obj);
	//清除某个备份服务器组ID
	bool clearBakServer(int nGroupID);
	//通过类型获取服务器,Key对应的Value值来取模
	IKxComm* getServer(int serverId, int nValue = 0);
    //获取某个服务器组
    std::vector<IKxComm*>* getGroupServer(int nGroupID);
	//获取某个服务器组备份
	std::vector<IKxComm*>* getBakGroupServer(int nGroupID);
    //获取所有服务器
	std::map<int, IKxComm*>& getAllServer() { return m_ServerList; }

	//将连接添加到待验证连接列表中
	bool addGuest(unsigned int guestId, IKxComm* obj);
	//获取待验证连接客户端
    IKxComm* getGuest(unsigned int guestId);
    bool removeGuest(unsigned int guestId);

	//将连接转为连接OK的客户端
    bool changeGuestToUser(SessionClient* guest, unsigned int userId);
	//获取已经验证过的客户端
    IKxComm* getUser(unsigned int userId);
    bool removeUser(unsigned int userId);

    //数据广播给所有玩家
    bool broadCastData(char *pszContext, int nLen);

	bool sendDataToClient(SessionClient* pClient, int nMainCmd, int nSubCmd, char *pszContext, int nLen);

    //获取当前连接玩家数
    unsigned int getCurClientNum();
	//活动动态唯一ID
	unsigned int genGuestId();

	void closeAllServer();

	void onTimer(const kxTimeVal& now);

	void setChangeFlag(bool bChange) { m_bChanging = bChange; }
	bool getChangeFlag() { return m_bChanging; }

private:
    static NetWorkManager* m_Instance;

    unsigned int m_GuestId;													// 未验证客户端唯一ID

    std::map<int, std::vector<IKxComm*> >	m_ServerMapByGroup;		// 服务端连接管理
	std::map<int, std::vector<IKxComm*> >	m_BakServerMapByGroup;	// 备份连接管理

    std::map<unsigned int, IKxComm*> m_GuestMap;		        // 未验证的客户端连接管理
    std::map<unsigned int, IKxComm*> m_UserMap;       		// 已经验证过的客户端连接管理
	bool		m_bChanging;											//是否在切换中

	std::map<int, IKxComm* >	m_ServerList;		// 服务端连接管理

};

#endif
