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
	int userId;				      // ���id
	int headId;				      // ���ͷ��
	int	userExp;			      // ��Ҿ���
	int gold;				      // ��ҽ��
	int diamond;			      // �����ʯ
	int towerCoin;				  // ����
	int pvpCoin;				  // pvp��
	int unionContrib;			  // ���ṱ��
	int flashcard10;			  // 10���鿨ȯ
	int flashcard;				  // �鿨ȯ

	int resetTimeStamp;			  // �ָ�ʱ���(������ҹ������, �����������, �����ָ�)
	unsigned short sbagCapacity;  // �ֱ�������
	unsigned char  cuserLv;		  // ��ҵȼ�
	unsigned char  cfreeHeroTimes; //��ѳ鿨����
	unsigned char changeNameFree; // 0Ϊ���,1Ϊ��Ҫ����
	unsigned short sbuyGoldTimes; // �����Ҵ���

	int payment;				  // ��ֵ���
	int monthCardStamp;           // �¿�����ʱ���
	int nTotalSignDay;			  //�ۼ�ǩ������
	int nMonthSignDay;			  //�����ۼ�ǩ������
	int nTotalSignFlag;			  //�ۼ�ǩ���Ĵ���

	int nFondFlag;				  //������
	unsigned char cDaySignFlag;	  //�Ƿ��Ѿ�ǩ����0-δǩ����1-�Ѿ�ǩ��
	unsigned char cFirstPayFlag;  //�׳���
	unsigned char cBDType;		  //�������� //�������� 1,3,5,7
	unsigned char cBDLv;		  //����ȼ�
	int nBDTimeStamp;			  //�������ʱ���
	char name[32];
};

#endif
