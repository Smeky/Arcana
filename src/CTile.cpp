#include "CTile.h"

CTile::CTile()
: m_solid       ( false )
{}

void CTile::update( const sf::Time& delta ) {

}

void CTile::draw( sf::RenderTarget& window, sf::RenderStates states ) const {
    states.transform *= getTransform();

    for( auto& it : m_sprites ) {
        window.draw( it, states );
    }
}

void CTile::addSprite( const CSpriteWrapper& spriteWrapper ) {
    m_sprites.push_back( spriteWrapper );
}

void CTile::clearSprites() {
    m_sprites.clear();
}

void CTile::setSolid( bool solid ) {
    m_solid     = solid;

    resetCollBox();
}

bool CTile::isSolid() const {
    return m_solid;
}
