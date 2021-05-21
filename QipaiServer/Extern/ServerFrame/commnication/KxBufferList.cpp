#include "KxBufferList.h"

KxBufferList::KxBufferList()
:m_Head(nullptr),
m_Tail(nullptr)
{
}

KxBufferList::~KxBufferList()
{
}

kxBufferNode* KxBufferList::next()
{
    kxBufferNode* node = m_Head;
    if (m_Tail == m_Head)
    {
        m_Head = m_Tail = nullptr;
    }
    else
    {
        m_Head = m_Head->next;
    }

    return node;
}
void KxBufferList::pushBack(char* buffer, unsigned int len)
{
    kxBufferNode* node = new kxBufferNode(buffer, len);
    pushBack(node);
}

void KxBufferList::pushBack(kxBufferNode* node)
{
    if (nullptr == m_Tail)
    {
        m_Tail = m_Head = node;
    }
    else
    {
        m_Tail->next = node;
        m_Tail = node;
    }
}

void KxBufferList::clear()
{
	while(nullptr != m_Head)
	{
		kxBufferNode* node = m_Head;
		m_Head = m_Head->next;
		delete node;
	}
}

