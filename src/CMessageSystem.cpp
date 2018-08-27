#include "CMessageSystem.h"

#include "CGame.h"

#include "iostream"
using namespace std;

CMessageSystem::~CMessageSystem () {
    // Prevents memory crash on game exit
    m_listeners.clear();

    while( !m_delayedMsgs.empty() ) delete m_delayedMsgs.back().message, m_delayedMsgs.pop_back();
}

void CMessageSystem::update( const sf::Time& delta ) {
    for( auto it = m_delayedMsgs.begin(); it != m_delayedMsgs.end(); ) {
        it->timer.update( delta );

        if( it->timer.isTime() ) {
            DelayedMessage temp = *it;

            it = m_delayedMsgs.erase( it );

            broadcast( temp.message );
        }
        else {
            it++;
        }
    }
}

void CMessageSystem::subscribe( CMessageListener* listener, std::string msg ) {
    if( m_listeners[ msg ].empty() ) {
        m_listeners[ msg ].push_back( listener );
    }
    else {
        for( auto it : m_listeners[ msg ] ) {
            if( it == listener ) {
                std::cout << "Error: Subscribe is already registered to this: " << msg << " message." << std::endl;

                return;
            }
        }

        m_listeners[ msg ].push_back( listener );
    }
}

void CMessageSystem::unsubscribe( CMessageListener* listener, std::string msg ) {
    if( !m_listeners[ msg ].empty() ) {
        for( auto it = m_listeners[ msg ].begin(); it != m_listeners[ msg ].end(); it++ ) {
            if( *it == listener ) {
                m_listeners[ msg ].erase( it );

                return;
            }
        }

        std::cout << "Warning: Unable to unsubscribe: " << msg << ". Subscriber is missing." << std::endl;
    }
}

void CMessageSystem::unsubscribeAll( CMessageListener* listener ) {
    for( auto mapIT = m_listeners.begin(); mapIT != m_listeners.end(); mapIT++ ) {
        for( auto it = mapIT->second.begin(); it != mapIT->second.end(); ) {
            if( *it == listener ) {
                it = mapIT->second.erase( it );
            }
            else {
                it++;
            }
        }
    }
}

void CMessageSystem::broadcast( CMessage* msg ) {
    if( msg == nullptr ) return;

    std::string msgID = msg->getMsg();

    if( !m_listeners[ msgID ].empty() ) {
        for( auto it : m_listeners[ msgID ] ) {
            it->handleMsg( msg );
        }
    }

    CGame::ScriptSystem.fireEvent( msg->getMsg() );

    delete msg;
    msg = nullptr;
}

void CMessageSystem::broadcastDelayedMsg( CMessage* msg, float delayInSeconds ) {
    if( msg == nullptr ) return;

    DelayedMessage delayed;

    delayed.message = msg;
    delayed.timer.setLimit( delayInSeconds );

    m_delayedMsgs.push_back( delayed );
}
