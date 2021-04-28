/*
 * ʵ��Linux�µ�Epoll
 *
 */
#ifndef __EPOLLER_H__
#define __EPOLLER_H__

#include <map>
#include "core/KxCore.h"
#include "commnication/KxCommInterfaces.h"

    
#if(KX_TARGET_PLATFORM == KX_PLATFORM_LINUX)

#include <sys/epoll.h>
#define MAX_EPOLL_EVENTS_PER_RUN 4096

class KxEpoller : public IKxCommPoller
{
public:
	KxEpoller(int maxEventQueue = 32000);
	virtual ~KxEpoller();

    // ������ѯ�������ش������¼���
    // ��ѯʧ�ܷ���-1
    virtual int poll();

    // ע��һ��IKxCommͨѶ���󣬲�retain
    // �����ظ�ע�ᣬ�ظ�ע���൱�ڵ���modifyCommObject
    // ��ӳɹ�(�����ظ����)����0��ʧ�ܷ���-1
    virtual int addCommObject(IKxComm* obj, int type);

    // �޸�IKxCommͨѶ�������������¼�
    // δע���IKxCommͨѶ������ø÷�����ʧ��
    // �޸ĳɹ�����0��ʧ�ܷ���-1
    virtual int modifyCommObject(IKxComm* obj, int type);

    // �Ƴ��Ѿ�ע���IKxCommͨѶ����
    // �Ƴ��ɹ�����0��ʧ�ܷ���-1
    virtual int removeCommObject(IKxComm* obj);

    IKxComm* getComm(KXCOMMID cid);

    inline void setTimeout(int timeout) { m_TimeOut = timeout; }

private:
    int applyChange(KXCOMMID fd, int opt, int events);

private:
	int m_EpollFd;
	int m_MaxEventQueue;
	int m_TimeOut;
	epoll_event m_Events[MAX_EPOLL_EVENTS_PER_RUN];
    std::map<KXCOMMID, IKxComm*> m_PollMap;
};

#endif

#endif
