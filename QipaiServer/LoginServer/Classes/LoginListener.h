#ifndef __SESSION_LISTENER_H__
#define __SESSION_LISTENER_H__

#include "KxTCPListener.h"

class LoginListener : public KxTCPListener
{
public:
    LoginListener();
    virtual ~LoginListener(void);

    virtual KxTCPClienter* onAccept(KXCOMMID client);
};

#endif //__SESSION_LISTENER_H__
