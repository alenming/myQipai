#ifndef __GAME_MODULE_H__
#define __GAME_MODULE_H__

#include "BaseModule.h"
#include "GameInterface.h"

class ConnectModule : public BaseModule
{
public:
    ConnectModule(void);
    virtual ~ConnectModule(void);

	bool init(IGameEvent *pSink = NULL);

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
