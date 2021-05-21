/*
*   KxServer框架的基础对象
*   实现了引用计数
*/
#ifndef __KXOBJECT_H__
#define __KXOBJECT_H__

class Object
{
public:
    Object();
    virtual ~Object();

public:
    virtual void retain();

    virtual void release();

    virtual unsigned int getReferenceCount() const;

protected:
    unsigned int m_ReferenceCount;
};

#define KXSAFE_RETAIN(obj) if(nullptr != obj) { obj->retain(); }
#define KXSAFE_RELEASE(obj) if(nullptr != obj) { obj->release(); obj = nullptr; }
#define KXSAFE_DELETE(obj) if(nullptr != obj) { delete obj; obj = nullptr; }

#endif