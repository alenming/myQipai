#include "BufferData.h"

BufferData::BufferData()
: m_IsInit(false)
, m_IsReadMode(false)
, m_Offset(0)
, m_DataLength(0)
, m_BufferSize(0)
, m_Buffer(nullptr)
{
}


BufferData::~BufferData()
{
    clean();
}

bool BufferData::init(int bufferSize)
{
    if (m_IsInit)
    {
        return false;
    }
	m_BufferSize = bufferSize;
	m_Buffer = reinterpret_cast<char*>(new char[bufferSize]);
    if (nullptr == m_Buffer)
    {
        return false;
    }
    
    m_DataLength = 0;
    m_Offset = 0;
    m_IsInit = true;
    m_IsReadMode = false;
    return true;
}

bool BufferData::init(char* buffer, unsigned int dataLength)
{
    if (nullptr == buffer || m_IsInit)
    {
        return false;
    }

    m_Buffer = buffer;
    m_BufferSize = dataLength;
    m_DataLength = dataLength;
    m_Offset = 0;
    m_IsInit = true;
    m_IsReadMode = true;
    return true;
}

bool BufferData::init(BufferData* pBuffData)
{
    if (m_IsInit)
    {
        return false;
    }
	m_BufferSize = pBuffData->getBufferSize();
	m_Buffer = reinterpret_cast<char*>(new char[pBuffData->getBufferSize()]);
    if (nullptr == m_Buffer)
    {
        return false;
    }

    memcpy(m_Buffer, pBuffData->getBuffer(), pBuffData->getDataLength());
    m_DataLength = pBuffData->getDataLength();
    m_Offset = pBuffData->getOffset();
    m_IsInit = true;
    m_IsReadMode = false;
    return true;
}

void BufferData::clean()
{
    if (nullptr != m_Buffer && !m_IsReadMode)
    {
		delete[](char*)(m_Buffer);
        m_Buffer = nullptr;
    }
    m_IsInit = false;
}

bool BufferData::writeData(const void* data, unsigned int length)
{
    if (!checkBufferSize(length))
    {
        return false;
    }
    else
    {
        memcpy(m_Buffer + m_DataLength, data, length);
        m_DataLength += length;
        return true;
    }
}

bool BufferData::readData(void* data, unsigned int dataSize)
{
    if (!checkDataLength(dataSize))
    {
        return false;
    }

    memcpy(data, m_Buffer + m_Offset, dataSize);
    m_Offset += dataSize;
    return true;
}

bool BufferData::checkDataLength(unsigned int dataLength)
{
    return (m_IsInit && m_IsReadMode
        && m_DataLength >= m_Offset + dataLength);
}

bool BufferData::checkBufferSize(unsigned int newLength)
{
    if (!m_IsInit || m_IsReadMode)
    {
        return false;
    }

    if (newLength + m_DataLength > m_BufferSize)
    {
        unsigned int newBufferSize = 0;
		newBufferSize = newLength + m_DataLength;
		char* newBuffer = reinterpret_cast<char*>(new char[newLength + m_DataLength]);
        if (nullptr == newBuffer)
        {
            return false;
        }
        memcpy(newBuffer, m_Buffer, m_DataLength);
		delete[](char*)(m_Buffer);
        m_Buffer = newBuffer;
        m_BufferSize = newBufferSize;
    }
    return true;
}

