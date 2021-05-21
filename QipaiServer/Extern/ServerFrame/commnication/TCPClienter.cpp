#include "TCPClienter.h"
#include "log/LogManager.h"

TCPClienter::TCPClienter()
{
	//KXLOGERROR("TCP new clienter ref = %d", m_ReferenceCount);
}

TCPClienter::~TCPClienter()
{
}

bool TCPClienter::init(KXCOMMID fd)
{
    if (nullptr == m_Socket)
    {
        m_Socket = new Sock();
    }
    changePollType(KXPOLLTYPE_IN);
	KX_LOGDEBUG("warn: socket %d KxTCPClienter::init m_PollType %d", getCommId(), m_PollType);
    if (m_Socket->init(fd))
    {
        m_Socket->setSockNonblock();
        m_Socket->setSockKeepAlive();
        return true;
    }
    return false;
}

void TCPClienter::retain()
{
	TCPUnit::retain();
	//KXLOGERROR("TCP retain clienter ref = %d", m_ReferenceCount);
}

void TCPClienter::release()
{
	TCPUnit::release();
	//KXLOGERROR("TCP release clienter ref = %d", m_ReferenceCount);
}
