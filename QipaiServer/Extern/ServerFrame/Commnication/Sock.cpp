#include "Sock.h"
#include "LogManager.h"


bool Sock::s_IsInit = false;

bool Sock::initSock()
{
#if(KX_TARGET_PLATFORM == KX_PLATFORM_WIN32)
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    
    wVersionRequested = MAKEWORD(2, 2);
    
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0)
    {
        return false;
    }
    
    if (LOBYTE(wsaData.wVersion) != 2 ||
        HIBYTE(wsaData.wVersion) != 2)
    {
        WSACleanup();
        return false;
    }
#endif
    s_IsInit = true;
    return true;
}

void Sock::uninitSock()
{
#ifdef WIN32
    WSACleanup();
#endif
    s_IsInit = false;
}

Sock::Sock()
: m_SockType(SOCK_UNKNOWN)
, m_Sock(INVALID_COMMID)
{
	if (!s_IsInit)
	{
		initSock();
	}
}

Sock::~Sock()
{
	close();
}

bool Sock::init(SOCK_TYPE type, SOCK_VERSION sv)
{
	if (INVALID_COMMID != m_Sock)
	{
		return false;
	}

	m_SockType = type;
	m_SockVersion = sv;
	switch (m_SockType)
	{
	case SOCK_TCP:
	{
		if (sv == V_IPV6)
		{
			return true;
		}
		m_Sock = socket(AF_INET, SOCK_STREAM, 0);
	}
	break;

	case SOCK_UDP:
		m_Sock = socket(sv == V_IPV4 ? AF_INET : AF_INET6, SOCK_DGRAM, 0);
		break;

	default:
		break;
	}
	if (m_Sock == INVALID_COMMID)
	{
		echoSockError("init");
		return false;
	}
	return true;
}

bool Sock::init(COMMID fd)
{
    if (INVALID_COMMID != m_Sock)
    {
        LOGERROR("error: KxSock::init sock %d init with %d faile", m_Sock, fd);
        return false;
    }
    
    m_Sock = fd;
    m_SockType = SOCK_TCP;
    return m_Sock != INVALID_COMMID;
}

int Sock::listen(int maxListenQueue)
{
    int ret = ::listen(m_Sock, maxListenQueue);
    if (ret < 0)
    {
        echoSockError("listen");
    }
    return ret;
}

int Sock::connect(const char* addr, int port)
{
	sockaddr_in name;
	sockInitAddr(name, port, addr);
    return ::connect(m_Sock, (sockaddr*)&name, sizeof(sockaddr));
}

int Sock::bind(const char* addr, int port)
{
	sockaddr_in name;
	sockInitAddr(name, port, addr);	
    int ret = ::bind(m_Sock, (sockaddr*)&name, sizeof(sockaddr));
    if (ret < 0)
    {
        echoSockError("bind");
    }
    return ret;
}

COMMID Sock::accept()
{
	sockaddr_in name;
	int len = sizeof(sockaddr);
    COMMID ret = ::accept(m_Sock, (sockaddr*)&name, (kxSockLen*)&len);
    if (ret != INVALID_COMMID)
    {
       //KX_LOGDEBUG("debug: KxSock::accept ip %s prot %d socketId %d", inet_ntoa(name.sin_addr), name.sin_port, ret);
    }
    else
    {
        echoSockError("accept");
    }
    return ret;
}

int Sock::send(const char* buffer, int size)
{
    switch (m_SockType)
    {
        case SOCK_TCP:
            return (int)::send(m_Sock, buffer, size, 0);
    
        case SOCK_UDP:
            return (int)::sendto(m_Sock, buffer, size, 0, (sockaddr*)&m_SockAddr, sizeof(m_SockAddr));
    
        default:
            return SOCK_ERRORTYPE;
    }
}

int Sock::recv(char* buffer, int size)
{
    switch (m_SockType)
    {
        case SOCK_TCP:
            return (int)::recv(m_Sock, buffer, size, 0);

        case SOCK_UDP:
            {
                int len = sizeof(sockaddr);
                return (int)::recvfrom(m_Sock, buffer, size, 0, (sockaddr*)&m_SockAddr, (kxSockLen*)&len);
            }

        default:
            return SOCK_ERRORTYPE;
    }
}

void Sock::close()
{
    LOGDEBUG("debug: KxSock::close %d close", m_Sock);
#if(KX_TARGET_PLATFORM == KX_PLATFORM_WIN32)
    ::closesocket(m_Sock);
#else
	::close(m_Sock);
#endif
    m_Sock = INVALID_COMMID;
}

void Sock::setSockAddr(kxSocketAddr &name)
{
	m_SockAddr = name;
}

void Sock::setSockAddr(const char* ip, int port)
{
	sockInitAddr(m_SockAddr, port, ip);
}

void Sock::setSockNonblock()
{
#if(KX_TARGET_PLATFORM == KX_PLATFORM_WIN32)
    u_long nonblocking = 1;
	if (ioctlsocket(m_Sock, FIONBIO, &nonblocking) == SOCKET_ERROR)
    {
        echoSockError("setSockNonblock");
    }
#else
    int flags;
	if ((flags = fcntl(m_Sock, F_GETFL, nullptr)) < 0) 
    {
        echoSockError("setSockNonblock fcntl F_GETFL");
        return;
    }
	if (fcntl(m_Sock, F_SETFL, flags | O_NONBLOCK) == -1) 
    {
        echoSockError("setSockNonblock fcntl F_SETFL");
        return;
    }
#endif
}

void Sock::setSockKeepAlive()
{
    int on = 1;
	if (setsockopt(m_Sock, SOL_SOCKET, SO_KEEPALIVE, (char*)&on, sizeof(on))<0)
    {
        echoSockError("setSockKeepAlive setsockopt SO_KEEPALIVE");
    }
}

void Sock::setSockNondelay()
{
	int on = 1;
	if (setsockopt(m_Sock, IPPROTO_TCP, TCP_NODELAY, (char*)&on, sizeof(on)) < 0)
    {
        echoSockError("setSockNondelay");
    }
}

void Sock::setSockAddrReuse()
{
    int on = 1;
	if (setsockopt(m_Sock, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0)
    {
        echoSockError("setSockAddrReuse");
    }
}

void Sock::sockInitAddr(kxSocketAddr &name, int port, const char* ip)
{
	name.sin_family		= AF_INET;
	name.sin_port		= htons(port);
#if(KX_TARGET_PLATFORM == KX_PLATFORM_WIN32)
    name.sin_addr.S_un.S_addr = (nullptr == ip) ? htonl(INADDR_ANY) : inet_addr(ip);
#else
    name.sin_addr.s_addr = (nullptr == ip) ? htonl(INADDR_ANY) : inet_addr(ip);
#endif
}

bool Sock::isSockBlockError()
{
#if(KX_TARGET_PLATFORM == KX_PLATFORM_WIN32)
    int errorCode = WSAGetLastError();
    if (WSAEWOULDBLOCK == errorCode || WSAEINPROGRESS == errorCode
        || WSAEINTR == errorCode)
    {
        return true;
    }
    //KX_LOGERROR("error: sock %d isSockBlockError %d ", m_Sock, errorCode);
#else
    if (errno == EWOULDBLOCK || errno == EAGAIN 
        || errno == EINPROGRESS || errno == EINTR)
    {
        return true;
    }
    //KX_LOGERROR("error: sock %d isSockBlockError %d %s", m_Sock, errno, strerror(errno));
#endif
    return false;
}

int Sock::getSockError()
{
#if(KX_TARGET_PLATFORM == KX_PLATFORM_WIN32)
    return WSAGetLastError();
#else
    return errno;
#endif
}

void Sock::echoSockError(const char* msg)
{
#if(KX_TARGET_PLATFORM == KX_PLATFORM_WIN32)
    int errorCode = WSAGetLastError();
    //KX_LOGERROR("error: KxSock::echoSockError sock %d msg %s SockError %d ", m_Sock, msg, errorCode);
#else
    //KX_LOGERROR("error: KxSock::echoSockError sock %d msg %s SockError %d %s", m_Sock, msg, errno, strerror(errno));
#endif
}

