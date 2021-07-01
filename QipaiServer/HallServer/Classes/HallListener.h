#ifndef __HALL_LISTENER_H__
#define __HALL_LISTENER_H__

#include "TCPListener.h"

class HallListener : public TCPListener
{
public:
    HallListener();
    virtual ~HallListener(void);

    virtual TCPClienter* onAccept(COMMID client);
};

#endif //__HALL_LISTENER_H__
