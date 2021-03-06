/*
 * 实现UDP协议Socket传输单元
 */
#ifndef __UDPUNIT_H__
#define __UDPUNIT_H__

#include "Core.h"
#include "CommInterfaces.h"
#include "Sock.h"

class UDPUnit : public IComm
{
public:
	UDPUnit();
	virtual ~UDPUnit();

    // 初始化
    virtual bool init();

    // 发送数据
    virtual int sendData(const char* buffer, unsigned int len);
    virtual int sendData(const char* buffer, unsigned int len, char* ip, int port);
    // 接收数据
    virtual int recvData(char* buffer, unsigned int len);
    // 关闭该通讯对象
    virtual void close();

    // 接收到数据时触发的回调，由IKxCommPoller调用
    virtual int onRecv();
    // 数据可被发送时触发的回调，由IKxCommPoller调用
    virtual int onSend();

    // 绑定Socket
	int bind(char* ip, int port);
    // 设置发送目标IP和端口
    int setSendToAddr(char* ip, int port);

    // 获取通讯ID（通常是socket fd）
    virtual COMMID getCommId() { return m_Socket->getSockFd(); }

private:
	Sock*     m_Socket;
    char*       m_RecvBuffer;
};


#endif
