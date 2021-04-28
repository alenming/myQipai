#include "LuaStack_.h"


#ifdef RunningInServer

LuaStack_* LuaStack_::m_Instance = NULL;

static const std::string BYTECODE_FILE_EXT = ".luac";
static const std::string NOT_BYTECODE_FILE_EXT = ".lua";

LuaStack_::LuaStack_()
: _state(nullptr)
{
	_state = luaL_newstate();
	luaL_openlibs(_state);
}

LuaStack_::~LuaStack_()
{
	lua_close(_state);
}

LuaStack_* LuaStack_::getInstance()
{
	if (NULL == m_Instance)
	{
		m_Instance = new LuaStack_();
	}

	return m_Instance;
}

void LuaStack_::destroy()
{
	if (NULL != m_Instance)
	{
		delete m_Instance;
		m_Instance = NULL;
	}
}

#ifdef RunningInServer
Data_ LuaStack_::getDataFromFile(const std::string &path)
{
	Data_ data;

	//FILE *fp = fopen(path.c_str(), "rb");
	FILE *fp = fopen(path.c_str(), "rt");
	if (NULL == fp)
	{
		return data;
	}

	int size = 0;
	int readsize = 0;

	fseek(fp, 0, SEEK_END);
	size = ftell(fp) + 1;
	fseek(fp, 0, SEEK_SET);

	data.setSize(size);
	readsize = fread(data.getBytes(), sizeof(unsigned char), size, fp);
	fclose(fp);
	data.setSize(readsize);

	return data;
}

std::string LuaStack_::getStringFromFile(const std::string &path)
{
	FILE *fp = fopen(path.c_str(), "rt");
	if (NULL == fp)
	{
		return "";
	}

	char* buffer = NULL;
	int size = 0;
	int readsize = 0;

	fseek(fp, 0, SEEK_END);
	size = ftell(fp) + 1;
	fseek(fp, 0, SEEK_SET);

	buffer = (char*)malloc(size);
	memset(buffer, 0, size);

	readsize = fread(buffer, sizeof(unsigned char), size, fp);
	fclose(fp);

	std::string ret(buffer, readsize);
	free(buffer);
	return ret;
}
#endif

int LuaStack_::executeScriptFile(const char* filename)
{
	std::string buf(filename);
	// remove .lua or .luac

	size_t pos = buf.rfind(BYTECODE_FILE_EXT);
	if (pos != std::string::npos)
	{
		buf = buf.substr(0, pos);
	}
	else
	{
		pos = buf.rfind(NOT_BYTECODE_FILE_EXT);
		if (pos == buf.length() - NOT_BYTECODE_FILE_EXT.length())
		{
			buf = buf.substr(0, pos);
		}
	}
	// 1. check .lua suffix
	// 2. check .luac suffix
	std::string tmpfilename = buf + NOT_BYTECODE_FILE_EXT;

#ifndef RunningInServer
	if (FileUtils::getInstance()->isFileExist(tmpfilename))
#else
	if (true)
#endif
	{
		buf = tmpfilename;
	}
	else
	{
		tmpfilename = buf + BYTECODE_FILE_EXT;
#ifndef RunningInServer
		if (FileUtils::getInstance()->isFileExist(tmpfilename))
#else
		if (false)
#endif
		{
			buf = tmpfilename;
		}
	}


	std::string fullPath = "";
#ifndef RunningInServer
	fullPath = FileUtils::getInstance()->fullPathForFilename(buf);
	cocos2d::Data data = FileUtils::getInstance()->getDataFromFile(fullPath);
	int rn = 0;
	if (!data.isNull())
	{
		if (luaLoadBuffer(_state, (const char*)data.getBytes(), (int)data.getSize(), fullPath.c_str()) == 0)
		{
			rn = executeFunction(0);
		}
	}
	return rn;
#else
	fullPath = buf;
	std::string& data = getStringFromFile(fullPath);
	int rn = 0;
	if (!data.size() == 0)
	{
		if (luaLoadBuffer(_state, (const char*)data.c_str(), (int)data.size(), fullPath.c_str()) == 0)
		{
			rn = executeFunction(0);
		}
	}
	return rn;
#endif

}

int LuaStack_::executeGlobalFunction(const char* functionName)
{
	lua_getglobal(_state, functionName);       /* query function by name, stack: function */
	if (!lua_isfunction(_state, -1))
	{
		printf("name '%s' does not represent a Lua function", functionName);
		lua_pop(_state, 1);
		return 0;
	}
	return executeFunction(0);
}

int LuaStack_::executeFunction(int numArgs)
{
	int functionIndex = -(numArgs + 1);
	if (!lua_isfunction(_state, functionIndex))
	{
		printf("value at stack [%d] is not function", functionIndex);
		lua_pop(_state, numArgs + 1); // remove function and arguments
		return 0;
	}

	int traceback = 0;
	lua_getglobal(_state, "__G__TRACKBACK__");                         /* L: ... func arg1 arg2 ... G */
	if (!lua_isfunction(_state, -1))
	{
		lua_pop(_state, 1);                                            /* L: ... func arg1 arg2 ... */
	}
	else
	{
		lua_insert(_state, functionIndex - 1);                         /* L: ... G func arg1 arg2 ... */
		traceback = functionIndex - 1;
	}

	int error = 0;
	error = lua_pcall(_state, numArgs, 1, traceback);                  /* L: ... [G] ret */

	if (error)
	{
		if (traceback == 0)
		{
			printf("[LUA ERROR] %s", lua_tostring(_state, -1));        /* L: ... error */
			lua_pop(_state, 1); // remove error message from stack
		}
		else                                                            /* L: ... G error */
		{
			lua_pop(_state, 2); // remove __G__TRACKBACK__ and error message from stack
		}
		return 0;
	}

	// get return value
	int ret = 0;
	if (lua_isnumber(_state, -1))
	{
		ret = (int)lua_tointeger(_state, -1);
	}
	else if (lua_isboolean(_state, -1))
	{
		ret = (int)lua_toboolean(_state, -1);
	}
	// remove return value from stack
	lua_pop(_state, 1);                                                /* L: ... [G] */

	if (traceback)
	{
		lua_pop(_state, 1); // remove __G__TRACKBACK__ from stack      /* L: ... */
	}

	return ret;
}

int LuaStack_::executeString(const char *codes)
{
	luaL_loadstring(_state, codes);
	return executeFunction(0);
}

lua_State* LuaStack_::getLuaState()
{
	return _state;
}

void LuaStack_::addSearchPath(const char* path)
{
	lua_getglobal(_state, "package");                                  /* L: package */
	lua_getfield(_state, -1, "path");                /* get package.path, L: package path */
	const char* cur_path = lua_tostring(_state, -1);
	lua_pushfstring(_state, "%s;%s/?.lua", cur_path, path);            /* L: package path newpath */
	lua_setfield(_state, -3, "path");          /* package.path = newpath, L: package path */
	lua_pop(_state, 2);                                                /* L: - */
}

int LuaStack_::luaLoadBuffer(lua_State *L, const char *chunk, int chunkSize, const char *chunkName)
{
	int r = 0;

	auto skipBOM = [](const char*& chunk, int& chunkSize){
		// UTF-8 BOM? skip
		if (static_cast<unsigned char>(chunk[0]) == 0xEF &&
			static_cast<unsigned char>(chunk[1]) == 0xBB &&
			static_cast<unsigned char>(chunk[2]) == 0xBF)
		{
			chunk += 3;
			chunkSize -= 3;
		}
	};

	skipBOM(chunk, chunkSize);
	r = luaL_loadbuffer(L, chunk, chunkSize, chunkName);

#if defined(_DEBUG)
	if (r)
	{
		switch (r)
		{
		case LUA_ERRSYNTAX:
			printf("load \"%s\", error: syntax error during pre-compilation.", chunkName);
			break;

		case LUA_ERRMEM:
			printf("load \"%s\", error: memory allocation error.", chunkName);
			break;

		case LUA_ERRFILE:
			printf("load \"%s\", error: cannot open/read file.", chunkName);
			break;

		default:
			printf("load \"%s\", error: unknown.", chunkName);
		}
	}
#endif
	// r==0 is ok
	return r;
}

#endif