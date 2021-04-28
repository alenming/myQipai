#ifndef _LUA_TOOLS_
#define _LUA_TOOLS_


#include "CommonHead.h"

#ifdef RunningInServer
#include "LuaStack_.h"
#else
#include "cocos2d.h"
#include "CCLuaEngine.h"
#endif

#ifdef RunningInServer
extern "C"
{
#include "luajit/lua.h"
#include "luajit/lauxlib.h"
#include "luajit/lualib.h"
}
#endif

namespace LuaTools
{
    template<typename T>
    void pushMapKeys(std::map<int, T>& m)
    {
		
#ifdef RunningInServer
		lua_State* luaState = LuaStack_::getInstance()->getLuaState();
#else
		lua_State* luaState = cocos2d::LuaEngine::getInstance()->getLuaStack();
#endif
        lua_newtable(luaState);
        // table ���±����1��ʼ
        int index = 1;
        for (auto& item : m)
        {
            lua_pushinteger(luaState, item.first);
			lua_rawseti(luaState, -2, index++);
        }
    }

    template<typename T>
    T* checkClass(lua_State* l, int index, const char* name)
    {
        T **p = reinterpret_cast<T**>(lua_touserdata(l, index));
        if (p != NULL)
        {
            if (lua_getmetatable(l, index))
            {
                luaL_getmetatable(l, name);
                if (!lua_rawequal(l, -1, -2))
                    *p = NULL;
                lua_pop(l, 2);
            }
		}
		else
		{
			return NULL;
		}
        return *p;
    }

    // pushһ�������
    void pushClass(lua_State* l, void* p, const char* className);

	void pushMapIntInt(std::map<int, int>& m, lua_State* luaState);
	void pushMapIntIntToTableField(const std::map<int, int>& m, lua_State* luaState, const char* tableName);
	void pushMapIntStrToTableField(const std::map<int, std::string>& m, lua_State* luaState, const char* tableName);

	//��vector<int>��������luaջ��
	void pushVecFloatToArray(const std::vector<float>& v, lua_State* luaState);
	//��vector<int>��������luaջ��
	void pushVecIntToArray(const std::vector<int>& v, lua_State* luaState);
    void pushVecStringToArray(const std::vector<std::string>& v, lua_State* luaState);

	//��vector<int>��������luaջ��, ������table��
	void pushVecIntToTableField(const std::vector<int>& v, lua_State* luaState, const char* tableName);
	void pushVecFloatToTableField(const std::vector<float>& v, lua_State* luaState, const char* tableName);
    void pushVecStringToTableField(const std::vector<std::string>& v, lua_State* luaState, const char* tableName);

	void pushSetIntToArray(const std::set<int>& v, lua_State* luaState);
	void pushSetIntToTableField(const std::set<int>& v, lua_State* luaState, const char* tableName);
    
    //��lua talbe��ȡ��key��ֵ
    void getStructBoolValueByKey(lua_State* luaState, const char* key, bool& value);
    void getStructIntValueByKey(lua_State* luaState, const char* key, int& value);
    void getStructStringValueByKey(lua_State* luaState, const char* key, char* value, int len);
    void getStructStringValueByKey(lua_State* luaState, const char* key, std::string& value);

	///////////////////////////����������(����, ����, �ַ���)����luaջ��///////////////////////////////////////////////
	void pushBaseKeyValue(lua_State* luaState, const int& value, const char* key);
	void pushBaseKeyValue(lua_State* luaState, const float& value, const char* key);
	void pushBaseKeyValue(lua_State* luaState, const double& value, const char* key);
	void pushBaseKeyValue(lua_State* luaState, const bool& value, const char* key);
	void pushBaseKeyValue(lua_State* luaState, const char* value, const char* key);
	void pushBaseKeyValue(lua_State* luaState, const std::string& value, const char* key);
}



#endif
