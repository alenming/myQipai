/*
*   召唤师项目特殊事件处理
*
*   1. 顶号处理
*
*/
#ifndef _GAME_CHANGE_CLIENT_H__
#define _GAME_CHANGE_CLIENT_H__

#include "EventInterface.h"
#include "SessionClienter.h"

class ConnectEvent :public IGameEvent
{
public:
	ConnectEvent();
	virtual ~ConnectEvent();

    virtual bool onUserEvent(int eventId, IKxComm* target);
	virtual bool onUserProc(unsigned int id);

protected:
    bool sendDataToActor(SessionClienter* pClient, int nMainCmd, int nSubCmd, char *pszContext, int nLen);
};

#endif //_GAME_CHANGE_CLIENT_H__
