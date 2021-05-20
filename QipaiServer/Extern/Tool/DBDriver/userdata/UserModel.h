#ifndef __USER_MODEL_H__
#define __USER_MODEL_H__

#include <string>
#include <map>
#include "IDBModel.h"
#include "DBDriver/IStorer.h"


enum USERFIELD
{
	USR_FD_NONE,			    //
	USR_FD_USERNAME,		    // ����
	USR_FD_PASSWROD,			// ����
	USR_FD_USERID = 10,         // id
	USR_FD_PERMISSION,			//Ȩ��
	USR_FD_USERLV,			    // �ȼ�
	USR_FD_EXP,				    // ����
	USR_FD_GOLD,			    // ���
	USR_FD_DIAMOID,				// ��ʯ

	USR_FD_LOGINTIME,		    // ��ɫ��½ʱ��
	USR_FD_LOGINOUTTIME,	    // ��ɫ�ǳ�ʱ��
	USR_FD_CREATETIME,		    // ��ɫ����ʱ��

	USR_FD_END
};

class UserModel : public IDBModel
{
public:
	UserModel();
	~UserModel();

public:

	bool init(int accoundId);
	// ˢ���û�����
	bool Refresh();
	// ���û�
	bool NewUser(int accoundId, std::string name,std::string passWord, std::map<int, int> &info);
	// ����û�����
	std::string& GetName();
	// ����û�������Ϣ
	std::map<int, int>& GetUserInfo();
	// ����û�ָ����Ϣ
	bool GetUserFieldVal(int field, int &value, bool bNew = false);
	// ��������û����ݽӿ�
	bool GetUserFieldVal(std::map<int, int> &kvs, bool bNew = false);
	// �޸��û�ָ����Ϣ
	bool SetUserFieldVal(int field, int value);
	// ����ĳЩ���Ե�ֵ ע�ⲻҪ����, ����ͷ��, ���ֵȵ�
	bool AddUserFieldVal(int field, int value);
	// ����ĳ��ָ��ֵ, ���ص���֮���ֵ
	int IncreaseFieldVal(int field, int increase = 1);
	// ��������
	bool SetUserFieldVal(std::map<int, int> &kvs);
	// �޸��û���
	bool ModUserName(std::string name);
	// �޸��û�����
	bool ModUserPassWord(std::string passWrod);
	// �޸��û���(�������ݿ�)
	void AlterUserName(std::string name) { m_strUserName = name; }


	// �޸��û���Ϣ(�������ݿ�)
	bool AlterUserFieldVal(int field, int value);
	// ɾ���������
	bool DeleteUser();
	// �����û�������Ϣ
	bool SetUserInfo(std::map<int, int> &userInfo);
	int GetUserID(){ return m_uId; }

protected:

	bool GetRealDataFromDB(int field, int &value);

private:

	int						m_uId;
	DBRule *				m_pStorageDB;
	std::string				m_strUsrKey;
	std::string				m_strUserName;
	std::string				m_strUserPassWord;
	std::map<int, int>		m_mapUserInfo;
};

#endif //__USER_MODEL_H__
