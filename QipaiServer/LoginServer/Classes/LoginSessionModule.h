#ifndef __SESSION_MODULE_H__
#define __SESSION_MODULE_H__
/************************************************************************/
/*                  这是个与SessionServer的连接模型                     */
/************************************************************************/

#include "BaseModule.h"

class LoginSessionModule : public BaseModule
{
public:
    LoginSessionModule(void);
    virtual ~LoginSessionModule(void);

    //处理客户端发来消息
    virtual void processLogic(char* buffer, unsigned int len, IKxComm *target);

	virtual int processLength(char* buffer, unsigned int len);

	virtual void processError(IKxComm *target);

};

#endif
