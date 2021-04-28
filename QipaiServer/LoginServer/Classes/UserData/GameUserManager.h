#ifndef __GAMEUSER_MANAGER_H__
#define __GAMEUSER_MANAGER_H__
/*
	���ģ�͹�����
*/
#include <map>
#include "GameUser.h"
#include "KXServer.h"


//�ӳٴ���ṹ
struct SDelayDelData
{
	int nDelayTime;
	int nUid;
};



class CGameUserManager : public KxTimerObject
{
private:
	CGameUserManager();
	~CGameUserManager();

public:

	static CGameUserManager *getInstance();
	static void destroy();

public:

	bool init(KxTimerManager *pTimerManager);

	void addModelType(ModelType Type);

	void onTimer(const kxTimeVal& now);
	// ��������û�����createWhenNullΪtrueʱ����ȡ�������Զ������ݿ��г�ʼ����
	GameUser* getGameUser(int uid, bool createWhenNull = true);
	// ��ʼ���û�
	GameUser* initGameUser(int uid);
	GameUser *newGameUser(int uid, int accountId);
	//�����û���Ҫ���õ�����
	void reSetGameUserData(int uid, bool bLogin = false);
    // ˢ���û�����
    void updateGameUserData(GameUser* gameUsr, bool bLogin = false);
	// ����û�, ע���ʱ�����
	void addGameUser(int uid, GameUser* gameUsr);
	//����û��ڸ÷������Ƿ����
	bool checkUserIsExist(int uid);
	// �Ƴ��û�
	void removeGameUser(int uid);
	// ɾ���Ƴ��û�����
	void donotDeleteUser(int uid);


	// �������ģ������
	std::vector<ModelType>& getModelType() { return m_VectServerModel; }

	bool initUserData(GameUser* gameUsr);

	const std::map<int, GameUser *>& getGameUsers() { return m_GameUsers; }

private:

	//����ɾ���û�
	void RealremoveGameUser(int uid);

private:

	static CGameUserManager *							m_pInstance;
	std::map<int, GameUser *>							m_GameUsers;
	std::vector<ModelType>								m_VectServerModel;					//�����������������ģ��
	std::map<int, std::list<SDelayDelData>::iterator>	m_MapDelUserList;					//��Ҷ�Ӧ������������
	std::list<SDelayDelData>							m_DelUserList;						//�ӳ���������б�
	KxTimerManager *							m_TimeManager;
};

#endif //__GAMEUSER_MANAGER_H__
