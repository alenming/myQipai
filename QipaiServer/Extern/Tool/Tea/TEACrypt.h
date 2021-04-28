#ifndef __TEACRYPT_H__
#define __TEACRYPT_H__


#include "CommonHead.h"

typedef unsigned int	UnInt32_t;

/*
用法:

UnInt32_t mKey[] = { 0x01, 0x02, 0x03, 0x04 };
TEACrypt* tea = new TEACrypt();
tea->setKey(mKey);
TEACrypt* tea1 = new TEACrypt();
tea1->setKey(mKey);

string name = "wangxiangming";

tea->encrypt(name.c_str(), strlen(name.c_str()));

char* re = tea->getBuffer();

cout << re << endl << "size=" << tea->getBufferSize() << endl;

tea1->decrypt(re, strlen(re));

cout << tea1->getBuffer() << endl << "size=" << tea1->getBufferSize() << endl;

tea->clear();
tea1->clear();
delete tea;
delete tea1;
	
*/

class TEACrypt
{

public:

	TEACrypt();
	~TEACrypt();

public:

	//设置key,128位密钥
	void setKey(UnInt32_t key[]);

	//加密算法
	void encrypt(const char* src, size_t size);

	//解密算法
	void decrypt(const char* src, size_t size);

	//获取计算结果
	char* getBuffer();

	//获取计算结果长度
	size_t getBufferSize();

	//清空运算结果缓存
	void clear();

	//设置加密轮数
	void setLoop(int loop);

private:

	//加密
	void encrypt(char src[]);

	//解密
	void decrypt(char src[]);
	
	//加密算法，64位明文，128位密钥
	void encrypt (UnInt32_t v[], UnInt32_t k[]);

	//解密算法，64位明文，128位密钥
	void decrypt (UnInt32_t v[], UnInt32_t k[]);

	//将4个char转为 32位无符号整型
    UnInt32_t charToUint32(char c1, char c2, char c3, char c4);

	//将两个无符号整型转换成char
	void uint32ToChar(UnInt32_t n1, UnInt32_t n2, char src[]);

private:

	//128位密钥
	UnInt32_t mKey[4];

	//加密、解密结果
	char* mResultBuffer;

	//加密、解密数据长度
	size_t mResultBufferSize;

	//加密轮数
	UnInt32_t mLoop;

};



#endif //__TEACRYPT_H__


