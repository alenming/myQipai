#include "SessionEvent.h"
#include "SessionClient.h"
#include "Head.h"


SessionEvent::SessionEvent()
{
}

SessionEvent::~SessionEvent()
{
}

bool SessionEvent::onUserEvent(int nType, IKxComm* target) //Session本地路由时根据nType对客户端进行不同的响应
{
	SessionClient *pClient = static_cast<SessionClient *>(target);
	if (pClient != NULL)
	{
		switch (nType)
		{
		case EventClosed:
			break;
		default:
			break;
		}
	}
	return true;
}

bool SessionEvent::onUserProc(unsigned int id)//Session本地路由时根据命令ID执行操作
{
	switch (id) //路由到SessionServer时，id表示执行不同的命令
	{
	case 1:

	default:
		break;
	}
	
	return true;
}

bool SessionEvent::sendDataToActor(SessionClient* pClient, int nMainCmd, int nSubCmd, char *pszContext, int nLen)
{
	unsigned int buffSize = sizeof(Head);
	bool ret = false;
	char* buff = reinterpret_cast<char*>(kxMemMgrAlocate(buffSize));

	//设置头部
	Head* head = reinterpret_cast<Head*>(buff);
	head->MakeCommand(nMainCmd, nSubCmd);
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
