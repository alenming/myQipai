/*
 *  
 *  �������������ṹ, ͨ��NetworkManager���Զ�ָ��������������Ϣ
 *  
 */
#ifndef __NETWORKMANAGER_H__
#define __NETWORKMANAGER_H__

#include <map>
#include<vector>

#include "TimerManager.h"
#include "SessionClient.h"

class NetWorkManager : public TimerObject
{
private:
    NetWorkManager(void);
    virtual ~NetWorkManager(void);

public:
    static NetWorkManager* getInstance();
    static void destroy();

    //��������˷�������ID
	bool addServer(int serverId, IComm* obj);
	//��ӱ��ݺ�˷�������ID
	bool addBakServer(int nGroupID, IComm* obj);
	//���ĳ�����ݷ�������ID
	bool clearBakServer(int nGroupID);
	//ͨ�����ͻ�ȡ������,Key��Ӧ��Valueֵ��ȡģ
	IComm* getServer(int serverId, int nValue = 0);
    //��ȡĳ����������
	std::vector<IComm*>* getGroupServer(int nGroupID);
	//��ȡĳ���������鱸��
	std::vector<IComm*>* getBakGroupServer(int nGroupID);
    //��ȡ���з�����
	std::map<int, IComm*>& getAllServer() { return m_ServerList; }

	//��������ӵ�����֤�����б���
	bool addGuest(unsigned int guestId, IComm* obj);
	//��ȡ����֤���ӿͻ���
	IComm* getGuest(unsigned int guestId);
	bool removeGuest(unsigned int guestId);
	//������תΪ����OK�Ŀͻ���
	bool changeGuestToUser(SessionClient* guest, unsigned int userId);

	bool addUser(int guestId, IComm* obj);
	IComm* getUser(int guestId);
	bool removeUser(int guestId);

    //���ݹ㲥���������
    bool broadCastData(char *pszContext, int nLen);

	bool sendDataToClient(SessionClient* pClient, int nMainCmd, int nSubCmd, char *pszContext, int nLen);

    //��ȡ��ǰ���������
    unsigned int getCurClientNum();
	//���̬ΨһID
	unsigned int genGuestId();

	void closeAllServer();

	void onTimer(const TimeVal& now);

	void setChangeFlag(bool bChange) { m_bChanging = bChange; }
	bool getChangeFlag() { return m_bChanging; }

private:
    static NetWorkManager* m_Instance;

	unsigned int m_GuestId;											// δ��֤�ͻ���ΨһID
	bool		m_bChanging;										//�Ƿ����л���
	std::map<int, std::vector<IComm*> >	m_ServerMapByGroup;		// ��������ӹ���
	std::map<int, std::vector<IComm*> >	m_BakServerMapByGroup;	// �������ӹ���
	std::map<unsigned int, IComm*> m_UserMap;       				// �Ѿ���֤���Ŀͻ������ӹ���
	std::map<unsigned int, IComm*> m_GuestMap;					// δ��֤�Ŀͻ������ӹ���
	std::map<int, IComm* >	m_ServerList;						// ��������ӹ���
};

#endif
