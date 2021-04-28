#ifndef __SessionEvent_H__
#define __SessionEvent_H__
#include "GameInterface.h"
#include "KXServer.h"
#include "SessionServer.h"
#include "SessionClient.h"


class SessionEvent :public IGameEvent
{
public:
	SessionEvent();
	virtual ~SessionEvent();

	virtual bool onUserEvent(int eventId, IKxComm* target);
	virtual bool onUserProc(unsigned int id);
	bool sendDataToActor(SessionClient* pClient, int nMainCmd, int nSubCmd, char *pszContext, int nLen);
private:

};

#endif

