#include "ValueCrypt.h"



float ValueCrypt::encryptFloat( float v, int key1, int key2 )
{
	long long a = (long long)(v * CRYPT_ACCURACY);
	return ((a ^ key1) / (float)CRYPT_ACCURACY) + key2;
}

float ValueCrypt::decryptFloat( float v, int key1, int key2 )
{
	long long a = (long long)((v - key2) * CRYPT_ACCURACY);
	return ((a ^ key1) / (float)CRYPT_ACCURACY);
}

int ValueCrypt::encryptInt( int v, int key1, int key2 )
{
	return (v ^ key1) + key2;
}

int ValueCrypt::decryptInt( int v, int key1, int key2 )
{
	return (v - key2) ^ key1;
}

//加密长整型数据
long long ValueCrypt::encryptLong(long long v, int key1, int key2)
{
	return (v ^ key1) + key2;
}

//解密长整型数据
long long ValueCrypt::decryptLong(long long v, int key1, int key2)
{
	return (v - key2) ^ key1;
}

double ValueCrypt::encryptDouble( double v, int key1, int key2 )
{
	long long a = (long long)(v * CRYPT_ACCURACY);
	return ((a ^ key1) / (double)CRYPT_ACCURACY) + key2;
}

double ValueCrypt::decryptDouble( double v, int key1, int key2 )
{
	long long a = (long long)((v - key2) * CRYPT_ACCURACY);
	return ((a ^ key1) / (double)CRYPT_ACCURACY);
}


