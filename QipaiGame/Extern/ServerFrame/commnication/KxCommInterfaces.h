/*
*   ͨѶ��ؽӿ�ͷ�ļ�
*/
#ifndef __KXCOMM_INTERFACE_H__
#define __KXCOMM_INTERFACE_H__

#include "core/KxCore.h"

enum KXPOLL_TYPE
{
    KXPOLLTYPE_UNKNOWN = 0,
    KXPOLLTYPE_IN = 1,
    KXPOLLTYPE_OUT = 4,
    KXPOLLTYPE_ERR = 8
};

class IKxComm;
class IKxCommPoller;

// �߼�����ģ��ӿ�
// ���ڴ���IKxComm���յ�����Ϣ�Լ��쳣
// һ��IKxModule���Զ�Ӧ���IKxComm���������Ӧ�ܶ���ͻ��˶���
class IKxModule : public KxObject
{
public:
    // IKxComm������յ����ݺ����ڼ�������Ƿ������Ļص�
    virtual int processLength(char* buffer, unsigned int len)
    {
        return len;
    }
    // IKxComm������յ��������ݺ󣬵��õĻص�
    virtual void processLogic(char* buffer, unsigned int len, IKxComm *target) = 0;
    // IKxComm����������ʱ�����õĻص�
    virtual void processError(IKxComm *target) = 0;
    // IKxComm�����ڲ����������¼�ʱ�����Ļص�������TCP���������ӳɹ�
    virtual void processEvent(int eventId, IKxComm* target) {}
};

// ��ѯ���ӿڣ�����Poll��Select��Epoll(linux), [IOCP(windows)]��ʵ��
class IKxCommPoller : public KxObject
{
public:
    // ������ѯ�������ش������¼���
    // ��ѯʧ�ܷ���-1
    virtual int poll() = 0;

    // ע��һ��IKxCommͨѶ���󣬲�retain
    // �����ظ�ע�ᣬ�ظ�ע���൱�ڵ���modifyCommObject
    // ��ӳɹ�(�����ظ�ע��)����0��ʧ�ܷ���-1
    virtual int addCommObject(IKxComm* obj, int type) = 0;

    // �޸�IKxCommͨѶ�������������¼�
    // δע���IKxCommͨѶ������ø÷�����ʧ��
    // �޸ĳɹ�����0��ʧ�ܷ���-1
    virtual int modifyCommObject(IKxComm* obj, int type) = 0;

    // �Ƴ��Ѿ�ע���IKxCommͨѶ����
    // �Ƴ��ɹ�����0��ʧ�ܷ���-1
    virtual int removeCommObject(IKxComm* obj) = 0;

    IKxComm* getCurrentPollObject() { return m_CurrentPollObject; }

protected:
    IKxComm* m_CurrentPollObject;
};


// ͨѶ����ӿڣ�����TCP��UDP��UnixSocket�������ڴ��ʵ��
class IKxComm : virtual public KxObject
{
    friend class IKxCommPoller;

public:
    IKxComm()
        : m_PollType(KXPOLLTYPE_UNKNOWN)
		, m_Poller(NULL)
		, m_ProcessModule(NULL)
    {
    }

    virtual ~IKxComm()
    {
        KXSAFE_RELEASE(m_ProcessModule);
        KXSAFE_RELEASE(m_Poller);
    }

    // ��ʼ��
    virtual bool init() = 0;

    // ��������
    virtual int sendData(const char* buffer, unsigned int len) = 0;
    // ��������
    virtual int recvData(char* buffer, unsigned int len) = 0;
    // �رո�ͨѶ����
    virtual void close() = 0;
    // ��ȡͨѶID��ͨ����socket fd��
    virtual KXCOMMID getCommId() = 0;

    virtual void setModule(IKxModule *model)
    {
        if (model != m_ProcessModule)
        {
            KXSAFE_RELEASE(m_ProcessModule);
            m_ProcessModule = model;
            KXSAFE_RETAIN(m_ProcessModule);
        }
    }

    virtual IKxModule* getModule()
    {
        return m_ProcessModule;
    }

    // ����Poller
    virtual void setPoller(IKxCommPoller* poller)
    {
        if (poller != m_Poller)
        {
            KXSAFE_RELEASE(m_Poller);
            m_Poller = poller;
            KXSAFE_RETAIN(m_Poller);
        }
    }

    virtual IKxCommPoller* getPoller()
    {
        return m_Poller;
    }

    inline void addPollType(int type)
    {
        changePollType(m_PollType | type);
    }

    inline void delPollType(int type)
    {
        changePollType(m_PollType & (~type));
    }

    inline void changePollType(int type)
    {
        //if (m_PollType != type)
        {
            if (m_Poller != NULL && m_Poller->getCurrentPollObject() != this)
            {
                m_Poller->modifyCommObject(this, type);
            }
            else
            {
                m_PollType = type;
            }
        }
    }

    inline int getPollType()
    {
        return m_PollType;
    }

    inline void setPollType(int type)
    {
        m_PollType = type;
    }

    // ���յ�����ʱ�����Ļص�����IKxCommPoller����
    // -1 ��ʾ����
    // 0 ��ʾ��ȷ
    // ����0 ��ʾ���ݿ���δ������
    virtual int onRecv() = 0;
    // ���ݿɱ�����ʱ�����Ļص�����IKxCommPoller����
    // -1 ��ʾ����
    // 0 ��ʾ��ȷ
    // ����0 ��ʾ���ݿ���δ������
    virtual int onSend() = 0;
    // ��������ʱ�ص�����IKxCommPoller����
    virtual int onError()
    {
        if (NULL != m_ProcessModule)
        {
            m_ProcessModule->processError(this);
        }
        return 0;
    }

protected:
    int	        m_PollType;
    IKxCommPoller* m_Poller;            // Poller�������ڷ���������ʧ��ʱ���Զ����ͻ�������
    IKxModule*  m_ProcessModule;
};


#endif