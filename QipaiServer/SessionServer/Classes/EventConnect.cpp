#include "EventConnect.h"
#include "NetworkManager.h"
#include "Head.h"



EventConnect::EventConnect()
{
}


EventConnect::~EventConnect()
{
}

bool EventConnect::onUserProc(unsigned int id)
{
	
	return true;
}

bool EventConnect::onUserEvent(int nType, IKxComm* target)
{
    SessionClient *pClient = static_cast<SessionClient *>(target);
    if (pClient != NULL)
    {

    }
    return true;
}

bool EventConnect::sendDataToActor(SessionClient* pClient, int nMainCmd, int nSubCmd, char *pszContext, int nLen)
{
    unsigned int buffSize = sizeof(Head);
    bool ret = false;
	char* buff = reinterpret_cast<char*>(kxMemMgrAlocate(buffSize));

	//…Ë÷√Õ∑≤ø
	Head* head = reinterpret_cast<Head*>(buff);
	head->MakeCommand(nMainCmd,nSubCmd);
	head->length = buffSize + nLen;
    head->id = pClient->getUserId();
    ret = pClient->sendData(buff, buffSize) >= 0;
    if (ret && NULL != pszContext && nLen > 0)
	{
        ret = pClient->sendData(pszContext, nLen) >= 0;
    }
	kxMemMgrRecycle(buff, buffSize);
    return ret;
}
