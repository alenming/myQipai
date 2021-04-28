#ifndef _DB_STORER_H_
#define _DB_STORER_H_
#pragma comment(lib,"libmysql.lib")
#include <string>
#include "mysqld/mysql.h"
#include "IStorer.h"


/************************************************************************
	DataBaseManager::getInstance()->InitWithXML(STORAGE_XML_FILE);
 	Storage* m_pStorage1 = DataBaseManager::getInstance()->GetStorage(STORE_MYSQL);
	MysqlStorer *pStorer1 = reinterpret_cast<MysqlStorer*>(m_pStorage1->GetStorer(2));
	pStorer1->Execute("SELECT *FROM player WHERE id = 10001");
	pStorer1->GetRowResult();
	cout << pStorer1->GetFieldValue(0);
************************************************************************/

//����ع� ��δ��װ
enum emExecuteType
{
	EXECTYPE_NONE,		//
	EXECTYPE_INSERT,	//���� 
	EXECTYPE_DELETE,	//ɾ�� 
	EXECTYPE_UPDATE,	//����
	EXECTYPE_SELECT,	//��ѯ
};

enum emDBStorerState
{
	ESTORER_NONE,				//
	ESTORER_CONCATENATE,		// ������
	ESTORER_DISCONNEXION,		// �Ͽ�����
};

class MysqlStorer : public IStorer
{
public: 
    //���캯������������
    MysqlStorer();
    MysqlStorer(std::string dbname, std::string user, std::string pwd, std::string addr, int port);
    ~MysqlStorer();
    
    //���ò���
    //�������ݿ�����
    void SetDbName(std::string db);
    //���ò����ߵ��ʺ�
    void SetDbUser(std::string user);
    //���ò����ߵ�����
    void SetDbPwd(std::string pwd);
    //�������ݿ�������ĵ�ַ
    void SetDbAddress(std::string addr);
    //�������ݿ�������Ķ˿�
    void SetDbPort(int port);
    
    //�������
    //�����ݿ��������������
    bool Connect();
    //�����ݿ���������½�������
    bool ReConnect(std::string dbname, std::string user, std::string pwd, std::string addr, int port);
    //�ر����ݿ�����
    bool Disconnect();
    //ִ��SQL���
    bool Execute(const char* sql,emExecuteType execType = EXECTYPE_SELECT);
    //�����ѽ��������Ƿ�Ͽ�
    bool Ping();
    //������ݿ��Ƿ����
    bool IsDbExist(std::string db);
    //�������ݿ�����ѡ��
    bool SetOption(enum mysql_option opt, const void* arg);
	//����ʼ
	bool TransactionStart();
	//�����ύ
	bool TransactionCommit();
	//����ع�
	bool TransactionRollback();
	//������� Mysql �﷨����ľ����������Ĳ�����
    //bool TransactionEnd();

    //�������
	//��ȡSQL���Ӱ�������
	unsigned long GetAffectedCount();
    //��ȡ��¼����
    unsigned long GetRowCount();
    //��ȡ���ݿ���������
    bool StoreResult();
    //�ͷ����ݿ���������
    bool FreeResult();
    //��ȡһ�в�ѯ���
    bool GetRowResult();
    //��ȡ�ֶε���ֵ
    char* GetFieldValue(unsigned int index);
    //��ȡ�ֶ���Ŀ
    int GetFieldCount();
    //��ȡ�ֶγ���
    int GetFieldLength(unsigned int index);
	//��ȡ�����������ID
	int GetLastInsertID();

    //Ҫ�������ݽṹ��ʱ��,���ô˺���,�÷��ص����ݲ嵽Sql�����,PS һ��Sql��ѯֻ�ܵ�һ��
    char* GetDataStr(void* obj, unsigned int objSize);

private:
    //������ݿ�ָ���Ƿ��ʼ��
    inline bool CheckDBPtr();

private:
    std::string   m_DbName;             //���ݿ�����      
    std::string   m_DbUser;             //���ݿ�����ߵ��ʺ�
    std::string   m_DbPwd;              //���ݿ�����ߵ�����
    std::string   m_DbAddress;          //���ݿ�������ĵ�ַ
    int           m_DbPort;             //���ݿ�������Ķ˿� 
	MYSQL*        m_ConnectPtr;         //���Ӽ����
    MYSQL_RES*    m_ResultPtr;          //�������� 
    MYSQL_ROW     m_RowRecord;          //��¼�����
	unsigned char m_CntState;			//����״̬
    char          m_DataStr[7872];      //Ҫ��������ݽṹ (InteractModel ���������Ҫ 3932*2 + 2)
 };


#endif //_DB_STORER_H_
