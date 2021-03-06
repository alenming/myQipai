#ifndef __USER_MODEL_H__
#define __USER_MODEL_H__

#include <string>
#include <map>
#include "IDBModel.h"
#include "IStorer.h"


enum USERFIELD
{
	USR_FD_NONE,			    //
	USR_FD_USERNAME,		    // 名称
	USR_FD_PASSWROD,			// 密码
	USR_FD_USERID = 10,         // id
	USR_FD_PERMISSION,			// 权限
	USR_FD_USERLV,			    // 等级
	USR_FD_EXP,				    // 经验
	USR_FD_GOLD,			    // 金币
	USR_FD_DIAMOID,				// 钻石


	USR_FD_CREATETIME,		    // 角色创建时间
	USR_FD_LOGINTIME,		    // 角色登陆时间
	USR_FD_LOGINOUTTIME,	    // 角色登出时间


	USR_FD_END
};

class UserModel : public IDBModel
{
public:
	UserModel();
	~UserModel();

public:

	bool init(int uId);
	// 刷新用户数据
	bool Refresh();
	// 新用户
	bool writeNewUserData(int uId, std::string name,std::string passWord, std::map<int, int> &info);

	bool updateData(int feild, int value, bool isWrite = false);

	bool updateData();
	// 获得用户名字
	std::string& GetName();
	// 获得用户基本信息
	std::map<int, int>& GetUserInfo();
	// 获得用户指定信息
	bool GetUserFieldVal(int field, int &value, bool bNew = false);
	// 批量获得用户数据接口
	bool GetUserFieldVal(std::map<int, int> &kvs, bool bNew = false);
	// 修改用户指定信息
	bool SetUserFieldVal(int field, int value);
	// 增加某些属性的值 注意不要乱用, 比如头像, 名字等等
	bool AddUserFieldVal(int field, int value);
	// 递增某个指定值, 返回递增之后的值
	int IncreaseFieldVal(int field, int increase = 1);
	// 批量处理
	bool SetUserFieldVal(std::map<int, int> &kvs);
	// 修改用户名
	bool ModUserName(std::string name);
	// 修改用户密码
	bool ModUserPassWord(std::string passWrod);
	// 修改用户名(不存数据库)
	void AlterUserName(std::string name) { m_strUserName = name; }
	// 删除玩家数据
	bool DeleteUser();
	// 设置用户基本信息
	bool SetUserInfo(std::map<int, int> &userInfo);

protected:
	// 修改用户信息(不存数据库)
	bool AlterUserFieldVal(int field, int value);
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
