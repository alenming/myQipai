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

#include "core/KxCore.h"
#include "KxTimeVal.h"


class KxTimerList;

#define KXREPEAT_FOREVER    -1

// ʱ�䳬ʱ���󣬿ɼ̳���дonTimer�ص�
// ��ע�ᵽKxTimerManager����������ʱ����
class KxTimerObject : public KxObject
{
    friend class KxTimerList;
    friend class KxTimerManager;
public:
    KxTimerObject();
    // delay��ʾ��Ҫ�ȴ����ӳ�ʱ�䣬repeat��ʾҪ��delayΪ����ظ�ִ�ж��ٴ�
    // repeatΪ0��ʾִֻ��һ��
    KxTimerObject(float delay, int repeat = 0);
    virtual ~KxTimerObject();

    // �ڼƻ���ʱ�䵽��ʱ�ᱻ����
    virtual void onTimer(const kxTimeVal& now);

    // ֹͣ��ʱ
    void stop();

	// ��ʱ����true�����ڼ�ⳬʱʱ��
	inline bool checkTime(const kxTimeVal& now)
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

    inline const kxTimeVal& getDelay()
	{
		return m_Delay;
	}

    // �������������޸�delay������stop������ע��
    inline void setDelay(const kxTimeVal& d)
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
    
    inline const  kxTimeVal& getTimeVal()
    {
        return m_TimeVal;
    }
private:
    // ��ʼ��ʱ����ʱʱ��Ϊnow + delay����KxTimerManager����
    void start(const kxTimeVal& now);
    // ��ʼ��ʱ����ʱʱ��Ϊt����KxTimerManager����
    void startWithTime(const kxTimeVal& t);

private:
    KxTimerList* m_TimerList;
    KxTimerObject* m_Prev;
    KxTimerObject* m_Next;
    int m_Repeat;               // �ظ�����
    kxTimeVal m_TimeVal;		// ��ʱʱ��
    kxTimeVal m_Delay;          // ִ�м��
};

// ��ʱ������
// �ṩ�˸�Ч�����ɾ������
class KxTimerList : public KxObject
{
public:
    KxTimerList();
    virtual ~KxTimerList();

    inline KxTimerObject* head()
    {
        return m_Head;
    }

    inline KxTimerObject* tail()
    {
        return m_Tail;
    }

    // ���ص�ǰ���ڴ����Ķ�ʱ������
    // Ĭ��ΪNULL����ʼ����ʱ�Զ���ֵ��������������ΪNULL��ֻ�����ڴ���ʱ�Ż���ֵ
    inline const KxTimerObject* currentTimer()
    {
        return m_Timer;
    }

    inline unsigned int length()
    {
        return m_Length;
    }

    bool pushBack(KxTimerObject* obj);

    bool pushFront(KxTimerObject* obj);

    bool remove(KxTimerObject* obj);

    bool insert(KxTimerObject* obj);

    void update(const kxTimeVal& now);

private:
    unsigned int m_Length;
    KxTimerObject* m_Head;
    KxTimerObject* m_Tail;
    KxTimerObject* m_Timer;
};

// ��ʱ��������
// �ڲ���Ϊ�̶��б��Լ������б����ֶ�ʱ���б�
// �����б��Զ�ʱ����ʱ�������ظ��������л���
// �ڲ�������µ�Ч���ϣ��̶��б�ҪԶ���������б��ʺ�ִ��ÿ��XX�봥���Ķ�ʱ��
// �̶��б��Ϊÿһ��ʱ��������һ���б������һʱ�����Ķ�ʱ�����ٽ������Դ���˷�
// �����ʱ����Ӧ�ñ���ӵ������б���
// ���㲻ȷ��Ӧ����ӵ��ĸ��б�ʱ������������ӵ������б�ʱ
// ֱ�ӵ���addTimer����KxTimerManager���������ѡ��
class KxTimerManager : public KxObject
{
public:
    KxTimerManager();
    virtual ~KxTimerManager();

    // �������е�Timer
	void updateTimers();

    // ��Ӽ�ʱ��������obj�����delay��repeat���жϳ�ʱ
    // ��repeatΪ0ʱ����û���볬ʱʱ��ƥ��Ĺ̶��б�������ӵ������б�
    // ���������������ӵ��̶��б�
    bool addTimer(KxTimerObject* obj);

    // ��Ӽ�ʱ����delay��repeat�������õ�obj���ϣ��ö�ʱ��������ȷ
    // ��repeatΪ0ʱ����û���볬ʱʱ��ƥ��Ĺ̶��б�������ӵ������б�
    // ���������������ӵ��̶��б�
    bool addTimer(KxTimerObject* obj, float delay, int repeat = 0);

	bool addTimer(KxTimerObject* obj, int delay, int repeat = 0);

    // ��ʱ��ָ����ʱ�����
    bool addTimerOnTime(KxTimerObject* obj, long timestamp);
	
    // ��ȷָ����ӵ��̶�ʱ����ʱ���б�
	bool attachToFixList(KxTimerObject* obj);
	
    // ��ȷָ����ӵ�����ʱ����ʱ���б�
    bool attachToAglieList(KxTimerObject* obj);

    // ��ȡ��ǰʱ��
    inline const kxTimeVal& getNow()
    {
        return m_Now;
    }

    // ����ʱʱ��ת��ΪKey������Ϊǧ��֮һ��
    inline long timeToKey(const kxTimeVal& delay)
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
	kxTimeVal					    m_Now;
	KxTimerList*         	        m_AglieTimerList;
	std::map<long, KxTimerList*>    m_FixTimerMap;	
};


#endif
