#include "KxTCPListener.h"
#include "log/LogManager.h"

KxTCPListener::KxTCPListener()
: m_Socket(NULL)
, m_ClientModule(NULL)
{
}

KxTCPListener::~KxTCPListener()
{
    KXSAFE_RELEASE(m_Socket);
    KXSAFE_RELEASE(m_ClientModule);
}

// ��ʼ��Sock
bool KxTCPListener::init()
{
    changePollType(KXPOLLTYPE_IN);
    m_Socket = new KxSock();
    return m_Socket->init(KXSOCK_TCP);
}

// �󶨵��˿ڲ�����
bool KxTCPListener::listen(int port, char* addr, bool nonblock, int listenQ)
{
    if (nonblock)
    {
        m_Socket->setSockNonblock();
    }
    m_Socket->setSockAddrReuse();
    m_Socket->setSockKeepAlive();
    if (0 > m_Socket->bind(addr, port) || 0 > m_Socket->listen(listenQ))
    {
        return false;
    }
    return true;
}

// Acceptʱ�ص�
KxTCPClienter* KxTCPListener::onAccept(KXCOMMID client)
{
    KxTCPClienter* tcpClient = new KxTCPClienter();
    if (tcpClient->init(client))
    {
        return tcpClient;
    }
    else
    {
        KX_LOGERROR("error: KxTCPListener::onAccept tcpClient %d init faile", client);
        tcpClient->release();
        return NULL;
    }
}

// ���յ�����ʱ�����Ļص�����IKxCommPoller����
int KxTCPListener::onRecv()
{
    KxTCPClienter* client = NULL;
    while (true)
    {
        KXCOMMID fd = m_Socket->accept();
        if (KXINVALID_COMMID == fd || fd < 0)
        {
            // ������
            int errorNo = m_Socket->getSockError();
			if (!m_Socket->isSockBlockError())
            {
                KX_LOGERROR("error: KxTCPListener::onRecv accept error fd %d, errno %d", fd, errorNo);
            }
            break;
        }

        client = onAccept(fd);
        if (NULL != client)
        {
            client->setModule(m_ClientModule);
            if (NULL != m_ProcessModule)
            {
                // ��Ҫ����retain
                m_ProcessModule->processLogic(NULL, 0, client);
            }
            client->release();
        }
    }
    return 0;
}

// ���ݿɱ�����ʱ�����Ļص�����IKxCommPoller����
int KxTCPListener::onSend()
{
    return 0;
}

void KxTCPListener::setClientModule(IKxModule* module)
{
    KXSAFE_RELEASE(m_ClientModule);
    m_ClientModule = module; 
    KXSAFE_RETAIN(m_ClientModule);
}

// ��������
int KxTCPListener::sendData(const char* buffer, unsigned int len)
{
    return 0;
}

// ��������
int KxTCPListener::recvData(char* buffer, unsigned int len)
{
    return 0;
}
