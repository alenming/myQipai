#ifndef __STRING_FORMAT_H__
#define __STRING_FORMAT_H__
 
#include <vector>
#include <list>
#include <map>

#ifndef _TO_STR
#define _TO_STR(val) StringFormat::toString(val)
	#define TO_STR _TO_STR
#endif

class StringFormat
{
public:
	// ����csv�ļ��д��зָ������ַ���;
	static void parseCsvStringVec(const std::string strSrc, std::vector<std::string>& vecDst, char separetor = '|');
	static void parseCsvStringVec(const std::string strSrc, std::vector<std::vector<int> >& vecDst);
	static void parseCsvStringMap(const std::string strSrc, std::map<int, int>& mapDst);

	//����ָ����ͺ� 
	static void parseCsvStringVecByChar(const std::string strSrc, std::vector<int>& vecDst, char separetor);

	//�����Ӧ��������ģ��
	template<typename T>
	static void parseCsvMathVec(const std::string strSrc, std::vector<T>& vecDst, char separetor)
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

			// �ָ��Ӵ�; 
			string strTmp = strSrc.substr(nStart, nEnd - nStart);
			vecDst.push_back((T)atof(strTmp.c_str()));

			nStart = nEnd + 1;
			nEnd = strSrc.find_first_of(separetor, nStart);
		}
	}

	//��ʮ�������ַ���ת��Ϊ����(int)��ֵ
	static int c2i(char ch);
	static int hex2dec(const char *hex);


	template<typename T>
	static T toAbs(T num)
	{
		return num > 0 ? num : -num;
	}

	template<typename T>
	static std::string toString(T var)
	{
		std::string ret;
		std::stringstream st;
		st << var;
		st >> ret;
		return ret;
	}

	template<class T>
	static T toNum(const std::string& str)
	{
		std::istringstream st;
		st.str(str);
		T ret;
		st >> ret;
		return ret;
	}

	static double atof(const char* str)
	{
		if (str == nullptr)
		{
			return 0.0;
		}

		char buf[256];
		strncpy(buf, str, 256);
		char* dot = strchr(buf, '.');
		if (dot != nullptr && dot - buf + 8 < 256)
		{
			dot[8] = '\0';
		}

		return ::atof(buf);
	}

	//�ַ����ָ�
	static void split(std::vector<std::string>& dest, std::string str, std::string pattern);

	//��windowsת�뺯��
	static std::wstring StringUtf8ToWideChar(const std::string& strUtf8);
	static std::string StringWideCharToUtf8(const std::wstring& strWideChar);
	static std::string UTF8StringToMultiByte(const std::string& strUtf8);
private:

};

inline void splitStringToVec(const std::string& str, const std::string& splt, std::vector<std::string>& out)
{
	if (str.empty() || splt.empty())
	{
		return;
	}

	size_t pos = 0;
	size_t offset = 0;
	while (true)
	{
		pos = str.find(splt, offset);
		if (pos == std::string::npos)
		{
			out.push_back(str.substr(offset, str.length() - offset));
			break;
		}
		else
		{
			out.push_back(str.substr(offset, pos - offset));
			offset = splt.length() + pos;
		}
	}
}

inline std::vector<std::string> splitStringToVec(const std::string& str, const std::string& splt)
{
	std::vector<std::string> ret;
	splitStringToVec(str, splt, ret);
	return ret;
}

template<typename T>
inline bool eraseObjectInList(std::list<T>& l, T obj)
{
	for (typename std::list<T>::iterator iter = l.begin();
		iter != l.end(); ++iter)
	{
		if (*iter == obj)
		{
			l.erase(iter);
			return true;
		}
	}

	return false;
}

template<typename T>
inline bool eraseObjectInVec(std::vector<T>& l, T obj)
{
	for (typename std::vector<T>::iterator iter = l.begin();
		iter != l.end(); ++iter)
	{
		if (*iter == obj)
		{
			l.erase(iter);
			return true;
		}
	}

	return false;
}


// ������� 1 / sqrt(x)����ȷ��С�����3λ
inline float fastInvSqrt(float x)
{
	float xhalf = 0.5f*x;
	int i = *(int*)&x;		        // get bits for floating VALUE
	i = 0x5f375a86 - (i >> 1);      // gives initial guess y0
	x = *(float*)&i;		        // convert bits BACK to float
	x = x*(1.5f - xhalf*x*x);       // Newton step, repeating increases accuracy
	return x;
}

// ������� sqrt����ȷ��С�����3λ
inline float fastSqrt(float x)
{
	float xhalf = 0.5f * x;
	int i = *(int*)&x;              // get bits for floating VALUE         
	i = 0x5f3759df - (i >> 1);      // gives initial guess y0      
	x = *(float*)&i;                // convert bits BACK to float    
	x = x*(1.5f - xhalf*x*x);       // Newton step, repeating increases accuracy
	return (1 / x);
}

// ������� 1 / sqrt(x)����ȷ��С�����10λ
inline float preciseInvSqrt(float x)
{
	float xhalf = 0.5f*x;
	int i = *(int*)&x;		        // get bits for floating VALUE
	i = 0x5f375a86 - (i >> 1);      // gives initial guess y0
	x = *(float*)&i;		        // convert bits BACK to float
	x = x*(1.5f - xhalf*x*x);       // Newton step, repeating increases accuracy
	x = x*(1.5f - xhalf*x*x);       // Newton step, repeating increases accuracy
	x = x*(1.5f - xhalf*x*x);       // Newton step, repeating increases accuracy
	return x;
}

// ������� sqrt����ȷ��С�����10λ
inline float preciseSqrt(float x)
{
	float xhalf = 0.5f * x;
	int i = *(int*)&x;              // get bits for floating VALUE         
	i = 0x5f3759df - (i >> 1);      // gives initial guess y0      
	x = *(float*)&i;                // convert bits BACK to float    
	x = x*(1.5f - xhalf*x*x);       // Newton step, repeating increases accuracy
	x = x*(1.5f - xhalf*x*x);       // Newton step, repeating increases accuracy
	x = x*(1.5f - xhalf*x*x);       // Newton step, repeating increases accuracy
	return (1 / x);
}

// �����������Ĳ�� P1 X P2 = x1y2 - x2y1
inline float crossProduct(float x1, float y1, float x2, float y2)
{
	return x1*y2 - x2*y1;
}

// �����������߶εķ��� (P2 - P0) X (P1 - P0)
// Ϊ����ʾ�߶� P0P2 ���߶� P0P1 ��˳ʱ�뷽��Ϊ����ʾ�߶� P0P2 ���߶� P0P1 ��ʱ��
inline float direction(float x0, float y0, float x1, float y1, float x2, float y2)
{
	return crossProduct(x2 - x0, y2 - y0, x1 - x0, y1 - y0);
}

// �������߶� P0P1 �� P2P3 �Ƿ��ཻ
inline bool lineIntersect(float x0, float y0, float x1, float y1,
	float x2, float y2, float x3, float y3)
{
	float d1 = direction(x0, y0, x2, y2, x3, y3);
	float d2 = direction(x1, y1, x2, y2, x3, y3);
	float d3 = direction(x2, y2, x0, y0, x1, y1);
	float d4 = direction(x3, y3, x0, y0, x1, y1);

	//ֻ�����ཻ�����Ǵ�ֱ
	if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0))
		&& ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0)))
	{
		return true;
	}

	return false;
}

// ��ֵ������
template<typename T>
void limitValue(T& v, const T& min, const T& max)
{
	if (v < min)
	{
		v = min;
	}
	else if (v > max)
	{
		v = max;
	}
};


#endif //__STRING_FORMAT_H__