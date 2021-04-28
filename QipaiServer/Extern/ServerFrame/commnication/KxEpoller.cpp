#include "KxEpoller.h"
#include "log/LogManager.h"

#define KXEPOLL_ERROR(obj, iter, pollMap) {\
	KXLOGDEBUG("error: KxEpoller::poll line %d error objid %d", __LINE__, obj->getCommId()); \
	applyChange(fd, EPOLL_CTL_DEL, obj->getPollType()); \
	obj->setPoller(NULL); \
	pollMap.erase(iter); \
    obj->release(); \
    obj->onError(); \
	obj->release(); \
	continue; \
}

using namespace std;


#if(KX_TARGET_PLATFORM == KX_PLATFORM_LINUX)

	KxEpoller::KxEpoller(int maxEventQueue)
		:m_MaxEventQueue(maxEventQueue)
	{
		m_TimeOut = 1;
		m_EpollFd = epoll_create(maxEventQueue);
		if (m_EpollFd < 0)
		{
			KX_LOGERROR("error: KxEpoller::poll epoll_create error %d", errno);
		}
	}

	KxEpoller::~KxEpoller()
	{
		for (map<KXCOMMID, IKxComm*>::iterator iter = m_PollMap.begin();
			iter != m_PollMap.end(); ++iter)
		{
			(iter->second)->release();
		}
		m_PollMap.clear();
		close(m_EpollFd);
	}

	int KxEpoller::poll()
	{
		int maxnotify = epoll_wait(m_EpollFd, m_Events, m_MaxEventQueue, m_TimeOut);
		if (maxnotify < 0)
		{
			//linux�¿���ֱ��ʹ��errno��������Ӧ�÷�װδȫ�ֺ���
			KX_LOGERROR("error: KxEpoller::poll epoll_wait error ret %d errno %d", maxnotify, errno);
            maxnotify = 0;
		}

		for (int i = 0; i < maxnotify; ++i)
		{
			KXCOMMID fd = m_Events[i].data.fd;
			map<KXCOMMID, IKxComm*>::iterator iter = m_PollMap.find(fd);
			if (iter == m_PollMap.end())
			{
				KX_LOGDEBUG("warn: KxEpoller::poll epoll_wait missing IKxComm with fd %d event %d",
					fd, m_Events[i].events);
				continue;
			}
			IKxComm* obj = iter->second;
            m_CurrentPollObject = obj;
			int event = 0;
			if (m_Events[i].events & (EPOLLHUP | EPOLLERR))
			{
				event = EPOLLIN | EPOLLOUT;
			}
			else
			{
				event = m_Events[i].events;
			}

			int oldPollType = obj->getPollType();
			obj->retain();
			if (event & EPOLLIN)
			{
				//KXLOGDEBUG("warn: KxEpoller::poll onRecv IKxComm with fd %d event %d",
				//	fd, m_Events[i].events);
                int ret = 1;
                while (ret > 0)
                {
                    ret = obj->onRecv();
                }

				if(0 > ret)
				{
					KXEPOLL_ERROR(obj, iter, m_PollMap);
				}
			}

			if (event & EPOLLOUT)
			{
				KX_LOGDEBUG("warn: KxEpoller::poll onSend IKxComm with fd %d event %d",
					fd, m_Events[i].events);
				if(0 > obj->onSend())
				{
					KXEPOLL_ERROR(obj, iter, m_PollMap);
				}
			}

			// ETģʽ���������
			if(oldPollType != obj->getPollType())
			{
				KX_LOGDEBUG("warn: KxEpoller::poll auto modify %d type %d to %d", fd, oldPollType, obj->getPollType());
				modifyCommObject(obj, obj->getPollType());
			}
            m_CurrentPollObject = NULL;
			obj->release();
		}
		
		return maxnotify;
	}

	int KxEpoller::addCommObject(IKxComm* obj, int events)
	{
		if (NULL == obj)
		{
			KX_LOGERROR("error: KxEpoller::addCommObject error NULL");
			return -1;
		}
		KXCOMMID fd = obj->getCommId();
		map<KXCOMMID, IKxComm*>::iterator iter = m_PollMap.find(fd);
		if (iter != m_PollMap.end())
		{
			if(iter->second != obj)
			{
				// ������ڹر���socket��û��removeCommObject���쳣�����ǿ��Խ��ɵ�Object�ͷ�
				KX_LOGERROR("error: KxEpoller::addCommObject has a invalid object %x new obj %x of sock fd %d",
					iter->second, obj, fd);
				iter->second->setPoller(NULL);
				iter->second->release();
				m_PollMap.erase(iter);
			}
			else
			{
				// �ظ�addCommObject���ڶ���add�Զ�����ΪModify����
				// ���applyChangeʧ�ܣ�������Ӧ�ð����Ƴ���
				KX_LOGDEBUG("warn: KxEpoller::addCommObject add sock %d duplicate", fd);
				int ret = applyChange(fd, EPOLL_CTL_MOD, events);
				if (ret == -1)
				{
					iter->second->setPoller(NULL);
					iter->second->release();
					m_PollMap.erase(iter);
				}
				return ret;
			}
		}

		int ret = applyChange(fd, EPOLL_CTL_ADD, events);
		if (ret == 0)
		{
			// ֻ����ӳɹ����ܲ��뵽PollMap��
			m_PollMap[fd] = obj;
			obj->retain();
			obj->setPollType(events);
			obj->setPoller(this);
			KX_LOGDEBUG("warn: KxEpoller::addCommObject object %x fd %d events %d success", obj, fd, events);
		}

		return ret;
	}

	int KxEpoller::modifyCommObject(IKxComm* obj, int events)
	{
		if (NULL == obj)
		{
			KX_LOGERROR("error: KxEpoller::modifyCommObject error NULL");
			return -1;
		}
		KXCOMMID fd = obj->getCommId();
		map<KXCOMMID, IKxComm*>::iterator iter = m_PollMap.find(fd);
		if (iter == m_PollMap.end())
		{
			KX_LOGERROR("error: KxEpoller::modifyCommObject object %x fd %d faile, isn't in pollMap",
				obj, fd);
			return -1;
		}
		else if (iter->second != obj)
		{
			KX_LOGERROR("error: KxEpoller::modifyCommObject object %x fd %d faile, invalid object %x",
				obj, fd, iter->second);
			return -1;
		}

		int ret = applyChange(fd, EPOLL_CTL_MOD, events);
		if (ret == -1)
		{
			iter->second->setPoller(NULL);
			iter->second->release();
			m_PollMap.erase(iter);
		}

		return ret;
	}

	int KxEpoller::removeCommObject(IKxComm* obj)
	{
		if (NULL == obj)
		{
			KX_LOGERROR("error: KxEpoller::removeCommObject error NULL");
			return -1;
		}
		KXCOMMID fd = obj->getCommId();
		// �ȴ�epoll���Ƴ�
		int ret = applyChange(fd, EPOLL_CTL_DEL, obj->getPollType());
		obj->setPoller(NULL);

		map<KXCOMMID, IKxComm*>::iterator iter = m_PollMap.find(fd);
		if (iter != m_PollMap.end())
		{
			if (iter->second != obj)
			{
				// ��һ��object�ڱ��ر�֮��û�д�poller���Ƴ�����fd������֮��û����ӵ�poller�У�ȴ�������Ƴ�
				// ��������£���objectҲ�ǲ��ᱻ������Ӧ�ñ��Ƴ�������ֻ���һ�м���־��¼
				KX_LOGERROR("error: KxEpoller::removeCommObject object %x fd %d faile, invalid object %x",
					obj, fd, iter->second);
			}
			iter->second->release();
			m_PollMap.erase(iter);
		}
		else
		{
			KX_LOGERROR("error: KxEpoller::removeCommObject object %x fd %d faile, isn't in pollMap",
				obj, fd);
			return -1;
		}
		return ret;
	}

	int KxEpoller::applyChange(KXCOMMID fd, int opt, int events)
	{
		struct epoll_event ev;
		ev.data.fd = fd;
		ev.events  = events | EPOLLET;
		int ret = epoll_ctl(m_EpollFd, opt, fd, &ev);
		if (ret == -1)
		{
			if (opt == EPOLL_CTL_ADD && errno == EEXIST)
			{
				opt = EPOLL_CTL_MOD;
			}
			else if (opt == EPOLL_CTL_MOD && errno == ENOENT)
			{
				opt = EPOLL_CTL_ADD;
			}
			else
			{
				KX_LOGERROR("error: KxEpoller::applyChange fd %d opt %d events %d faile, errno %d",
					fd, opt, events, errno);
				return ret;
			}
			ret = epoll_ctl(m_EpollFd, opt, fd, &ev);
			if (ret == -1)
			{
				KX_LOGERROR("error: KxEpoller::applyChange try again fd %d opt %d events %d faile, errno %d",
					fd, opt, events, errno);
				return ret;
			}
		}
		KX_LOGDEBUG("warn: KxEpoller::applyChange fd %d opt %d evetns %d, success", fd, opt, events);
		return ret;
	}

	IKxComm* KxEpoller::getComm(KXCOMMID cid)
	{
		map<KXCOMMID, IKxComm*>::iterator iter = m_PollMap.find(cid);
		if (iter == m_PollMap.end())
		{
			return NULL;
		}

		return iter->second;
	}

#endif
