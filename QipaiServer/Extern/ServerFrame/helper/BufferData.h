/*
* ���ݻ���������
* �������л��ͷ����л�

*/
#ifndef __BUFFERDATA_H__
#define __BUFFERDATA_H__

#include <stdio.h>
#include <memory.h>
#include <assert.h>

#include "server/Head.h"

//���л�����һ��ֵ,�����л�����Ӧ��λ�ö�ȡ��ֵ,���ж��Ƿ�ƥ��,���ƥ����û�д�λ, ��ƥ�������Ϣ
//���л�һ��ֵ,��������ƥ��
#define SERIALIZE_MATCH(_buffData_)					\
{													\
	(_buffData_).writeData(12345678);				\
}
//���Է����л���ֵ�Ƿ�ƥ��
#define UN_SERIALIZE_MATCH(_buffData_)											\
{																				\
	int _readValue_ = 0;														\
	(_buffData_).readData(_readValue_);											\
	LOGDEBUG("readValue= %d, File %s, Line %d", _readValue_, __FILE__, __LINE__);	\
	assert("�����л���ƥ��" && (12345678 ==_readValue_));							\
}

#define SERIALIZE_MATCH_VALUE(_buffData_, _value_)	\
{													\
	(_buffData_).writeData(_value_);				\
}
//���Է����л���ֵ�Ƿ�ƥ��
#define UN_SERIALIZE_MATCH_VALUE(_buffData_, _value_)							\
{																				\
	int _readValue_ = 0;														\
	(_buffData_).readData(_readValue_);											\
	LOGDEBUG("readValue= %d, File %s, Line %d", _readValue_, __FILE__, __LINE__);	\
	assert("�����л���ƥ��" && (_value_ ==_readValue_));							\
}

class BufferData
{
public:
    BufferData();
    virtual ~BufferData();

    bool init(int bufferSize);
    bool init(char* buffer, unsigned int dataLength);
    bool init(BufferData* pBuffData);

    void clean();

    template<typename T>
    bool writeData(T data)
    {
        if (!checkBufferSize(sizeof(data)))
        {
            return false;
        }
        else
        {
            memcpy(m_Buffer + m_DataLength, &data, sizeof(data));
            m_DataLength += sizeof(data);
            return true;
        }
    }
    bool writeData(const void* data, unsigned int length);

    template<typename T>
    bool readData(T& data)
    {
        if (!checkDataLength(sizeof(data)))
        {
            return false;
        }

        memcpy(&data, m_Buffer + m_Offset, sizeof(data));
        m_Offset += sizeof(data);
        return true;
    }
    bool readData(void* data, unsigned int dataSize);

    inline void resetOffset() { m_Offset = 0; }
    inline void setIsReadModel(bool isReadModel){ m_IsReadMode = isReadModel; }
    inline void updateOffset(unsigned int offset) { m_Offset = offset; }
    inline unsigned int getOffset() { return m_Offset; }
    inline unsigned int getDataLength() { return m_DataLength; }
    inline unsigned int getBufferSize() { return m_BufferSize; }

	char *getBuffer(){ return m_Buffer; };
    void resetDataLength() { m_Offset = 0; m_DataLength = 0; }

private:
    bool checkDataLength(unsigned int dataLength);
    bool checkBufferSize(unsigned int newLength);

private:
    bool m_IsInit;              // ��ֹ�ظ���ʼ��
    bool m_IsReadMode;          // ��дģʽ
    unsigned int m_Offset;      // ��ǰ��ȡƫ��
    unsigned int m_DataLength;  // ��ǰʵ�����ݳ���
    unsigned int m_BufferSize;  // ��������С
    char* m_Buffer;
};

// �����л���ɷ����л��ӿ�
class ISerializable
{
    virtual bool serialize(BufferData& data) = 0;
    virtual bool unserialize(BufferData& data) = 0;
};


inline BufferData* newBufferData(int mainCMD, int subCmd)
{
	BufferData* buffer = new BufferData();
	buffer->init(256);
	buffer->writeData(0); //Head::length
	buffer->writeData(MakeCommand(mainCMD, subCmd));
	buffer->writeData(0); //Head::id
	return buffer;

	/*
	char* bu = buffer->getBuffer();
	Head* head = reinterpret_cast<Head*>(bu);
	head->length = buffer->getDataLength();
	int mainCmd = head->MainCommand();
	int subCmd = head->SubCommand();
	*/
}

inline int deleteBufferData(BufferData* buffer)
{
	if (nullptr != buffer)
	{
		delete buffer;
	}
	return 0;
}

#endif
