/*
*
*   客户端连接对象
*
*   1. 实现心跳超时自动断开功能
*   2. 记录权限、访客ID、用户ID
*   3. 记录该用户的路由转发规则
*
*/
#ifndef __SESSION_CLIENTER_H__
#define __SESSION_CLIENTER_H__
#include<map>
#include "TCPClienter.h"
#include "TimerCallback.h"

class SessionClient : public TCPClienter
{
public:
	SessionClient();
	virtual ~SessionClient(void);

    virtual void retain() 
    {
        _Object::retain();
    }
    virtual void release()
    {
        _Object::release();
    }

	bool setConServer(int nKey,int nValue);
	bool sendDataToServer(int mainCmd, int subCmd, char *pszContext, int nLen);
    bool sendDataToGroupServer(int nGroupID, char *pszContext, int nLen);
    bool sendDataToAllServer(char *pszContext, int nLen);
	int  getRouteValue(int nKey);

    virtual int  onRecv();
    virtual void setTimer();
    virtual void onTimer();
    virtual void clean();

	inline void setPermission(unsigned int permission) { m_Permission = permission; }
	inline int getPermission() { return m_Permission; }

	inline void setUserId(int userId) { m_UserId = userId; }
    inline unsigned int getUserId() { return m_UserId; }
	inline void setGuestId(unsigned int guestId) { m_GuestId = guestId; }
	inline unsigned int getGuestId() { return m_GuestId; }
private:
	int m_Permission;	                    // 权限
	int m_UserId;							//已验证的用户ID
	unsigned int m_GuestId;	                // 未验证的访客ID
    std::map<int, int> m_MapConKeyValue;    // 获取对应的key相应的值
	TimerCallback<SessionClient>* m_TimerCallBack;                    
};

#endif //__SESSION_CLIENTER_H__
