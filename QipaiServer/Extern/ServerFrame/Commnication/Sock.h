/*
 * 基础Socket通讯对象
 * 定义了非阻塞，非延迟，发送和接收接口
 * 定义Socket错误异常
 */
#ifndef _SOCKET__
#define _SOCKET__

#include "Core.h"

enum SOCK_VERSION
{
	V_IPV4,							//使用ipv4
	V_IPV6,							//使用ipv6
};

enum SOCK_TYPE
{
    SOCK_UNKNOWN,
    _SOCK_RAW,
    SOCK_TCP,
    SOCK_UDP,
};

enum SOCK_CODE
{
    SOCK_ERRORUNKNOW = -9999,		    // 未知的Socket错误
    SOCK_ERRORFAILE,                  // Socket操作失败
    SOCK_ERRORTYPE,                   // Socket类型错误:非TCP和UDP，或UDP调用TCP操作

	SOCK_SUCCESS		= 0				// 成功返回
};

typedef sockaddr_in kxSocketAddr;

class Sock : public _Object
{
public:
	Sock();
	
	virtual ~Sock();

    static bool initSock();

    static void uninitSock();
	
	// 初始化Socket
	bool init(SOCK_TYPE type, SOCK_VERSION sv = SOCK_VERSION::V_IPV4);
	// 根据SockFd初始化，主要用于初始化accept后的client
	bool init(COMMID fd);
	
	// 监听，TCPSocket类型下可以调用
	int listen(int maxListenQueue);
	// 链接指定的IP和端口
	int connect(const char* addr, int port);
	// 绑定到指定的Ip和端口
	int bind(const char* addr, int port);
	// Accept返回一个Socket连接对象
	COMMID accept();
    // 发送数据
    int send(const char* buffer, int size);
    // 接收数据
    int recv(char* buffer, int size);	
	// 关闭Socket
	void close();

	// 设置当前的SocketAddr,这个SocketAddr将会被用于UDP发送
	void setSockAddr(kxSocketAddr &name);
	void setSockAddr(const char* ip, int port);

    // 获取当前的Socket地址,这个地址在每次UDP接收以及SetAddr时会被更新
    inline const kxSocketAddr& getSockAddr() { return m_SockAddr; }

    // 设置非阻塞
	void setSockNonblock();
    // 设置keepalive
    void setSockKeepAlive();
	// 设置延迟（是否使用Ngale算法延时发送）
	void setSockNondelay();
    // 设置可重用地址
	void setSockAddrReuse();

    // 查询是否阻塞错误
    bool isSockBlockError();
    // 获取错误码
    int getSockError();
    // 打印错误信息
    void echoSockError(const char* msg = "");

    // 获取SockFD
    inline COMMID getSockFd()
    {
        return m_Sock;
    }

private:
	//初始化Addr结构体，包含了不同平台的实现
	void sockInitAddr(kxSocketAddr &name, int port, const char* ip = nullptr);

private:
    int             m_SockType;
    COMMID        m_Sock;
	SOCK_VERSION  m_SockVersion;
    kxSocketAddr    m_SockAddr;
    static bool     s_IsInit;
};

#endif
