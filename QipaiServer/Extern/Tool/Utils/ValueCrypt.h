/*
	��������,�����𵽼����ڴ������,����������ı���ʹ��Щ����, ��֤������ֱ�Ӳ鿴���ڴ��ֵʱ,ֵ��һ�������ֵ
	��ֹ�����޸�����

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

//�������ͳ�Ա����getter��setter������
#define ENCRYPT_INT_GETTER_SETTER(funcName, var) \
public: int get##funcName(void) { return DECRYPT_INT(var); }; \
public: void set##funcName(int v) { var = ENCRYPT_INT(v); };

//���ܸ����ͳ�Ա����getter��setter������
#define ENCRYPT_FLOAT_GETTER_SETTER(funcName, var) \
public: float get##funcName(void) { return DECRYPT_FLOAT(var); }; \
public: void set##funcName(float v) { var = ENCRYPT_FLOAT(v); };

//���ܳ����ͳ�Ա����getter��setter������
#define ENCRYPT_LONG_GETTER_SETTER(funcName, var) \
public: long long get##funcName(void) { return DECRYPT_LONG(var); }; \
public: void set##funcName(long long v) { var = ENCRYPT_LONG(v); };

//���ܸ����ͳ�Ա����getter��setter������
#define ENCRYPT_DOUBLE_GETTER_SETTER(funcName, var) \
public: double get##funcName(void) { return DECRYPT_DOUBLE(var); }; \
public: void set##funcName(double v) { var = ENCRYPT_DOUBLE(v); };


class ValueCrypt
{
public:

	//���ܸ���������
	static float encryptFloat(float v, int key1, int key2);

	//���ܸ���������
	static float decryptFloat(float v, int key1, int key2);

	//������������
	static int encryptInt(int v, int key1, int key2);

	//������������
	static int decryptInt(int v, int key1, int key2);

	//���ܳ���������
	static long long encryptLong(long long v, int key1, int key2);

	//���ܳ���������
	static long long decryptLong(long long v, int key1, int key2);

	//����˫��������
	static double encryptDouble(double v, int key1, int key2);

	//����˫��������
	static double decryptDouble(double v, int key1, int key2);

};

#endif //ValueCrypt_h__