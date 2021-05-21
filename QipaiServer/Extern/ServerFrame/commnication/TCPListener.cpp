#include "TCPListener.h"
#include "log/LogManager.h"

TCPListener::TCPListener()
: m_Socket(nullptr)
, m_ClientModule(nullptr)
{
}

TCPListener::~TCPListener()
{
    KXSAFE_RELEASE(m_Socket);
    KXSAFE_RELEASE(m_ClientModule);
}

// 初始化Sock
bool TCPListener::init()
{
    changePollType(KXPOLLTYPE_IN);
    m_Socket = new Sock();
    return m_Socket->init(KXSOCK_TCP);
}

// 绑定到端口并监听
bool TCPListener::listen(int port, char* addr, bool nonblock, int listenQ)
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

// Accept时回调
TCPClienter* TCPListener::onAccept(KXCOMMID client)
{
    TCPClienter* tcpClient = new TCPClienter();
    if (tcpClient->init(client))
    {
        return tcpClient;
    }
    else
    {
        KX_LOGERROR("error: KxTCPListener::onAccept tcpClient %d init faile", client);
        tcpClient->release();
        return nullptr;
    }
}

// 接收到数据时触发的回调，由IKxCommPoller调用
int TCPListener::onRecv()
{
    TCPClienter* client = nullptr;
    while (true)
    {
        KXCOMMID fd = m_Socket->accept();
        if (KXINVALID_COMMID == fd || fd < 0)
        {
            // 检查错误
            int errorNo = m_Socket->getSockError();
			if (!m_Socket->isSockBlockError())
            {
                //KX_LOGERROR("error: KxTCPListener::onRecv accept error fd %d, errno %d", fd, errorNo);
            }
            break;
        }

        client = onAccept(fd);
        if (nullptr != client)
        {
            client->setModule(m_ClientModule);
            if (nullptr != m_ProcessModule)
            {
                // 需要进行retain
                m_ProcessModule->processLogic(nullptr, 0, client);
            }
            client->release();
        }
    }
    return 0;
}

// 数据可被发送时触发的回调，由IKxCommPoller调用
int TCPListener::onSend()
{
    return 0;
}

void TCPListener::setClientModule(IKxModule* module)
{
    KXSAFE_RELEASE(m_ClientModule);
    m_ClientModule = module; 
    KXSAFE_RETAIN(m_ClientModule);
}

// 发送数据
int TCPListener::sendData(const char* buffer, unsigned int len)
{
    return 0;
}

// 接收数据
int TCPListener::recvData(char* buffer, unsigned int len)
{
    return 0;
}
