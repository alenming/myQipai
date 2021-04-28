#include "ServerConfig.h"
#include <time.h>
#include <string.h>
#include "helper/CsvLoader.h"

using namespace std;

#define SERVER_PATH   "..//GameConfig//ServerConfig.csv"
#define SERVER_PATH_FOR_DEBUG   "..//..//GameConfig//ServerConfig.csv"
ServerConfig::ServerConfig()
{
}


ServerConfig::~ServerConfig()
{
}

bool ServerConfig::create()
{
	clean();
	if (!loadServerData())
	{
		return false;
	}

	return true;
}

void ServerConfig::clean()
{
	m_ServerData.clear();
}

bool ServerConfig::loadServerData()
{
	CsvLoader pLoader;
	if (!pLoader.LoadCSV(SERVER_PATH))
	{
		if (!pLoader.LoadCSV(SERVER_PATH_FOR_DEBUG))
		{
			return false;
		}
	}

	while (pLoader.NextLine())
	{
		int groupId = pLoader.NextInt();
		int serverId = pLoader.NextInt();
		int port = pLoader.NextInt();
		std::string ip = pLoader.NextStr();
		std::string name = pLoader.NextStr();

		std::map<int, ServerData>::iterator ator = m_ServerData.find(serverId);
		if (ator == m_ServerData.end())
		{
			ServerData sd;
			sd.groupId = groupId;
			sd.serverId = serverId;
			sd.port = port;
			sd.ip = ip;
			sd.name = name;
			m_ServerData[serverId] = sd;
		}
	}
	return true;
}
