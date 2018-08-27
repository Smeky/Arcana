#include "CZBuffer.h"

#include <CDimensional.h>

void CZBuffer::render( sf::RenderTarget& window, sf::RenderStates states ) {
    sort();

    for( size_t i = 0; i < ZLAYERS; i++ ) {
        for( auto it : m_dims[ i ] ) {
            window.draw( *it, states );
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
