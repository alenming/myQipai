#include "UtfGbk.h"

#ifdef _WIN32
#include "iconv.h"
#endif
#ifdef _WIN32
#include <Windows.h>
#endif //



CommonTool* CommonTool::mInstance = nullptr;

CommonTool::CommonTool()
{
}

CommonTool::~CommonTool()
{
}

CommonTool* CommonTool::getInstance()
{
	if (!mInstance)
	{
		mInstance = new CommonTool();
	}
	return mInstance;
}

void CommonTool::destroyInstance()
{
	if (mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

std::string CommonTool::GBKToUTF8(std::string str)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	const char * szStr = str.c_str();
	char szBuf[1024];
	int nOutLen = 1024;
	memset(szBuf, 0, nOutLen);
	iconv_t cd;
	cd = iconv_open("UTF-8", "GB2312");
	if (cd == 0) return szBuf;
	int nInlen = strlen(szStr);
	const char **pin = &szStr;
	char *pout = szBuf;
	if (iconv(cd, (const char **)(&szStr), (size_t *)&nInlen, &pout, (size_t *)&nOutLen) == -1)
	{
		szBuf[0] = 0;
	}
	iconv_close(cd);
	return szBuf;
#endif
	return str;
}

std::string CommonTool::UTF8ToGBK(std::string str)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	const char* szStr = str.c_str();
	char *inbuf = const_cast<char*>(szStr);
	size_t inlen = strlen(inbuf);
	size_t outlen = inlen * 4;
	char *outbuf = (char *)malloc(inlen * 4);
	memset(outbuf, 0, inlen * 4);
	const char *in = inbuf;
	char *out = outbuf;
	iconv_t cd = iconv_open("GB2312", "UTF-8");
	iconv(cd, &in, &inlen, &out, &outlen);
	iconv_close(cd);
	std::string ret(outbuf);
	free(outbuf);
	return ret;
#endif
	return str;
}

std::string CommonTool::gb23122utf8(const char* gb2312)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	int len = MultiByteToWideChar(0, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(0, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(65001, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(65001, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	std::string desStr = str;
	delete str;
	return desStr;
#endif
	return "";
}


