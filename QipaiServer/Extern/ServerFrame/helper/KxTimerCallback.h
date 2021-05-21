#ifndef __KX_TIMER_CALLBACK_H__
#define __KX_TIMER_CALLBACK_H__

#include "KxTimerManager.h"


template<class T>
class KxTimerCallback : public KxTimerObject
{
public:
	typedef void(T::*pTimerCallback)();
	KxTimerCallback()
	{
		m_Object = nullptr;
		m_Callback = nullptr;
	}

	KxTimerCallback(float delay, int repeat = 0)
	{
		KxTimerObject::KxTimerObject(delay, repeat);
		m_Object = nullptr;
		m_Callback = nullptr;
	}

	virtual ~KxTimerCallback()
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

    virtual void onTimer(const kxTimeVal& now)
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