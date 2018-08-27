#ifndef CMESSAGESYSTEM_H
#define CMESSAGESYSTEM_H

#include "memory"
#include "string"
#include "unordered_map"
#include "vector"

#include "CMessageListener.h"
#include "CMessage.h"
#include "CTimer.h"

class CMessageSystem {
public:
                ~CMessageSystem     ();

    void        update              ( const sf::Time& delta );

    void        subscribe           ( CMessageListener* listener, std::string msg );
    void        unsubscribe         ( CMessageListener* listener, std::string msg );
    void        unsubscribeAll      ( CMessageListener* listener );

    void        broadcast           ( CMessage* msg );
    void        broadcastDelayedMsg ( CMessage* msg, float delayInSeconds );

private:
    struct DelayedMessage {
        CMessage*   message     = nullptr;
        CTimer      timer;
    };

private:
    std::unordered_map<std::string,std::vector<CMessageListener*>> m_listeners;

    std::vector<DelayedMessage> m_delayedMsgs;
};


#endif // CMESSAGESYSTEM_H
