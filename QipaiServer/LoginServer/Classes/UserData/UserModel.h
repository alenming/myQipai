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
	USR_FD_USERLV,			    // 等级
	USR_FD_EXP,				    // 经验
	USR_FD_GOLD,			    // 金币
	USR_FD_DIAMOND,			    // 钻石
	USR_FD_CREATETIME,			// 创建时间
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
	// 刷新用户数据
	bool Refresh();
	// 新用户
	bool NewUser(int uid, std::string name, std::map<int, int> &info);
	// 获得用户名字
	std::string& GetName();
	// 获得用户基本信息
	std::map<int, int>& GetUserInfo();
private:

	int						m_nUid;
	IStorer *				m_pStorer;
	std::string				m_strUsrKey;
	std::string				m_strUserName;
	std::map<int, int>		m_mapUserInfo;
};

#endif //__USER_MODEL_H__
