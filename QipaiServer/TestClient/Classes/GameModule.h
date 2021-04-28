#ifndef __GAMEMODULE_H__
#define __GAMEMODULE_H__

#include "helper/BaseModule.h"

class GameModule : public BaseModule
{
public:
    GameModule(void);
    ~GameModule(void);

    // �����յ�������
    virtual void processLogic(char* buffer, unsigned int len, IKxComm *target);
    // ������ֵĴ���
    virtual void processError(IKxComm *target);
    // �����¼�
    virtual void processEvent(int eventId, IKxComm* target);
};

#endif 
