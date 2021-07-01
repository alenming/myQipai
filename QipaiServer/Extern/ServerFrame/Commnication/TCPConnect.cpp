#include "TCPConnect.h"
#include "LogManager.h"


TCPConnect::TCPConnect()
: m_IsConnecting(false)
{
}

TCPConnect::~TCPConnect()
{
}

bool TCPConnect::connect(const char* addr, int port, bool nonblock)
{
    if (nullptr == m_Socket)
    {
        return false;
    }
    
    if (nonblock)
    {
        m_Socket->setSockNonblock();
    }
    m_Socket->setSockKeepAlive();

    if (0 == m_Socket->connect(addr, port))
    {
        onConnected(true);
        return true;
    }
    else
    {
        // 如果是非阻塞连接
        if (nonblock && m_Socket->isSockBlockError())
        {
            m_IsConnecting = true;
            changePollType(POLLTYPE_OUT);
			//KX_LOGDEBUG("warn: socket %d KxTCPConnector::connect m_PollType %d", getCommId(), m_PollType);
            return true;
        }
        onConnected(false);
        return false;
    }
}

int TCPConnect::onSend()
{
    if (m_IsConnecting)
    {
        int e;
        kxSockLen elen = sizeof(e);
        if (getsockopt(getCommId(), SOL_SOCKET, SO_ERROR, (char*)&e, &elen) < 0)
        {
            LOGERROR("error: KxTCPConnector::onSend %d getsockopt faile, errno %d", getCommId(), m_Socket->getSockError());
            onConnected(false);
            return -1;
        }
        if (e) 
        {
            if (SOCKERR_CONNECT_RETRIABLE(e))
            {
                return 0;
            }
            LOGERROR("error: KxTCPConnector::onSend sock %d errno %d", getCommId(), e);
            onConnected(false);
            return -1;
        }
        onConnected(true);
        return 0;
    }
    else
    {
        return TCPUnit::onSend();
    }
}

int TCPConnect::onRecv()
{
    if (m_IsConnecting)
    {
        return 0;
    }
    else
    {
        return TCPUnit::onRecv();
    }
}

void TCPConnect::onConnected(bool success)
{
    if (nullptr != m_ProcessModule)
    {
        m_ProcessModule->processEvent(success ? EVENT_CONNECT_SUCCESS : EVENT_CONNECT_FAILE, this);
    }

	m_IsConnecting = false;

    if (success)
    {
        // 切换polltype，开启pollin，关闭pollout
        // m_PollType |= KXPOLLTYPE_IN;
        // m_PollType &= ~KXPOLLTYPE_OUT;
        changePollType(POLLTYPE_IN);
		//KX_LOGDEBUG("warn: socket %d KxTCPConnector::onConnected m_PollType %d", getCommId(), m_PollType);
    }
}

