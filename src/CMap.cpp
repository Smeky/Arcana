#include "CMap.h"

#include <CGame.h>
#include <CTileLayer.h>
#include <Utility.h>

#include <iostream>
using namespace std;

CMap::CMap()
: m_ID              ( 0 )
, m_tileSize        ( sf::Vector2f( 64, 64 ) )
, m_debugGrid       ( false )
{}

CMap::~CMap() {
    close();
}

void CMap::close() {
    while( !m_layers.empty() )      delete m_layers.back(),     m_layers.pop_back();
    while( !m_topLayers.empty() )   delete m_topLayers.back(),  m_topLayers.pop_back();
}

void CMap::update( const sf::Time& delta ) {
    sf::IntRect indexBox = calcIndex( CGame::WorldManager.Camera );

    for( auto it : m_layers ) {
        it->update( delta, indexBox );
    }
    for( auto it : m_topLayers ) {
        it->update( delta, indexBox );
    }
}

void CMap::render( sf::RenderTarget& window, sf::RenderStates states ) const {
    sf::IntRect indexBox = calcIndex( CGame::WorldManager.Camera );

    for( auto it : m_layers ) {
        it->render( window, states, indexBox );
    }
}

void CMap::renderTopLayer( sf::RenderTarget& window, sf::RenderStates states ) const {
    sf::IntRect indexBox = calcIndex( CGame::WorldManager.Camera );

    for( auto it : m_topLayers ) {
        it->render( window, states, indexBox );
    }

    if( m_debugGrid ) {
        debugRenderGrid( window, states, indexBox );
    }
}

void CMap::setID( size_t ID ) {
    m_ID    = ID;
}

size_t CMap::getID() const {
    return m_ID;
}

void CMap::addTileLayer( CTileLayer* layer ) {
    m_layers.push_back( layer );
}

void CMap::addTopTileLayer( CTileLayer* layer ) {
    m_topLayers.push_back( layer );
}

void CMap::setSize( const sf::Vector2f& size ) {
    m_size      = size;
}

sf::Vector2f CMap::getSize() const {
    return m_size;
}

sf::Vector2f CMap::getSizeInTiles() const {
    return Util::divideVectors<float>( m_size, m_tileSize );
}

C_Dimensionals CMap::getNearby( const sf::Vector2f& pos, const sf::Vector2f& size ) {
    sf::IntRect indexBox = calcIndex( pos, size );

    C_Dimensionals tiles;

    sf::Vector2f inTiles = getSizeInTiles();

    for( auto it : m_layers ) {
        for( int y = indexBox.top; y < indexBox.height; y++ ) {
            for( int x = indexBox.left; x < indexBox.width; x++ ) {
                size_t index = y * (int)inTiles.x + x;

                const CTile* tile = it->getTile( index );

                if( tile != nullptr ) {
                    if( tile->isSolid() ) {
                        tiles.push_back( tile );
                    }
                }
            }
        }
    }

    return tiles;
}

C_Dimensionals CMap::getNearby( const CDimensional* dim ) {
    sf::IntRect indexBox = calcIndex( *dim );

    C_Dimensionals tiles;

    sf::Vector2f inTiles = getSizeInTiles();

    for( auto it : m_layers ) {
        for( int y = indexBox.top; y < indexBox.height; y++ ) {
            for( int x = indexBox.left; x < indexBox.width; x++ ) {
                size_t index = y * (int)inTiles.x + x;

                const CTile* tile = it->getTile( index );

                if( tile != nullptr ) {
                    if( tile->isSolid() ) {

                        tiles.push_back( tile );
                    }
                }
            }
        }
    }

    return tiles;
}

bool CMap::debugDisplayGrid() {
    m_debugGrid     = !m_debugGrid;

    return m_debugGrid;
}

void CMap::debugRenderGrid( sf::RenderTarget& window, sf::RenderStates states, const sf::IntRect& indexBox ) const {
    int right   = indexBox.width    - indexBox.left;
    int bottom  = indexBox.height   - indexBox.top;

    sf::RectangleShape  box[ right * bottom ];
    sf::Vector2f        pos[ right * bottom ];

    for( int i = 0; i < right * bottom; i++ ) {
        box[ i ].setSize( m_tileSize );
        box[ i ].setFillColor( sf::Color::Transparent );
        box[ i ].setOutlineColor( sf::Color::Red );
        box[ i ].setOutlineThickness( 1.0 );
    }


    int index = 0;
    for( int y = indexBox.top; y < indexBox.height; y++ ) {
        for( int x = indexBox.left; x < indexBox.width; x++ ) {
            pos[ index ] = sf::Vector2f( x * m_tileSize.x, y * m_tileSize.y );

            index++;
        }
    }

    sf::Vector2f        inTiles = getSizeInTiles();

    for( auto it : m_layers ) {
        index = 0;
        for( int y = indexBox.top; y < indexBox.height; y++ ) {
            for( int x = indexBox.left; x < indexBox.width; x++ ) {
                int tileIndex = y * inTiles.x + x;

                if( it->getTile( tileIndex )->isSolid() ) {
                    box[ index ].setOutlineColor( sf::Color::Blue );
                }

                index++;
            }
        }
    }

    for( int i = 0; i < right * bottom; i++ ) {
        sf::Transformable transformable;

        transformable.setPosition( pos[ i ] );

        sf::RenderStates tempState = states;
        tempState.transform *= transformable.getTransform();

        window.draw( box[ i ], tempState );
    }
}

/** PRIVATE */

sf::IntRect CMap::calcIndex( const sf::Vector2f& pos, const sf::Vector2f& size ) const {
    sf::IntRect box;

    box.left    = floorf( pos.x / m_tileSize.x - 1 );
    box.top     = floorf( pos.y / m_tileSize.y - 1 );
    box.width   = floorf( ( pos.x + size.x ) / m_tileSize.x + 1 );
    box.height  = floorf( ( pos.y + size.y ) / m_tileSize.y + 1 );

    sf::Vector2f inTiles    = getSizeInTiles();

    if( box.left    < 0 )               box.left    = 0;
    if( box.top     < 0 )               box.top     = 0;
    if( box.width   > (int)inTiles.x )  box.width   = (int)inTiles.x;
    if( box.height  > (int)inTiles.y )  box.height  = (int)inTiles.y;

    return box;
}

sf::IntRect CMap::calcIndex( const CDimensional& dim ) const {
    sf::IntRect box;

    sf::Vector2f pos    = dim.getPos();
    sf::Vector2f size   = dim.getSize();

    box.left    = floorf( pos.x / m_tileSize.x - 1 );
    box.top     = floorf( pos.y / m_tileSize.y - 1 );
    box.width   = floorf( ( pos.x + size.x ) / m_tileSize.x + 1 );
    box.height  = floorf( ( pos.y + size.y ) / m_tileSize.y + 1 );

    sf::Vector2f inTiles    = getSizeInTiles();

    if( box.left    < 0 )               box.left    = 0;
    if( box.top     < 0 )               box.top     = 0;
    if( box.width   > (int)inTiles.x )  box.width   = (int)inTiles.x;
    if( box.height  > (int)inTiles.y )  box.height  = (int)inTiles.y;

    return box;
}
