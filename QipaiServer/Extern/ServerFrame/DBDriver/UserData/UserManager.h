#ifndef __USER_MANAGER_H__
#define __USER_MANAGER_H__
/*
	���ģ�͹�����
*/
#include <map>
#include <string>
#include "GameUser.h"
#include "TimerManager.h"

//�ӳٴ���ṹ
struct SDelayDelData
{
	int nDelayTime;
	int nUid;
};



class UserManager : public TimerObject
{
private:
	UserManager();
	~UserManager();

public:

	static UserManager *getInstance();
	static void destroy();

public:

	bool init(TimerManager *pTimerManager);

	void addModelType(ModelType Type);

	void onTimer(const TimeVal& now);

	// ��������û�����createWhenNullΪtrueʱ����ȡ�������Զ������ݿ��г�ʼ����
	GameUser* getGameUser(int uid, std::string userName, std::string passWord, bool createWhenNull = true);
	GameUser* getGameUser(int uid);
	GameUser* newGameUser(int uid, std::string userName, std::string passWord);
	GameUser* initGameUser(int uid, std::string userName, std::string passWord);

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

	const std::map<int, GameUser *>& getGameUsers() { return m_GameUsers; }

	//����ɾ���û�
	void RealremoveGameUser(int uid);

private:
	static UserManager *								m_pInstance;
	std::map<int, GameUser*>							m_GameUsers;
	std::vector<ModelType>								m_VectServerModel;					//�����������������ģ��
	std::map<int, std::list<SDelayDelData>::iterator>	m_MapDelUserList;					//��Ҷ�Ӧ������������
	std::list<SDelayDelData>							m_DelUserList;						//�ӳ���������б�
	TimerManager *										m_TimeManager;
};

#endif //__USER_MANAGER_H__
