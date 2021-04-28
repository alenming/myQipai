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
