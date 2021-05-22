/*
* 通讯协议 主要定义主命令和包头
*/

#ifndef __LOGINPROTOCOL_H__
#define __LOGINPROTOCOL_H__

#define PASSWD_SIZE 32


//登录模块
enum LOGIN_SUB_CMD
{
	CMD_C2S_BEIGN = 0,
	CMD_C2S_LOGIN,
	CMD_C2S_PERMISSION,
	CMD_C2S_LOGINOUT,
	CMD_C2S_END,

	CMD_S2C_BGINE = 100,
	CMD_S2C_LOGIN,
	CMD_S2C_PERMISSION,
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
	int towerCoin;				  // 塔币
	int pvpCoin;				  // pvp币
	int unionContrib;			  // 公会贡献
	int flashcard10;			  // 10连抽卡券
	int flashcard;				  // 抽卡券

	int resetTimeStamp;			  // 恢复时间戳(包括金币购买次数, 体力购买次数, 体力恢复)
	unsigned short sbagCapacity;  // 现背包容量
	unsigned char  cuserLv;		  // 玩家等级
	unsigned char  cfreeHeroTimes; //免费抽卡次数
	unsigned char changeNameFree; // 0为免费,1为需要费用
	unsigned short sbuyGoldTimes; // 购买金币次数

	int payment;				  // 充值额度
	int monthCardStamp;           // 月卡过期时间戳
	int nTotalSignDay;			  //累计签到天数
	int nMonthSignDay;			  //当月累计签到天数
	int nTotalSignFlag;			  //累计签到的次数

	int nFondFlag;				  //基金标记
	unsigned char cDaySignFlag;	  //是否已经签到，0-未签到，1-已经签到
	unsigned char cFirstPayFlag;  //首冲标记
	unsigned char cBDType;		  //蓝钻类型 //蓝钻类型 1,3,5,7
	unsigned char cBDLv;		  //蓝钻等级
	int nBDTimeStamp;			  //蓝钻结束时间点
	char name[32];
};

#endif
