/*
*   MemPool �ڴ��
*   �����ڴ���Ƭ����߷����ڴ�Ч�ʣ��ڴ渴��
*/
#ifndef __MEMPOOL_H__
#define __MEMPOOL_H__

#include <set>
#include <map>
#include <list>

typedef std::map<unsigned int, std::list<void*>*>   MemML;
typedef std::map<unsigned int, std::set<void*>*>    MemMS;

class KxMemPool
{
public:
    KxMemPool();
    virtual ~KxMemPool();

    // �����СΪsize���ڴ�,ʵ�ʷ����ڴ��С���ܻ����,��Ϊ�ڲ���size�����˶���
    void* memAlocate(unsigned int size);

    // �����ڴ�,��MemPool������ڴ����,���յ��ڴ��СΪsize,�ɹ�����0
    int memRecycle(void* mem, unsigned int size);

    //��ʾ�ڴ�ص�ǰ״̬
    void memDumpInfo();

    // �Զ���������Ҫ������ڴ���С
    // Ĭ����С����32�ֽ�
    // 33-64����64�ֽ�
    // 65-128����128�ֽ�
    // 129-256����256�ֽڣ��Դ�����
    inline unsigned int memFitSize(unsigned int size);

private:
    //��ȡһ���Է������������
    inline unsigned int memFitCounts(unsigned int size);

    //��չ�ڴ��
    int memExtend(unsigned int size, std::list<void*>* plist, std::set<void*>* pset);

    //��չ���ڴ��
    int memExtendNewSize(unsigned int size);

    //�ͷ�ָ����С���ڴ�
    int memReleaseWithSize(unsigned int size);

    //��Ҫ�ͷŶ��ٿ��ڴ�
    unsigned int memRelsaseCount(unsigned int size, unsigned int freecount, unsigned int stubcount);

    //���ݵ�ǰˮλ��״̬�Զ�����Ƿ��ͷ��ڴ�
    int memAutoRelease(unsigned size, std::list<void*>* plist, std::set<void*>* pset);

private:
    MemML m_Free;                                           //�����ڴ��
    MemMS m_Stub;                                           //�ڴ����

    unsigned int m_AlocatedSize;                            //�ѷ����С
    unsigned int m_WaterMark;                               //���ˮλ
    unsigned int m_MinAlocateSize;                          //������С�ڴ��СΪ 1 << m_MinAlocateSize

    static const unsigned int MAX_WATER_MARK = 1 << 30;     //���ˮλ
    static const unsigned int MAX_POOL_SIZE = 1 << 31;      //�ڴ��������� 

    static const unsigned int MEM_BASE_COUNT = 32;          //ÿ�η�������
    static const unsigned int MEM_SIZE_MIN = 1 << 20;       //�ڴ��������С 1M
    static const unsigned int MEM_SIZE_MID = 1 << 26;       //�ڴ���������� 64M
    static const unsigned int MEM_SIZE_BIG = 1 << 27;       //�ڴ���������� 128M
};

class KxMemManager
{
public:
    static KxMemManager* getInstance();
    static void destroy();

    // �����СΪsize���ڴ�,ʵ�ʷ����ڴ��С���ܻ����
    void* memAlocate(unsigned int size);
    void* memAlocate(unsigned int size, unsigned int& outSize);

    // �����ڴ�,��MemPool������ڴ����,���յ��ڴ��СΪsize,�ɹ�����0
    int memRecycle(void* mem, unsigned int size);

    //��ʾ�ڴ�ص�ǰ״̬
    void memDumpInfo();

private:
    KxMemManager();
    ~KxMemManager();

private:
    KxMemPool* m_MemPool;
    static KxMemManager* m_Instance;
};

inline void* kxMemMgrAlocate(unsigned int size)
{
    return new char[size];
    //return KxMemManager::getInstance()->memAlocate(size);
}

inline void* kxMemMgrAlocate(unsigned int size, unsigned int& outSize)
{
    outSize = size;
    return new char[size];
    //return KxMemManager::getInstance()->memAlocate(size, outSize);
}

inline int kxMemMgrRecycle(void* mem, unsigned int size)
{
    delete[] (char*)mem;
    return size;
    //return KxMemManager::getInstance()->memRecycle(mem, size);
}

#define KXSAFE_RECYCLE(mem, size) if(NULL != (mem)) { kxMemMgrRecycle(mem, size); }



#endif