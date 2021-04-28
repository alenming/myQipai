/*
 * BufferList �������б�
 * ���ڻ������ݿ�
 */
#ifndef __BUFFERLIST_H__
#define __BUFFERLIST_H__

#include <stdlib.h>

struct kxBufferNode
{
	kxBufferNode(char* buf, unsigned int l)
		:buffer(buf),
		len(l),
		next(NULL)
	{
	}

	kxBufferNode()
		:buffer(NULL),
		len(0),
		next(NULL)
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

    inline bool isEmpty() { return NULL == m_Head; }
    
    inline kxBufferNode* head()
    {
        return m_Head;
    }

private:
	kxBufferNode* m_Head;
	kxBufferNode* m_Tail;
};


#endif
