#ifndef _KX_I_LOGMANAGER_H__
#define _KX_I_LOGMANAGER_H__

#include <string>

// 日志级别
enum KX_LOG_LEVEL
{
	KX_LOG_INFO,         // 信息级别
	KX_LOG_DEBUG,        // 调试级别
    KX_LOG_ERROR         // 错误级别
};

// 日志处理句柄对象
// 实现日志的处理
class ILogHandler
{
public:
	ILogHandler()
		:m_Tag(1)
	{ }
	virtual ~ILogHandler() { }
	// 程序记录日志时回调，传入日志信息
	virtual bool onLog(int level, const std::string& log) = 0;
	// 被添加进KxLogger时回调
	virtual void onEnter() { }
	// 从KxLogger中移除时回调
	virtual void onExit() { }
	// 程序清除日志时回调
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
