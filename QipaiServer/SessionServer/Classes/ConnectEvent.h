/*
*   �ٻ�ʦ��Ŀ�����¼�����
*
*   1. ���Ŵ���
*
*/
#ifndef _GAME_CHANGE_CLIENT_H__
#define _GAME_CHANGE_CLIENT_H__

#include "GameInterface.h"
#include "KXServer.h"
#include "SessionClient.h"

class ConnectEvent :public IGameEvent
{
public:
	ConnectEvent();
	virtual ~ConnectEvent();

    virtual bool onUserEvent(int eventId, IKxComm* target);
	virtual bool onUserProc(unsigned int id);

protected:
    bool sendDataToActor(SessionClient* pClient, int nMainCmd, int nSubCmd, char *pszContext, int nLen);
};

#endif //_GAME_CHANGE_CLIENT_H__
