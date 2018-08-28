#include "CZBuffer.h"

#include <CDimensional.h>
#include <CGame.h>
#include <Utility.h>

void CZBuffer::render( sf::RenderTarget& window, sf::RenderStates states ) {
    sort();

    // Store window box + offset
    sf::FloatRect windowBox( -50, -50, 0, 0 );
    windowBox.width     = CGame::GameWindow.getSize().x + 100;
    windowBox.height    = CGame::GameWindow.getSize().y + 100;

    // Store camera pos
    sf::Vector2f cameraPos = CGame::WorldManager.Camera.getPos();

    for( size_t i = 0; i < ZLAYERS; i++ ) {
        for( auto it : m_dims[ i ] ) {
            // Store relative box
            sf::FloatRect box = it->getBox();
            box.left    -= cameraPos.x;
            box.top     -= cameraPos.y;

            // Make sure box is inside the window, prevent rendering outside window
            if( box.intersects( windowBox ) ) {
                window.draw( *it, states );
            }
        }
    }
}

void CZBuffer::clean() {
    for( size_t i = 0; i < ZLAYERS; i++ ) {
        m_dims[ i ].clear();
    }
}

void CZBuffer::insert( const CDimensional* dim ) {
    if( dim == nullptr ) return;

    int layer = dim->getZLayer();

         if( layer < 0 ) layer = 0;
    else if( layer > ZLAYERS - 1 ) layer = ZLAYERS - 1;

    m_dims[ layer ].push_back( dim );
}

void CZBuffer::sort() {
    for( size_t i = 0; i < ZLAYERS; i++ ) {
        if( !m_dims[ i ].empty() ) {
            m_dims[ i ].sort( []( const CDimensional* first, const CDimensional* second ) {
                return first->getPosSize().y < second->getPosSize().y;
            } );
        }
    }
}
