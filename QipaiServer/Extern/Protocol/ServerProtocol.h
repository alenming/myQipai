/*
* ͨѶЭ�� ��Ҫ����������Ͱ�ͷ
*/

#ifndef __SERVERPROTOCOL_H__
#define __SERVERPROTOCOL_H__

//������֮��ͨѶ
enum SERVER_MAIN_CMD
{
	SERVER_MAIN = 100,
};
enum SERVER_SUB_CMD
{
	SERVER_SUB_OFFLINE = 1,
	SERVER_SUB_NEW_USER,
	SERVER_SUB_UPDATE_PER,
};
//�ͻ��˷��������.ֵ�����ĸ�������(.exe)�����ñ��ϵĶ�Ӧ
enum MAIN_CMD
{
	CMD_HEARTBEAT,
	CMD_LOGIN_SERVER,

};



#endif
