/*
* ͨѶЭ�� ��Ҫ����������Ͱ�ͷ
*/

#ifndef __LOGINPROTOCOL_H__
#define __LOGINPROTOCOL_H__

#define PASSWD_SIZE 32


//��¼ģ��
enum LOGIN_SUB_CMD
{
	CMD_C2S_BEIGN = 0,
	CMD_C2S_REGISTER,
	CMD_C2S_LOGIN,
	CMD_C2S_LOGINOUT,
	CMD_C2S_END,

	CMD_S2C_BGINE = 100,
	CMD_S2C_REGISTER,
	CMD_S2C_LOGIN,
	CMD_S2S_LOGINOUT,
	CMD_S2C_END,
};


struct REGISTER_DATA
{
	int  accountId;
	char passWord[16];
};

struct LOGIN_DATA
{
	int accountId;
};

#endif
