/*
*   ͨѶ��ؽӿ�ͷ�ļ�
*/
#ifndef __COMM_INTERFACE_H__
#define __COMM_INTERFACE_H__

#include "Core.h"


enum POLL_TYPE
{
    POLLTYPE_UNKNOWN = 0,
    POLLTYPE_IN = 1,
    POLLTYPE_OUT = 4,
    POLLTYPE_ERR = 8
};

class IComm;
class ICommPoller;

// �߼�����ģ��ӿ�
// ���ڴ���IKxComm���յ�����Ϣ�Լ��쳣
// һ��IKxModule���Զ�Ӧ���IKxComm���������Ӧ�ܶ���ͻ��˶���
class IModule : public Object
{
public:
    // IKxComm������յ����ݺ����ڼ�������Ƿ������Ļص�
    virtual int processLength(char* buffer, unsigned int len)
    {
        return len;
    }
    // IKxComm������յ��������ݺ󣬵��õĻص�
    virtual void processLogic(char* buffer, unsigned int len, IComm *target) = 0;
    // IKxComm����������ʱ�����õĻص�
    virtual void processError(IComm *target) = 0;
    // IKxComm�����ڲ����������¼�ʱ�����Ļص�������TCP���������ӳɹ�
    virtual void processEvent(int eventId, IComm* target) {}
};

// ��ѯ���ӿڣ�����Poll��Select��Epoll(linux), [IOCP(windows)]��ʵ��
class ICommPoller : public Object
{
public:
    // ������ѯ�������ش������¼���
    // ��ѯʧ�ܷ���-1
    virtual int poll() = 0;

    // ע��һ��IKxCommͨѶ���󣬲�retain
    // �����ظ�ע�ᣬ�ظ�ע���൱�ڵ���modifyCommObject
    // ��ӳɹ�(�����ظ�ע��)����0��ʧ�ܷ���-1
    virtual int addCommObject(IComm* obj, int type) = 0;

    // �޸�IKxCommͨѶ�������������¼�
    // δע���IKxCommͨѶ������ø÷�����ʧ��
    // �޸ĳɹ�����0��ʧ�ܷ���-1
    virtual int modifyCommObject(IComm* obj, int type) = 0;

    // �Ƴ��Ѿ�ע���IKxCommͨѶ����
    // �Ƴ��ɹ�����0��ʧ�ܷ���-1
    virtual int removeCommObject(IComm* obj) = 0;

    IComm* getCurrentPollObject() { return m_CurrentPollObject; }

protected:
    IComm* m_CurrentPollObject;
};


// ͨѶ����ӿڣ�����TCP��UDP��UnixSocket�������ڴ��ʵ��
class IComm : virtual public Object
{
    friend class ICommPoller;
public:
    IComm()
        : m_PollType(POLLTYPE_UNKNOWN)
		, m_Poller(nullptr)
		, m_ProcessModule(nullptr)
    {
    }

    virtual ~IComm()
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
    virtual COMMID getCommId() = 0;

    virtual void setModule(IModule *model)
    {
        if (model != m_ProcessModule)
        {
            KXSAFE_RELEASE(m_ProcessModule);
            m_ProcessModule = model;
            KXSAFE_RETAIN(m_ProcessModule);
        }
    }

    virtual IModule* getModule()
    {
        return m_ProcessModule;
    }

    // ����Poller
    virtual void setPoller(ICommPoller* poller)
    {
        if (poller != m_Poller)
        {
            KXSAFE_RELEASE(m_Poller);
            m_Poller = poller;
            KXSAFE_RETAIN(m_Poller);
        }
    }

    virtual ICommPoller* getPoller()
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
            if (m_Poller != nullptr && m_Poller->getCurrentPollObject() != this)
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
        if (nullptr != m_ProcessModule)
        {
            m_ProcessModule->processError(this);
        }
        return 0;
    }

protected:
    int	        m_PollType;
    ICommPoller* m_Poller;            // Poller�������ڷ���������ʧ��ʱ���Զ����ͻ�������
    IModule*  m_ProcessModule;
};


#endif