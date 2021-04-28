#include "BufferData.h"
#include "core/KxMemPool.h"

BufferData::BufferData()
: m_IsInit(false)
, m_IsReadMode(false)
, m_Offset(0)
, m_DataLength(0)
, m_BufferSize(0)
, m_Buffer(NULL)
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

    m_Buffer = reinterpret_cast<char*>(kxMemMgrAlocate(bufferSize, m_BufferSize)); 
    if (NULL == m_Buffer)
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
    if (NULL == buffer || m_IsInit)
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

    m_Buffer = reinterpret_cast<char*>(kxMemMgrAlocate(pBuffData->getBufferSize(), m_BufferSize));
    if (NULL == m_Buffer)
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
    if (NULL != m_Buffer && !m_IsReadMode)
    {
        kxMemMgrRecycle(m_Buffer, m_BufferSize);
        m_Buffer = NULL;
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
        char* newBuffer = reinterpret_cast<char*>(kxMemMgrAlocate(newLength + m_DataLength, newBufferSize));
        if (NULL == newBuffer)
        {
            return false;
        }
        memcpy(newBuffer, m_Buffer, m_DataLength);
        kxMemMgrRecycle(m_Buffer, m_BufferSize);
        m_Buffer = newBuffer;
        m_BufferSize = newBufferSize;
    }
    return true;
}

