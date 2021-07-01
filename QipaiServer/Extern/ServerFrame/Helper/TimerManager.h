/* 
*   TimerManager 定时器管理器
*   1.定时器对象的定义
*   2.定时器的计时，触发
*   3.FixTime可以高效处理大量时间相等的定时器
*     例如每隔XX秒执行一次
*   4.AgileTime可以灵活处理各种时间不等的定时器
*     例如XX秒后执行一次
*     
*/
#ifndef __TIMERMANAGER_H__
#define __TIMERMANAGER_H__

#include <list>
#include <map>

#include "TimeVal.h"


class TimerList;

#define KXREPEAT_FOREVER    -1

// 时间超时对象，可继承重写onTimer回调
// 并注册到KxTimerManager中来开启计时任务
class TimerObject : public _Object
{
    friend class TimerList;
    friend class TimerManager;
public:
    TimerObject();
    // delay表示需要等待的延迟时间，repeat表示要以delay为间隔重复执行多少次
    // repeat为0表示只执行一次
    TimerObject(float delay, int repeat = 0);
    virtual ~TimerObject();

    // 在计划的时间到达时会被触发
    virtual void onTimer(const TimeVal& now);

    // 停止计时
    void stop();

	// 超时返回true，用于检测超时时间
	inline bool checkTime(const TimeVal& now)
	{
        return m_TimeVal < now;
	}

    inline int getRepeat()
    {
        return m_Repeat;
    }

    inline void setRepeat(int t)
    {
        m_Repeat = t;
    }

    inline const TimeVal& getDelay()
	{
		return m_Delay;
	}

    // 如需在运行中修改delay，请先stop再重新注册
    inline void setDelay(const TimeVal& d)
    {
        m_Delay = d;
    }

    // 设置delay时间为每隔sec秒 + usec百万分之一秒
    inline void setDelay(long sec, long usec)
    {
        m_Delay.tv_sec = sec;
        m_Delay.tv_usec = usec;
    }

    // 如需在运行中修改delay，请先stop再重新注册
    inline void setDelay(float d)
    {
        m_Delay = d;
    }
    
    inline const  TimeVal& getTimeVal()
    {
        return m_TimeVal;
    }
private:
    // 开始计时，超时时间为now + delay，由KxTimerManager调用
    void start(const TimeVal& now);
    // 开始计时，超时时间为t，由KxTimerManager调用
    void startWithTime(const TimeVal& t);

private:
    TimerList* m_TimerList;
    TimerObject* m_Prev;
    TimerObject* m_Next;
    int m_Repeat;				// 重复次数
    TimeVal m_TimeVal;			// 超时时间
    TimeVal m_Delay;			// 执行间隔
};

// 定时器链表
// 提供了高效的添加删除方法
class TimerList : public _Object
{
public:
    TimerList();
    virtual ~TimerList();

    inline TimerObject* head()
    {
        return m_Head;
    }

    inline TimerObject* tail()
    {
        return m_Tail;
    }

    // 返回当前正在触发的定时器对象
    // 默认为NULL，开始触发时自动赋值，触发结束后置为NULL，只有正在触发时才会有值
    inline const TimerObject* currentTimer()
    {
        return m_Timer;
    }

    inline unsigned int length()
    {
        return m_Length;
    }

    bool pushBack(TimerObject* obj);

    bool pushFront(TimerObject* obj);

    bool remove(TimerObject* obj);

    bool insert(TimerObject* obj);

    void update(const TimeVal& now);

private:
    unsigned int m_Length;
    TimerObject* m_Head;
    TimerObject* m_Tail;
    TimerObject* m_Timer;
};

// 定时器管理类
// 内部分为固定列表以及自由列表两种定时器列表
// 两种列表以定时器的时间间隔和重复次数进行划分
// 在插入与更新的效率上，固定列表要远高于自由列表，适合执行每隔XX秒触发的定时器
// 固定列表会为每一种时间间隔分配一个列表，如果这一时间间隔的定时器过少将造成资源的浪费
// 琐碎的时间间隔应该被添加到自由列表中
// 当你不确定应该添加到哪个列表时，或者期望添加到自由列表时
// 直接调用addTimer，让KxTimerManager帮你做最佳选择
class TimerManager : public _Object
{
public:
    TimerManager();
    virtual ~TimerManager();

    // 更新所有的Timer
	void updateTimers();

    // 添加计时器，根究obj自身的delay和repeat来判断超时
    // 当repeat为0时，且没有与超时时间匹配的固定列表，将被添加到自由列表
    // 其它的情况将被添加到固定列表
    bool addTimer(TimerObject* obj);

    // 添加计时器，delay和repeat都会设置到obj身上，该定时器并不精确
    // 当repeat为0时，且没有与超时时间匹配的固定列表，将被添加到自由列表
    // 其它的情况将被添加到固定列表
    bool addTimer(TimerObject* obj, float delay, int repeat = 0);

	bool addTimer(TimerObject* obj, int delay, int repeat = 0);

    // 定时到指定的时间戳中
    bool addTimerOnTime(TimerObject* obj, long timestamp);
	
    // 明确指定添加到固定时长计时器列表
	bool attachToFixList(TimerObject* obj);
	
    // 明确指定添加到自由时长计时器列表
    bool attachToAglieList(TimerObject* obj);

    // 获取当前时间
    inline const TimeVal& getNow()
    {
        return m_Now;
    }

    // 将超时时间转换为Key，精度为千分之一秒
    inline long timeToKey(const TimeVal& delay)
    {
        return delay.tv_sec * 1000 + delay.tv_usec / 1000;
    }

    int getTimestamp()
    {
#if KX_TARGET_PLATFORM == KX_PLATFORM_WIN32
        return m_Timestamp;
#else
        return m_Now.tv_sec;
#endif
    }

private:
#if KX_TARGET_PLATFORM == KX_PLATFORM_WIN32
    int                             m_Timestamp;
#endif
	TimeVal					    m_Now;
	TimerList*         	        m_AglieTimerList;
	std::map<long, TimerList*>    m_FixTimerMap;	
};


#endif
