/* 
*   TimeVal结构体
*   处理时间计算
*   
*/
#ifndef __KXTIMEVAL_H__
#define __KXTIMEVAL_H__

#define MILLION 1000000

#include "Core.h"

// 时间结构体
class TimeVal
{
public:
    TimeVal()
    {
        tv_sec = 0;
        tv_usec = 0;
    }
    
    TimeVal(long sec, long usec)
    {
        tv_sec = sec;
        tv_usec = usec;
    }
    
    TimeVal(float t)
    {
        tv_sec = (long)t;
        tv_usec = (long)((t - tv_sec) * MILLION);
    }
    
    inline void add(const TimeVal& v)
    {
        tv_sec += v.tv_sec;
        tv_usec += v.tv_usec;
    }
    
    inline void sub(const TimeVal& v)
    {
        tv_sec -= v.tv_sec;
        tv_usec -= v.tv_usec;
    }
    
    // 以秒为单位设置时间
    inline void setFromfloat(float t)
    {
        tv_sec = (long)t;
        tv_usec = (long)((t - tv_sec) * MILLION);
    }
    
    // 以秒为单位返回当前设置的时间
    inline float getfloat() const
    {
        return tv_sec + 1.0f * tv_usec / MILLION;
    }
    
    // 设置为当前系统时间
    inline TimeVal& now()
    {
        gettimeofday((struct timeval *)this,  (struct timezone*)0);
        return *this;
    }
    
    inline const TimeVal operator+(const TimeVal& v) const
    {
        TimeVal ret = *this;
        ret.add(v);
        return ret;
    }
    
    inline TimeVal& operator+=(const TimeVal& v)
    {
        add(v);
        return *this;
    }
    
    inline TimeVal& operator+=(float v)
    {
        long t = (long)v;
        tv_sec += t;
        tv_usec += (long)((v - t) * MILLION);
        return *this;
    }
    
    inline const TimeVal operator-(const TimeVal& v) const
    {
        TimeVal ret = *this;
        ret.sub(v);
        return ret;
    }
    
    inline TimeVal& operator-=(const TimeVal& v)
    {
        sub(v);
        return *this;
    }
    
    inline TimeVal& operator-=(float v)
    {
        long t = (long)v;
        tv_sec -= t;
        tv_usec -= (long)((v - t) * MILLION);
        return *this;
    }
    
    inline bool operator==(const TimeVal& v) const
    {
        return tv_sec == v.tv_sec && tv_usec == v.tv_usec;
    }
    
    inline bool operator!=(const TimeVal& v) const
    {
        return tv_sec != v.tv_sec || tv_usec != v.tv_usec;
    }
    
    inline bool operator>(const TimeVal& v) const
    {
        if(tv_sec > v.tv_sec)
        {
            return true;
        }
        else if(tv_sec == v.tv_sec)
        {
            return tv_usec > v.tv_usec;
        }
        else
        {
            return false;
        }
    }
    
    inline bool operator<(const TimeVal& v) const
    {
        if(tv_sec < v.tv_sec)
        {
            return true;
        }
        else if(tv_sec == v.tv_sec)
        {
            return tv_usec < v.tv_usec;
        }
        else
        {
            return false;
        }
    }

    long tv_sec;		// 秒
    long tv_usec;		// 毫秒(百万分之一秒)
};


#endif