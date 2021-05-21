#ifndef __GAMEMODULE_H__
#define __GAMEMODULE_H__

#include "BaseModule.h"

class GameModule : public BaseModule
{
public:
    GameModule(void);
    ~GameModule(void);

    // �����յ�������
    virtual void processLogic(char* buffer, unsigned int len, IComm *target);
    // ������ֵĴ���
    virtual void processError(IComm *target);
    // �����¼�
    virtual void processEvent(int eventId, IComm* target);
};

#endif 
