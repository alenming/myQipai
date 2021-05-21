#include "GateManager.h"


GateManager* GateManager::m_Instance = nullptr;

GateManager::GateManager(void)
: m_Gate(nullptr)
{
}

GateManager::~GateManager(void)
{
}


GateManager* GateManager::getInstance()
{
    if(nullptr == m_Instance)
    {
        m_Instance = new GateManager();
    }

    return m_Instance;
}

void GateManager::destroy()
{
    if (nullptr != m_Instance)
    {
        delete m_Instance;
        m_Instance = nullptr;
    }
}

int GateManager::Send( char* buffer, unsigned int len)
{
	IComm *pKxComm = this->GetGate();

    if (pKxComm == nullptr)
    {
        return -1;
    }

	return pKxComm->sendData(buffer, len);
}

