#ifndef __SessionEvent_H__
#define __SessionEvent_H__

#include "EventInterface.h"



class ClienterEvent :public IGameEvent
{
public:
	ClienterEvent();
	virtual ~ClienterEvent();

	virtual bool onUserEvent(int eventId, IKxComm* target);
	virtual bool onUserProc(unsigned int id);
private:

};

#endif

