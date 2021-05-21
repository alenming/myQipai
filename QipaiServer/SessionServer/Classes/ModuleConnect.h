#ifndef __GAME_MODULE_H__
#define __GAME_MODULE_H__

#include "BaseModule.h"
#include "EventInterface.h"

class ModuleConnect : public BaseModule
{
public:
    ModuleConnect(void);
    virtual ~ModuleConnect(void);

	bool init(IGameEvent *pSink = nullptr);

    //��Ϸ���������ط�����Э�飬ת����ָ����ǰ��
    virtual void processLogic(char* buffer, unsigned int len, IKxComm *target);

    //����Ϸ�������Ͽ����ӣ���������
    //����������newһ����ͨѶID�仯���ɿ�����������Դ��
    virtual void processError(IKxComm *target);
    virtual void processEvent(int eventId, IKxComm* target);

    bool processUserEvent(int nType, IKxComm* target);

private:
	IGameEvent* m_pSink;    // ������Ϸ�߼��ص�
};

#endif
