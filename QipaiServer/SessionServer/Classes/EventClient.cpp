#include "EventClient.h"
#include "SessionClient.h"

EventClient::EventClient()
{
}

EventClient::~EventClient()
{
}

bool EventClient::onUserEvent(int nType, IKxComm* target) //Session����·��ʱ����nType�Կͻ��˽��в�ͬ����Ӧ
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

bool EventClient::onUserProc(unsigned int id)//Session����·��ʱ��������IDִ�в���
{
	switch (id) //·�ɵ�SessionServerʱ��id��ʾִ�в�ͬ������
	{
	case 1:

	default:
		break;
	}
	
	return true;
}
