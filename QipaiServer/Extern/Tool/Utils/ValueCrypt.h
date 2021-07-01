/*
	加密数字,功能起到加密内存的作用,存在类里面的变量使用些加密, 保证第三方直接查看此内存的值时,值是一个错误的值
	防止恶意修改数据

*/
#ifndef ValueCrypt_h__
#define ValueCrypt_h__


#define KEY1 2734581
#define KEY2 168

#define CRYPT_ACCURACY 1000

#define ENCRYPT_FLOAT(v) ValueCrypt::encryptFloat(v, KEY1, KEY2) 
#define DECRYPT_FLOAT(v) ValueCrypt::decryptFloat(v, KEY1, KEY2)

#define ENCRYPT_INT(v) ValueCrypt::encryptInt(v, KEY1, KEY2)
#define DECRYPT_INT(v) ValueCrypt::decryptInt(v, KEY1, KEY2)

#define ENCRYPT_LONG(v) ValueCrypt::encryptLong(v, KEY1, KEY2)
#define DECRYPT_LONG(v) ValueCrypt::decryptLong(v, KEY1, KEY2)

#define ENCRYPT_DOUBLE(v) ValueCrypt::encryptDouble(v, KEY1, KEY2)
#define DECRYPT_DOUBLE(v) ValueCrypt::decryptDouble(v, KEY1, KEY2)

//加密整型成员变量getter和setter方法宏
#define ENCRYPT_INT_GETTER_SETTER(funcName, var) \
public: int get##funcName(void) { return DECRYPT_INT(var); }; \
public: void set##funcName(int v) { var = ENCRYPT_INT(v); };

//加密浮点型成员变量getter和setter方法宏
#define ENCRYPT_FLOAT_GETTER_SETTER(funcName, var) \
public: float get##funcName(void) { return DECRYPT_FLOAT(var); }; \
public: void set##funcName(float v) { var = ENCRYPT_FLOAT(v); };

//加密长整型成员变量getter和setter方法宏
#define ENCRYPT_LONG_GETTER_SETTER(funcName, var) \
public: long long get##funcName(void) { return DECRYPT_LONG(var); }; \
public: void set##funcName(long long v) { var = ENCRYPT_LONG(v); };

//加密浮点型成员变量getter和setter方法宏
#define ENCRYPT_DOUBLE_GETTER_SETTER(funcName, var) \
public: double get##funcName(void) { return DECRYPT_DOUBLE(var); }; \
public: void set##funcName(double v) { var = ENCRYPT_DOUBLE(v); };


class ValueCrypt
{
public:

	//加密浮点型数据
	static float encryptFloat(float v, int key1, int key2);

	//解密浮点型数据
	static float decryptFloat(float v, int key1, int key2);

	//加密整型数据
	static int encryptInt(int v, int key1, int key2);

	//解密整型数据
	static int decryptInt(int v, int key1, int key2);

	//加密长整型数据
	static long long encryptLong(long long v, int key1, int key2);

	//解密长整型数据
	static long long decryptLong(long long v, int key1, int key2);

	//加密双精度数据
	static double encryptDouble(double v, int key1, int key2);

	//解密双精度数据
	static double decryptDouble(double v, int key1, int key2);

};

#endif //ValueCrypt_h__