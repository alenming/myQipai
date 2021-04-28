#include "GateManager.h"


GateManager* GateManager::m_Instance = NULL;

GateManager::GateManager(void)
: m_Gate(NULL)
{
}

GateManager::~GateManager(void)
{
}


GateManager* GateManager::getInstance()
{
    if(NULL == m_Instance)
    {
        m_Instance = new GateManager();
    }

    return m_Instance;
}

void GateManager::destroy()
{
    if (NULL != m_Instance)
    {
        delete m_Instance;
        m_Instance = NULL;
    }
}

int GateManager::Send( char* buffer, unsigned int len)
{
	IKxComm *pKxComm = this->GetGate();

    if (pKxComm == NULL)
    {
        return -1;
    }

	return pKxComm->sendData(buffer, len);
}

