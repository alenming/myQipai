#ifndef __USER_MODEL_H__
#define __USER_MODEL_H__

#include <string>
#include <map>
#include "IDBModel.h"

enum USERFIELD
{
	USR_FD_NONE,			    //
	USR_FD_USERNAME,			// name
	USR_FD_ACCOUNTID = 10,			// accountid
	USR_FD_USERLV,			    // �ȼ�
	USR_FD_EXP,				    // ����
	USR_FD_GOLD,			    // ���
	USR_FD_DIAMOND,			    // ��ʯ
	USR_FD_CREATETIME,			// ����ʱ��
	USR_FD_END
};

class IStorer;
class UserModel : public IDBModel
{
public:
	UserModel();
	~UserModel();

public:

	bool init(int uid);
	// ˢ���û�����
	bool Refresh();
	// ���û�
	bool NewUser(int uid, std::string name, std::map<int, int> &info);
	// ����û�����
	std::string& GetName();
	// ����û�������Ϣ
	std::map<int, int>& GetUserInfo();
private:

	int						m_nUid;
	IStorer *				m_pStorer;
	std::string				m_strUsrKey;
	std::string				m_strUserName;
	std::map<int, int>		m_mapUserInfo;
};

#endif //__USER_MODEL_H__
