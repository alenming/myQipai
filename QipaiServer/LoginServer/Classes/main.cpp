/*
 * SessionServer ��Ҫ����ά���Ϳͻ���֮�������
 * 1.�ͻ������ӶϿ�ʱ���͵���Ϸ������
 * 2.�����ѵ�½�û�+�����οͣ����û��Ϸ��Խ�����֤
 * 3.���տͻ�����Ϣת����ָ���ĺ��
 * 4.���յ������Ϣת����ָ����ǰ��
 * 5.�յ����ָ��ǿ�ƹر�ָ����ǰ��
 * 6.UserId ��Ӧ CommunicationId �Ĺ�ϣ

 */
#include "KXServer.h"
#include "LoginServer.h"


int main(int argc, char ** argv) 
{
	LoginServer* server = LoginServer::getInstance();
	if (server->initServerInfo())
	{
		server->startServer();
	}
	LoginServer::destroy();

    return 0;
}