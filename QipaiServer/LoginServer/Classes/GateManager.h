/*
 * ��ڹ����������ڹ���Ψһ��Session���
 */

#ifndef __GATE_MANAGER_H__
#define __GATE_MANAGER_H__


#include "CommInterfaces.h"

class GateManager
{
private:
    GateManager(void);
    virtual ~GateManager(void);

public:
    
    static GateManager* getInstance();
    static void destroy();

    //���͸�Session, ���ݲ��޶�, �����Լ�����.
    int Send(char* buffer, unsigned int len);

    //����ָ��������
    void SetGate(IComm* gate)
    {
        m_Gate = gate;
    }
    //���ָ������
    IComm* GetGate()
    {
        return m_Gate;
    }

private:

    IComm* m_Gate;
    static GateManager* m_Instance;
};

#endif
