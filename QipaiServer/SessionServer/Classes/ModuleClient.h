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

    //�����ͻ��˷�����Ϣ
    virtual void processLogic(char* buffer, unsigned int len, IKxComm *target);
    //�ͻ�����Session�������Ͽ���֪ͨ��Ϸ�������������������Դ
    virtual void processError(IKxComm *target);

private:
    //��Ҷ���
    void userDisconnect(IKxComm *target);
};

#endif