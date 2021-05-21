#ifndef __SESSION_MODULE_H__
#define __SESSION_MODULE_H__
/************************************************************************/
/*                  这是个与SessionServer的连接模型                     */
/************************************************************************/
#include "CommInterfaces.h"
#include "BaseModule.h"

class SessionModule : public BaseModule
{
public:
    SessionModule(void);
    virtual ~SessionModule(void);

    //处理客户端发来消息
    virtual void processLogic(char* buffer, unsigned int len, IComm *target);

	virtual int processLength(char* buffer, unsigned int len);

	virtual void processError(IComm *target);

};

#endif
