/*
*   ��������ĳ�ʼ��, ����״̬����, �Լ�������ѯ
*
*/

#ifndef __GAMENETWORKNODE_H__
#define __GAMENETWORKNODE_H__

#include <functional>
#include "log/LogManager.h"
#include "Sock.h"
#include "TCPConnect.h"
enum EServerConnType
{
    SERVER_CONN_SESSION,      // session����������
    SERVER_CONN_CHAT,         // chat����������
};

struct ServerConn
{
    std::string                 ConnIP;         // ���ӵ�ip��ַ
    int                         Port;               // ���ӵĶ˿�
    int                         ConnectingTimes;    // ���������еĴ���������10��ǿ�ƹر�������
    KXSOCK_VERSION	SockVersion;        // ipv4/ipv6
    TCPConnect*   Connector;          // ����
    std::function<void(bool)>   ConnectCallback;

    ServerConn() :Port(0)
        , ConnectingTimes(0)
		, SockVersion(KXSOCK_VERSION::KXV_IPV6)
        , Connector(nullptr)
        , ConnectCallback(nullptr)
    {
    }
};

class BaseModule;
class CGameNetworkNode  : public Object
{
private:
    CGameNetworkNode(void);
    ~CGameNetworkNode(void);

public:

    static CGameNetworkNode *getInstance();
	static CGameNetworkNode* create();
    static void destroy();

    bool init();
    void onEnter();
    void onExit();
    void update(float dt);
    //void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4& parentTransform, uint32_t parentFlags);


    // ���ӵ�ָ��ip, port
	bool connectToServer(const char *ip, int port, EServerConnType connType = SERVER_CONN_SESSION, KXSOCK_VERSION version = KXSOCK_VERSION::KXV_IPV4, 
		std::function<void(bool)> callBack = nullptr);
    // ��������
    int sendData(char* buffer, unsigned int len, EServerConnType connType = SERVER_CONN_SESSION);
    // �ṩ�����ӿ�, ����֮ǰѡ��ķ�����ip��ַ
    bool reconnectToServer(EServerConnType connType, std::function<void(bool)> callBack = nullptr);
    // �Ƿ���������, Ĭ������
    void setIsHeartbeat(bool bEnabled);
    // ��������ʱ��
    void setHeartbeatTime(float fHbTime);
    // �Ͽ�����
    void closeConnect();
    // �������
	TCPConnect *getConnector(EServerConnType connType = SERVER_CONN_SESSION);
    // ��ȡ������Ϣ
    ServerConn* getServerConn(EServerConnType connType = SERVER_CONN_SESSION);
	// �Ƿ��û������ر�
    bool isUserClose(){ return m_bUserClose; }

	ICommPoller* getPoller()
	{
		return m_pPoller;
	}

private:
    // ����, �������״̬
    void heartbeatScheduler(float dt);
    // ע������ͨ����Ϣ, ��������������ʱ��
    void registeNetworkCommunicate(void *data);

private:
    bool                                m_bUserClose;
    bool                                m_bEnabledHb;                   // �Ƿ���������
    float                               m_fMaxHbTime;                   // ���û���ݽ�������һ��������
    float                               m_fTickNet;                     // �������ϴν������ݼ��ʱ��
    static CGameNetworkNode*            m_pInstance;                    // 
    ICommPoller*            m_pPoller;                      // ��ѯ��
    BaseModule*                        m_pGameModule;                  // �����߼�ģ��ʵ��

    std::map<int, ServerConn>           m_mapServerConns;               // ����������
};

#endif 
