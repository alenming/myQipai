#ifndef __GAMEMODULE_H__
#define __GAMEMODULE_H__

#include "commnication/KxCommInterfaces.h"
#include "helper/BaseModule.h"

class GameModule : public BaseModule
{
public:
    GameModule(void);
    ~GameModule(void);

    // 处理收到的数据
    virtual void processLogic(char* buffer, unsigned int len, IKxComm *target);
    // 处理出现的错误
    virtual void processError(IKxComm *target);
    // 处理事件
    virtual void processEvent(int eventId, IKxComm* target);
};

#endif 
