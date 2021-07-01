#include "TCPUnit.h"
#include "LogManager.h"

#define BUFF_SIZE 4096
#define MAX_TCP_PKG_LEN 1<<16


char* TCPUnit::s_RecvBuffer = nullptr;

TCPUnit::TCPUnit()
: m_Socket(nullptr)
, m_SendBuffer(nullptr)
, m_RecvBuffer(nullptr)
, m_SendBufferLen(0)
, m_RecvBufferLen(0)
, m_SendBufferOffset(0)
, m_RecvBufferOffset(0)
{
    // 分配全局的接收缓冲区
    if (nullptr == s_RecvBuffer)
    {
		s_RecvBuffer = static_cast<char*>(new char[BUFF_SIZE]);
    }
	memset(m_ServerName, 0, sizeof(m_ServerName));
}

TCPUnit::~TCPUnit()
{
    KXSAFE_RELEASE(m_Socket);
    KXSAFE_RELEASE(m_Poller);

	delete[](char*)m_SendBuffer;
	delete[](char*)m_RecvBuffer;
	// 先清空列表中缓存的数据
    BufferNode* node = m_BufferList.head();
    while (nullptr != node)
    {
		delete[](char*)node->buffer;
        node = node->next;
    }
	// 清空列表
	m_BufferList.clear();
}
bool TCPUnit::init()
{
	return init(V_IPV4);
}

bool TCPUnit::init(SOCK_VERSION ipv)
{
	m_PollType = POLLTYPE_IN;
	m_Socket = new Sock();
	return m_Socket->init(SOCK_TCP, ipv);
}

// 发送数据
int TCPUnit::sendData(const char* buffer, unsigned int len)
{
    int ret = 0;
    // 1. 当待发送列表为空时说明前面没有数据待发送，可以发送
    // 2. 否则当要发送的buffer等于m_SendBuffer + m_SendBufferOffset时说明是onSend的自动发送，可以发送
    if (m_BufferList.isEmpty() || (m_SendBuffer != nullptr && buffer == (m_SendBuffer + m_SendBufferOffset)))
    {
        ret = m_Socket->send(buffer, len);
        if (ret < 0)
        {
            int errorNo = m_Socket->getSockError();
            if (SOCKERR_RW_RETRIABLE(errorNo))
            {
                ret = 0;
            }
            else
            {
                LOGERROR("error: KxTCPUnit::sendData %d send datalen %d faile, errno %d", getCommId(), len, errorNo);
                // 如果是onSend调用的，则不执行onError
                if (m_Poller != nullptr && m_Poller->getCurrentPollObject() != this)
                {
                    retain();
                    m_Poller->removeCommObject(this);
                    onError();
                    release();
                }
                return ret;
            }
        }
    }

    // 如果ret < len，说明有数据未发送完——要发送的长度小于实际发送的长度
    // 前面已经过滤了发送异常，这里将要发送的数据缓存，并依次发送
    if (ret < static_cast<int>(len))
    {
        // 如果发送的不是m_SendBuffer中的数据，追加到m_BufferList尾部
        if (buffer != (m_SendBuffer + m_SendBufferOffset))
        {
            len -= ret;
            // 未能一次性发送完的日志
            LOGDEBUG("warn: socket %d send %d data ret %d, cache unsend data!!!",
                getCommId(), len + ret, ret);
            char* buf = static_cast<char*>(new char[len]);
            memcpy(buf, buffer + ret, len);
            m_BufferList.pushBack(buf, len);
			LOGDEBUG("warn: socket %d changePollType m_PollType %d, cache unsend data!!!", getCommId(), m_PollType);
            addPollType(POLLTYPE_OUT);
        }
        // 如果是m_SendBuffer中的数据，说明是在onSend回调中发送的
        // onSend处理了再次发送失败或没发送完的逻辑
    }

    return ret;
}

// 接收数据
int TCPUnit::recvData(char* buffer, unsigned int len)
{
    int ret = m_Socket->recv(buffer, len);
    if (ret <= 0)
    {
        if (m_Socket->isSockBlockError())
        {
            ret = 0;
        }
        else
        {
            if (m_Poller != nullptr && m_Poller->getCurrentPollObject() != this)
            {
                retain();
                m_Poller->removeCommObject(this);
                onError();
                release();
            }
            ret = -1;
        }
    }
    return ret;
}

// 接收到数据时触发的回调，由IKxCommPoller调用
int TCPUnit::onRecv()
{
    memset(s_RecvBuffer, 0, BUFF_SIZE);
    int requestLen = 0;
    int state = 0;
    int ret = m_Socket->recv(s_RecvBuffer, BUFF_SIZE);
    if (ret == BUFF_SIZE)
    {
        state = 1;
    }
	
    // 如果是非阻塞
    if (ret < 0 && m_Socket->isSockBlockError())
    {
        return 0;
    }
    else if (ret <= 0)
    {
		//KX_LOGERROR("warn: ret <= 0 %d bit data in socket %d error package length %d", ret, getCommId(), requestLen);
        return -1;
    }
    else if (nullptr != m_ProcessModule && ret > 0)
    {
		//KxLogger::getInstance()->HexDumpImp(s_RecvBuffer, ret);
        char* processBuf = s_RecvBuffer;
        char* stickBuf = nullptr;

        // 如果有半包，先拼接到半包的后面，注意newsize
        if (nullptr != m_RecvBuffer)
        {
            unsigned int newsize = ret;
            // 如果接到的包有多余的内容，为stickBuf赋值，并调整newsize，使其只拷贝该包的剩余内容到半包中
            if ((m_RecvBufferLen - m_RecvBufferOffset) < static_cast<unsigned int>(ret))
            {
                newsize = m_RecvBufferLen - m_RecvBufferOffset;
                stickBuf = processBuf + newsize;
            }
            memcpy(m_RecvBuffer + m_RecvBufferOffset, processBuf, newsize);
            //KXLOGDEBUG("contact half package to m_RecvBuffer, buffer offset %d buffer size %d ", m_RecvBufferOffset, m_RecvBufferLen);
            ret += m_RecvBufferOffset;
            m_RecvBufferOffset += newsize;
            processBuf = m_RecvBuffer;
        }

        // 完整包长度判断
        requestLen = m_ProcessModule->processLength(processBuf, ret);
        if (requestLen <= 0 || requestLen > MAX_TCP_PKG_LEN)
        {
            LOGERROR("warn: recv %d bit data in socket %d error package length %d", ret, getCommId(), requestLen);
            return -1;
        }

        // 如果不足以处理，且没有半包，则为半包分配空间
        if (ret < requestLen && nullptr == m_RecvBuffer)
        {
            m_RecvBuffer = static_cast<char*>(new char[requestLen]);
            m_RecvBufferLen = requestLen;
            m_RecvBufferOffset = ret;
            memcpy(m_RecvBuffer, processBuf, ret);
            return state;
        }
        // 这是一种复杂的情况...，当半包变大
        // 首先需要保证原先的半包正常拼接，然后再处理
        else if (nullptr != stickBuf)
        {
            // 分配一块新的内存来缓存更大的半包
            char* oldRecvBuffer = m_RecvBuffer;
            unsigned int oldRecvBufferLen = m_RecvBufferLen;
			m_RecvBuffer = static_cast<char*>(new char[requestLen]);

            // 先将原先半包内容复制到新内存中
            memcpy(m_RecvBuffer, oldRecvBuffer, oldRecvBufferLen);
			delete[](char*)oldRecvBuffer;
            // 将后面额外的半包内容追加
            // 如果半包还是不能处理，变成了一个更大的半包，则进一步拼接后返回
            if (ret < requestLen)
            {
                memcpy(m_RecvBuffer + oldRecvBufferLen, stickBuf, ret - oldRecvBufferLen);
                m_RecvBufferLen = requestLen;
                m_RecvBufferOffset = ret;
                LOGERROR("warn: something terrible, you are so lucky");
            }
            // 如果半包可以处理了，但后面还有更多内容
            else
            {
                memcpy(m_RecvBuffer + oldRecvBufferLen, stickBuf, requestLen - oldRecvBufferLen);
                m_RecvBufferLen = requestLen;
                m_RecvBufferOffset = requestLen;
                // 处理m_RecvBuffer，并将stickBuf进行偏移
                processBuf = m_RecvBuffer;
                stickBuf += (requestLen - oldRecvBufferLen);
                LOGERROR("warn: another terrible, you are so lucky too");
            }
        }

        // 粘包处理
        while (ret >= requestLen)
        {
            m_ProcessModule->processLogic(processBuf, requestLen, this);
            //KXLOGDEBUG("process stick package, ret %d requestLen %d processBuf %x\n", ret, requestLen, processBuf);
            processBuf += requestLen;

            if (nullptr != m_RecvBuffer)
            {
                processBuf = stickBuf;
				delete[](char*)m_RecvBuffer;
                m_RecvBuffer = nullptr;
                m_RecvBufferOffset = m_RecvBufferLen = 0;
            }

            ret -= requestLen;
            if (ret > 0 && nullptr != processBuf)
            {
                requestLen = m_ProcessModule->processLength(processBuf, ret);
                if (requestLen <= 0 || requestLen > MAX_TCP_PKG_LEN)
                {
                    LOGERROR("warn: recv %d bit data in socket %d error package length %d", ret, getCommId(), requestLen);
					//KxLogger::getInstance()->HexDumpImp(processBuf, requestLen);
                    return -1;
                }
                //半包缓存
                else if (ret < requestLen)
                {
                    m_RecvBuffer = static_cast<char*>(new char[requestLen]);
                    m_RecvBufferLen = requestLen;
                    m_RecvBufferOffset = ret;
                    memcpy(m_RecvBuffer, processBuf, ret);
                    return state;
                }
            }
        }
    }

    return state;
}

// 数据可被发送时触发的回调，由IKxCommPoller调用
int TCPUnit::onSend()
{
    // 将缓存列表中的数据逐个发送，直到发送完或触发EAGAIN等非阻塞错误
again:
    // 将m_BufferList的第一个节点放到m_SendBuffer中
    if (nullptr == m_SendBuffer)
    {
        // 取出下一个待发送的缓存数据
        BufferNode* node = m_BufferList.next();
        if (nullptr != node)
        {
            m_SendBuffer = node->buffer;
            m_SendBufferLen = node->len;
            m_SendBufferOffset = 0;
            delete node;
			LOGDEBUG("warn: KxTCPUnit::onSend() NULL != node fd %d", getCommId());
        }
        else
        {
            // 无数据需要发送
            changePollType(POLLTYPE_IN);
			LOGDEBUG("warn: socket %d KxTCPUnit::onSend() NULL = node m_PollType %d", getCommId(), m_PollType);
            return 0;
        }
    }

    // 应该发送的数据
    int sendLen = m_SendBufferLen - m_SendBufferOffset;
    int len = sendData(m_SendBuffer + m_SendBufferOffset, sendLen);
	LOGDEBUG("warn: KxTCPUnit::onSend() fd %d sendlen %d success", getCommId(), len);
    if (len >= sendLen)
    {
        //回收已经发送出去的内存
		delete[](char*)m_SendBuffer;
        BufferNode* node = m_BufferList.head();
        m_SendBuffer = nullptr;
        m_SendBufferLen = m_SendBufferOffset = 0;

        if (nullptr == node)
        {
            changePollType(POLLTYPE_IN);
        }
        else
        {
            goto again;
        }
    }
    else if (len < sendLen && len >= 0)
    {
        m_SendBufferOffset += len;
        addPollType(POLLTYPE_OUT);
		LOGDEBUG("warn: socket %d KxTCPUnit::onSend() m_PollType %d", getCommId(), m_PollType);    
    }

    return len;
}
