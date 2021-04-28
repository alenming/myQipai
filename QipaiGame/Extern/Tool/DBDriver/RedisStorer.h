#ifndef __REDIS_STORER_H__
#define __REDIS_STORER_H__

#include "hiredis/hiredis.h"
#include "IStorer.h"
#include <set>
#include <list>
#include <vector>
#include <map>

//DataBaseManager::getInstance()->InitWithXML(STORAGE_XML_FILE);
//Storage* m_pStorage = DataBaseManager::getInstance()->GetStorage(STORE_REDIS);
//RedisStorer *pStorer = reinterpret_cast<RedisStorer*>(m_pStorage->GetStorer(DATA_BASE_TEST));
//if (pStorer)
//{
//	string key = "adfaf";
//	//pStorer->SetString(key, key);
//	string re = "";
//	pStorer->GetString(key, re);
//	MINGLOG(re.c_str());
//}

#define MAX_RANGESCORE 999999999		//ZRANGE ȡ�������ֵ

enum ResultStatus
{
    DATAEXCEPTION   = -3,              // �����쳣
    RETRYFAILED     = -2,              // ����ʧ��
    FAILED          = -1,              // ʧ�ܷ���
    SUCCESS         = 0                // �ɹ�����
};

struct RankData
{
    int id;
    int source;
};

struct RedisBinaryData
{
    unsigned int length;
	char* data;

	RedisBinaryData()
		:length(0)
		, data(NULL)
	{
	}

    RedisBinaryData(const char *d, int len)
    {
		data = (char*)malloc(len);
		memcpy(data, d, len);
		length = len;
    }

	~RedisBinaryData()
	{
		if (NULL != data)
		{
			free(data);
			length = 0;
		}
	}

	void Write(const char *d, int len)
	{
		if (NULL != data)
		{
			free(data);
			length = 0;
		}
		data = (char*)malloc(len);
		memcpy(data, d, len);
		length = len;
	}
};

class RedisStorer : public IStorer
{
public:

    RedisStorer();
    ~RedisStorer();

public:
    
	// ����ָ��redis������, ʹ������ʽ����
    int Connect(std::string ip, int port,std::string passward);

	// ����֮ǰ��ip�˿�����redis������
    int reconnect();

	// ���redis����
	redisContext *GetRedisContext();

    //ɾ��KEY
    int DelKey(const std::string &key);

	//����ɾ��KEY
	int DelKey(std::vector<std::string> &keys);

	//������ȡKEY(��keyΪǰ׺)
	int GetKey(const std::string &key, std::vector<std::string> vec);

    //�ж�Key�Ƿ����
    int ExistKey(const std::string &key);

    //ָ��Key��ĳ��ʱ������
    int ExpireAt(const std::string &key, unsigned int timestamp);

    //ָ��Key�೤ʱ��֮�����
    int Expire(const std::string &key, int seconds);

    //��ȡKey���ڵ�ʣ��ʱ��
    int TTL(const std::string &key, int &seconds);

	//ȡ��key�Ĺ���ʱ��
	int Persist(const std::string &key);

	//*********************Set��������*********************
    //�����Int�Ƕ�����
    int SetAdd(const std::string &key,const std::string &value);
    	
    int SetAdd(const std::string &key, const char* value, unsigned int len);

	int SetAdd(const std::string &key, int& value);

    int SetDel(const std::string &key, const std::string &value);
    	
    int SetDel(const std::string &key, const char* value, unsigned int len);
	
	int SetDel(const std::string &key, int &value);

    int GetSetAll(const std::string &key, std::set<std::string> &setret);
    
	//Set�洢���Ƕ�����ָ
    int GetSetAll(const std::string &key, std::set<int> &setret);

	//Set�洢�����ַ���
	int GetSetAllString(const std::string &key, std::set<int> &setret);
    
    //�ж��ַ���value�Ƿ���Set�У��Ƿ���0
    int IsSet(const std::string &key, std::string value);
    
    //�ж϶���������value�Ƿ���Set�У��Ƿ���0
    int IsSet(const std::string &key, const char* value, unsigned int len);

    int SetMove(const std::string &src, const std::string &dest, const std::string &value);
    	
   	int SetMove(const std::string &src, const std::string &dest, const char* value, unsigned int len);

    int SetScard();
		
    //*********************�ַ�������**********************
    
    //Get String by String Key
    //key: in
    //value: out
    //len: out
    //return 0 Success other faile
    int GetString(const std::string &key, char** value, unsigned int &len);
    	
    int GetString(const std::string &key, std::string &value);
    
    int GetString(const std::string &key, int &value);

    //Set String by String key
    //value: in
    //len: in, the length of value
    //return: 0 success
    int SetString(const std::string &key, const char* value, unsigned int len);
    	
    int SetString(const std::string &key, const std::string &value);

    int SetString(const std::string &key, int value);
    
    //��ȡString��һС��(���������ݽṹ��ĳ���ֶ�)
    int GetStringRange(const std::string &key, int offset, int len, char** value);
    
    //����String��һС��(���������ݽṹ��ĳ���ֶ�)
    //offset String��Ӧ�ֶε�ƫ��  valuelenҪ���õ���ֵ���� value Ҫ���õ���ֵ
    //return: 0 success  
    int SetStringRange(const std::string &key, int offset, int valuelen, char* value);

    int IncrString(const std::string &key, int &value);
		
    //*********************ZSet��������*********************
    //Add or Update User's Rank By source
    //int key, id, source
    //return: 0 success
	int Zadd(const std::string &key, int id, int source);
    
    //��minsource��maxsourceΪ0ʱ����Sorted-Set�ĳ�Ա����
    //���򷵻���η����е��������
    //return counts
    int ZCount(const std::string &key, int &count, int minsource = 0, int maxsource = 0);
    
    //��ȡĳ����Ա������
    int ZRank(const std::string &key, int id, int &rank);

	//��ȡĳ����Ա�Ļ���
	int ZScore(const std::string &key, int id, int &Score);
    
    //��ȡĳ�����е�����Լ����ǵĵ÷�,Ĭ�ϻ�ȡtop 10
    int ZRange(const std::string &key, std::vector<RankData> &ranks, int start = 0, int stop = 9);

	//ɾ����������
	int ZDel(const std::string &key, int start = 0, int stop = -1);

	//*********************Hash��������*********************
    //�����Int���ַ���
    //��������һ��Hash "hmset %s %s "field1 value1 [field2 value2] .....
    // or "hmset %b %b "field1 field1len value1 value1len [field2 value2] .....
    int SetHash(const std::string &key, const char *format, ...);
        
    int SetHash(const std::string &key, std::map<int, int> &mapset);
    
    //��������һ��Hash
    int SetHash(const std::string &key, std::map<std::string, std::string> &mapset);

	//��������һ��Hash
	int SetHash(const std::string &key, std::map<int, std::string> &mapset);
    
	//��������hash
	int SetHash(const std::string &key, std::map<std::string, int> &mapset);

    //��ȡ����Hash
    int GetHash(const std::string &key, std::map<std::string, std::string> &mapret);
    
	//��ȡ����hash
    int GetHash(const std::string &key, std::map<int, int> &mapret);

	//��ȡ����hash
	int GetHash(const std::string &key, std::map<std::string, int> &mapret);
    
	//��ȡ����hash
	int GetHash(const std::string &key, std::map<int, std::string> &mapret);

    //������е�hash����������
    int GetHash(const std::string &key, std::map<int, RedisBinaryData> &mapret);

	//��ȡ���е�hash����������
	int GetHash(const std::string &key, std::map<std::string, RedisBinaryData> &mapret);

    //��ȡĳ��Field�ĺ���, mapret��key�����
    int GetHashByField(const std::string &key, std::map<std::string, std::string> &mapret);
    
	//��ȡĳ��Field�ĺ���, mapret��key�����
	int GetHashByField(const std::string &key, std::map<int, int> &mapret);

    //��ȡĳ��Field�ĺ���
    int GetHashByField(const std::string &key, const std::string &field, std::string &value);

    //���ĳ��field����
    int GetHashByField(const std::string &key, const std::string &field, int &value);

    //��ȡĳ��Field�ĺ��� %d %d
    int GetHashByField(const std::string &key, int field, int &value);

    //���ĳ��Fieldֵ
    int GetHashByField(const std::string &key, int field, std::string &value);

	//�������filedֵ
	int GetHashBy2Field(const std::string &key, int field1, int field2, int &value1, int &value2);

    //���field�Ķ�����ֵ
    int GetHashByField(const std::string &key, std::string field, char *value, int &len);
	
    //���field�Ķ�����ֵ
    int GetHashByField(const std::string &key, int field, char *value, int &len);

    //����ĳ��Field�ĺ���
    int SetHashByField(const std::string &key, const std::string &field, const std::string &value);

    //����ĳ��field�ĺ���
    int SetHashByField(const std::string &key, const std::string &field, int &value);
	
    //��ȡĳ��Field�ĺ��� %d %d
    int SetHashByField(const std::string &key, int field, int value);

    //����ĳ��field�ĺ���
    int SetHashByField(const std::string &key, int field, std::string value);

    //����ĳ��field�Ķ�����ֵ
    int SetHashByField(const std::string &key, const std::string &field, char *value, int len);

    //����ĳ��field�Ķ�����ֵ
    int SetHashByField(const std::string &key, int field, char *value, int len);

    //ɾ��ĳ��field
    int DelHashByField(const std::string &key, const std::string &field);

    //ɾ��ĳ��field
    int DelHashByField(const std::string &key, int field);

    //�ж�Field�Ƿ����
    int HashFieldExist(const std::string &key, const std::string &field);

    //�ж�Field�Ƿ����
    int HashFieldExist(const std::string &key, int field);

    //�������Լ�key filed value
    //valueΪ��������
    //valueΪ��������
    //value��������ӻ���ٺ��ֵ
    int IncreHashByField(const std::string &key, const std::string &field, int &value);

    //�������Լ�key filed value
    //valueΪ��������
    //valueΪ��������
    //value��������ӻ���ٺ��ֵ
    int IncreHashByField(const std::string &key, int field, int &value);

    // ���һ��hash key�������
    int HashLen(const std::string &key, long long &value);

    //*********************List��������*********************

    // ��һ��������ֵ�����ڱ�ͷ
    int ListPush(const std::string &key, char *value, int len);
    
    // ����int�б�
    int ListPush(const std::string &key, int value);

	// ����string���б�
	int ListPush(const std::string &key, std::string &str);

    // block ���������� pop�׸�Ԫ�أ� ����б���û��Ԫ�أ���������Ԫ�زŻ᷵��
    //int ListBlockPop(const std::string &key, char *value, int len, int seconds);

    // non-block ������������ pop�׸�Ԫ�أ� �Ƴ����������׸�ֵ
    int ListPop(const std::string &key, char *value, int &len);

	// �Ƴ��������׸�Ԫ��
	int ListPop(const std::string &key, int &value);

    // �Ƴ�������β��Ԫ��
    int ListRPop(const std::string &key, int &value);

    // ������е�Ԫ��, �Ƕ����ƻ�ȡ�ӿ�
    int ListRange(const std::string &key, std::set<std::string> &str);

    // ��ȡint�б�
    int ListRange(const std::string &key, std::vector<int> &str);

	//��ȡstring�б�
	int ListRange(const std::string &key, std::vector<std::string> &str);

	//���Ԫ�ص��б���
	int ListRange(const std::string &key, std::list<int> &lst);
    // ���ָ��index�Ķ�����Ԫ�أ� 
    // 0�����׸�Ԫ�أ�1����ڶ���Ԫ�أ�
    // -1�������һ��Ԫ�أ� -2�������ڶ���Ԫ��
    // ���ָ��λ�õĶ�����
    int ListIndex(const std::string &key, int index, char *value, int &len);

    // ����ַ���
    int ListIndex(const std::string &key, int index, std::string &value);

    //count > 0 : �ӱ�ͷ��ʼ���β�������Ƴ��� value ��ȵ�Ԫ�أ�����Ϊ count
    //count < 0 : �ӱ�β��ʼ���ͷ�������Ƴ��� value ��ȵ�Ԫ�أ�����Ϊ count �ľ���ֵ��
    //count = 0 : �Ƴ����������� value ��ȵ�ֵ��
    int ListRemove(const std::string &key, int count, std::string &value);
    int ListRemove(const std::string &key, int count, int value);

private:

	redisContext*   m_pContext;         //Redis���������Ķ���
	redisReply*     m_pReply;           //Redis�������
	
	int             m_Port;
    std::string     m_Ip;
    char            m_Buffer[2048];
	std::string     m_pssward;
};


#endif //__REDIS_STORER_H__
