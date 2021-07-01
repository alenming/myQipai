#include "TEACrypt.h"


TEACrypt::TEACrypt()
	: mResultBuffer(nullptr)
	, mResultBufferSize(0)
	, mLoop(32) //默认加密轮数32轮
{
}

TEACrypt::~TEACrypt()
{
	clear();
}

void TEACrypt::clear()
{
	if (mResultBuffer)
	{
		delete[] mResultBuffer;
		mResultBuffer = nullptr;
	}
	mResultBufferSize = 0;
}

char* TEACrypt::getBuffer()
{
	return mResultBuffer;
}

size_t TEACrypt::getBufferSize()
{
	return mResultBufferSize;
}

void TEACrypt::setKey(UnInt32_t key[])
{
	memcpy(mKey, key, 16);
}

void TEACrypt::setLoop(int loop)
{
	mLoop = loop;
}

void TEACrypt::encrypt(const char* src, size_t size)
{
	clear();	//清空缓存
	const int group_len = 8; //每组加密数据的长度，8字节64位
	int data_len = size; //数据的长度
	int residues = data_len % group_len; //余数
	int dlen = data_len - residues;	

	// 用于储存加密的密文，第一字节为余数的大小
	int result_len = data_len + 1;
	if (residues > 0) {
		result_len += group_len - residues;
	}

	mResultBufferSize = result_len;
	mResultBuffer = new char[mResultBufferSize];
	memset(mResultBuffer, 0 , mResultBufferSize);
	mResultBuffer[0] = (char)residues;

	char plain[group_len];
	for (int i = 0; i < dlen; i += group_len) {
		for (int j = 0; j < group_len; j++) {
			plain[j] = src[i + j];
		}
		//将明文转换位密文
		encrypt(plain);
		for (int k = 0; k < group_len; k++) {
			mResultBuffer[i + k + 1] = plain[k];
		}
	}

	if (residues > 0) {
		for (int j = 0; j < residues; j++) {
			plain[j] = src[dlen + j];
		}
		int padding = group_len - residues;
		for (int j = 0; j < padding; j++) {
			plain[residues + j] = (char)0x00;
		}
		encrypt(plain);
		for (int k = 0; k < group_len; k++) {
			mResultBuffer[dlen + k + 1] = plain[k];
		}
	}
}

void TEACrypt::decrypt(const char* src, size_t size)
{
	clear();
	const int group_len = 8;
	int data_len = size - 1, dlen; // 数据的长度
	int residues = (int)(src[0]); // 余数
	if (residues > 0) {
		dlen = data_len - group_len; 
	} else {
		dlen = data_len;
	}

	mResultBufferSize = dlen + residues;
	mResultBuffer = new char[mResultBufferSize + 1];
	memset(mResultBuffer, 0 , mResultBufferSize + 1);

	char plain[group_len];
	for (int i = 0; i < dlen; i += group_len) {
		for (int j = 0; j < group_len; j++) {
			plain[j] = src[i + j + 1];
		}
		//将密文转换为明文
		decrypt(plain);
		for (int k = 0; k < group_len; k++) {
			mResultBuffer[i + k] = plain[k];
		}
	}
	if (residues > 0) {
		for (int j = 0; j < group_len; j++) {
			plain[j] = src[dlen + j + 1];
		}
		decrypt(plain);
		for (int k = 0; k < residues; k++) {
			mResultBuffer[dlen + k] = plain[k];
		}
	}
}

 UnInt32_t TEACrypt::charToUint32(char c1, char c2, char c3, char c4) 
 {
	 return ((c1<<24) & 0xff000000) +
		 ((c2<<16) & 0x00ff0000) +
		 ((c3<<8) & 0x0000ff00) +
		 (c4 & 0x000000ff);
 }

void TEACrypt::uint32ToChar(UnInt32_t n1, UnInt32_t n2, char src[])
{
	src[0] = (char)((n1 & 0xff000000) >> 24);
	src[1] = (char)((n1 & 0x00ff0000) >> 16);
	src[2] = (char)((n1 & 0x0000ff00) >> 8);
	src[3] = (char)(n1 & 0x000000ff);

	src[4] = (char)((n2 & 0xff000000) >> 24);
	src[5] = (char)((n2 & 0x00ff0000) >> 16);
	src[6] = (char)((n2 & 0x0000ff00) >> 8);
	src[7] = (char)(n2 & 0x000000ff);
}

 void TEACrypt::encrypt(char src[])
 {
	 //将char型转换成无符号整型
	 UnInt32_t v[2];
	 v[0] = charToUint32(src[0], src[1], src[2], src[3]);
	 v[1] = charToUint32(src[4], src[5], src[6], src[7]);
	 encrypt(v, mKey);
	 //将无符号整型转换成char型
	 uint32ToChar(v[0], v[1], src);
 }

 void TEACrypt::decrypt(char src[])
 {
	 //将char型转换成无符号整型
	 UnInt32_t v[2];
	 v[0] = charToUint32(src[0], src[1], src[2], src[3]);
	 v[1] = charToUint32(src[4], src[5], src[6], src[7]);
	 decrypt(v, mKey);
	 //将无符号整型转换成char型
	 uint32ToChar(v[0], v[1], src);
 }

void TEACrypt::encrypt(UnInt32_t v[], UnInt32_t k[])
{
	UnInt32_t v0=v[0], v1=v[1], sum=0, i;           /* set up */
	UnInt32_t delta=0x9e3779b9;                     /* a key schedule constant */
	UnInt32_t k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
	for (i=0; i < mLoop; i++) {                       /* basic cycle start */
		sum += delta;
		v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
		v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
	}                                              /* end cycle */
	v[0]=v0; v[1]=v1;
}

void TEACrypt::decrypt(UnInt32_t v[], UnInt32_t k[])
{
	UnInt32_t v0=v[0], v1=v[1], sum=0xC6EF3720, i;  /* set up */
	UnInt32_t delta=0x9e3779b9;                     /* a key schedule constant */
	UnInt32_t k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
	for (i=0; i < mLoop; i++) {                         /* basic cycle start */
		v1 -= ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
		v0 -= ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
		sum -= delta;                                   
	}                                              /* end cycle */
	v[0]=v0; v[1]=v1;
}


