/*
*   KxServer��ܵĻ�������
*   ʵ�������ü���
*/
#ifndef __KXOBJECT_H__
#define __KXOBJECT_H__

class KxObject
{
public:
    KxObject();
    virtual ~KxObject();

public:
    virtual void retain();

    virtual void release();

    virtual unsigned int getReferenceCount() const;

protected:
    unsigned int m_ReferenceCount;
};

#define KXSAFE_RETAIN(obj) if(NULL != obj) { obj->retain(); }
#define KXSAFE_RELEASE(obj) if(NULL != obj) { obj->release(); obj = NULL; }
#define KXSAFE_DELETE(obj) if(NULL != obj) { delete obj; obj = NULL; }

#endif