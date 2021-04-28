#ifndef _KX_I_LOGMANAGER_H__
#define _KX_I_LOGMANAGER_H__

#include <string>

// ��־����
enum KX_LOG_LEVEL
{
	KX_LOG_INFO,         // ��Ϣ����
	KX_LOG_DEBUG,        // ���Լ���
    KX_LOG_ERROR         // ���󼶱�
};

// ��־����������
// ʵ����־�Ĵ���
class ILogHandler
{
public:
	ILogHandler()
		:m_Tag(1)
	{ }
	virtual ~ILogHandler() { }
	// �����¼��־ʱ�ص���������־��Ϣ
	virtual bool onLog(int level, const std::string& log) = 0;
	// ����ӽ�KxLoggerʱ�ص�
	virtual void onEnter() { }
	// ��KxLogger���Ƴ�ʱ�ص�
	virtual void onExit() { }
	// ���������־ʱ�ص�
	virtual void onClean() { }

	inline int getHandleId()
	{
		return m_HandlerId;
	}

	inline void setHandleId(int handleId)
	{
		m_HandlerId = handleId;
	}

	int getTag()
	{
		return m_Tag;
	}

	inline void setTag(int tag)
	{
		m_Tag = tag;
	}

	virtual std::string getFileName()
	{
		return "";
	}

protected:
	int m_HandlerId;
	int m_Tag;
};

#endif //_KX_I_LOGMANAGER_H__
