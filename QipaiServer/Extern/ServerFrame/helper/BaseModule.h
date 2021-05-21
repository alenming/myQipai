#ifndef __BASE_MODULE_H__
#define __BASE_MODULE_H__
#include "KxCommInterfaces.h"
#include "Head.h"

class BaseModule : public IKxModule
{
public:
    BaseModule(void){}
    virtual ~BaseModule(void){}
    
    virtual int processLength(char* buffer, unsigned int len)
    {
        if (len < sizeof(Head))
        {
            //len至少为4个字节, 上层调用如果不够4个字节则等待新数据.
			return sizeof(Head);
        }
        else
        {
			Head* head = (Head*)(buffer);
            //KXLOGDEBUG("CBaseModule Request Len %d main cmd %d sub cmd %d",
             //   head->length, head->MainCommand(), head->SubCommand());
            return head->length;
        }
    }
};

#endif
