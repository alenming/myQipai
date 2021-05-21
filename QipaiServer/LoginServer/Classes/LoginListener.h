#ifndef __SESSION_LISTENER_H__
#define __SESSION_LISTENER_H__

#include "TCPListener.h"

class LoginListener : public TCPListener
{
public:
    LoginListener();
    virtual ~LoginListener(void);

    virtual TCPClienter* onAccept(KXCOMMID client);
};

#endif //__SESSION_LISTENER_H__
