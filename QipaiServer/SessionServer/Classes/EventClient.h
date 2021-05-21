#ifndef __SessionEvent_H__
#define __SessionEvent_H__

#include "EventInterface.h"



class EventClient :public IGameEvent
{
public:
	EventClient();
	virtual ~EventClient();

	virtual bool onUserEvent(int eventId, IKxComm* target);
	virtual bool onUserProc(unsigned int id);
private:

};

#endif

