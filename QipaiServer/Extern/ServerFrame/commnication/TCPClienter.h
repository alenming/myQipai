/*
 * 服务端监听Socket Accept之后返回的Socket对象
 * 通过此对象可以与客户端通讯
 * 
 */
#ifndef __TCPCLIENTER_H__
#define __TCPCLIENTER_H__

#include "TCPUnit.h"

class TCPClienter : public TCPUnit
{
public:
	TCPClienter();
	virtual ~TCPClienter();

    virtual bool init(KXCOMMID fd);

	virtual void retain();

	virtual void release();

};

#endif
