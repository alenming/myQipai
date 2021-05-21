/*
 * ʵ��Linux�µ�Epoll
 *
 */
#ifndef __EPOLLER_H__
#define __EPOLLER_H__

#include <map>
#include "Core.h"
#include "commnication/CommInterfaces.h"

    
#if(KX_TARGET_PLATFORM == KX_PLATFORM_LINUX)

#include <sys/epoll.h>
#define MAX_EPOLL_EVENTS_PER_RUN 4096

class Epoller : public ICommPoller
{
public:
	Epoller(int maxEventQueue = 32000);
	virtual ~Epoller();

    // ������ѯ�������ش������¼���
    // ��ѯʧ�ܷ���-1
    virtual int poll();

    // ע��һ��IKxCommͨѶ���󣬲�retain
    // �����ظ�ע�ᣬ�ظ�ע���൱�ڵ���modifyCommObject
    // ���ӳɹ�(�����ظ����)����0��ʧ�ܷ���-1
    virtual int addCommObject(IComm* obj, int type);

    // �޸�IKxCommͨѶ�������������¼�
    // δע���IKxCommͨѶ������ø÷�����ʧ��
    // �޸ĳɹ�����0��ʧ�ܷ���-1
    virtual int modifyCommObject(IComm* obj, int type);

    // �Ƴ��Ѿ�ע���IKxCommͨѶ����
    // �Ƴ��ɹ�����0��ʧ�ܷ���-1
    virtual int removeCommObject(IComm* obj);

    IComm* getComm(KXCOMMID cid);

    inline void setTimeout(int timeout) { m_TimeOut = timeout; }

private:
    int applyChange(KXCOMMID fd, int opt, int events);

private:
	int m_EpollFd;
	int m_MaxEventQueue;
	int m_TimeOut;
	epoll_event m_Events[MAX_EPOLL_EVENTS_PER_RUN];
    std::map<KXCOMMID, IComm*> m_PollMap;
};

#endif

#endif