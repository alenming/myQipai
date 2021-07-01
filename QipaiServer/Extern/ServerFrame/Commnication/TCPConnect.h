/*
 * 服务器之间连接的对象
 */
#ifndef __TCPCONNECTOR_H__
#define __TCPCONNECTOR_H__

#include "Core.h"
#include "TCPUnit.h"

enum EVENT_CONNECTER
{
    EVENT_CONNECT_SUCCESS,
    EVENT_CONNECT_FAILE,
};

class TCPConnect : public TCPUnit
{
public:
	TCPConnect();
	virtual ~TCPConnect();

    // 连接指定的IP和端口
	virtual bool connect(const char* addr, int port, bool nonblock = true);

    // 接收到数据时触发的回调，由IKxCommPoller调用
    virtual int onSend();
    virtual int onRecv();
    
    virtual void onConnected(bool success);
	bool isConnecting() { return m_IsConnecting; }
protected:
    bool m_IsConnecting;    // 是否正在连接
};

#endif
