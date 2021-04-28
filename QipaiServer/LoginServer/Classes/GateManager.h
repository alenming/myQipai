/*
 * 入口管理器，用于管理唯一的Session入口
 */

#ifndef __GATE_MANAGER_H__
#define __GATE_MANAGER_H__

#include "KXServer.h"
#include <vector>

class GateManager
{
private:
    GateManager(void);
    virtual ~GateManager(void);

public:
    
    static GateManager* getInstance();
    static void destroy();

    //发送给Session, 内容不限定, 功能自己定制.
    int Send(char* buffer, unsigned int len);

    //设置指定的网关
    void SetGate(IKxComm* gate)
    {
        m_Gate = gate;
    }
    //获得指定网关
    IKxComm* GetGate()
    {
        return m_Gate;
    }

private:

    IKxComm* m_Gate;
    static GateManager* m_Instance;
};

#endif
