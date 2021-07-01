#include "EventConnect.h"
#include "NetManager.h"
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

bool EventConnect::onUserEvent(int nType, IComm* target)
{
    SessionClient *pClient = static_cast<SessionClient *>(target);
    if (pClient != nullptr)
    {

    }
    return true;
}

bool EventConnect::sendDataToActor(SessionClient* pClient, int nMainCmd, int nSubCmd, char *pszContext, int nLen)
{
    unsigned int buffSize = sizeof(Head);
    bool ret = false;
	char* buff = reinterpret_cast<char*>(new char[buffSize]);

	//…Ë÷√Õ∑≤ø
	Head* head = reinterpret_cast<Head*>(buff);
	head->MakeCommand(nMainCmd,nSubCmd);
	head->length = buffSize + nLen;
    head->id = pClient->getUserId();
    ret = pClient->sendData(buff, buffSize) >= 0;
    if (ret && nullptr != pszContext && nLen > 0)
	{
        ret = pClient->sendData(pszContext, nLen) >= 0;
    }
	
	delete[](char*)buff;
    return ret;
}
