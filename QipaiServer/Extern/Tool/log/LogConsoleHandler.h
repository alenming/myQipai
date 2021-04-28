#ifndef __KX_LOG_CONSOLE_HANDLER_H__
#define __KX_LOG_CONSOLE_HANDLER_H__

#include "ILogHandler.h"

class LogConsoleHandler : public ILogHandler
{
public:
	LogConsoleHandler()
	{	
	}
	virtual ~LogConsoleHandler()
	{
	}
    virtual bool onLog(int level, const std::string& log);
};



#endif // __KX_LOG_CONSOLE_HANDLER_H__
