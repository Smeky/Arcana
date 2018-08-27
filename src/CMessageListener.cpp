#include "CMessageListener.h"

#include "CGame.h"

CMessageListener::~CMessageListener() {
    CGame::MessageSystem.unsubscribeAll( this );
}
