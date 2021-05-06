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
	USR_FD_USERID = 10,         // id
	USR_FD_USERLV,			    // �ȼ�
	USR_FD_EXP,				    // ����
	USR_FD_GOLD,			    // ���
	USR_FD_DIAMOND,			    // ��ʯ
	USR_FD_ENERGY,			    // ����(1.5�汾ȥ��)
	USR_FD_TOWERCOIN,		    // ����
	USR_FD_PVPCOIN,			    // PVP��
	USR_FD_UNIONCONTRIB,	    // ���ṱ��
	USR_FD_FLASHCARD,		    // ��ȯ
	USR_FD_FLASHCARD10,			// ʮ���鿨��

	USR_FD_HEADICON,            // ͷ��
	USR_FD_HEROREF,			    // Ӣ��id����
	USR_FD_EQUIPREF,		    // װ��id����
	USR_FD_MAILREF,             // �ʼ�id����
	USR_FD_BAGCAPACITY,		    // �������Ӹ���
	USR_FD_HEROCAPCITY,		    // Ӣ�ۿ�������
	USR_FD_FREEHEROTIMES,	    // ���Ӣ�۳�ȡ����

	USR_FD_EXTRAHEROTIMES,	    // ��ȡӢ�۴���
	USR_FD_EXTRAHERTOTALTIMES,  // ��ȡӢ���ܴ���
	USR_FD_BUYGOLDTIMES,        // �����Ҵ���
	USR_FD_BUYENERGYTIMES,      // ������������
	USR_FD_ACCUMULATEGOLD,      // �ۼƻ�ý��

	USR_FD_RESETTIMESTAMP,      // �ָ�ʱ���(������ҹ������, �����������, �����ָ�)
	USR_FD_FREERENAME,          // ������(0Ϊ���,1��Ҫ����)

	USR_FD_LOGINTIME,		    // ��ɫ��½ʱ��
	USR_FD_LOGINOUTTIME,	    // ��ɫ�ǳ�ʱ��
	USR_FD_CREATETIME,		    // ��ɫ����ʱ��
	USR_FD_LOGIN_DAY,			// �ۼƵ�½����
	USR_FC_CONLOGIN_DAY,		// ������½����

	USR_FD_PAYMENT,				//��ֵ����
	USR_FD_MONTHCARDSTAMP,      // �¿�����ʱ��

	USR_FD_BANTIME,			    //��ɫ��ͣ��ֹʱ���
	USR_FD_SIGNDAY,				//�ۼ�ǩ������
	USR_FD_MONTHDAY,			//�����ۼ�ǩ������
	USR_FD_DAYFLAG,				//����ǩ����ʶ
	USR_FD_INCSIGNFLAG,			//�ۼ�ǩ��������ʶ
	USR_FD_PAYTIMES,			//��ֵ����

	//�ۼƳ�ֵ��ȡ��ʶ
	USR_FD_FIRSTPAYFLAG,		//�׳���ȡ��ʶ
	//����
	USR_FD_FUND_STARTTIME,		//����ʼ���
	USR_FD_FUND_GETTIME,		//������ȡ���

	USR_FD_BILLNO,              // ��ǰ����֧������ˮ�� - Ĭ��Ϊ0���
	USR_FD_QQVIPLV,				//����ȼ�
	USR_FD_QQVIPTIMESTAMP,		//�������ʱ���
	USR_FD_QQVIPTYPE,			//�������� 1,3,5,7
	USR_FD_END
};

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
	// �޸��û���(�������ݿ�)
	void AlterUserName(std::string name) { m_strUserName = name; }
	// �޸��û���Ϣ(�������ݿ�)
	bool AlterUserFieldVal(int field, int value);
	// ɾ���������
	bool DeleteUser();
	// �����û�������Ϣ
	bool SetUserInfo(std::map<int, int> &userInfo);
	int GetUserID(){ return m_nUid; }

protected:

	bool GetRealDataFromDB(int field, int &value);

private:

	int						m_nUid;
	DBRule *				m_pStorageDB;
	std::string				m_strUsrKey;
	std::string				m_strUserName;
	std::map<int, int>		m_mapUserInfo;
};

#endif //__USER_MODEL_H__
