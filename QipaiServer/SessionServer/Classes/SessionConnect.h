#ifndef SESSION_CONNECT_H__
#define SESSION_CONNECT_H__

#include <map>
#include "TCPConnect.h"
#include "TimerCallback.h"

#define RECONNECT_TIME_INTERVAL		5			// 重连间隔5秒
#define MAX_RECONECT_TIME			0xFFFFFFFF	// 重连最大次数

class SessionConnect : public TCPConnect
{
public:
	SessionConnect();
	virtual ~SessionConnect();

	// 连接指定的IP和端口
	virtual bool connect(const char* addr, int port, int serverId = 0, bool nonblock = true);
	// 连接成功失败回调
	virtual void onConnected(bool success);
	// 发生错误时回调，由IKxCommPoller调用
	virtual int onError();

	// 设置定时重连
	virtual void setTimer(int nSec);
	// 定时触发
	virtual void onTimer();

	int getServerId() { return m_ServerId; }

	std::string getServerName()
	{
		return m_ServerName;
	}
	void setServerName(std::string serverName)
	{
		memcpy(m_ServerName, serverName.c_str(), sizeof(m_ServerName));
	}

private:
	// 重连
	bool reconnect();

private:
	int m_ServerId;			// 服务器组ID
	int m_Port;			    // 服务器端口
	char m_strIP[16];	    // 服务器IP
	TimerCallback<SessionConnect>* m_TimerCallBack;
};


#endif //SESSION_CONNECT_H__
