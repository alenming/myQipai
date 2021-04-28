#ifndef __LUA_STACK_H_
#define __LUA_STACK_H_

#include "CommonHead.h"

#ifdef RunningInServer
#include "Common/Data_.h"
extern "C"
{
#include "luajit/lua.h"
#include "luajit/lauxlib.h"
#include "luajit/lualib.h"
}


#if _MSC_VER > 1800
#pragma comment(lib,"lua51-2015.lib")
#else
#pragma comment(lib,"lua51.lib")
#endif


class LuaStack_
{
public:
	LuaStack_();
	virtual ~LuaStack_();
	static LuaStack_* getInstance();
	static void destroy();
private:
	int luaLoadBuffer(lua_State *L, const char *chunk, int chunkSize, const char *chunkName);
public:
	lua_State* getLuaState();
	int executeScriptFile(const char* filename);
	int executeFunction(int numArgs);
	int executeGlobalFunction(const char* functionName);
	int executeString(const char *codes);
	void addSearchPath(const char* path);
#ifdef RunningInServer
	Data_ getDataFromFile(const std::string &path);
	std::string getStringFromFile(const std::string &path);
#endif


protected:
	lua_State *_state;
	static LuaStack_* m_Instance;
};

#endif


#endif // __LUA_STACK_H_
