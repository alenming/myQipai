/*
* 通讯协议 主要定义主命令和包头
*/

#ifndef __LOGINPROTOCOL_H__
#define __LOGINPROTOCOL_H__

#define PASSWD_SIZE 32


//登录模块
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
	int userId;				      // 玩家id
	int headId;				      // 玩家头像
	int	userExp;			      // 玩家经验
	int gold;				      // 玩家金币
	int diamond;			      // 玩家钻石
	char name[16];
	unsigned char  cuserLv;		  // 玩家等级

};

#endif
