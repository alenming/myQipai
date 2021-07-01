#include "UDPUnit.h"

// 单个UDP包的最大长度
#define MAX_UDP_PKG_LEN 2048



UDPUnit::UDPUnit()
: m_Socket(nullptr)
, m_RecvBuffer(nullptr)
{
}

UDPUnit::~UDPUnit()
{
    if (nullptr != m_RecvBuffer)
    {
		delete[](char*)m_RecvBuffer;
    }
    m_Socket->release();
}

bool UDPUnit::init()
{
    m_Socket = new Sock();
    if (m_Socket->init(SOCK_UDP))
    {
        return false;
    }

	m_RecvBuffer = static_cast<char*>(new char[MAX_UDP_PKG_LEN]);
    return m_RecvBuffer != nullptr;
}

int UDPUnit::sendData(const char* buffer, unsigned int len)
{
    int ret = m_Socket->send(buffer, len);
    if (ret < 0 && !m_Socket->isSockBlockError())
    {
        onError();
    }
    return ret;
}

int UDPUnit::sendData(const char* buffer, unsigned int len, char* ip, int port)
{
    setSendToAddr(ip, port);
    return sendData(buffer, len);
}

int UDPUnit::recvData(char* buffer, unsigned int len)
{
	int ret = m_Socket->recv(buffer, len);
    if (ret < 0 && !m_Socket->isSockBlockError())
    {
        onError();
    }
    return ret;
}

int UDPUnit::onRecv()
{
    int ret = m_Socket->recv(m_RecvBuffer, MAX_UDP_PKG_LEN);
    if (ret < 0 && !m_Socket->isSockBlockError())
    {
        return -1;
    }
    else if (nullptr != m_ProcessModule)
    {
        m_ProcessModule->processLogic(m_RecvBuffer, ret, this);
    }
	return ret;
}

int UDPUnit::onSend()
{
	return 0;
}

void UDPUnit::close()
{
    m_Socket->close();
}

int UDPUnit::bind(char* ip, int port)
{
	return m_Socket->bind(ip, port);
}

int UDPUnit::setSendToAddr(char* ip, int port)
{
	m_Socket->setSockAddr(ip, port);
	return 0;
}

