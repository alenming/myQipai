/*
 * ����˼���Socket Accept֮�󷵻ص�Socket����
 * ͨ���˶��������ͻ���ͨѶ
 * 
 */
#ifndef __TCPCLIENTER_H__
#define __TCPCLIENTER_H__

#include "KxTCPUnit.h"

class KxTCPClienter : public KxTCPUnit
{
public:
	KxTCPClienter();
	virtual ~KxTCPClienter();

    virtual bool init(KXCOMMID fd);

	virtual void retain();

	virtual void release();

};

#endif
