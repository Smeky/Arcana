#include "CTileLayer.h"

#include <CGame.h>

#include <iostream>
using namespace std;

CTileLayer::CTileLayer()
: m_tileSize        ( sf::Vector2f( 64, 64 ) )  // Remove me
{}

CTileLayer::~CTileLayer() {
    m_tiles.clear();
}

void CTileLayer::update( const sf::Time& delta, const sf::IntRect& indexBox ) {
    for( int y = indexBox.top; y < indexBox.height; y++ ) {
        for( int x = indexBox.left; x < indexBox.width; x++ ) {
            size_t index = y * (int)m_size.x + x;

            m_tiles[ index ].update( delta );
        }
    }
}

void CTileLayer::render( sf::RenderTarget& window, sf::RenderStates states, const sf::IntRect& indexBox ) const {
    for( int y = indexBox.top; y < indexBox.height; y++ ) {
        for( int x = indexBox.left; x < indexBox.width; x++ ) {
            size_t index = y * (int)m_size.x + x;

            window.draw( m_tiles[ index ], states );
        }
    }

//    for( auto& it : m_tiles ) {
//        window.draw( it, states );
//    }
}

void CTileLayer::setSize( const sf::Vector2f& size ) {
    m_size  = size;
}

void CTileLayer::addTile( const CTile& tile ) {
    m_tiles.push_back( tile );
}

void CTileLayer::addSprite( size_t index, const CSpriteWrapper& spriteWrapper ) {
    if( index >= m_tiles.size() ) return;

    m_tiles[ index ].addSprite( spriteWrapper );
}

void CTileLayer::setSolid( size_t index, bool solid ) {
    if( index >= m_tiles.size() ) return;

    m_tiles[ index ].setSolid( solid );
}

const CTile* CTileLayer::getTile( size_t index ) const {
    if( index >= m_tiles.size() ) return nullptr;

    return &m_tiles[ index ];
}
