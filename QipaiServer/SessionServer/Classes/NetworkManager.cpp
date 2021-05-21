#include "NetworkManager.h"
#include "SessionClient.h"
#include "SessionServer.h"
#include "SessionConnect.h"
#include "Head.h"
#include "KxMemPool.h"
using namespace std;

NetWorkManager::NetWorkManager(void)
	: m_GuestId(1)
{
	m_bChanging = false;
}

NetWorkManager::~NetWorkManager(void)
{
	m_bChanging = false;
	map<unsigned int, IKxComm*>::iterator iterGuest = m_GuestMap.begin();
	for (; iterGuest != m_GuestMap.end(); ++iterGuest)
	{
		iterGuest->second->release();
	}
    map<unsigned int, IKxComm*>::iterator iterUser = m_UserMap.begin();
    for (; iterUser != m_UserMap.end(); ++iterUser)
    {
        iterUser->second->release();
    }
    map<int, vector<IKxComm*> >::iterator iterServer = m_ServerMapByGroup.begin();
    for (; iterServer != m_ServerMapByGroup.end(); ++iterServer)
    {
        vector<IKxComm*>& vecServer = iterServer->second;
        for (vector<IKxComm*>::iterator iterSock = vecServer.begin();
            iterSock != vecServer.end(); ++iterSock)
        {
            (*iterSock)->release();
        }
    }
}

NetWorkManager* NetWorkManager::m_Instance = nullptr;
NetWorkManager* NetWorkManager::getInstance()
{
    if (nullptr == m_Instance)
    {
        m_Instance = new NetWorkManager();
    }

    return m_Instance;
}

void NetWorkManager::destroy()
{
    if (nullptr != m_Instance)
    {
        delete m_Instance;
        m_Instance = nullptr;
    }
}

// 通过类型获取服务器,Key对应的Value值来取模
IKxComm* NetWorkManager::getServer(int serverId, int nValue)
{
	return m_ServerList[serverId];
}

// 获取某个服务器组
std::vector<IKxComm*>* NetWorkManager::getGroupServer(int nGroupID)
{
    map<int, std::vector<IKxComm*> >::iterator ator = m_ServerMapByGroup.find(nGroupID);
    if (ator == m_ServerMapByGroup.end())
    {
        return nullptr;
    }
    vector<IKxComm*> *pVectServer = &(ator->second);
    if (pVectServer->size() == 0)
    {
        return nullptr;
    }
    return pVectServer;
}

//获取某个服务器组备份
std::vector<IKxComm*>* NetWorkManager::getBakGroupServer(int nGroupID)
{
	map<int, std::vector<IKxComm*> >::iterator ator = m_BakServerMapByGroup.find(nGroupID);
	if (ator == m_BakServerMapByGroup.end())
	{
		return nullptr;
	}

	vector<IKxComm*> *pVectServer = &(ator->second);
	if (pVectServer->size() == 0)
	{
		return nullptr;
	}
	return pVectServer;
}

bool NetWorkManager::addServer(int serverId, IKxComm* obj)
{
    if (obj == nullptr)
    {
        return false;
    }

	std::map<int, IKxComm* >::iterator ator = m_ServerList.find(serverId);

	if (nullptr != obj && ator == m_ServerList.end())
    {
		m_ServerList[serverId] = obj;
    }

    obj->retain();
    return true;
}

//添加备份后端服务器组ID
bool NetWorkManager::addBakServer(int nGroupID, IKxComm* obj)
{
	if (obj == nullptr)
	{
		return false;
	}

	std::map<int, vector<IKxComm*> >::iterator ator = m_BakServerMapByGroup.find(nGroupID);
	if (nullptr != obj && ator == m_BakServerMapByGroup.end())
	{
		vector<IKxComm*> ServerList;
		ServerList.push_back(obj);
		m_BakServerMapByGroup[nGroupID] = ServerList;
	}
	else
	{
		vector<IKxComm*> &serverList = ator->second;
		serverList.push_back(obj);
	}
	obj->retain();
	return true;
}

//清除某个备份服务器组ID
bool NetWorkManager::clearBakServer(int nGroupID)
{
	std::map<int, vector<IKxComm*> >::iterator ator = m_BakServerMapByGroup.find(nGroupID);
	if (ator != m_BakServerMapByGroup.end())
	{
		vector<IKxComm*> ServerList = ator->second;
		for (unsigned int i = 0; i < ServerList.size(); i++)
		{
			ServerList[i]->release();
		}

		m_BakServerMapByGroup.erase(ator);
	}

	return true;
}

bool NetWorkManager::addUser(int guestId, IKxComm* obj)
{
	map<unsigned int, IKxComm*>::iterator iter = m_UserMap.find(guestId);
	if (iter != m_UserMap.end())
	{
		//客户端ID重复
		return false;
	}

	m_UserMap[guestId] = obj;
	KXSAFE_RETAIN(obj);
	return true;
}

IKxComm* NetWorkManager::getUser(int guestId)
{
	map<unsigned int, IKxComm*>::iterator iter = m_UserMap.find(guestId);
	if (iter == m_UserMap.end())
	{
		//客户端ID找不到
		return nullptr;
	}
	return iter->second;
}

bool NetWorkManager::removeUser(int guestId)
{
	map<unsigned int, IKxComm*>::iterator iter = m_UserMap.find(guestId);
	if (iter == m_UserMap.end())
    {
        return false;
    }

    KXSAFE_RELEASE(iter->second);
	m_UserMap.erase(iter);
    return true;
}

// 数据广播给所有玩家
bool NetWorkManager::broadCastData(char *pszContext, int nLen)
{
    for (map<unsigned int, IKxComm*>::iterator ator = m_UserMap.begin();
        ator != m_UserMap.end(); ++ator)
    {
        ator->second->sendData(pszContext, nLen);
    }
    return true;
}
//给某个玩家发数据
bool NetWorkManager::sendDataToClient(SessionClient* pClient, int nMainCmd, int nSubCmd, char *pszContext, int nLen)
{
	unsigned int buffSize = sizeof(Head) + nLen;
	bool ret = false;
	char* buff = reinterpret_cast<char*>(kxMemMgrAlocate(buffSize));

	//设置头部
	Head* head = reinterpret_cast<Head*>(buff);
	head->MakeCommand(nMainCmd, nSubCmd);
	head->length = buffSize + nLen;
	head->id = pClient->getUserId();
	memcpy(buff + sizeof(Head), pszContext, nLen);
	pClient->sendData(buff, buffSize);
	kxMemMgrRecycle(buff, buffSize);
	return ret;
}

//获取当前连接玩家数
unsigned int NetWorkManager::getCurClientNum()
{
	return m_UserMap.size() + m_UserMap.size();
}

void NetWorkManager::closeAllServer()
{
	std::map<int, std::vector<IKxComm*> >::iterator mapit = m_ServerMapByGroup.begin();
	std::vector<IKxComm*>::iterator vecit;
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

void NetWorkManager::onTimer(const TimeVal& now)
{
	for (std::map<int, std::vector<IKxComm*> >::iterator ator = m_BakServerMapByGroup.begin(); ator != m_BakServerMapByGroup.end(); ++ator)
	{
		int nGroupID = ator->first;
		//删除旧连接
		std::map<int, std::vector<IKxComm*> >::iterator mapit = m_ServerMapByGroup.find(nGroupID);
		if (mapit != m_ServerMapByGroup.end())
		{
			std::vector<IKxComm*> &VectComm = mapit->second;
			for (unsigned int i = 0; i < VectComm.size(); i++)
			{
				VectComm[i]->release();
				SessionServer::getInstance()->getMainPoller()->removeCommObject(VectComm[i]);
			}
			m_ServerMapByGroup.erase(mapit);
		}

		//添加新的连接
		m_ServerMapByGroup[nGroupID] = ator->second;
	}

	m_BakServerMapByGroup.clear();
	m_bChanging = false;
}

bool NetWorkManager::addGuest(unsigned int guestId, IKxComm* obj)
{
	map<unsigned int, IKxComm*>::iterator iter = m_GuestMap.find(guestId);
	if (iter != m_GuestMap.end())
	{
		//客户端ID重复
		return false;
	}

	m_GuestMap[guestId] = obj;
	KXSAFE_RETAIN(obj);
	return true;
}

// 获取待验证连接客户端
IKxComm* NetWorkManager::getGuest(unsigned int guestId)
{
	map<unsigned int, IKxComm*>::iterator iter = m_GuestMap.find(guestId);
	if (iter == m_GuestMap.end())
	{
		//客户端ID找不到
		return nullptr;
	}
	return iter->second;
}

bool NetWorkManager::removeGuest(unsigned int guestId)
{
	map<unsigned int, IKxComm*>::iterator iter = m_GuestMap.find(guestId);
	if (iter == m_GuestMap.end())
	{
		return false;
	}

	KXSAFE_RELEASE(iter->second);
	m_GuestMap.erase(iter);
	return true;
}

// 将连接转为连接OK的客户端
bool NetWorkManager::changeGuestToUser(SessionClient* guest, unsigned int userId)
{
	map<unsigned int, IKxComm*>::iterator iter = m_GuestMap.find(guest->getGuestId());
	if (iter == m_GuestMap.end())
	{
		// 客户端ID找不到
		return false;
	}
	// 此处本该有一次release和retain，相互抵消
	m_UserMap[userId] = guest;
	m_GuestMap.erase(iter);
	return true;
}

unsigned int NetWorkManager::genGuestId()
{
	if (m_GuestId >= 0xFFFFFFFF)
	{
		m_GuestId = 1;
	}
	return m_GuestId++;

}
