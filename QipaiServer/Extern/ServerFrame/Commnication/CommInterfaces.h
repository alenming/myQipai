/*
*   通讯相关接口头文件
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

// 逻辑处理模块接口
// 用于处理IKxComm接收到的消息以及异常
// 一个IKxModule可以对应多个IKxComm对象，例如对应很多个客户端对象
class IModule : public _Object
{
public:
    // IKxComm对象接收到数据后，用于检测数据是否完整的回调
    virtual int processLength(char* buffer, unsigned int len)
    {
        return len;
    }
    // IKxComm对象接收到完整数据后，调用的回调
    virtual void processLogic(char* buffer, unsigned int len, IComm *target) = 0;
    // IKxComm对象发生错误时，调用的回调
    virtual void processError(IComm *target) = 0;
    // IKxComm对象内部发生特殊事件时触发的回调——如TCP非阻塞连接成功
    virtual void processEvent(int eventId, IComm* target) {}
};

// 轮询器接口，会有Poll，Select，Epoll(linux), [IOCP(windows)]等实现
class ICommPoller : public _Object
{
public:
    // 进行轮询，并返回触发的事件数
    // 轮询失败返回-1
    virtual int poll() = 0;

    // 注册一个IKxComm通讯对象，并retain
    // 允许重复注册，重复注册相当于调用modifyCommObject
    // 添加成功(包括重复注册)返回0，失败返回-1
    virtual int addCommObject(IComm* obj, int type) = 0;

    // 修改IKxComm通讯对象所监听的事件
    // 未注册的IKxComm通讯对象调用该方法会失败
    // 修改成功返回0，失败返回-1
    virtual int modifyCommObject(IComm* obj, int type) = 0;

    // 移除已经注册的IKxComm通讯对象
    // 移除成功返回0，失败返回-1
    virtual int removeCommObject(IComm* obj) = 0;

    IComm* getCurrentPollObject() { return m_CurrentPollObject; }

protected:
    IComm* m_CurrentPollObject;
};


// 通讯对象接口，会有TCP，UDP，UnixSocket，共享内存等实现
class IComm : virtual public _Object
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

    // 初始化
	virtual bool init() = 0;
    // 发送数据
    virtual int sendData(const char* buffer, unsigned int len) = 0;
    // 接收数据
    virtual int recvData(char* buffer, unsigned int len) = 0;
    // 关闭该通讯对象
    virtual void close() = 0;
    // 获取通讯ID（通常是socket fd）
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

    // 设置Poller
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

    // 接收到数据时触发的回调，由IKxCommPoller调用
    // -1 表示错误
    // 0 表示正确
    // 大于0 表示数据可能未接收完
    virtual int onRecv() = 0;
    // 数据可被发送时触发的回调，由IKxCommPoller调用
    // -1 表示错误
    // 0 表示正确
    // 大于0 表示数据可能未发送完
    virtual int onSend() = 0;
    // 发生错误时回调，由IKxCommPoller调用
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
    ICommPoller* m_Poller;            // Poller对象，用于非阻塞发送失败时，自动发送缓存数据
    IModule*  m_ProcessModule;
};


#endif