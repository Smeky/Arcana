#ifndef CMESSAGELISTENER_H
#define CMESSAGELISTENER_H

#include "CMessage.h"

class CMessageListener {
public:
    virtual        ~CMessageListener();

    virtual void    handleMsg       ( CMessage* msg ) = 0;
};

#endif // CMESSAGELISTENER_H
