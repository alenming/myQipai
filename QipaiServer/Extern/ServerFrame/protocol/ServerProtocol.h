/*
* ͨѶЭ�� ��Ҫ����������Ͱ�ͷ
*/

#ifndef __SERVERPROTOCOL_H__
#define __SERVERPROTOCOL_H__
//�û�Ȩ��
enum USER_PERMISSION
{
	PERMISSION_GUES,
	PERMISSION_USER,
};

//������֮��ͨѶ
enum SERVER_MAIN_CMD
{
	SERVER_MAIN = 100,
};
enum SERVER_SUB_CMD
{
	SERVER_SUB_OFFLINE = 1,
	SERVER_SUB_NEW_USER,
};
//�ͻ��˷��������.ֵ�����ĸ�������(.exe)�����ñ��ϵĶ�Ӧ
enum CMD_MAIN
{
	CMD_HEARTBEAT,
	CMD_LOGIN_SERVER,

};



#endif
