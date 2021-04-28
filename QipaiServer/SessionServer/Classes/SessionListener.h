#ifndef __SESSION_LISTENER_H__
#define __SESSION_LISTENER_H__

#include "KxTCPListener.h"

class SessionListener : public KxTCPListener
{
public:
    SessionListener();
    virtual ~SessionListener(void);

    virtual KxTCPClienter* onAccept(KXCOMMID client);
};

#endif //__SESSION_LISTENER_H__
