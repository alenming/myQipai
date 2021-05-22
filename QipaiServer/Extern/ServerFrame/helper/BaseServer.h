/*
 * ���������࣬����������ĳ�ʼ�������е���Ϊ
 *
 *  2013-04-20 By ��ү
 *  
 */
#ifndef __BASESERVER_H__
#define __BASESERVER_H__

#include "CommInterfaces.h"
#include "TimerManager.h"


class BaseServer : public Object
{
protected:
	BaseServer();
	virtual ~BaseServer();

public:
    static BaseServer* getInstance() 
    {
        return m_Server; 
    }
    static void destory()
    {
        if (m_Server != nullptr)
        {
            delete m_Server;
            m_Server = nullptr;
        }
    }

	// ����������
	virtual void startServer();

	// ��������ѭ��
	virtual void runServer();
    
    // ��������ѭ���ص�
    virtual void onServerUpdate();

	// ��������ʼ��
	virtual bool onServerInit();

	// ����������
	virtual void onServerUninit();

	virtual int getCurrentTime();

    virtual void stopServer() { m_IsRunning = false; }

    TimerManager* getTimerManager()
    {
        return m_TimerMgr;
    }

    ICommPoller* getMainPoller()
    {
        return m_Poller;
    }

	void SetServerID(int nServerID) { m_ServerID = nServerID; }

	int  GetServerID() { return m_ServerID; }

	virtual void setServerName(std::string serverName) { m_ServerName = serverName; }
	std::string getServerName() const { return m_ServerName; }

protected:
	ICommPoller*	m_Poller;
    TimerManager* m_TimerMgr;
	bool            m_IsRunning;
	int				m_ServerID;
	int				m_Tick;
    static BaseServer* m_Server;
	std::string		m_ServerName;
};

#endif
