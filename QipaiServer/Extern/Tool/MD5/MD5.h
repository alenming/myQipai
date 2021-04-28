/*
	MD5加密
*/
#ifndef __MD5__
#define __MD5__

#include <string>

/************************************************************************/
/* 
用法:
string md5str = "abc";
cout << MD5::GetMD5OfString(md5str) << endl;
*/
/************************************************************************/
typedef unsigned char		UnChar_t;
typedef unsigned int		UnInt32_t;
typedef unsigned long		UnLong_t;
typedef unsigned long int	UnLongInt_t;

class MD5
{
public:
	//加密普通字符串
    static std::string GetMD5OfString(std::string strString);
    //加密文件路径
    static std::string GetMD5(const std::string& strFilePath);
    
protected:
    MD5();
    virtual ~MD5() {};
    
	void Transform(UnChar_t Block[64]);
	void Update(UnChar_t* Input, UnInt32_t nInputLen);
    std::string Final();
    inline UnInt32_t RotateLeft(UnInt32_t x, int n);
    inline void FF( UnInt32_t& A, UnInt32_t B, UnInt32_t C, UnInt32_t D, UnInt32_t X, UnInt32_t S, UnInt32_t T);
    inline void GG( UnInt32_t& A, UnInt32_t B, UnInt32_t C, UnInt32_t D, UnInt32_t X, UnInt32_t S, UnInt32_t T);
    inline void HH( UnInt32_t& A, UnInt32_t B, UnInt32_t C, UnInt32_t D, UnInt32_t X, UnInt32_t S, UnInt32_t T);
    inline void II( UnInt32_t& A, UnInt32_t B, UnInt32_t C, UnInt32_t D, UnInt32_t X, UnInt32_t S, UnInt32_t T);

	inline void uint32_tToByte(UnChar_t* Output, UnInt32_t* Input, UnInt32_t nLength);
	inline void ByteTouint32_t(UnInt32_t* Output, UnChar_t* Input, UnInt32_t nLength);
    
private:
	UnChar_t  m_lpszBuffer[64];
    UnInt32_t m_nCount[2];
    UnInt32_t m_lMD5[4];
};

#endif // __MD5__
