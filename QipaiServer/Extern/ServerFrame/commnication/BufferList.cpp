#include "BufferList.h"

BufferList::BufferList()
:m_Head(nullptr),
m_Tail(nullptr)
{
}

BufferList::~BufferList()
{
}

BufferNode* BufferList::next()
{
    BufferNode* node = m_Head;
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
void BufferList::pushBack(char* buffer, unsigned int len)
{
    BufferNode* node = new BufferNode(buffer, len);
    pushBack(node);
}

void BufferList::pushBack(BufferNode* node)
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

void BufferList::clear()
{
	while(nullptr != m_Head)
	{
		BufferNode* node = m_Head;
		m_Head = m_Head->next;
		delete node;
	}
}

