#ifndef __IDBPROXY_H__
#define __IDBPROXY_H__



#ifdef DBPROXY_EXPORTS
#define DBPROXY_API __declspec(dllexport)
#else
#define DBPROXY_API __declspec(dllimport)
#endif


#ifdef _DEBUG
#define DBPROXY_DLL_NAME	TEXT("DBProxy_d.dll")			//��� DLL ����
#else
#define DBPROXY_DLL_NAME	TEXT("DBProxy.dll")			//��� DLL ����
#endif



// ���ݿ�������󷵻ػص��ӿ�
struct IDBProxyRetSink
{
	// ���ݿ�������󷵻ػص�����
	// nReturn: �ο� IDBEngine.h �������ݿ����󷵻�ֵ����
	// pOutData: ������ݻ�����ָ��
	// nOutLen: ������ݻ���������
	virtual void OnRet(long nIndex, int nUserID, int nRequestID, char *pInData, int nInLen,
		int nReturn, char *pOutData, int nOutLen) = 0;

};

// ���ݿ�����û�����ص�
struct IDBProxyUserSink
{
	// ����û�����ص�����
	virtual void OnAddUser(DWORD dwServerID, long lUserID, long lActorID) = 0;
	// ɾ���û�����ص�����
	virtual void OnDelUser(DWORD dwServerID, long lUserID) = 0;

};

// ���ݿ����ӿ�
struct IDBProxy
{
	// �������ݿ����
	virtual void Release(void) = 0;

	// �������ݿ�
	// nIndex: ������ˮ��, Ψһ��Ӧÿ������
	// nRequestID: ����ID
	// nUserID: ������������û�ID, ����ȡģ���и��ؾ���
	// pInData: �������ݻ�����
	// nInDataLen: �������ݻ���������
	// pRet: ���󷵻ػص��ӿ�
	virtual BOOL Request(long nIndex, int nRequestID, int nUserID,
		char *pInData, int nInDataLen, IDBProxyRetSink *pRet, DWORD dwServerID) = 0;

	//�رշ����������ڴ��������ݵı���
	virtual void CloseServer(DWORD dwServerID) = 0;
};

// �������ݿ����(�������)
extern "C" __declspec(dllexport) BOOL CreateDBProxy(DWORD dwSaveTimes,IDBProxy **ppDBProxy, char *szKey,
													ILog *pTrace, ILogicThread *pLogicThread, ITimerAxis *pTimeAxis,
													IDBProxyUserSink *pUserSink);
typedef BOOL (__cdecl *ProcCreateDBProxy)(DWORD dwSaveTimes,IDBProxy **ppDBProxy, char *szKey,
										  ILog *pTrace, ILogicThread *pLogicThread, ITimerAxis *pTimeAxis,
										  IDBProxyUserSink *pUserSink);

// ���ݿ������������
class CDBProxyCreateHelper
{
public:
	// ���캯��
	CDBProxyCreateHelper()
	{
		m_hDll = NULL;
		m_pDBProxy = NULL;
	}

	// ��������
	~CDBProxyCreateHelper()
	{
		Close();
	}

	// ����ͨ�����ݿ�����
	BOOL Create(DWORD dwSaveTimes,char *szKey, 
				ILog *pTrace,
				ILogicThread *pLogicThread,
				ITimerAxis *pTimeAxis, 
				IDBProxyUserSink *pUserSink)
	{
		if (m_hDll != NULL)
		{
			return FALSE;
		}

		try
		{
			m_hDll = ::LoadLibrary(DBPROXY_DLL_NAME);
			if (NULL == m_hDll)
			{
				throw "Cann't load DBProxy.dll!";
			}

			ProcCreateDBProxy pProc = NULL;
			pProc = (ProcCreateDBProxy)::GetProcAddress(m_hDll, "CreateDBProxy");
			if (NULL == pProc)
			{
				throw "Cann't get process CreateDBProxy!";
			}

			if (FALSE == pProc(dwSaveTimes,&m_pDBProxy, szKey, pTrace, pLogicThread,
				pTimeAxis, pUserSink))
			{
				throw "Create DBProxy error!";
			}

			return TRUE;
		}
		catch (LPCSTR szMsg)
		{
			OutputDebugString(szMsg);
			OutputDebugString("\r\n");
		}
		catch (...)
		{
			OutputDebugString("DBProxy unknow error!");
			OutputDebugString("\r\n");
		}

		return FALSE;
	}

	// �ͷ����ݿ����
	void Close(void)
	{
		if (m_pDBProxy != NULL)
		{
			m_pDBProxy->Release();
			m_pDBProxy = NULL;
		}

		if (m_hDll != NULL)
		{
			::FreeLibrary(m_hDll);
			m_hDll = NULL;
		}
	}

public:
	IDBProxy	*m_pDBProxy;	// ���ݿ����ָ��

private:
	HINSTANCE	m_hDll;			// ��̬����

};

#endif // __IDBPROXY_H__
