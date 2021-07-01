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
    // ����ȫ�ֵĽ��ջ�����
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
	// ������б��л��������
    BufferNode* node = m_BufferList.head();
    while (nullptr != node)
    {
		delete[](char*)node->buffer;
        node = node->next;
    }
	// ����б�
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

// ��������
int TCPUnit::sendData(const char* buffer, unsigned int len)
{
    int ret = 0;
    // 1. ���������б�Ϊ��ʱ˵��ǰ��û�����ݴ����ͣ����Է���
    // 2. ����Ҫ���͵�buffer����m_SendBuffer + m_SendBufferOffsetʱ˵����onSend���Զ����ͣ����Է���
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
                // �����onSend���õģ���ִ��onError
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

    // ���ret < len��˵��������δ�����ꡪ��Ҫ���͵ĳ���С��ʵ�ʷ��͵ĳ���
    // ǰ���Ѿ������˷����쳣�����ｫҪ���͵����ݻ��棬�����η���
    if (ret < static_cast<int>(len))
    {
        // ������͵Ĳ���m_SendBuffer�е����ݣ�׷�ӵ�m_BufferListβ��
        if (buffer != (m_SendBuffer + m_SendBufferOffset))
        {
            len -= ret;
            // δ��һ���Է��������־
            LOGDEBUG("warn: socket %d send %d data ret %d, cache unsend data!!!",
                getCommId(), len + ret, ret);
            char* buf = static_cast<char*>(new char[len]);
            memcpy(buf, buffer + ret, len);
            m_BufferList.pushBack(buf, len);
			LOGDEBUG("warn: socket %d changePollType m_PollType %d, cache unsend data!!!", getCommId(), m_PollType);
            addPollType(POLLTYPE_OUT);
        }
        // �����m_SendBuffer�е����ݣ�˵������onSend�ص��з��͵�
        // onSend�������ٴη���ʧ�ܻ�û��������߼�
    }

    return ret;
}

// ��������
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

// ���յ�����ʱ�����Ļص�����IKxCommPoller����
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
	
    // ����Ƿ�����
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

        // ����а������ƴ�ӵ�����ĺ��棬ע��newsize
        if (nullptr != m_RecvBuffer)
        {
            unsigned int newsize = ret;
            // ����ӵ��İ��ж�������ݣ�ΪstickBuf��ֵ��������newsize��ʹ��ֻ�����ð���ʣ�����ݵ������
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

        // �����������ж�
        requestLen = m_ProcessModule->processLength(processBuf, ret);
        if (requestLen <= 0 || requestLen > MAX_TCP_PKG_LEN)
        {
            LOGERROR("warn: recv %d bit data in socket %d error package length %d", ret, getCommId(), requestLen);
            return -1;
        }

        // ��������Դ�����û�а������Ϊ�������ռ�
        if (ret < requestLen && nullptr == m_RecvBuffer)
        {
            m_RecvBuffer = static_cast<char*>(new char[requestLen]);
            m_RecvBufferLen = requestLen;
            m_RecvBufferOffset = ret;
            memcpy(m_RecvBuffer, processBuf, ret);
            return state;
        }
        // ����һ�ָ��ӵ����...����������
        // ������Ҫ��֤ԭ�ȵİ������ƴ�ӣ�Ȼ���ٴ���
        else if (nullptr != stickBuf)
        {
            // ����һ���µ��ڴ����������İ��
            char* oldRecvBuffer = m_RecvBuffer;
            unsigned int oldRecvBufferLen = m_RecvBufferLen;
			m_RecvBuffer = static_cast<char*>(new char[requestLen]);

            // �Ƚ�ԭ�Ȱ�����ݸ��Ƶ����ڴ���
            memcpy(m_RecvBuffer, oldRecvBuffer, oldRecvBufferLen);
			delete[](char*)oldRecvBuffer;
            // ���������İ������׷��
            // ���������ǲ��ܴ��������һ������İ�������һ��ƴ�Ӻ󷵻�
            if (ret < requestLen)
            {
                memcpy(m_RecvBuffer + oldRecvBufferLen, stickBuf, ret - oldRecvBufferLen);
                m_RecvBufferLen = requestLen;
                m_RecvBufferOffset = ret;
                LOGERROR("warn: something terrible, you are so lucky");
            }
            // ���������Դ����ˣ������滹�и�������
            else
            {
                memcpy(m_RecvBuffer + oldRecvBufferLen, stickBuf, requestLen - oldRecvBufferLen);
                m_RecvBufferLen = requestLen;
                m_RecvBufferOffset = requestLen;
                // ����m_RecvBuffer������stickBuf����ƫ��
                processBuf = m_RecvBuffer;
                stickBuf += (requestLen - oldRecvBufferLen);
                LOGERROR("warn: another terrible, you are so lucky too");
            }
        }

        // ճ������
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
                //�������
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

// ���ݿɱ�����ʱ�����Ļص�����IKxCommPoller����
int TCPUnit::onSend()
{
    // �������б��е�����������ͣ�ֱ��������򴥷�EAGAIN�ȷ���������
again:
    // ��m_BufferList�ĵ�һ���ڵ�ŵ�m_SendBuffer��
    if (nullptr == m_SendBuffer)
    {
        // ȡ����һ�������͵Ļ�������
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
            // ��������Ҫ����
            changePollType(POLLTYPE_IN);
			LOGDEBUG("warn: socket %d KxTCPUnit::onSend() NULL = node m_PollType %d", getCommId(), m_PollType);
            return 0;
        }
    }

    // Ӧ�÷��͵�����
    int sendLen = m_SendBufferLen - m_SendBufferOffset;
    int len = sendData(m_SendBuffer + m_SendBufferOffset, sendLen);
	LOGDEBUG("warn: KxTCPUnit::onSend() fd %d sendlen %d success", getCommId(), len);
    if (len >= sendLen)
    {
        //�����Ѿ����ͳ�ȥ���ڴ�
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
