/*
*   �ٻ�ʦ��Ŀ�����¼�����
*
*   1. ���Ŵ���
*
*/
#ifndef _GAME_CHANGE_CLIENT_H__
#define _GAME_CHANGE_CLIENT_H__

#include "EventInterface.h"
#include "SessionClient.h"

class EventConnect :public IGameEvent
{
public:
	EventConnect();
	virtual ~EventConnect();

    virtual bool onUserEvent(int eventId, IComm* target);
	virtual bool onUserProc(unsigned int id);

protected:
    bool sendDataToActor(SessionClient* pClient, int nMainCmd, int nSubCmd, char *pszContext, int nLen);
};

#endif //_GAME_CHANGE_CLIENT_H__
