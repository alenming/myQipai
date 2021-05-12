#ifndef __SESSION_MODULE_H__
#define __SESSION_MODULE_H__
/************************************************************************/
/*                  ���Ǹ���SessionServer������ģ��                     */
/************************************************************************/

#include "BaseModule.h"

class SessionModule : public BaseModule
{
public:
    SessionModule(void);
    virtual ~SessionModule(void);

    //�����ͻ��˷�����Ϣ
    virtual void processLogic(char* buffer, unsigned int len, IKxComm *target);

	virtual int processLength(char* buffer, unsigned int len);

	virtual void processError(IKxComm *target);

};

#endif