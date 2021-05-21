///////////////////////////////////////////////////////
//服务器配置读取
//ServerConfig.h
//黄城
///////////////////////////////////////////////////////

#ifndef _SERVER_CONFIG_H__
#define _SERVER_CONFIG_H__

#include <map>
#include <string>

#define  SERVER_NAME_SESSION		"SessionServer"
#define  SERVER_NAME_LOGIN			"LoginServer"

struct ServerData
{
	int groupId;
	int serverId;
	int port;
	std::string ip;
	std::string name;
	ServerData()
	{
		memset(this, 0, sizeof(*this));
	}
}; 

class ServerConfig
{
public:
	ServerConfig();
	~ServerConfig();

	bool create();
	void clean();


	ServerData getServerDataById(int id)
	{
		return m_ServerData[id];
	}
	ServerData getServerDataByName(std::string name)
	{
		for (std::map<int, ServerData>::iterator iter = m_ServerData.begin(); iter != m_ServerData.end(); ++iter)
		{
			if (iter->second.name == name)
			{
				return iter->second;
			}
		}

		ServerData serverData = ServerData();
		serverData.port = 8888;
		serverData.ip = "127.0.0.1";
		return serverData;
	}

	std::map<int, ServerData>& getServerData()
	{
		return m_ServerData;
	}

protected:

	bool loadServerData();

private:
	std::map<int, ServerData>   m_ServerData;
};

#endif //_SERVER_CONFIG_H__
