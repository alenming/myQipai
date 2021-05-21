/*
 * BufferList 缓冲区列表
 * 用于缓存数据块
 */
#ifndef __BUFFERLIST_H__
#define __BUFFERLIST_H__

#include <stdlib.h>

struct BufferNode
{
	BufferNode(char* buf, unsigned int l)
		:buffer(buf),
		len(l),
		next(nullptr)
	{
	}

	BufferNode()
		:buffer(nullptr),
		len(0),
		next(nullptr)
	{
	}

	char* buffer;
	unsigned int len;
	BufferNode* next;
};

class BufferList
{
public:
	BufferList();
	virtual ~BufferList();

    BufferNode* next();

    void pushBack(char* buffer, unsigned int len);

    void pushBack(BufferNode* node);

	void clear();

    inline bool isEmpty() { return nullptr == m_Head; }
    
    inline BufferNode* head()
    {
        return m_Head;
    }

private:
	BufferNode* m_Head;
	BufferNode* m_Tail;
};


#endif
