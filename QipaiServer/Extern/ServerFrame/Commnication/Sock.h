/*
 * ����SocketͨѶ����
 * �����˷����������ӳ٣����ͺͽ��սӿ�
 * ����Socket�����쳣
 */
#ifndef _SOCKET__
#define _SOCKET__

#include "Core.h"

enum SOCK_VERSION
{
	V_IPV4,							//ʹ��ipv4
	V_IPV6,							//ʹ��ipv6
};

enum SOCK_TYPE
{
    SOCK_UNKNOWN,
    _SOCK_RAW,
    SOCK_TCP,
    SOCK_UDP,
};

enum SOCK_CODE
{
    SOCK_ERRORUNKNOW = -9999,		    // δ֪��Socket����
    SOCK_ERRORFAILE,                  // Socket����ʧ��
    SOCK_ERRORTYPE,                   // Socket���ʹ���:��TCP��UDP����UDP����TCP����

	SOCK_SUCCESS		= 0				// �ɹ�����
};

typedef sockaddr_in kxSocketAddr;

class Sock : public _Object
{
public:
	Sock();
	
	virtual ~Sock();

    static bool initSock();

    static void uninitSock();
	
	// ��ʼ��Socket
	bool init(SOCK_TYPE type, SOCK_VERSION sv = SOCK_VERSION::V_IPV4);
	// ����SockFd��ʼ������Ҫ���ڳ�ʼ��accept���client
	bool init(COMMID fd);
	
	// ������TCPSocket�����¿��Ե���
	int listen(int maxListenQueue);
	// ����ָ����IP�Ͷ˿�
	int connect(const char* addr, int port);
	// �󶨵�ָ����Ip�Ͷ˿�
	int bind(const char* addr, int port);
	// Accept����һ��Socket���Ӷ���
	COMMID accept();
    // ��������
    int send(const char* buffer, int size);
    // ��������
    int recv(char* buffer, int size);	
	// �ر�Socket
	void close();

	// ���õ�ǰ��SocketAddr,���SocketAddr���ᱻ����UDP����
	void setSockAddr(kxSocketAddr &name);
	void setSockAddr(const char* ip, int port);

    // ��ȡ��ǰ��Socket��ַ,�����ַ��ÿ��UDP�����Լ�SetAddrʱ�ᱻ����
    inline const kxSocketAddr& getSockAddr() { return m_SockAddr; }

    // ���÷�����
	void setSockNonblock();
    // ����keepalive
    void setSockKeepAlive();
	// �����ӳ٣��Ƿ�ʹ��Ngale�㷨��ʱ���ͣ�
	void setSockNondelay();
    // ���ÿ����õ�ַ
	void setSockAddrReuse();

    // ��ѯ�Ƿ���������
    bool isSockBlockError();
    // ��ȡ������
    int getSockError();
    // ��ӡ������Ϣ
    void echoSockError(const char* msg = "");

    // ��ȡSockFD
    inline COMMID getSockFd()
    {
        return m_Sock;
    }

private:
	//��ʼ��Addr�ṹ�壬�����˲�ͬƽ̨��ʵ��
	void sockInitAddr(kxSocketAddr &name, int port, const char* ip = nullptr);

private:
    int             m_SockType;
    COMMID        m_Sock;
	SOCK_VERSION  m_SockVersion;
    kxSocketAddr    m_SockAddr;
    static bool     s_IsInit;
};

#endif
