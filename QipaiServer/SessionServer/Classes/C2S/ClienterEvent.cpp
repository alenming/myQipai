#include "ClienterEvent.h"
#include "SessionClienter.h"

ClienterEvent::ClienterEvent()
{
}

ClienterEvent::~ClienterEvent()
{
}

bool ClienterEvent::onUserEvent(int nType, IKxComm* target) //Session����·��ʱ����nType�Կͻ��˽��в�ͬ����Ӧ
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

bool ClienterEvent::onUserProc(unsigned int id)//Session����·��ʱ��������IDִ�в���
{
	switch (id) //·�ɵ�SessionServerʱ��id��ʾִ�в�ͬ������
	{
	case 1:

	default:
		break;
	}
	
	return true;
}
