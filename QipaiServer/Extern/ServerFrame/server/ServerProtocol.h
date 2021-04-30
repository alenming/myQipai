/*
* 通讯协议 主要定义主命令和包头
*/

#ifndef __SERVERPROTOCOL_H__
#define __SERVERPROTOCOL_H__

//服务器之间通讯
enum SERVER_MAIN_CMD
{
	SERVER_MAIN,
};
enum SERVER_SUB_CMD
{
	SERVER_SUB_OFFLINE,
};
//客户端发给服务端.值代表哪个服务器(.exe)与配置表上的对应
enum CMD_MAIN
{
	CMD_HEARTBEAT,
	CMD_LOGIN_SERVER,



};



#endif
