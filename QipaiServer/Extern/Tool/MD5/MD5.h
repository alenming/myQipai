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

//好像不可用
class CMD5
{
private:
	struct md5_context
	{
		UnLongInt_t total[2];
		UnLongInt_t state[4];
		UnChar_t buffer[64];
	};

	void md5_starts(struct md5_context *ctx);
	void md5_process(struct md5_context *ctx, UnChar_t data[64]);
	void md5_update(struct md5_context *ctx, UnChar_t *input, UnLongInt_t length);
	void md5_finish(struct md5_context *ctx, UnChar_t digest[16]);

public:
	//! construct a MD5 from any buffer
	void GenerateMD5(UnChar_t* buffer, int bufferlen);
	//! construct a MD5
	CMD5();
	//! construct a md5src from char *
	CMD5(const char * md5src);
	//! construct a MD5 from a 16 bytes md5
	CMD5(UnLong_t* md5src);
	//! add a other md5
	CMD5 operator +(CMD5 adder);
	//! just if equal
	bool operator ==(CMD5 cmper);
	//! give the value from equer
	// void operator =(MD5 equer);
	//! to a string
	std::string ToString();
	UnLong_t m_data[4];
};



#endif // __MD5__
