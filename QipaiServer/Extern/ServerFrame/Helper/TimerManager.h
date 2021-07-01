/* 
*   TimerManager ��ʱ��������
*   1.��ʱ������Ķ���
*   2.��ʱ���ļ�ʱ������
*   3.FixTime���Ը�Ч�������ʱ����ȵĶ�ʱ��
*     ����ÿ��XX��ִ��һ��
*   4.AgileTime�����������ʱ�䲻�ȵĶ�ʱ��
*     ����XX���ִ��һ��
*     
*/
#ifndef __TIMERMANAGER_H__
#define __TIMERMANAGER_H__

#include <list>
#include <map>

#include "TimeVal.h"


class TimerList;

#define KXREPEAT_FOREVER    -1

// ʱ�䳬ʱ���󣬿ɼ̳���дonTimer�ص�
// ��ע�ᵽKxTimerManager����������ʱ����
class TimerObject : public _Object
{
    friend class TimerList;
    friend class TimerManager;
public:
    TimerObject();
    // delay��ʾ��Ҫ�ȴ����ӳ�ʱ�䣬repeat��ʾҪ��delayΪ����ظ�ִ�ж��ٴ�
    // repeatΪ0��ʾִֻ��һ��
    TimerObject(float delay, int repeat = 0);
    virtual ~TimerObject();

    // �ڼƻ���ʱ�䵽��ʱ�ᱻ����
    virtual void onTimer(const TimeVal& now);

    // ֹͣ��ʱ
    void stop();

	// ��ʱ����true�����ڼ�ⳬʱʱ��
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

    // �������������޸�delay������stop������ע��
    inline void setDelay(const TimeVal& d)
    {
        m_Delay = d;
    }

    // ����delayʱ��Ϊÿ��sec�� + usec�����֮һ��
    inline void setDelay(long sec, long usec)
    {
        m_Delay.tv_sec = sec;
        m_Delay.tv_usec = usec;
    }

    // �������������޸�delay������stop������ע��
    inline void setDelay(float d)
    {
        m_Delay = d;
    }
    
    inline const  TimeVal& getTimeVal()
    {
        return m_TimeVal;
    }
private:
    // ��ʼ��ʱ����ʱʱ��Ϊnow + delay����KxTimerManager����
    void start(const TimeVal& now);
    // ��ʼ��ʱ����ʱʱ��Ϊt����KxTimerManager����
    void startWithTime(const TimeVal& t);

private:
    TimerList* m_TimerList;
    TimerObject* m_Prev;
    TimerObject* m_Next;
    int m_Repeat;				// �ظ�����
    TimeVal m_TimeVal;			// ��ʱʱ��
    TimeVal m_Delay;			// ִ�м��
};

// ��ʱ������
// �ṩ�˸�Ч�����ɾ������
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

    // ���ص�ǰ���ڴ����Ķ�ʱ������
    // Ĭ��ΪNULL����ʼ����ʱ�Զ���ֵ��������������ΪNULL��ֻ�����ڴ���ʱ�Ż���ֵ
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

// ��ʱ��������
// �ڲ���Ϊ�̶��б��Լ������б����ֶ�ʱ���б�
// �����б��Զ�ʱ����ʱ�������ظ��������л���
// �ڲ�������µ�Ч���ϣ��̶��б�ҪԶ���������б��ʺ�ִ��ÿ��XX�봥���Ķ�ʱ��
// �̶��б��Ϊÿһ��ʱ��������һ���б������һʱ�����Ķ�ʱ�����ٽ������Դ���˷�
// �����ʱ����Ӧ�ñ���ӵ������б���
// ���㲻ȷ��Ӧ����ӵ��ĸ��б�ʱ������������ӵ������б�ʱ
// ֱ�ӵ���addTimer����KxTimerManager���������ѡ��
class TimerManager : public _Object
{
public:
    TimerManager();
    virtual ~TimerManager();

    // �������е�Timer
	void updateTimers();

    // ��Ӽ�ʱ��������obj�����delay��repeat���жϳ�ʱ
    // ��repeatΪ0ʱ����û���볬ʱʱ��ƥ��Ĺ̶��б�������ӵ������б�
    // ���������������ӵ��̶��б�
    bool addTimer(TimerObject* obj);

    // ��Ӽ�ʱ����delay��repeat�������õ�obj���ϣ��ö�ʱ��������ȷ
    // ��repeatΪ0ʱ����û���볬ʱʱ��ƥ��Ĺ̶��б�������ӵ������б�
    // ���������������ӵ��̶��б�
    bool addTimer(TimerObject* obj, float delay, int repeat = 0);

	bool addTimer(TimerObject* obj, int delay, int repeat = 0);

    // ��ʱ��ָ����ʱ�����
    bool addTimerOnTime(TimerObject* obj, long timestamp);
	
    // ��ȷָ����ӵ��̶�ʱ����ʱ���б�
	bool attachToFixList(TimerObject* obj);
	
    // ��ȷָ����ӵ�����ʱ����ʱ���б�
    bool attachToAglieList(TimerObject* obj);

    // ��ȡ��ǰʱ��
    inline const TimeVal& getNow()
    {
        return m_Now;
    }

    // ����ʱʱ��ת��ΪKey������Ϊǧ��֮һ��
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
