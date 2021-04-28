#include "StringFormat.h"
#include <regex>  
#include <tchar.h>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include <Windows.h>
#endif

void StringFormat::parseCsvStringMap(const std::string strSrc, std::map<int, int>& mapDst)
{
	if (strSrc.empty() || strSrc.compare("-1") == 0)
	{
		return;
	}

	int nCount = count(strSrc.begin(), strSrc.end(), '|');
	int nStart = 0;
	int nEnd = strSrc.find_first_of('|', 0);
	for (int i = 0; i <= nCount; i++)
	{
		if (-1 == nEnd)
		{
			nEnd = strSrc.length();
		}

		if (nStart >= nEnd)
		{
			break;
		}

		// 分割子串; 
		std::string strTmp = strSrc.substr(nStart, nEnd - nStart);

		// 在子串中继续分解单条数据;
		int nIndex1 = strTmp.find_first_of(',', 0);
		std::string strTmp1 = strTmp.substr(0, nIndex1);
		int nKey = atoi(strTmp1.c_str());
		std::string strTmp2 = strTmp.erase(0, nIndex1 + 1);
		int nValue = atoi(strTmp2.c_str());
		mapDst.insert(std::pair<int, int>(nKey, nValue));

		nStart = nEnd+1;
		nEnd = strSrc.find_first_of('|', nStart);

	}
}

void StringFormat::parseCsvStringVec(const std::string strSrc, std::vector<std::vector<int> >& vecDst)
{
	if (strSrc.empty() || strSrc.compare("-1") == 0)
	{
		return;
	}

	int nCount = count(strSrc.begin(), strSrc.end(), '|');
	int nStart = 0;
	int nEnd = strSrc.find_first_of('|', 0);
	for (int i = 0; i <= nCount; ++i)
	{
		if (-1 == nEnd)
		{
			nEnd = strSrc.length();
		}

		if (nStart >= nEnd)
		{
			break;
		}

		// 分割子串; 
		std::string strTmp = strSrc.substr(nStart, nEnd - nStart);

		// 在子串中继续分解单条数据;
		std::vector<int>  vecSingle;

		int nSingleCount = count(strTmp.begin(), strTmp.end(), ',');
		for (int k = 0; k <= nSingleCount; ++k)
		{
			int nIndex1 = strTmp.find_first_of(',', 0);
			std::string strTmp1 = strTmp.substr(0, nIndex1);
			vecSingle.push_back(atoi(strTmp1.c_str()));
			strTmp = strTmp.erase(0, nIndex1+1);
		}

		vecDst.push_back(vecSingle);

		nStart = nEnd+1;
		nEnd = strSrc.find_first_of('|', nStart);

	}
}

void StringFormat::parseCsvStringVec(const std::string strSrc, std::vector<std::string>& vecDst, char separetor /*= '|'*/)
{
	if (strSrc.empty() || strSrc.compare("-1") == 0)
	{
		return;
	}

	int nCount = count(strSrc.begin(), strSrc.end(), separetor);
	int nStart = 0;
	int nEnd = strSrc.find_first_of(separetor, 0);
	for (int i = 0; i <= nCount; i++)
	{
		if (-1 == nEnd)
		{
			nEnd = strSrc.length();
		}

		if (nStart >= nEnd)
		{
			break;
		}

		// 分割子串; 
		std::string strTmp = strSrc.substr(nStart, nEnd - nStart);
		vecDst.push_back(strTmp);

		nStart = nEnd + 1;
		nEnd = strSrc.find_first_of(separetor, nStart);

	}
}

void StringFormat::parseCsvStringVecByChar(const std::string strSrc, std::vector<int>& vecDst, char separetor)
{
	if (strSrc.empty())
	{
		return;
	}

	int nCount = count(strSrc.begin(), strSrc.end(), separetor);
	int nStart = 0;
	int nEnd = strSrc.find_first_of(separetor, 0);
	for (int i = 0; i <= nCount; i++)
	{
		if (-1 == nEnd)
		{
			nEnd = strSrc.length();
		}

		if (nStart >= nEnd)
		{
			break;
		}

		// 分割子串; 
		std::string strTmp = strSrc.substr(nStart, nEnd - nStart);
		vecDst.push_back(atoi(strTmp.c_str()));

		nStart = nEnd + 1;
		nEnd = strSrc.find_first_of(separetor, nStart);
	}
}

//转码
std::wstring StringFormat::StringUtf8ToWideChar(const std::string& strUtf8)
{
	std::wstring ret;
	if (!strUtf8.empty())
	{
		int nNum = MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, nullptr, 0);
		if (nNum)
		{
			WCHAR* wideCharString = new WCHAR[nNum + 1];
			wideCharString[0] = 0;

			nNum = MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, wideCharString, nNum + 1);

			ret = wideCharString;
			delete[] wideCharString;
		}
		else
		{
			printf("Wrong convert to WideChar code:0x%x", GetLastError());
		}
	}
	return ret;
}

std::string StringFormat::StringWideCharToUtf8(const std::wstring& strWideChar)
{
	std::string ret;
	if (!strWideChar.empty())
	{
		int nNum = WideCharToMultiByte(CP_UTF8, 0, strWideChar.c_str(), -1, nullptr, 0, nullptr, FALSE);
		if (nNum)
		{
			char* utf8String = new char[nNum + 1];
			utf8String[0] = 0;

			nNum = WideCharToMultiByte(CP_UTF8, 0, strWideChar.c_str(), -1, utf8String, nNum + 1, nullptr, FALSE);

			ret = utf8String;
			delete[] utf8String;
		}
		else
		{
			printf("Wrong convert to Utf8 code:0x%x", GetLastError());
		}
	}
	return ret;
}

std::string StringFormat::UTF8StringToMultiByte(const std::string& strUtf8)
{
	std::string ret;
	if (!strUtf8.empty())
	{
		std::wstring strWideChar = StringUtf8ToWideChar(strUtf8);
		int nNum = WideCharToMultiByte(CP_ACP, 0, strWideChar.c_str(), -1, nullptr, 0, nullptr, FALSE);
		if (nNum)
		{
			char* ansiString = new char[nNum + 1];
			ansiString[0] = 0;

			nNum = WideCharToMultiByte(CP_ACP, 0, strWideChar.c_str(), -1, ansiString, nNum + 1, nullptr, FALSE);

			ret = ansiString;
			delete[] ansiString;
		}
		else
		{
			printf("Wrong convert to Ansi code:0x%x", GetLastError());
		}
	}

	return ret;
}

int StringFormat::c2i(char ch)
{  
	// 如果是数字，则用数字的ASCII码减去48, 如果ch = '2' ,则 '2' - 48 = 2  
	if(isdigit(ch))  
		return ch - 48;  
  
	// 如果是字母，但不是A~F,a~f则返回  
	if( ch < 'A' || (ch > 'F' && ch < 'a') || ch > 'z' )  
		return -1;  
  
	// 如果是大写字母，则用数字的ASCII码减去55, 如果ch = 'A' ,则 'A' - 55 = 10  
	// 如果是小写字母，则用数字的ASCII码减去87, 如果ch = 'a' ,则 'a' - 87 = 10  
	if(isalpha(ch))  
		return isupper(ch) ? ch - 55 : ch - 87;  
  
	return -1;  
}  
  
// 功能：将十六进制字符串转换为整型(int)数值 
int StringFormat::hex2dec(const char *hex)
{  
	int len;  
	int num = 0;  
	int temp;  
	int bits;
	int i;  
          
	len = strlen(hex);  
  
	for (i=0, temp=0; i<len; i++, temp=0)  
	{  
		// 第一次：i=0, *(hex + i) = *(hex + 0) = '1', 即temp = 1  
		// 第二次：i=1, *(hex + i) = *(hex + 1) = 'd', 即temp = 13  
		// 第三次：i=2, *(hex + i) = *(hex + 2) = 'd', 即temp = 14  
		temp = c2i( *(hex + i) );
		// 总共3位，一个16进制位用 4 bit保存  
		// 第一次：'1'为最高位，所以temp左移 (len - i -1) * 4 = 2 * 4 = 8 位  
		// 第二次：'d'为次高位，所以temp左移 (len - i -1) * 4 = 1 * 4 = 4 位  
		// 第三次：'e'为最低位，所以temp左移 (len - i -1) * 4 = 0 * 4 = 0 位  
		bits = (len - i - 1) * 4;  
		temp = temp << bits;  
  
		// 此处也可以用 num += temp;进行累加  
		num = num | temp;  
	}  
  
	// 返回结果  
	return num;  
} 

void StringFormat::split(std::vector<std::string>& dest, std::string str, std::string pattern)
{  
	std::string::size_type pos;
	str+=pattern;//扩展字符串以方便操作  
	int size=str.size();  

	for(int i=0; i<size; i++)  
	{  
		pos = str.find(pattern,i);  
		if (pos<(std::string::size_type)size)
		{  
			std::string s = str.substr(i, pos - i);
			dest.push_back(s);  
			i=pos+pattern.size()-1;  
		}  
	}
}


