#include "CMapGen.h"

#include <lua.hpp>
#include <list>
#include <algorithm>

#include <CMap.h>
#include <CMapRoom.h>
#include <CTile.h>
#include <CTileLayer.h>
#include <CRNG.h>
#include <CGame.h>
#include <Utility.h>
#include <LuaWrapper.h>

#include <iostream>
using namespace std;

CMapGen::Vec2DBool      CMapGen::m_empty;
CMapGen::MapLayer       CMapGen::m_emptyData;
int                     CMapGen::m_mapWidth     = 64;
int                     CMapGen::m_mapHeight    = 64;
int                     CMapGen::m_emptyTresh   = 40;
int                     CMapGen::m_solidTresh   = 40;
int                     CMapGen::m_passageRad   = 3;
std::string             CMapGen::m_tileset;
CMapGen::VecMapLayer    CMapGen::m_emptyLayers;
CMapGen::VecMapLayer    CMapGen::m_solidLayers;

CMap* CMapGen::generateMap( lua_State* state, int index ) {
    setupData( state, index );

    CMap* map = new CMap();

    m_empty.clear();

    map->setSize( sf::Vector2f( m_mapWidth * TILESIZE, m_mapHeight * TILESIZE ) );

    int emptyLayerCount = m_emptyLayers.empty() ? 1 : m_emptyLayers.size();
    int solidLayerCount = m_solidLayers.empty() ? 1 : m_solidLayers.size();

    m_empty     = setupLayer( m_emptyData.chance, m_emptyData.smoothing );

    processMap();

    std::vector<Vec2DBool> emptyLayers;
    std::vector<Vec2DBool> solidLayers;

    for( int i = 0; i < emptyLayerCount; i++ ) {
        if( m_emptyLayers.empty() ) {
            emptyLayers.push_back( setupLayer( 0.5, 4 ) );
        }
        else {
            emptyLayers.push_back( setupLayer( m_emptyLayers[ i ].chance, m_emptyLayers[ i ].smoothing ) );
        }
    }

    for( int i = 0; i < solidLayerCount; i++ ) {
        if( m_emptyLayers.empty() ) {
            solidLayers.push_back( setupLayer( 0.5, 4 ) );
        }
        else {
            solidLayers.push_back( setupLayer( m_solidLayers[ i ].chance, m_solidLayers[ i ].smoothing ) );
        }
    }

    CTileLayer* layer = new CTileLayer();
    layer->setSize( sf::Vector2f( m_mapWidth, m_mapHeight ) );

    for( int y = 0; y < m_mapHeight; y++ ) {
        for( int x = 0; x < m_mapWidth; x++ ) {
            CTile tile;

            tile.setPos( x * TILESIZE, y * TILESIZE );
            tile.setSize( TILESIZE, TILESIZE );

            if( m_empty[ y ][ x ] ) {
                for( int i = 0; i < emptyLayerCount; i++ ) {
                    CSpriteWrapper sprite;
                    sprite.setTexture( m_tileset );

                    // Specifies if sprite should be used ( sprite wont be used in case non-base layer )
                    bool    valid   = true;

                    sf::IntRect clip( 0, 0, TILESIZE, TILESIZE );

                    int     bitmask = countBitmask( emptyLayers[ i ], sf::Vector2i( x, y ) );
                    int     row     = floorf( bitmask / 4 );

                    if( emptyLayers[ i ][ y ][ x ] ) {
                        clip.left   = clip.width * ( bitmask - row * 4 );
                        clip.top    = clip.height * row;
                    }
                    else {
                        if( i == 0 ) {
                            clip.left   = clip.width * 4;
                        }
                        else {
                            valid = false;
                        }
                    }

                    if( valid ) {
                        // Move clip to correct position on tileset ( based on layers )
                        clip.top    += ( clip.height * 4 ) * i;

                        sprite.setClip( clip );
                        sprite.setSize( TILESIZE, TILESIZE );

                        if( i > 0 && bitmask == 15 ) {
                            tile.clearSprites();
                        }

                        tile.addSprite( sprite );
                    }
                }
            }
            else {
                for( int i = 0; i < solidLayerCount; i++ ) {
                    CSpriteWrapper sprite;
                    sprite.setTexture( m_tileset );


                    sf::IntRect clip( 0, 0, TILESIZE, TILESIZE );

                    int     bitmask = countBitmask( m_empty, sf::Vector2i( x, y ), false );
                    int     row     = floorf( bitmask / 4 );

                    if( bitmask == 1 || bitmask == 4 || bitmask == 5 ) {
                        m_empty[ y ][ x ]   = true;

                        x--;

                        break;
                    }

                    clip.left   = clip.width * ( bitmask - row * 4 );
                    clip.top    = clip.height * row;

                    // Move clip to correct position on tileset ( based on layers )
                    clip.left   += clip.width * 5;
                    clip.top    += ( clip.height * 4 ) * i;

                    sprite.setClip( clip );
                    sprite.setSize( TILESIZE, TILESIZE );

                    if( i > 0 && bitmask == 15 ) {
                        tile.clearSprites();
                    }

                    tile.addSprite( sprite );
                    tile.setSolid( true );
                }
            }

            layer->addTile( tile );
        }
    }

    map->addTileLayer( layer );

    return map;
}

void CMapGen::setupData( lua_State* state, int index ) {
    if( lua_istable( state, index ) ) {
        sf::Vector2i mapSize    = Util::vectorFromTable<int>( state, index, "size" );
        m_mapWidth  = mapSize.x;
        m_mapHeight = mapSize.y;

        lWrapper::valueFromLuaTable<int>( state, index, "emptyTresh", m_emptyTresh );
        lWrapper::valueFromLuaTable<int>( state, index, "solidTresh", m_solidTresh );
        lWrapper::valueFromLuaTable<int>( state, index, "passageRad", m_passageRad );
        lWrapper::stringFromLuaTable( state, index, "tileset", m_tileset );

        lua_getfield( state, index, "layers" );

        if( lua_istable( state, - 1 ) ) {
            bool baseSet = false;

            for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
                MapLayer data;

                lWrapper::valueFromLuaTable<float>( state, lua_gettop( state ), "chance",    data.chance );
                lWrapper::valueFromLuaTable<int>(   state, lua_gettop( state ), "smoothing", data.smoothing );

                if( !baseSet ) {
                    m_emptyData = data;

                    baseSet = true;
                }
                else {
                    bool empty  = true;

                    lWrapper::valueFromLuaTable<bool>( state, lua_gettop( state ), "empty", empty );

                    if( empty ) {
                        m_emptyLayers.push_back( data );
                    }
                    else {
                        m_solidLayers.push_back( data );
                    }
                }

            }
        }
        else {
            std::cout << "Error: Unable to generate map. field -layers- is missing" << std::endl;
        }

        lua_pop( state, 1 );
    }
    else {
        std::cout << "Error: Unable to setup map data out of given value: " << lua_typename( state, lua_type( state, index ) ) << std::endl;
    }
}

CMapGen::Vec2DBool CMapGen::setupLayer( float spawnChance, int smoothing ) {
    Vec2DBool  layer;

    for( int y = 0; y < m_mapHeight; y++ ) {
        layer.push_back( std::vector<bool>() );

        for( int x = 0; x < m_mapWidth; x++ ) {
            layer[ y ].push_back( true );

            if( x != 0 && x != m_mapWidth - 1 && y != 0 && y != m_mapHeight - 1 ) {
                if( spawnChance < CRNG::percentRandom() ) {
                    layer[ y ][ x ] = false;
                }
            }
        }
    }

    for( int i = 0; i < smoothing; i++ ) {
        smooth( layer );
    }

    return layer;
}

void CMapGen::smooth( Vec2DBool& layer ) {
    for( int y = 0; y < m_mapHeight; y++ ) {
        for( int x = 0; x < m_mapWidth; x++ ) {
            int neighbours  = countNeighbours( layer, x, y );

            if( neighbours < 4 ) {
                layer[ y ][ x ] = true;
            }
            else if( neighbours > 4 ) {
                layer[ y ][ x ] = false;
            }
        }
    }
}

int CMapGen::countNeighbours( Vec2DBool& layer, int xIndex, int yIndex ) {
    int counter = 0;

    for( int y = yIndex - 1; y <= yIndex + 1; y++ ) {
        for( int x = xIndex - 1; x <= xIndex + 1; x++ ) {
            if( x > 0 && x < m_mapWidth - 1 && y > 0 && y < m_mapHeight - 1 ) {
                if( x != xIndex || y != yIndex ) {
                    if( !layer[ y ][ x ] ) {
                        counter++;
                    }
                }
            }
            else {
                counter++;
            }
        }
    }

    return counter;
}

void CMapGen::processMap() {
    const size_t treshold = 50;

    // Filled tiles
    VecOfVectors2i regions = getRegions( false );

    for( auto region : regions ) {
        if( region.size() < treshold ) {
            for( sf::Vector2i vec : region ) {
                m_empty[ vec.y ][ vec.x ] = true;
            }
        }
    }

    // Empty tiles
    regions = getRegions( true );

    std::vector<CMapRoom*>   rooms;

    for( auto region : regions ) {
        if( region.size() < treshold ) {
            for( sf::Vector2i vec : region ) {
                m_empty[ vec.y ][ vec.x ] = false;
            }
        }
        else {
            rooms.push_back( new CMapRoom( region, m_empty ) );
        }
    }

    // Sort rooms based on their size ( from largest to smallest )
    std::sort( rooms.begin(), rooms.end(), []( const CMapRoom* first, const CMapRoom* second ) {
        return first->m_size > second->m_size;
    } );

    if( !rooms.empty() ) {
        rooms[ 0 ]->m_isMainRoom            = true;
        rooms[ 0 ]->m_accessibleFromMain    = true;

        connectRooms( rooms );
    }

    while( !rooms.empty() ) delete rooms.back(), rooms.pop_back();
}

CMapGen::VecOfVectors2i CMapGen::getRegions( bool type ) {
    VecOfVectors2i regions;
    bool flags[ m_mapHeight ][ m_mapWidth ];

    for( int i = 0; i < m_mapHeight; i++ ) {
        for( int j = 0; j < m_mapWidth; j++ ) {
            flags[ i ][ j ] = false;
        }
    }

    for( int y = 0; y < m_mapHeight; y++ ) {
        for( int x = 0; x < m_mapWidth; x++ ) {
            if( !flags[ y ][ x ] && m_empty[ y ][ x ] == type ) {
                Vectors2i region = getRegion( x, y );
                regions.push_back( region );

                for( sf::Vector2i vec : region ) {
                    flags[ vec.y ][ vec.x ] = true;
                }
            }
        }
    }

    return regions;
}

CMapGen::Vectors2i CMapGen::getRegion( int xStart, int yStart ) {
    // Vector of indexes of all tiles in one region
    CMapGen::Vectors2i tiles;

    // Determines if tile has already been looked at
    bool flags[ m_mapHeight ][ m_mapWidth ];

    for( int i = 0; i < m_mapHeight; i++ ) {
        for( int j = 0; j < m_mapWidth; j++ ) {
            flags[ i ][ j ] = false;
        }
    }

    // Store tile type at given index
    bool tileType = m_empty[ yStart ][ xStart ];

    // Queue of tiles that will be added to result vector
    std::list<sf::Vector2i> queue;


    queue.push_back( sf::Vector2i( xStart, yStart ) );
    flags[ yStart ][ xStart ] = true;

    while( queue.size() > 0 ) {
        sf::Vector2i tile = queue.front();
        queue.pop_front();

        tiles.push_back( tile );

        for( int y = tile.y - 1; y <= tile.y + 1; y++ ) {
            for( int x = tile.x - 1; x <= tile.x + 1; x++ ) {
                if( isInMap( x, y ) && ( x == tile.x || y == tile.y ) ) {
                    if( !flags[ y ][ x ] && m_empty[ y ][ x ] == tileType ) {
                        flags[ y ][ x ] = true;

                        queue.push_back( sf::Vector2i( x, y ) );
                    }
                }
            }
        }
    }

    return tiles;
}

void CMapGen::connectRooms( std::vector<CMapRoom*> rooms, bool forceAccessibilityFromMain ) {
    std::vector<CMapRoom*>   aRooms;
    std::vector<CMapRoom*>   bRooms;

    if( forceAccessibilityFromMain ) {
        for( auto room : rooms ) {
            if( room->m_accessibleFromMain ) {
                bRooms.push_back( room );
            }
            else {
                aRooms.push_back( room );
            }
        }
    }
    else {
        aRooms  = rooms;
        bRooms  = rooms;
    }

    int             bestDistance    = 0;
    sf::Vector2i    bestTileA;
    sf::Vector2i    bestTileB;
    CMapRoom*       bestRoomA       = nullptr;
    CMapRoom*       bestRoomB       = nullptr;
    bool            connectionFound = false;

    for( size_t i = 0; i < aRooms.size(); i++ ) {
        if( !forceAccessibilityFromMain ) {
            connectionFound = false;

            if( !aRooms[ i ]->m_connectedRooms.empty() ) {
                continue;
            }
        }

        for( size_t j = 0; j < bRooms.size(); j++ ) {
            if( aRooms[ i ] == bRooms[ j ] || aRooms[ i ]->isConnected( bRooms[ j ] ) ) {
                continue;
            }

            for( size_t aIndex = 0; aIndex < aRooms[ i ]->m_edgeTiles.size(); aIndex++ ) {
                for( size_t bIndex = 0; bIndex < bRooms[ j ]->m_edgeTiles.size(); bIndex++ ) {
                    sf::Vector2i aTile  = aRooms[ i ]->m_edgeTiles[ aIndex ];
                    sf::Vector2i bTile  = bRooms[ j ]->m_edgeTiles[ bIndex ];
                    int distance        = (int)( powf( aTile.x - bTile.x, 2 ) + powf( aTile.y - bTile.y, 2 ) + 0.5 );

                    if( distance < bestDistance || !connectionFound ) {
                        bestDistance    = distance;
                        connectionFound = true;
                        bestTileA       = aTile;
                        bestTileB       = bTile;
                        bestRoomA       = aRooms[ i ];
                        bestRoomB       = bRooms[ j ];
                    }
                }
            }
        }

        if( connectionFound && !forceAccessibilityFromMain ) {
            createPassage( bestRoomA, bestRoomB, bestTileA, bestTileB );
        }
    }

    if( connectionFound && forceAccessibilityFromMain ) {
        createPassage( bestRoomA, bestRoomB, bestTileA, bestTileB );

        connectRooms( rooms, true );
    }

    if( !forceAccessibilityFromMain ) {
        connectRooms( rooms, true );
    }
}

void CMapGen::createPassage( CMapRoom* aRoom, CMapRoom* bRoom, sf::Vector2i aTile, sf::Vector2i bTile ) {
    CMapRoom::connect( aRoom, bRoom );

    Vectors2i line = getLine( aTile, bTile );

    for( sf::Vector2i vec : line ) {
        makeCircle( vec, 2 );
    }
}

void CMapGen::makeCircle( const sf::Vector2i& tile, int radius ) {
    for( int y = - radius; y <= radius; y++ ) {
        for( int x = - radius; x <= radius; x++ ) {
            if( x * x + y * y <= radius * radius ) {
                int drawX   = tile.x + x;
                int drawY   = tile.y + y;

                if( isInMap( drawX, drawY ) ) {
                    m_empty[ drawY ][ drawX ] = true;
                }
            }
        }
    }
}

CMapGen::Vectors2i CMapGen::getLine( const sf::Vector2i& from, const sf::Vector2i& to ) {
    Vectors2i line;

    int x   = from.x;
    int y   = from.y;

    int dx  = to.x - from.x;
    int dy  = to.y - from.y;

    bool inverted   = false;

    int step            = signbit( dx ) ? - 1 : 1;
    int gradientStep    = signbit( dy ) ? - 1 : 1;

    int longest         = abs( dx );
    int shortest        = abs( dy );

    if( longest < shortest ) {
        inverted    = true;
        longest     = abs( dy );
        shortest    = abs( dx );

        step            = signbit( dy ) ? - 1 : 1;
        gradientStep    = signbit( dx ) ? - 1 : 1;
    }

    int gradientAccumulation    = longest / 2;

    for( int i = 0; i < longest; i++ ) {
        line.push_back( sf::Vector2i( x, y ) );

        if( inverted ) {
            y   += step;
        }
        else {
            x   += step;
        }

        gradientAccumulation += shortest;
        if( gradientAccumulation >= longest ) {
            if( inverted ) {
                x   += gradientStep;
            }
            else {
                y   += gradientStep;
            }

            gradientAccumulation -= longest;
        }
    }

    return line;
}

int CMapGen::countBitmask( Vec2DBool& layer, const sf::Vector2i& tile ) {
    int     bitmask = 0;
    bool    type    = layer[ tile.y ][ tile.x ];

    if( tile.x - 1 < 0 ) {
        bitmask += 1;
    }
    else {
        if( layer[ tile.y ][ tile.x - 1 ] == type ) {
            bitmask += 1;
        }
    }

    if( tile.y + 1 > m_mapHeight - 1 ) {
        bitmask += 2;
    }
    else {
        if( layer[ tile.y + 1 ][ tile.x ] == type ) {
            bitmask += 2;
        }
    }

    if( tile.x + 1 > m_mapWidth - 1 ) {
        bitmask += 4;
    }
    else {
        if( layer[ tile.y ][ tile.x + 1 ] == type ) {
            bitmask += 4;
        }
    }

    if( tile.y - 1 < 0 ) {
        bitmask += 8;
    }
    else {
        if( layer[ tile.y - 1 ][ tile.x ] == type ) {
            bitmask += 8;
        }
    }

    return bitmask;
}

int CMapGen::countBitmask( Vec2DBool& layer, const sf::Vector2i& tile, bool type ) {
    int     bitmask = 0;

    if( tile.x - 1 < 0 ) {
        bitmask += 1;
    }
    else {
        if( layer[ tile.y ][ tile.x - 1 ] == type ) {
            bitmask += 1;
        }
    }

    if( tile.y + 1 > m_mapHeight - 1 ) {
        bitmask += 2;
    }
    else {
        if( layer[ tile.y + 1 ][ tile.x ] == type ) {
            bitmask += 2;
        }
    }

    if( tile.x + 1 > m_mapWidth - 1 ) {
        bitmask += 4;
    }
    else {
        if( layer[ tile.y ][ tile.x + 1 ] == type ) {
            bitmask += 4;
        }
    }

    if( tile.y - 1 < 0 ) {
        bitmask += 8;
    }
    else {
        if( layer[ tile.y - 1 ][ tile.x ] == type ) {
            bitmask += 8;
        }
    }

    return bitmask;
}

bool CMapGen::isInMap( int x, int y ) {
    return ( x >= 0 && x < m_mapWidth && y >= 0 && y < m_mapHeight );
}
