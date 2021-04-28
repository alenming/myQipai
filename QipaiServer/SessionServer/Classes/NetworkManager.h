/*
 *  
 *  �������������ṹ, ͨ��NetworkManager���Զ�ָ��������������Ϣ
 *  
 */
#ifndef __NETWORKMANAGER_H__
#define __NETWORKMANAGER_H__

#include <map>
#include<vector>

#include "KXServer.h"
#include "SessionClient.h"
#include "KXServer.h"

class NetWorkManager : public KxTimerObject
{
private:
    NetWorkManager(void);
    virtual ~NetWorkManager(void);

public:
    static NetWorkManager* getInstance();
    static void destroy();

    //��������˷�������ID
	bool addServer(int serverId, IKxComm* obj);
	//��ӱ��ݺ�˷�������ID
	bool addBakServer(int nGroupID, IKxComm* obj);
	//���ĳ�����ݷ�������ID
	bool clearBakServer(int nGroupID);
	//ͨ�����ͻ�ȡ������,Key��Ӧ��Valueֵ��ȡģ
	IKxComm* getServer(int serverId, int nValue = 0);
    //��ȡĳ����������
    std::vector<IKxComm*>* getGroupServer(int nGroupID);
	//��ȡĳ���������鱸��
	std::vector<IKxComm*>* getBakGroupServer(int nGroupID);
    //��ȡ���з�����
	std::map<int, IKxComm*>& getAllServer() { return m_ServerList; }

	//��������ӵ�����֤�����б���
	bool addGuest(unsigned int guestId, IKxComm* obj);
	//��ȡ����֤���ӿͻ���
    IKxComm* getGuest(unsigned int guestId);
    bool removeGuest(unsigned int guestId);

	//������תΪ����OK�Ŀͻ���
    bool changeGuestToUser(SessionClient* guest, unsigned int userId);
	//��ȡ�Ѿ���֤���Ŀͻ���
    IKxComm* getUser(unsigned int userId);
    bool removeUser(unsigned int userId);

    //���ݹ㲥���������
    bool broadCastData(char *pszContext, int nLen);

	bool sendDataToClient(SessionClient* pClient, int nMainCmd, int nSubCmd, char *pszContext, int nLen);

    //��ȡ��ǰ���������
    unsigned int getCurClientNum();
	//���̬ΨһID
	unsigned int genGuestId();

	void closeAllServer();

	void onTimer(const kxTimeVal& now);

	void setChangeFlag(bool bChange) { m_bChanging = bChange; }
	bool getChangeFlag() { return m_bChanging; }

private:
    static NetWorkManager* m_Instance;

    unsigned int m_GuestId;													// δ��֤�ͻ���ΨһID

    std::map<int, std::vector<IKxComm*> >	m_ServerMapByGroup;		// ��������ӹ���
	std::map<int, std::vector<IKxComm*> >	m_BakServerMapByGroup;	// �������ӹ���

    std::map<unsigned int, IKxComm*> m_GuestMap;		        // δ��֤�Ŀͻ������ӹ���
    std::map<unsigned int, IKxComm*> m_UserMap;       		// �Ѿ���֤���Ŀͻ������ӹ���
	bool		m_bChanging;											//�Ƿ����л���

	std::map<int, IKxComm* >	m_ServerList;		// ��������ӹ���

};

#endif
