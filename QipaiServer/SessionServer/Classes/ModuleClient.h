/*
*   
*	�ͻ�����ҵ�������
*
*/
#ifndef __SESSION_MODULE_H__
#define __SESSION_MODULE_H__

#include "BaseModule.h"


class ModuleClient : public BaseModule
{
public:
    ModuleClient(void);
    virtual ~ModuleClient(void);

    //����ͻ��˷�����Ϣ
    virtual void processLogic(char* buffer, unsigned int len, IComm *target);
    //�ͻ�����Session�������Ͽ���֪ͨ��Ϸ�������������������Դ
    virtual void processError(IComm *target);
};

#endif
