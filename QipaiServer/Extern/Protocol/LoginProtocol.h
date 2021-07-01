/*
* ͨѶЭ�� ��Ҫ����������Ͱ�ͷ
*/

#ifndef __LOGINPROTOCOL_H__
#define __LOGINPROTOCOL_H__

#define PASSWD_SIZE 32


//��¼ģ��
enum SUB_CMD_LOGIN
{
	CMD_C2S_BEIGN = 0,
	CMD_C2S_LOGIN,
	CMD_C2S_PERMISSION,
	CMD_C2S_LOGINOUT,
	CMD_C2S_PULLUSERDATA,//--------------
	CMD_C2S_END,

	CMD_S2C_BGINE = 100,
	CMD_S2C_LOGIN,
	CMD_S2C_PERMISSION,
	CMD_S2C_LOGINOUT,
	CMD_S2C_PULLUSERDATA,
	CMD_S2C_END,
};


struct REGISTER_DATA
{
	int  accountId;
	char passWord[16];
};

struct LOGIN_DATA
{
	char userName[16];
	char passWord[16];
};

//CMD_LOGIN_USERMODEL_SC
struct LoginUserModelInfo
{
	int userId;				      // ���id
	int headId;				      // ���ͷ��
	int	userExp;			      // ��Ҿ���
	int gold;				      // ��ҽ��
	int diamond;			      // �����ʯ
	char name[16];
	unsigned char  cuserLv;		  // ��ҵȼ�

};

#endif
