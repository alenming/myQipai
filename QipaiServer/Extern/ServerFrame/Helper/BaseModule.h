#ifndef __BASE_MODULE_H__
#define __BASE_MODULE_H__
#include "CommInterfaces.h"
#include "Head.h"

class BaseModule : public IModule
{
public:
    BaseModule(void){}
    virtual ~BaseModule(void){}
    
    virtual int processLength(char* buffer, unsigned int len)
    {
        if (len < sizeof(Head))
        {
            //len����Ϊ4���ֽ�, �ϲ�����������4���ֽ���ȴ�������.
			return sizeof(Head);
        }
        else
        {
			Head* head = (Head*)(buffer);
            //KXLOGDEBUG("CBaseModule Request Len %d main cmd %d sub cmd %d", head->length, head->MainCommand(), head->SubCommand());
            return head->length;
        }
    }
};

#endif