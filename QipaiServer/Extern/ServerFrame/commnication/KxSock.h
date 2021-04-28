/*
 * ����SocketͨѶ����
 * �����˷����������ӳ٣����ͺͽ��սӿ�
 * ����Socket�����쳣
 */
#ifndef __KX_SOCKET__
#define __KX_SOCKET__

#include "core/KxCore.h"

enum KXSOCK_VERSION
{
	KXV_IPV4,							//ʹ��ipv4
	KXV_IPV6,							//ʹ��ipv6
};

enum KXSOCK_TYPE
{
    KXSOCK_UNKNOWN,
    KXSOCK_RAW,
    KXSOCK_TCP,
    KXSOCK_UDP
};

enum KXSOCK_CODE
{
    KXSOCK_ERRORUNKNOW = -9999,		    // δ֪��Socket����
    KXSOCK_ERRORFAILE,                  // Socket����ʧ��
    KXSOCK_ERRORTYPE,                   // Socket���ʹ���:��TCP��UDP����UDP����TCP����

	KXSOCK_SUCCESS		= 0				// �ɹ�����
};

typedef sockaddr_in kxSocketAddr;

class KxSock : public KxObject
{
public:
	KxSock();
	
	virtual ~KxSock();

    static bool initSock();

    static void uninitSock();
	
	// ��ʼ��Socket
	bool init(KXSOCK_TYPE type, KXSOCK_VERSION sv = KXSOCK_VERSION::KXV_IPV4);
	// ����SockFd��ʼ������Ҫ���ڳ�ʼ��accept���client
	bool init(KXCOMMID fd);
	
	// ������TCPSocket�����¿��Ե���
	int listen(int maxListenQueue);
	// ����ָ����IP�Ͷ˿�
	int connect(const char* addr, int port);
	// �󶨵�ָ����Ip�Ͷ˿�
	int bind(const char* addr, int port);
	// Accept����һ��Socket���Ӷ���
	KXCOMMID accept();
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
    inline KXCOMMID getSockFd()
    {
        return m_Sock;
    }

private:
	//��ʼ��Addr�ṹ�壬�����˲�ͬƽ̨��ʵ��
	void sockInitAddr(kxSocketAddr &name, int port, const char* ip = NULL);

private:
    int             m_SockType;
    KXCOMMID        m_Sock;
	KXSOCK_VERSION  m_SockVersion;
    kxSocketAddr    m_SockAddr;
    static bool     s_IsInit;
};

#endif
