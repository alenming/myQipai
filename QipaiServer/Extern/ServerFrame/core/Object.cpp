#include "Object.h"

Object::Object()
:m_ReferenceCount(1)
{
}

Object::~Object()
{
}

void Object::retain()
{
    if (m_ReferenceCount > 0)
    {
        ++m_ReferenceCount;
    }
}

void Object::release()
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

unsigned int Object::getReferenceCount() const
{
    return m_ReferenceCount;
}
