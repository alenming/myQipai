#include "NetManager.h"
#include "SessionClient.h"
#include "SessionServer.h"
#include "SessionConnect.h"
#include "Head.h"
using namespace std;

NetManager::NetManager(void)
	: m_GuestId(1)
{
	m_bChanging = false;
}

NetManager::~NetManager(void)
{
	m_bChanging = false;
	map<unsigned int, IComm*>::iterator iterGuest = m_GuestMap.begin();
	for (; iterGuest != m_GuestMap.end(); ++iterGuest)
	{
		iterGuest->second->release();
	}
    map<unsigned int, IComm*>::iterator iterUser = m_UserMap.begin();
    for (; iterUser != m_UserMap.end(); ++iterUser)
    {
        iterUser->second->release();
    }
    map<int, vector<IComm*> >::iterator iterServer = m_ServerMapByGroup.begin();
    for (; iterServer != m_ServerMapByGroup.end(); ++iterServer)
    {
        vector<IComm*>& vecServer = iterServer->second;
        for (vector<IComm*>::iterator iterSock = vecServer.begin();
            iterSock != vecServer.end(); ++iterSock)
        {
            (*iterSock)->release();
        }
    }
}

NetManager* NetManager::m_Instance = nullptr;
NetManager* NetManager::getInstance()
{
    if (nullptr == m_Instance)
    {
        m_Instance = new NetManager();
    }

    return m_Instance;
}

void NetManager::destroy()
{
    if (nullptr != m_Instance)
    {
        delete m_Instance;
        m_Instance = nullptr;
    }
}

// ͨ�����ͻ�ȡ������,Key��Ӧ��Valueֵ��ȡģ
IComm* NetManager::getServer(int serverId, int nValue)
{
	return m_ServerList[serverId];
}

// ��ȡĳ����������
std::vector<IComm*>* NetManager::getGroupServer(int nGroupID)
{
    map<int, std::vector<IComm*> >::iterator ator = m_ServerMapByGroup.find(nGroupID);
    if (ator == m_ServerMapByGroup.end())
    {
        return nullptr;
    }
    vector<IComm*> *pVectServer = &(ator->second);
    if (pVectServer->size() == 0)
    {
        return nullptr;
    }
    return pVectServer;
}

//��ȡĳ���������鱸��
std::vector<IComm*>* NetManager::getBakGroupServer(int nGroupID)
{
	map<int, std::vector<IComm*> >::iterator ator = m_BakServerMapByGroup.find(nGroupID);
	if (ator == m_BakServerMapByGroup.end())
	{
		return nullptr;
	}

	vector<IComm*> *pVectServer = &(ator->second);
	if (pVectServer->size() == 0)
	{
		return nullptr;
	}
	return pVectServer;
}

bool NetManager::addServer(int serverId, IComm* obj)
{
    if (obj == nullptr)
    {
        return false;
    }

	std::map<int, IComm* >::iterator ator = m_ServerList.find(serverId);

	if (nullptr != obj && ator == m_ServerList.end())
    {
		m_ServerList[serverId] = obj;
    }

    obj->retain();
    return true;
}

//��ӱ��ݺ�˷�������ID
bool NetManager::addBakServer(int nGroupID, IComm* obj)
{
	if (obj == nullptr)
	{
		return false;
	}

	std::map<int, vector<IComm*> >::iterator ator = m_BakServerMapByGroup.find(nGroupID);
	if (nullptr != obj && ator == m_BakServerMapByGroup.end())
	{
		vector<IComm*> ServerList;
		ServerList.push_back(obj);
		m_BakServerMapByGroup[nGroupID] = ServerList;
	}
	else
	{
		vector<IComm*> &serverList = ator->second;
		serverList.push_back(obj);
	}
	obj->retain();
	return true;
}

//���ĳ�����ݷ�������ID
bool NetManager::clearBakServer(int nGroupID)
{
	std::map<int, vector<IComm*> >::iterator ator = m_BakServerMapByGroup.find(nGroupID);
	if (ator != m_BakServerMapByGroup.end())
	{
		vector<IComm*> ServerList = ator->second;
		for (unsigned int i = 0; i < ServerList.size(); i++)
		{
			ServerList[i]->release();
		}

		m_BakServerMapByGroup.erase(ator);
	}

	return true;
}

bool NetManager::addUser(int guestId, IComm* obj)
{
	map<unsigned int, IComm*>::iterator iter = m_UserMap.find(guestId);
	if (iter != m_UserMap.end())
	{
		//�ͻ���ID�ظ�
		return false;
	}

	m_UserMap[guestId] = obj;
	KXSAFE_RETAIN(obj);
	return true;
}

IComm* NetManager::getUser(int guestId)
{
	map<unsigned int, IComm*>::iterator iter = m_UserMap.find(guestId);
	if (iter == m_UserMap.end())
	{
		//�ͻ���ID�Ҳ���
		return nullptr;
	}
	return iter->second;
}

bool NetManager::removeUser(int guestId)
{
	map<unsigned int, IComm*>::iterator iter = m_UserMap.find(guestId);
	if (iter == m_UserMap.end())
    {
        return false;
    }

    KXSAFE_RELEASE(iter->second);
	m_UserMap.erase(iter);
    return true;
}

// ���ݹ㲥���������
bool NetManager::broadCastData(char *pszContext, int nLen)
{
    for (map<unsigned int, IComm*>::iterator ator = m_UserMap.begin();
        ator != m_UserMap.end(); ++ator)
    {
        ator->second->sendData(pszContext, nLen);
    }
    return true;
}
//��ĳ����ҷ�����
bool NetManager::sendDataToClient(SessionClient* pClient, int nMainCmd, int nSubCmd, char *pszContext, int nLen)
{
	unsigned int buffSize = sizeof(Head) + nLen;
	bool ret = false;
	char* buff = reinterpret_cast<char*>(new char[buffSize]);

	//����ͷ��
	Head* head = reinterpret_cast<Head*>(buff);
	head->MakeCommand(nMainCmd, nSubCmd);
	head->length = buffSize + nLen;
	head->id = pClient->getUserId();
	memcpy(buff + sizeof(Head), pszContext, nLen);
	pClient->sendData(buff, buffSize);
	delete[](char*)buff;
	return ret;
}

//��ȡ��ǰ���������
unsigned int NetManager::getCurClientNum()
{
	return m_UserMap.size() + m_UserMap.size();
}

void NetManager::closeAllServer()
{
	std::map<int, std::vector<IComm*> >::iterator mapit = m_ServerMapByGroup.begin();
	std::vector<IComm*>::iterator vecit;
	for (; mapit!=m_ServerMapByGroup.end(); ++mapit)
	{
		vecit = mapit->second.begin();
		for (; vecit!=mapit->second.end(); ++vecit)
		{
			SessionServer::getInstance()->getMainPoller()->removeCommObject((*vecit));
			(*vecit)->close();
			(*vecit)->release();
		}
	}
	m_ServerMapByGroup.clear();
}

void NetManager::onTimer(const TimeVal& now)
{
	for (std::map<int, std::vector<IComm*> >::iterator ator = m_BakServerMapByGroup.begin(); ator != m_BakServerMapByGroup.end(); ++ator)
	{
		int nGroupID = ator->first;
		//ɾ��������
		std::map<int, std::vector<IComm*> >::iterator mapit = m_ServerMapByGroup.find(nGroupID);
		if (mapit != m_ServerMapByGroup.end())
		{
			std::vector<IComm*> &VectComm = mapit->second;
			for (unsigned int i = 0; i < VectComm.size(); i++)
			{
				VectComm[i]->release();
				SessionServer::getInstance()->getMainPoller()->removeCommObject(VectComm[i]);
			}
			m_ServerMapByGroup.erase(mapit);
		}

		//����µ�����
		m_ServerMapByGroup[nGroupID] = ator->second;
	}

	m_BakServerMapByGroup.clear();
	m_bChanging = false;
}

bool NetManager::addGuest(unsigned int guestId, IComm* obj)
{
	map<unsigned int, IComm*>::iterator iter = m_GuestMap.find(guestId);
	if (iter != m_GuestMap.end())
	{
		//�ͻ���ID�ظ�
		return false;
	}

	m_GuestMap[guestId] = obj;
	KXSAFE_RETAIN(obj);
	return true;
}

// ��ȡ����֤���ӿͻ���
IComm* NetManager::getGuest(unsigned int guestId)
{
	map<unsigned int, IComm*>::iterator iter = m_GuestMap.find(guestId);
	if (iter == m_GuestMap.end())
	{
		//�ͻ���ID�Ҳ���
		return nullptr;
	}
	return iter->second;
}

bool NetManager::removeGuest(unsigned int guestId)
{
	map<unsigned int, IComm*>::iterator iter = m_GuestMap.find(guestId);
	if (iter == m_GuestMap.end())
	{
		return false;
	}

	KXSAFE_RELEASE(iter->second);
	m_GuestMap.erase(iter);
	return true;
}

// ������תΪ����OK�Ŀͻ���
bool NetManager::changeGuestToUser(SessionClient* guest, unsigned int userId)
{
	map<unsigned int, IComm*>::iterator iter = m_GuestMap.find(guest->getGuestId());
	if (iter == m_GuestMap.end())
	{
		// �ͻ���ID�Ҳ���
		return false;
	}
	// �˴�������һ��release��retain���໥����
	m_UserMap[userId] = guest;
	m_GuestMap.erase(iter);
	return true;
}

unsigned int NetManager::genGuestId()
{
	if (m_GuestId >= 0xFFFFFFFF)
	{
		m_GuestId = 1;
	}
	return m_GuestId++;

}
