#include "Object.h"

_Object::_Object()
:m_ReferenceCount(1)
{
}

_Object::~_Object()
{
}

void _Object::retain()
{
    if (m_ReferenceCount > 0)
    {
        ++m_ReferenceCount;
    }
}

void _Object::release()
{
    if (m_ReferenceCount > 0)
    {
        --m_ReferenceCount;
        if (m_ReferenceCount == 0)
        {
            delete this;
        }
    }
}

unsigned int _Object::getReferenceCount() const
{
    return m_ReferenceCount;
}
