/*
*   Core �����������ļ�
*   ��ƽ̨Ԥ����
*/
#ifndef __CORE_H__
#define __CORE_H__

#define KX_PLATFORM_UNKNOWN 0  
#define KX_PLATFORM_WIN32 1
#define KX_PLATFORM_LINUX 2
#define KX_PLATFORM_MAC 3

#define KX_TARGET_PLATFORM KX_PLATFORM_UNKNOWN

// mac
#if defined(__APPLE__)
#undef  KX_TARGET_PLATFORM
#define KX_TARGET_PLATFORM KX_PLATFORM_MAC
#endif

// win32
#if defined(_WIN32)
#undef  KX_TARGET_PLATFORM
#define KX_TARGET_PLATFORM         KX_PLATFORM_WIN32
#endif

// linux
#if defined(linux)
#undef  KX_TARGET_PLATFORM
#define KX_TARGET_PLATFORM         KX_PLATFORM_LINUX
#endif

#ifndef FD_SETSIZE
#define FD_SETSIZE 1024  //�޸ķ�����ͬʱ���ӵ����������
#endif

#include "Object.h"
#include <assert.h>
#include <algorithm>

#define RunningInServer

//����ƽ̨��ص�ʱ�䣬Socket����
#if(KX_TARGET_PLATFORM == KX_PLATFORM_WIN32)

#include <WinSock2.h>
#include <WinSock.h>
#include <Windows.h>
#include <time.h>

// ͨѶID
typedef SOCKET COMMID;
typedef int kxSockLen;

// ��д�����Ƿ������
#define SOCKERR_RW_RETRIABLE(e)			\
	((e) == WSAEWOULDBLOCK ||			\
	    (e) == WSAEINTR)

// connect�����Ƿ������
#define SOCKERR_CONNECT_RETRIABLE(e)	\
	((e) == WSAEWOULDBLOCK ||			\
	    (e) == WSAEINTR ||				\
	    (e) == WSAEINPROGRESS ||		\
	    (e) == WSAEINVAL)

// accept�����Ƿ������
#define SOCKERR_ACCEPT_RETRIABLE(e)		\
	SOCKERR_RW_RETRIABLE(e)

// connect�����Ƿ񱻾ܾ�
#define SOCKERR_CONNECT_REFUSED(e)		\
	((e) == WSAECONNREFUSED)

#ifndef snprintf
#define snprintf  sprintf_s
#endif // snprintf

#define stlocaltime(pTime,ptm) localtime_s(ptm,pTime)
#define strncpys strncpy_s
#define sstrdup _strdup
#define INVALID_COMMID (INVALID_SOCKET)

struct timezone
{
	int tz_minuteswest;
	int tz_dsttime;
};

int gettimeofday(struct timeval * val, struct timezone * zone);

#pragma comment(lib, "ws2_32.lib")

#else

#define INVALID_COMMID (-1)

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/time.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netinet/tcp.h>
#include <signal.h>
#include <stdarg.h>


// ��д�����Ƿ������
#define SOCKERR_RW_RETRIABLE(e)			\
	((e) == EINTR || (e) == EAGAIN ||	\
	(e) == EWOULDBLOCK)

// connect�����Ƿ������
#define SOCKERR_CONNECT_RETRIABLE(e)	\
	((e) == EINTR || (e) == EINPROGRESS)

// accept�����Ƿ������
#define SOCKERR_ACCEPT_RETRIABLE(e)		\
	((e) == EINTR || (e) == EAGAIN ||	\
	(e) == ECONNABORTED)

// connect�����Ƿ񱻾ܾ�
#define SOCKERR_CONNECT_REFUSED(e)		\
	((e) == ECONNREFUSED)

typedef int COMMID;
typedef socklen_t kxSockLen;
#define strncpys strncpy
#define sstrdup strdup
#define vsnprintf_s vsnprintf
#define stlocaltime(pTime,ptm) localtime_r(pTime,ptm)
#endif

#endif
