/*
*
*   �ͻ������Ӷ���
*
*   1. ʵ��������ʱ�Զ��Ͽ�����
*   2. ��¼Ȩ�ޡ��ÿ�ID���û�ID
*   3. ��¼���û���·��ת������
*
*/
#ifndef __SESSION_CLIENTER_H__
#define __SESSION_CLIENTER_H__

#include "KXServer.h"
#include<map>

class SessionClient : public KxTCPClienter
{
public:
	SessionClient();
	virtual ~SessionClient(void);

    virtual void retain() 
    {
        KxObject::retain();
    }
    virtual void release()
    {
        KxObject::release();
    }

	bool setConServer(int nKey,int nValue);
	bool sendDataToServer(int mainCmd, int subCmd, char *pszContext, int nLen);
    bool sendDataToGroupServer(int nGroupID, char *pszContext, int nLen);
    bool sendDataToAllServer(char *pszContext, int nLen);
	int getRouteValue(int nKey);

    virtual int onRecv();
    virtual void setTimer();
    virtual void onTimer();
    virtual void clean();

    inline void setPermission(unsigned int permission) { m_Permission = permission; }
	inline unsigned int getPermission() { return m_Permission; }
    inline void setUserId(unsigned int userId) { m_UserId = userId; }
    inline unsigned int getUserId() { return m_UserId; }
    inline void setGuestId(unsigned int guestId) { m_GuestId = guestId; }
    inline unsigned int getGuestId() { return m_GuestId; }

private:
	unsigned int m_Permission;	                    // Ȩ��
    unsigned int m_GuestId;	                // δ��֤�ķÿ�ID
    unsigned int m_UserId;                  // ����֤���û�ID
    std::map<int, int> m_MapConKeyValue;    // ��ȡ��Ӧ��key��Ӧ��ֵ
	KxTimerCallback<SessionClient>* m_TimerCallBack;                    
};

#endif //__SESSION_CLIENTER_H__
