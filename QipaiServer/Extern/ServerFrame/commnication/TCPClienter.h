/*
 * ����˼���Socket Accept֮�󷵻ص�Socket����
 * ͨ���˶��������ͻ���ͨѶ
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
