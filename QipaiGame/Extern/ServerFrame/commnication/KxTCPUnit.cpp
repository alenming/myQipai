#include "KxTCPUnit.h"
#include "core/KxMemPool.h"
#include "log/LogManager.h"

#define BUFF_SIZE 4096
#define MAX_TCP_PKG_LEN 1<<16


char* KxTCPUnit::s_RecvBuffer = NULL;

KxTCPUnit::KxTCPUnit()
: m_Socket(NULL)
, m_SendBuffer(NULL)
, m_RecvBuffer(NULL)
, m_SendBufferLen(0)
, m_RecvBufferLen(0)
, m_SendBufferOffset(0)
, m_RecvBufferOffset(0)
{
    // ����ȫ�ֵĽ��ջ�����
    if (NULL == s_RecvBuffer)
    {
        s_RecvBuffer = static_cast<char*>(kxMemMgrAlocate(BUFF_SIZE));
    }
}

KxTCPUnit::~KxTCPUnit()
{
    KXSAFE_RELEASE(m_Socket);
    KXSAFE_RELEASE(m_Poller);
    KXSAFE_RECYCLE(m_SendBuffer, m_SendBufferLen);
    KXSAFE_RECYCLE(m_RecvBuffer, m_RecvBufferLen);

	// ������б��л��������
    kxBufferNode* node = m_BufferList.head();
    while (NULL != node)
    {
        kxMemMgrRecycle(node->buffer, node->len);
        node = node->next;
    }
	// ����б�
	m_BufferList.clear();
}

bool KxTCPUnit::init()
{
	return init(KXV_IPV4);
}

bool KxTCPUnit::init(KXSOCK_VERSION ipv)
{
	m_PollType = KXPOLLTYPE_IN;
	m_Socket = new KxSock();
	return m_Socket->init(KXSOCK_TCP, ipv);
}

// ��������
int KxTCPUnit::sendData(const char* buffer, unsigned int len)
{
    int ret = 0;
    // 1. ���������б�Ϊ��ʱ˵��ǰ��û�����ݴ����ͣ����Է���
    // 2. ����Ҫ���͵�buffer����m_SendBuffer + m_SendBufferOffsetʱ˵����onSend���Զ����ͣ����Է���
    if (m_BufferList.isEmpty()
        || (m_SendBuffer != NULL && buffer == (m_SendBuffer + m_SendBufferOffset)))
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
                KX_LOGERROR("error: KxTCPUnit::sendData %d send datalen %d faile, errno %d", getCommId(), len, errorNo);
                // �����onSend���õģ���ִ��onError
                if (m_Poller != NULL && m_Poller->getCurrentPollObject() != this)
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
            KX_LOGDEBUG("warn: socket %d send %d data ret %d, cache unsend data!!!",
                getCommId(), len + ret, ret);
            char* buf = static_cast<char*>(kxMemMgrAlocate(len));
            memcpy(buf, buffer + ret, len);
            m_BufferList.pushBack(buf, len);
			KX_LOGDEBUG("warn: socket %d changePollType m_PollType %d, cache unsend data!!!", getCommId(), m_PollType);
            addPollType(KXPOLLTYPE_OUT);
        }
        // �����m_SendBuffer�е����ݣ�˵������onSend�ص��з��͵�
        // onSend�������ٴη���ʧ�ܻ�û��������߼�
    }

    return ret;
}

// ��������
int KxTCPUnit::recvData(char* buffer, unsigned int len)
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
            if (m_Poller != NULL && m_Poller->getCurrentPollObject() != this)
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
int KxTCPUnit::onRecv()
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
		KX_LOGERROR("warn: ret <= 0 %d bit data in socket %d error package length %d", ret, getCommId(), requestLen);
        return -1;
    }
    else if (NULL != m_ProcessModule && ret > 0)
    {
		//KxLogger::getInstance()->HexDumpImp(s_RecvBuffer, ret);
        char* processBuf = s_RecvBuffer;
        char* stickBuf = NULL;

        // ����а������ƴ�ӵ�����ĺ��棬ע��newsize
        if (NULL != m_RecvBuffer)
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
            KX_LOGERROR("warn: recv %d bit data in socket %d error package length %d", ret, getCommId(), requestLen);
            return -1;
        }

        // ��������Դ�����û�а������Ϊ�������ռ�
        if (ret < requestLen && NULL == m_RecvBuffer)
        {
            m_RecvBuffer = static_cast<char*>(kxMemMgrAlocate(requestLen));
            m_RecvBufferLen = requestLen;
            m_RecvBufferOffset = ret;
            memcpy(m_RecvBuffer, processBuf, ret);
            return state;
        }
        // ����һ�ָ��ӵ����...����������
        // ������Ҫ��֤ԭ�ȵİ������ƴ�ӣ�Ȼ���ٴ���
        else if (NULL != stickBuf)
        {
            // ����һ���µ��ڴ����������İ��
            char* oldRecvBuffer = m_RecvBuffer;
            unsigned int oldRecvBufferLen = m_RecvBufferLen;
            m_RecvBuffer = static_cast<char*>(kxMemMgrAlocate(requestLen));

            // �Ƚ�ԭ�Ȱ�����ݸ��Ƶ����ڴ���
            memcpy(m_RecvBuffer, oldRecvBuffer, oldRecvBufferLen);
            kxMemMgrRecycle(oldRecvBuffer, oldRecvBufferLen);

            // ���������İ������׷��
            // ���������ǲ��ܴ��������һ������İ�������һ��ƴ�Ӻ󷵻�
            if (ret < requestLen)
            {
                memcpy(m_RecvBuffer + oldRecvBufferLen, stickBuf, ret - oldRecvBufferLen);
                m_RecvBufferLen = requestLen;
                m_RecvBufferOffset = ret;
                KX_LOGERROR("warn: something terrible, you are so lucky");
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
                KX_LOGERROR("warn: another terrible, you are so lucky too");
            }
        }

        // ճ������
        while (ret >= requestLen)
        {
            m_ProcessModule->processLogic(processBuf, requestLen, this);
            //KXLOGDEBUG("process stick package, ret %d requestLen %d processBuf %x\n", ret, requestLen, processBuf);
            processBuf += requestLen;

            if (NULL != m_RecvBuffer)
            {
                processBuf = stickBuf;
                kxMemMgrRecycle(m_RecvBuffer, m_RecvBufferLen);
                m_RecvBuffer = NULL;
                m_RecvBufferOffset = m_RecvBufferLen = 0;
            }

            ret -= requestLen;
            if (ret > 0 && NULL != processBuf)
            {
                requestLen = m_ProcessModule->processLength(processBuf, ret);
                if (requestLen <= 0 || requestLen > MAX_TCP_PKG_LEN)
                {
                    KX_LOGERROR("warn: recv %d bit data in socket %d error package length %d", ret, getCommId(), requestLen);
					//KxLogger::getInstance()->HexDumpImp(processBuf, requestLen);
                    return -1;
                }
                //�������
                else if (ret < requestLen)
                {
                    m_RecvBuffer = static_cast<char*>(kxMemMgrAlocate(requestLen));
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
int KxTCPUnit::onSend()
{
    // �������б��е�����������ͣ�ֱ��������򴥷�EAGAIN�ȷ���������
again:
    // ��m_BufferList�ĵ�һ���ڵ�ŵ�m_SendBuffer��
    if (NULL == m_SendBuffer)
    {
        // ȡ����һ�������͵Ļ�������
        kxBufferNode* node = m_BufferList.next();
        if (NULL != node)
        {
            m_SendBuffer = node->buffer;
            m_SendBufferLen = node->len;
            m_SendBufferOffset = 0;
            delete node;
			KX_LOGDEBUG("warn: KxTCPUnit::onSend() NULL != node fd %d", getCommId());
        }
        else
        {
            // ��������Ҫ����
            changePollType(KXPOLLTYPE_IN);
			KX_LOGDEBUG("warn: socket %d KxTCPUnit::onSend() NULL = node m_PollType %d", getCommId(), m_PollType);
            return 0;
        }
    }

    // Ӧ�÷��͵�����
    int sendLen = m_SendBufferLen - m_SendBufferOffset;
    int len = sendData(m_SendBuffer + m_SendBufferOffset, sendLen);
	KX_LOGDEBUG("warn: KxTCPUnit::onSend() fd %d sendlen %d success", getCommId(), len);
    if (len >= sendLen)
    {
        //�����Ѿ����ͳ�ȥ���ڴ�
        kxMemMgrRecycle(m_SendBuffer, m_SendBufferLen);
        kxBufferNode* node = m_BufferList.head();
        m_SendBuffer = NULL;
        m_SendBufferLen = m_SendBufferOffset = 0;

        if (NULL == node)
        {
            changePollType(KXPOLLTYPE_IN);
        }
        else
        {
            goto again;
        }
    }
    else if (len < sendLen && len >= 0)
    {
        m_SendBufferOffset += len;
        addPollType(KXPOLLTYPE_OUT);
		KX_LOGDEBUG("warn: socket %d KxTCPUnit::onSend() m_PollType %d", getCommId(), m_PollType);    
    }

    return len;
}
