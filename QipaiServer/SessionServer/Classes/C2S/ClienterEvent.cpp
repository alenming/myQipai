#include "ClienterEvent.h"
#include "SessionClienter.h"

ClienterEvent::ClienterEvent()
{
}

ClienterEvent::~ClienterEvent()
{
}

bool ClienterEvent::onUserEvent(int nType, IKxComm* target) //Session本地路由时根据nType对客户端进行不同的响应
{
	SessionClienter *pClient = static_cast<SessionClienter *>(target);
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

bool ClienterEvent::onUserProc(unsigned int id)//Session本地路由时根据命令ID执行操作
{
	switch (id) //路由到SessionServer时，id表示执行不同的命令
	{
	case 1:

	default:
		break;
	}
	
	return true;
}
