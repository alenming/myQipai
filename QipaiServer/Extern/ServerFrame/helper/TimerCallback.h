#ifndef __KX_TIMER_CALLBACK_H__
#define __KX_TIMER_CALLBACK_H__

#include "TimerManager.h"


template<class T>
class TimerCallback : public TimerObject
{
public:
	typedef void(T::*pTimerCallback)();
	TimerCallback()
	{
		m_Object = nullptr;
		m_Callback = nullptr;
	}

	TimerCallback(float delay, int repeat = 0)
	{
		TimerObject::TimerObject(delay, repeat);
		m_Object = nullptr;
		m_Callback = nullptr;
	}

	virtual ~TimerCallback()
	{
		KXSAFE_RELEASE(m_Object);
	}

	void init(float delay, int repeat = 0)
	{
		setDelay(delay);
		setRepeat(repeat);
	}

    void setCallback(T* obj, pTimerCallback callback)
    {
        KXSAFE_RELEASE(m_Object);
        KXSAFE_RETAIN(obj);

        m_Object = obj;
        m_Callback = callback;
    }

	void clean()
	{
		KXSAFE_RELEASE(m_Object);
	}

    virtual void onTimer(const TimeVal& now)
    {
        if (nullptr != m_Object)
        {
            (m_Object->*m_Callback)();
        }
    }

private:
    T* m_Object;
    pTimerCallback m_Callback;
};

#endif