/*
* 通讯协议 主要定义主命令和包头
*/

#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

enum BAG_CMD
{
	CMD_C2S_BEIGN = 0,
	CMD_C2S_BAG,

	CMD_S2C_BGINE = 100,
	CMD_S2C_BAG,
};

struct BAG_DATA
{
	int accountId;
};

#endif
