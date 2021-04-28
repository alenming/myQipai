/*
 * SessionServer 主要负责维护和客户端之间的连接
 * 1.客户端连接断开时推送到游戏服务器
 * 2.管理已登陆用户+连接游客，对用户合法性进行验证
 * 3.接收客户端消息转发到指定的后端
 * 4.接收到后端消息转发给指定的前端
 * 5.收到后端指令强制关闭指定的前端
 * 6.UserId 对应 CommunicationId 的哈希
 */
#include "KXServer.h"
#include "SessionServer.h"



int main(int argc, char ** argv) 
{
    SessionServer* server = SessionServer::getInstance();
	if (server->initServerInfo())
	{
		server->startServer();
	}
    SessionServer::destroy();

    return 0;
}
