/* 
*   TimeVal�ṹ��
*   ����ʱ�����
*   
*/
#ifndef __KXTIMEVAL_H__
#define __KXTIMEVAL_H__

#define MILLION 1000000

#include "core/KxCore.h"

// ʱ��ṹ��
class kxTimeVal
{
public:
    kxTimeVal()
    {
        tv_sec = 0;
        tv_usec = 0;
    }
    
    kxTimeVal(long sec, long usec)
    {
        tv_sec = sec;
        tv_usec = usec;
    }
    
    kxTimeVal(float t)
    {
        tv_sec = (long)t;
        tv_usec = (long)((t - tv_sec) * MILLION);
    }
    
    inline void add(const kxTimeVal& v)
    {
        tv_sec += v.tv_sec;
        tv_usec += v.tv_usec;
    }
    
    inline void sub(const kxTimeVal& v)
    {
        tv_sec -= v.tv_sec;
        tv_usec -= v.tv_usec;
    }
    
    // ����Ϊ��λ����ʱ��
    inline void setFromfloat(float t)
    {
        tv_sec = (long)t;
        tv_usec = (long)((t - tv_sec) * MILLION);
    }
    
    // ����Ϊ��λ���ص�ǰ���õ�ʱ��
    inline float getfloat() const
    {
        return tv_sec + 1.0f * tv_usec / MILLION;
    }
    
    // ����Ϊ��ǰϵͳʱ��
    inline kxTimeVal& now()
    {
        gettimeofday((struct timeval *)this,  (struct timezone*)0);
        return *this;
    }
    
    inline const kxTimeVal operator+(const kxTimeVal& v) const
    {
        kxTimeVal ret = *this;
        ret.add(v);
        return ret;
    }
    
    inline kxTimeVal& operator+=(const kxTimeVal& v)
    {
        add(v);
        return *this;
    }
    
    inline kxTimeVal& operator+=(float v)
    {
        long t = (long)v;
        tv_sec += t;
        tv_usec += (long)((v - t) * MILLION);
        return *this;
    }
    
    inline const kxTimeVal operator-(const kxTimeVal& v) const
    {
        kxTimeVal ret = *this;
        ret.sub(v);
        return ret;
    }
    
    inline kxTimeVal& operator-=(const kxTimeVal& v)
    {
        sub(v);
        return *this;
    }
    
    inline kxTimeVal& operator-=(float v)
    {
        long t = (long)v;
        tv_sec -= t;
        tv_usec -= (long)((v - t) * MILLION);
        return *this;
    }
    
    inline bool operator==(const kxTimeVal& v) const
    {
        return tv_sec == v.tv_sec && tv_usec == v.tv_usec;
    }
    
    inline bool operator!=(const kxTimeVal& v) const
    {
        return tv_sec != v.tv_sec || tv_usec != v.tv_usec;
    }
    
    inline bool operator>(const kxTimeVal& v) const
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
    
    inline bool operator<(const kxTimeVal& v) const
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

    long tv_sec;		// ��
    long tv_usec;		// ����(�����֮һ��)
};


#endif