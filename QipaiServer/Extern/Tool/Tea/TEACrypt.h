#ifndef __TEACRYPT_H__
#define __TEACRYPT_H__


#include "CommonHead.h"

typedef unsigned int	UnInt32_t;

/*
�÷�:

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

	//����key,128λ��Կ
	void setKey(UnInt32_t key[]);

	//�����㷨
	void encrypt(const char* src, size_t size);

	//�����㷨
	void decrypt(const char* src, size_t size);

	//��ȡ������
	char* getBuffer();

	//��ȡ����������
	size_t getBufferSize();

	//�������������
	void clear();

	//���ü�������
	void setLoop(int loop);

private:

	//����
	void encrypt(char src[]);

	//����
	void decrypt(char src[]);
	
	//�����㷨��64λ���ģ�128λ��Կ
	void encrypt (UnInt32_t v[], UnInt32_t k[]);

	//�����㷨��64λ���ģ�128λ��Կ
	void decrypt (UnInt32_t v[], UnInt32_t k[]);

	//��4��charתΪ 32λ�޷�������
    UnInt32_t charToUint32(char c1, char c2, char c3, char c4);

	//�������޷�������ת����char
	void uint32ToChar(UnInt32_t n1, UnInt32_t n2, char src[]);

private:

	//128λ��Կ
	UnInt32_t mKey[4];

	//���ܡ����ܽ��
	char* mResultBuffer;

	//���ܡ��������ݳ���
	size_t mResultBufferSize;

	//��������
	UnInt32_t mLoop;

};



#endif //__TEACRYPT_H__


