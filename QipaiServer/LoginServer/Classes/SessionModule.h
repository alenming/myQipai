#ifndef __SESSION_MODULE_H__
#define __SESSION_MODULE_H__
/************************************************************************/
/*                  ���Ǹ���SessionServer������ģ��                     */
/************************************************************************/
#include "CommInterfaces.h"
#include "BaseModule.h"

class SessionModule : public BaseModule
{
public:
    SessionModule(void);
    virtual ~SessionModule(void);

    //����ͻ��˷�����Ϣ
    virtual void processLogic(char* buffer, unsigned int len, IComm *target);

	virtual int processLength(char* buffer, unsigned int len);

	virtual void processError(IComm *target);

};

#endif
