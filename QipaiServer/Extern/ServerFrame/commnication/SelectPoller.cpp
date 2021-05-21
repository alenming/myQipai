#include "SelectPoller.h"

using namespace std;

#define KXSELECT_ERROR(obj, iter, pollset) {\
    obj->onError();\
    removeCommObject(obj); \
    pollset.erase(iter++);\
}

SelectPoller::SelectPoller()
: m_MaxCount(0)
, m_IsBlock(false)
{
	FD_ZERO(&m_InSet);
	FD_ZERO(&m_OutSet);
    FD_ZERO(&m_ExceptSet);
    m_TimeOut.tv_sec = 0;
    m_TimeOut.tv_usec = 10;
    m_MaxCount = 0;
}

SelectPoller::~SelectPoller()
{
    for (map<KXCOMMID, IComm*>::iterator iter = m_PollMap.begin();
        iter != m_PollMap.end(); ++iter)
    {
        iter->second->release();
    }
}

int SelectPoller::poll()
{
    fd_set inset = m_InSet;
    fd_set outset = m_OutSet;
    fd_set expset = m_ExceptSet;
    int ret = select(m_MaxCount, &inset, &outset, &expset, m_IsBlock ? nullptr : reinterpret_cast<timeval*>(&m_TimeOut));
    m_CurrentPollObject = nullptr;
    if (ret > 0)
    {
        int eventCounts = ret;
        map<KXCOMMID, IComm*>::iterator iter = m_PollMap.begin();
        while (eventCounts > 0 && iter != m_PollMap.end())
        {
            IComm* obj = iter->second;
            m_CurrentPollObject = obj;
            KXCOMMID cid = obj->getCommId();
            int pollType = obj->getPollType();
            do
            {
                bool isError = false;
                if (FD_ISSET(cid, &inset))
                {
                    --eventCounts;
                    if (!isError && 0 > obj->onRecv())
                    {
                        isError = true;
						//KXSELECT_ERROR(obj, iter, m_PollMap);
						obj->onError();
						removeCommObject(obj);
						m_PollMap.erase(iter++);
                    }
                }

                if (FD_ISSET(cid, &outset))
                {
                    --eventCounts;
                    if (!isError && 0 > obj->onSend())
                    {
                        isError = true;
						//KXSELECT_ERROR(obj, iter, m_PollMap);
						obj->onError();
						removeCommObject(obj);
						m_PollMap.erase(iter++);
                    }
                }

                if (FD_ISSET(cid, &expset))
                {
                    --eventCounts;
					if (!isError)
					{
						isError = true;
						//KXSELECT_ERROR(obj, iter, m_PollMap);
						obj->onError();
						removeCommObject(obj);
						m_PollMap.erase(iter++);
					}
                }

                if (!isError && pollType != obj->getPollType())
                {
                    modifyCommObject(obj, obj->getPollType());
                }
                
                isError ? iter : ++iter;
            } while (false);
        }
    }
    else if(0 < ret)
    {
        // error
    }
    m_CurrentPollObject = nullptr;
    clear();
    return ret;
}

int SelectPoller::addCommObject(IComm* obj, int type)
{
    if (nullptr == obj || m_PollMap.find(obj->getCommId()) != m_PollMap.end())
    {
        return -1;
    }

    KXCOMMID cid = obj->getCommId();
    obj->retain();
    m_PollMap[cid] = obj;
    obj->setPollType(type);
    obj->setPoller(this);
#if(KX_TARGET_PLATFORM != KX_PLATFORM_WIN32)
    if(m_MaxCount <= cid)
    {
        m_MaxCount = cid + 1;
    }
#endif

    if (type & KXPOLLTYPE_IN)
    {
        FD_SET(cid, &m_InSet);
    }
    if (type &  KXPOLLTYPE_OUT)
    {
        FD_SET(cid, &m_OutSet);
    }
    if (type & KXPOLLTYPE_ERR)
    {
        FD_SET(cid, &m_ExceptSet);
    }

    return 0;
}

int SelectPoller::modifyCommObject(IComm* obj, int type)
{
    if (obj != nullptr
        && m_PollMap.find(obj->getCommId()) == m_PollMap.end())
    {
        return -1;
    }

    KXCOMMID cid = obj->getCommId();
    if (type & KXPOLLTYPE_IN)
    {
        FD_SET(cid, &m_InSet);
    }
    else
    {
        FD_CLR(cid, &m_InSet);
    }

    if (type & KXPOLLTYPE_OUT)
    {
        FD_SET(cid, &m_OutSet);
    }
    else
    {
        FD_CLR(cid, &m_OutSet);
    }

    if (type & KXPOLLTYPE_ERR)
    {
        FD_SET(cid, &m_ExceptSet);
    }
    else
    {
        FD_CLR(cid, &m_ExceptSet);
    }

    obj->setPollType(type);
    return 0;
}

int SelectPoller::removeCommObject(IComm* obj)
{
    if (nullptr != obj
        && m_PollMap.find(obj->getCommId()) != m_PollMap.end())
    {
        obj->setPoller(nullptr);
        m_RemoveSet.insert(obj);
        return 0;
    }
    return -1;
}

IComm* SelectPoller::getComm(KXCOMMID cid)
{
    map<KXCOMMID, IComm*>::iterator iter = m_PollMap.find(cid);
    if (iter == m_PollMap.end())
    {
        return nullptr;
    }
    else
    {
        return iter->second;
    }
}

void SelectPoller::clear()
{
    for (set<IComm*>::iterator iter = m_RemoveSet.begin();
        iter != m_RemoveSet.end(); ++iter)
    {
        IComm* obj = *iter;
        KXCOMMID cid = obj->getCommId();
        FD_CLR(cid, &m_InSet);
        FD_CLR(cid, &m_OutSet);
        FD_CLR(cid, &m_ExceptSet);
        obj->setPoller(nullptr);
        obj->release();
        m_PollMap.erase(cid);
    }

    m_RemoveSet.clear();
}

