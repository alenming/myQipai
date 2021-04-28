#ifndef __SESSION_MODULE_H__
#define __SESSION_MODULE_H__
/************************************************************************/
/*                  ���Ǹ���SessionServer������ģ��                     */
/************************************************************************/

#include "BaseModule.h"

class LoginSessionModule : public BaseModule
{
public:
    LoginSessionModule(void);
    virtual ~LoginSessionModule(void);

    //����ͻ��˷�����Ϣ
    virtual void processLogic(char* buffer, unsigned int len, IKxComm *target);

	virtual int processLength(char* buffer, unsigned int len);

	virtual void processError(IKxComm *target);

};

#endif
