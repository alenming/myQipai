#include "LuaRegiest.h"
#include "LuaTools.h"

#ifdef RunningInServer
#include "log/LogManager.h"
#include "LuaStack_.h"
#else
#include "CCLuaEngine.h"
#endif

int LogPrint(lua_State * luastate)
{
	int nargs = lua_gettop(luastate);

	std::string t;
	for (int i = 1; i <= nargs; i++)
	{
		if (lua_istable(luastate, i))
			t += "table";
		else if (lua_isnone(luastate, i))
			t += "none";
		else if (lua_isnil(luastate, i))
			t += "nil";
		else if (lua_isboolean(luastate, i))
		{
			if (lua_toboolean(luastate, i) != 0)
				t += "true";
			else
				t += "false";
		}
		else if (lua_isfunction(luastate, i))
			t += "function";
		else if (lua_islightuserdata(luastate, i))
			t += "lightuserdata";
		else if (lua_isthread(luastate, i))
			t += "thread";
		else
		{
			const char * str = lua_tostring(luastate, i);
			if (str)
				t += lua_tostring(luastate, i);
			else
				t += lua_typename(luastate, lua_type(luastate, i));
		}
		if (i != nargs)
			t += "  ";
	}
#ifdef KX_LOGDEBUG
	KX_LOGDEBUG("[LUA-print] %s", t.c_str());
#else
	printf("[LUA-print] %s", t.c_str());
#endif // KX_LOGDEBUG

	return 0;
}

int luaTest(lua_State * luastate)
{
	const char* ip = luaL_checkstring(luastate, 1);
	int port = luaL_checkint(luastate, 2);
	printf("return %s --%d, ", ip, port);
	return 0;
}

bool regiestLuaFunction()
{
#ifndef RunningInServer
	lua_State* luaState = cocos2d::LuaEngine::getInstance()->getLuaStack()->getLuaState();
#else
	lua_State* luaState = LuaStack_::getInstance()->getLuaState();
#endif
	lua_register(luaState, "print", LogPrint);
	lua_register(luaState, "luaTest", luaTest);
    return true;
}


