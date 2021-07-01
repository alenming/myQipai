#ifndef __LOGIN_LISTENER_H__
#define __LOGIN_LISTENER_H__

#include "TCPListener.h"

class LoginListener : public TCPListener
{
public:
    LoginListener();
    virtual ~LoginListener(void);

    virtual TCPClienter* onAccept(COMMID client);
};

#endif //__LOGIN_LISTENER_H__
