#ifndef __GAMEMODULE_H__
#define __GAMEMODULE_H__

#include "BaseModule.h"

class GameModule : public BaseModule
{
public:
    GameModule(void);
    ~GameModule(void);

    // 处理收到的数据
    virtual void processLogic(char* buffer, unsigned int len, IComm *target);
    // 处理出现的错误
    virtual void processError(IComm *target);
    // 处理事件
    virtual void processEvent(int eventId, IComm* target);
};

#endif 
