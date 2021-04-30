#include "ConnectEvent.h"
#include "NetworkManager.h"
#include "Head.h"



ConnectEvent::ConnectEvent()
{
}


ConnectEvent::~ConnectEvent()
{
}

bool ConnectEvent::onUserProc(unsigned int id)
{
	
	return true;
}

bool ConnectEvent::onUserEvent(int nType, IKxComm* target)
{
    SessionClienter *pClient = static_cast<SessionClienter *>(target);
    if (pClient != NULL)
    {

    }
    return true;
}

bool ConnectEvent::sendDataToActor(SessionClienter* pClient, int nMainCmd, int nSubCmd, char *pszContext, int nLen)
{
    unsigned int buffSize = sizeof(Head);
    bool ret = false;
	char* buff = reinterpret_cast<char*>(kxMemMgrAlocate(buffSize));

	//…Ë÷√Õ∑≤ø
	Head* head = reinterpret_cast<Head*>(buff);
	head->MakeCommand(nMainCmd,nSubCmd);
	head->length = buffSize + nLen;
    head->uid = pClient->getUserId();
    ret = pClient->sendData(buff, buffSize) >= 0;
    if (ret && NULL != pszContext && nLen > 0)
	{
        ret = pClient->sendData(pszContext, nLen) >= 0;
    }
	kxMemMgrRecycle(buff, buffSize);
    return ret;
}
