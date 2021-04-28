#include "KxTCPConnector.h"
#include "log/LogManager.h"


KxTCPConnector::KxTCPConnector()
: m_IsConnecting(false)
{
}

KxTCPConnector::~KxTCPConnector()
{
}

bool KxTCPConnector::connect(const char* addr, int port, bool nonblock)
{
    if (NULL == m_Socket)
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
        // ����Ƿ���������
        if (nonblock && m_Socket->isSockBlockError())
        {
            m_IsConnecting = true;
            changePollType(KXPOLLTYPE_OUT);
			//KX_LOGDEBUG("warn: socket %d KxTCPConnector::connect m_PollType %d", getCommId(), m_PollType);
            return true;
        }
        onConnected(false);
        return false;
    }
}

int KxTCPConnector::onSend()
{
    if (m_IsConnecting)
    {
        int e;
        kxSockLen elen = sizeof(e);
        if (getsockopt(getCommId(), SOL_SOCKET, SO_ERROR, (char*)&e, &elen) < 0)
        {
            KX_LOGERROR("error: KxTCPConnector::onSend %d getsockopt faile, errno %d", getCommId(), m_Socket->getSockError());
            onConnected(false);
            return -1;
        }
        if (e) 
        {
            if (SOCKERR_CONNECT_RETRIABLE(e))
            {
                return 0;
            }
            KX_LOGERROR("error: KxTCPConnector::onSend sock %d errno %d", getCommId(), e);
            onConnected(false);
            return -1;
        }
        onConnected(true);
        return 0;
    }
    else
    {
        return KxTCPUnit::onSend();
    }
}

int KxTCPConnector::onRecv()
{
    if (m_IsConnecting)
    {
        return 0;
    }
    else
    {
        return KxTCPUnit::onRecv();
    }
}

void KxTCPConnector::onConnected(bool success)
{
    if (NULL != m_ProcessModule)
    {
        m_ProcessModule->processEvent(
            success ? KXEVENT_CONNECT_SUCCESS : KXEVENT_CONNECT_FAILE, this);
    }

	m_IsConnecting = false;

    if (success)
    {
        // �л�polltype������pollin���ر�pollout
        // m_PollType |= KXPOLLTYPE_IN;
        // m_PollType &= ~KXPOLLTYPE_OUT;
        changePollType(KXPOLLTYPE_IN);
		KX_LOGDEBUG("warn: socket %d KxTCPConnector::onConnected m_PollType %d", getCommId(), m_PollType);
    }
}

