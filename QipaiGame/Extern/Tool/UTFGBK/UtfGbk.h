#ifndef Common_h__
#define Common_h__
#include <string>

#pragma comment(lib,"libiconv.lib")

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#define STR_UTF8(str) CommonTool::getInstance()->GBKToUTF8(str)
#define STR_ANSI(str) CommonTool::getInstance()->UTF8ToGBK(str)
#else
	#define STR_UTF8(str) string(str)
	#define STR_ANSI(str) string(str)
#endif

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) > (b)) ? (b) : (a))

class CommonTool
{
public:

	CommonTool();
	~CommonTool();
public:
	//singleton
	static CommonTool* getInstance();
	static void destroyInstance();

	std::string GBKToUTF8(std::string str);	//此函数只用在win32
	std::string UTF8ToGBK(std::string str);	//此函数只用在win32

	std::string gb23122utf8(const char* gb2312);


private:

	
private:

	static CommonTool* mInstance;
};



#endif // Common_h__
