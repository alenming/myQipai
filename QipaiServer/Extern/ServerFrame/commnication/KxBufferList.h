/*
 * BufferList 缓冲区列表
 * 用于缓存数据块
 */
#ifndef __BUFFERLIST_H__
#define __BUFFERLIST_H__

#include <stdlib.h>

struct kxBufferNode
{
	kxBufferNode(char* buf, unsigned int l)
		:buffer(buf),
		len(l),
		next(nullptr)
	{
	}

	kxBufferNode()
		:buffer(nullptr),
		len(0),
		next(nullptr)
	{
	}

	char* buffer;
	unsigned int len;
	kxBufferNode* next;
};

class KxBufferList
{
public:
	KxBufferList();
	virtual ~KxBufferList();

    kxBufferNode* next();

    void pushBack(char* buffer, unsigned int len);

    void pushBack(kxBufferNode* node);

	void clear();

    inline bool isEmpty() { return nullptr == m_Head; }
    
    inline kxBufferNode* head()
    {
        return m_Head;
    }

private:
	kxBufferNode* m_Head;
	kxBufferNode* m_Tail;
};


#endif
