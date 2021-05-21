#include "TimerManager.h"
#include "log/LogManager.h"

#if(KX_TARGET_PLATFORM == KX_PLATFORM_WIN32)

int gettimeofday(struct timeval * val, struct timezone * zone)
{
    if (nullptr != val)
    {
        LARGE_INTEGER liTime, liFreq;
        QueryPerformanceFrequency(&liFreq);
        QueryPerformanceCounter(&liTime);
        val->tv_sec = (long)(liTime.QuadPart / liFreq.QuadPart);
        val->tv_usec = (long)(liTime.QuadPart * 1000000.0 / liFreq.QuadPart - val->tv_sec * 1000000.0);
    }
    return 0;
}

#endif

using namespace std;

TimerObject::TimerObject()
:m_TimerList(nullptr),
m_Prev(nullptr),
m_Next(nullptr),
m_Delay(0.0f),
m_Repeat(0),
m_TimeVal()
{
}

TimerObject::TimerObject(float delay, int repeat)
:m_TimerList(nullptr),
m_Prev(nullptr),
m_Next(nullptr),
m_Delay(delay),
m_Repeat(repeat),
m_TimeVal()
{
}

TimerObject::~TimerObject()
{
}

void TimerObject::onTimer(const TimeVal& now)
{
}

void TimerObject::stop()
{
    if (nullptr != m_TimerList)
    {
        m_Repeat = 0;
        m_TimerList->remove(this);
    }
}

void TimerObject::start(const TimeVal& now)
{
    m_TimeVal = now + m_Delay;
}

void TimerObject::startWithTime(const TimeVal& now)
{
    m_TimeVal = now;
}

TimerList::TimerList()
    :m_Length(0),
    m_Head(nullptr),
    m_Tail(nullptr),
    m_Timer(nullptr)
{

}

TimerList::~TimerList()
{

}

bool TimerList::pushBack(TimerObject* obj)
{
    if (nullptr == obj
        || nullptr != obj->m_TimerList)
    {
        return false;
    }

    if (nullptr == m_Tail)
    {
        m_Head = obj;
        obj->m_Prev = obj->m_Next = nullptr;

    }
    else
    {
        m_Tail->m_Next = obj;
        obj->m_Prev = m_Tail;
        obj->m_Next = nullptr;
    }
    
    m_Tail = obj;
    obj->m_TimerList = this;
    KXSAFE_RETAIN(obj);
    ++m_Length;
    return true;
}

bool TimerList::pushFront(TimerObject* obj)
{
    if (nullptr == obj
        || nullptr != obj->m_TimerList)
    {
        return false;
    }

    if (nullptr == m_Head)
    {
        m_Tail = obj;
        obj->m_Prev = obj->m_Next = nullptr;
    }
    else
    {
        m_Head->m_Prev = obj;
        obj->m_Next = m_Head;
        obj->m_Prev = nullptr;
    }

    m_Head = obj;
    obj->m_TimerList = this;
    KXSAFE_RETAIN(obj);
    ++m_Length;
    return true;
}

bool TimerList::remove(TimerObject* obj)
{
    if (nullptr == obj
        || obj->m_TimerList != this)
    {
        return false;
    }

    if (m_Head == obj)
    {
        m_Head = obj->m_Next;
        if (nullptr != m_Head)
        {
            m_Head->m_Prev = nullptr;
        }
        else
        {
            m_Tail = nullptr;
        }
    }
    else if (m_Tail == obj)
    {
        m_Tail = obj->m_Prev;
        m_Tail->m_Next = nullptr;
    }
    else
    {
        obj->m_Prev->m_Next = obj->m_Next;
        obj->m_Next->m_Prev = obj->m_Prev;
    }

    obj->m_Next = obj->m_Prev = nullptr;
    obj->m_TimerList = nullptr;
    KXSAFE_RELEASE(obj);
    --m_Length;
    return true;
}

bool TimerList::insert(TimerObject* obj)
{
    if (nullptr == obj
        || nullptr != obj->m_TimerList)
    {
        return false;
    }

    // 从头部开始搜索
    TimerObject* pNode = m_Head;
    const TimeVal& timeOut = obj->getTimeVal();
    if (nullptr == pNode || !pNode->checkTime(timeOut))
    {
        pushFront(obj);
    }
    else
    {
        while(nullptr != pNode)
        {
            // 如果该节点的超时时间大于要插入节点的超时时间
            if (!pNode->checkTime(timeOut))
            {
                obj->m_Next = pNode;
                obj->m_Prev = pNode->m_Prev;
                obj->m_Prev->m_Next = obj;
                pNode->m_Prev = obj;
                obj->m_TimerList = this;
                KXSAFE_RETAIN(obj);
                ++m_Length;
                break;
            }
            // 一直遍历到队尾 
            else if (m_Tail == pNode)
            {
                pushBack(obj);
                break;
            }
            pNode = pNode->m_Next;
        }
    }
    return true;
}

void TimerList::update(const TimeVal& now)
{
    while(nullptr != m_Head)
    {
        if (m_Head->checkTime(now))
        {
            m_Timer = m_Head;
            m_Timer->retain();
            //KXLOGDEBUG("update time tick on %d %d", now.tv_sec, now.tv_usec);
            m_Timer->onTimer(now);
            
            // 如果在onTimer中，对m_Timer执行了stop操作，remove会失败（不stop的话加入其它列表的操作会失败）
            // 而对于AglieList中的Timer，Repeat必定为0
            // 只要是在本超时队列中的Timer，根据Repeat决定重复执行的次数
            if (remove(m_Timer) && m_Timer->getRepeat() != 0)
            {
                if (m_Timer->getRepeat() > 0)
                {
                    m_Timer->setRepeat(m_Timer->getRepeat() - 1);
                }
                m_Timer->start(now);
                pushBack(m_Timer);
            }
            KXSAFE_RELEASE(m_Timer);
        }
        else
        {
            break;
        }
    }
}

TimerManager::TimerManager()
{
    gettimeofday((struct timeval*)&(m_Now), nullptr);
#if KX_TARGET_PLATFORM == KX_PLATFORM_WIN32
    m_Timestamp = static_cast<int>(time(nullptr));
#endif
    m_AglieTimerList = new TimerList();
}

TimerManager::~TimerManager()
{
    for (map<long, TimerList*>::iterator iter = m_FixTimerMap.begin();
        iter != m_FixTimerMap.end(); ++iter)
    {
        KXSAFE_RELEASE(iter->second);
    }
    KXSAFE_RELEASE(m_AglieTimerList);
}

void TimerManager::updateTimers()
{
               	gettimeofday((struct timeval*)&(m_Now), 0);
#if KX_TARGET_PLATFORM == KX_PLATFORM_WIN32
    m_Timestamp = static_cast<int>(time(nullptr));
#endif

	for (map<long, TimerList*>::iterator iter = m_FixTimerMap.begin(); iter != m_FixTimerMap.end(); )
	{
        iter->second->update(m_Now);
        if (0 == iter->second->length())
		{
            KXSAFE_RELEASE(iter->second);
			m_FixTimerMap.erase(iter++);
		}
		else
		{
			++iter;	
		}
	}

    m_AglieTimerList->update(m_Now);
}

bool TimerManager::addTimer(TimerObject* obj)
{
    KXASSERT(nullptr != obj);

    /*long timeKey = timeToKey(obj->getDelay());
    KXLOGDEBUG("timer key %d", timeKey);
    if (obj->getRepeat() == 0 
        && m_FixTimerMap.find(timeKey) == m_FixTimerMap.end())
    {
        return attachToAglieList(obj);
    }*/

    return attachToFixList(obj);
}

bool TimerManager::addTimer(TimerObject* obj, float delay, int repeat)
{
    KXASSERT(nullptr != obj);

    obj->setDelay(delay);
    obj->setRepeat(repeat);
    return addTimer(obj);
}

bool TimerManager::addTimer(TimerObject* obj, int delay, int repeat)
{
	KXASSERT(nullptr != obj);
	TimeVal tv;
	tv.tv_sec = delay;
	tv.tv_usec = 0;
	obj->setDelay(tv);
	obj->setRepeat(repeat);
	return addTimer(obj);
}

bool TimerManager::addTimerOnTime(TimerObject* obj, long timestamp)
{
    KXASSERT(nullptr != obj);
    obj->setRepeat(0);
#if KX_TARGET_PLATFORM == KX_PLATFORM_WIN32
    TimeVal tv;
    tv.tv_sec = timestamp - m_Timestamp;
    tv.tv_usec = 0;
	TimeVal timeOut = m_Now + tv;
	obj->startWithTime(timeOut);
#else
    TimeVal tv;
    tv.tv_sec = timestamp;
    tv.tv_usec = 0;
    obj->startWithTime(tv);
#endif
    return m_AglieTimerList->insert(obj);
}

bool TimerManager::attachToFixList(TimerObject* obj)
{
    KXASSERT(nullptr != obj);

    long timeKey = timeToKey(obj->getDelay());
    map<long, TimerList*>::iterator iter = m_FixTimerMap.find(timeKey);
    TimerList* targetList = nullptr;
    if (iter == m_FixTimerMap.end())
    {
        targetList = new TimerList();
        m_FixTimerMap[timeKey] = targetList;
    }
    else
    {
        targetList = iter->second;
    }

    if (targetList->pushBack(obj))
    {
        obj->start(m_Now);
        //KX_LOGDEBUG("attachToFixList time tick on %d %d", obj->getTimeVal().tv_sec, obj->getTimeVal().tv_usec);
        return true;
    }

    return false;
}

//if you just wan't update once, this function will be better
bool TimerManager::attachToAglieList(TimerObject* obj)
{
    KXASSERT(nullptr != obj);
    KXASSERT(obj->getRepeat() == 0);

    TimeVal timeOut = m_Now + obj->getDelay();
    obj->startWithTime(timeOut);
    KX_LOGDEBUG("attachToAglieList time tick on %d %d", obj->getTimeVal().tv_sec, obj->getTimeVal().tv_usec);
    if (m_AglieTimerList->insert(obj))
    {
        return true;
    }
    
    return false;
}

