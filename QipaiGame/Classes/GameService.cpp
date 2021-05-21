#include "GameService.h"
#include "GameNetworkNode.h"
#include "server/Head.h"
#include "helper/BufferData.h"

#include "protocol/MainProtocol.h"
#include "protocol/LoginProtocol.h"

using namespace std;

// 发送客户端消息
void GameService::sendMessgToServer(int mainCmd, int subCmd)
{
	switch (mainCmd)
	{
	case LOGIN_CMD::CMD_C2S_LOGIN:
		CMD_C2S_LOGIN();
		break;
		break;
	default:
		break;
	}
}
// 处理服务端消息
void GameService::processMessgFromServer(int mainCmd, int subCmd, char* buffer)
{
	switch (subCmd)
	{
	case LOGIN_CMD::CMD_S2C_LOGIN:
		CMD_S2C_LOGIN(subCmd, buffer);
		break;
		break;
	default:
		break;
	}
}


void GameService::CMD_C2S_LOGIN()
{
	BufferData* buffer = newBufferData(MAIN_CMD::CMD_LOGIN_SERVER, LOGIN_CMD::CMD_C2S_LOGIN);

	LOGIN_DATA da = LOGIN_DATA();

	da.userId = 12345678;
	char passw[16] = "wangxiangming" ;
	memcpy(da.passWord, passw, sizeof(da.passWord));
	buffer->writeData(da);

	//重新写入数据长度
	char* bu = buffer->getBuffer();
	Head* head = reinterpret_cast<Head*>(bu);
	head->length = buffer->getDataLength();
	head->uid = 0;//服务器用ID
	bool isSuccessful = false;
	if (GameNetworkNode::getInstance()->sendData(buffer->getBuffer(), buffer->getDataLength()) > 0)
	{
		KX_LOGDEBUG("CGameNetworkNode::sendData to server successful!");
		isSuccessful = true;
	}
	else
	{
		KX_LOGDEBUG("CGameNetworkNode::sendData to server failed!");
	}
	deleteBufferData(buffer);
}

void GameService::CMD_C2S_REGISTER()
{

}

void GameService::CMD_S2C_LOGIN(int subCmd, char* buffer)
{
	KX_LOGDEBUG("CMD_S2C_LOGIN !");
}

void GameService::CMD_S2C_REGISTER(int subCmd, char* buffer)
{
	KX_LOGDEBUG("CMD_S2C_REGISTER !");
}




