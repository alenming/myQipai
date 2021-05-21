#ifndef __SESSION_LISTENER_H__
#define __SESSION_LISTENER_H__

#include "TCPListener.h"

class SessionListener : public TCPListener
{
public:
    SessionListener();
    virtual ~SessionListener(void);

    virtual TCPClienter* onAccept(KXCOMMID client);
};

#endif //__SESSION_LISTENER_H__
