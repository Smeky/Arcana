#include "CMapGenerator.h"

#include <cassert>
#include <lua.hpp>

#include <CGame.h>
#include <CRNG.h>
#include <CMap.h>
#include <CTile.h>
#include <CTileLayer.h>
#include <Utility.h>
#include <LuaWrapper.h>

#include <iostream>
using namespace std;

CMapGenerator::CMapGenerator()
{}

CMap* CMapGenerator::createFromLuaTable( lua_State* state, int index ) {
    assert( lua_istable( state, index ) && "<- is not table" );

    CMap* map = nullptr;

    std::string     tilesetID;
    sf::Vector2f    size;

    // Get tileset ID
    lua_getfield( state, index, "tileset" );
    assert( !lua_isnil( state, - 1 ) );
    tilesetID   = lua_tostring( state, - 1 );

    // Get map size
    lua_getfield( state, index, "size" );
    assert( lua_istable( state, - 1 ) );
    size        = Util::vector2fFromLuaTable( state, lua_gettop( state ) );

    lua_getfield( state, index, "static" );
    if( lua_isboolean( state, - 1 ) ) {
        map = loadMap( tilesetID, size, state, index );
    }
    else {
        map = generateMap( tilesetID, size );
    }

//    // Create map objects
//    lua_getfield( state, index, "objects" );
//    if( !lua_isnil( state, - 1 ) ) {
//        createObjects( state, lua_gettop( state ) );
//    }

    lua_pop( state, 3 );

    return map;
}

CMap* CMapGenerator::loadMap( const std::string& tilesetID, const sf::Vector2f& mapSize, lua_State* state, int index ) {
    // Set base tilesize
    sf::Vector2f tileSize( 64, 64 );

    // Look for table "tiles"
    lua_getfield( state, index, "layers" );

    // Check if table "tiles" are not missing
    assert( lua_istable( state, - 1 ) && "Table for tile layout is missing" );

    // Initialize map
    CMap* map = new CMap();
    map->setSize( Util::multiplyVectors<float>( mapSize, tileSize ) );

    // Go through all layers
    for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
        bool top    = false;

        lWrapper::valueFromLuaTable<bool>( state, lua_gettop( state ), "top",   top );

        // Initialize tile
        CTileLayer*         layer   = new CTileLayer();
        layer->setSize( mapSize );
        sf::Vector2u        tilesetSize = CGame::AssetManager.getTextureSize( tilesetID );
        sf::Vector2u        clipSize( 64, 64 );
        sf::Vector2u        tilesetInClips( tilesetSize.x / clipSize.x, tilesetSize.y / clipSize.y );

        // Initialize tiles ( amount is based on map size )
        for( size_t i = 0; i < mapSize.x * mapSize.y; i++ ) {
            CTile tile;

            sf::Vector2f tilePos;

            // Tile position calculation
            tilePos.x   = tileSize.x * ( ( i - ( floorf( (float)i / mapSize.x ) * mapSize.y ) ) );
            tilePos.y   = tileSize.y * ( floorf( (float)i / mapSize.x ) );

            tile.setSize( tileSize );
            tile.setPosRounded( tilePos );

            layer->addTile( tile );
        }

        lua_getfield( state, - 1, "tiles" );

        size_t tileCounter = 0;

        // Go through all tiles tables
        for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
            int tilesTableIndex = lua_gettop( state );

            tileCounter = 0;

            // Go through all tiles
            for( lua_pushnil( state ); lua_next( state, tilesTableIndex ) != 0; lua_pop( state, 1 ), tileCounter++ ) {
                int     clipIndex   = 0;
                bool    emptyTile   = false;

                if( lua_isinteger( state, - 1 ) ) {
                    clipIndex = lua_tointeger( state, - 1 );

                    if( clipIndex == 0 ) {
                        emptyTile = true;
                    }
                    else {
                        clipIndex = clipIndex - 1;
                    }
                }

                if( !emptyTile ) {
                    sf::IntRect     tilesetClip;

                    // Tileset clip calculation
                    tilesetClip.left    = clipSize.x * ( ( clipIndex - ( floorf( (float)clipIndex / tilesetInClips.x ) * tilesetInClips.y ) ) );
                    tilesetClip.top     = clipSize.y * ( floorf( (float)clipIndex / tilesetInClips.x ) );
                    tilesetClip.width   = clipSize.x;
                    tilesetClip.height  = clipSize.y;

                    CSpriteWrapper sprite;

                    sprite.setTexture( tilesetID );
                    sprite.setClip( tilesetClip );
                    sprite.setSize( tileSize );

                    layer->addSprite( tileCounter, sprite );
                }
            } // Tiles loop

        } // Tile tables loop

        // Clean tiles table
        lua_pop( state, 1 );

        tileCounter = 0;

        if( !top ) {
            lua_getfield( state, - 1, "solid" );

            if( lua_istable( state, - 1 ) ) {
                // Go through all solid tiles
                for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ), tileCounter++ ) {
                    bool solid = lua_tointeger( state, - 1 );

                    if( solid ) {
                        layer->setSolid( tileCounter );
                    }
                } // Solid tiles loop
            }

            // Clear table "tiles" that was found in tile layer
            lua_pop( state, 1 );
        }

        if( top ) {
            map->addTopTileLayer( layer );
        }
        else {
            map->addTileLayer( layer );
        }
    }

    // Clean layers table
    lua_pop( state, 1 );

    return map;
}

CMap* CMapGenerator::generateMap( const std::string& tilesetID, const sf::Vector2f& mapSize ) {
    sf::Vector2f tileSize( 64, 64 );

    CTileLayer*         layer   = new CTileLayer();
    layer->setSize( mapSize );
    doubleVecBool       alives;

    // Choose whether base tile will be alive or not
    // Alive means if it will be first or second type
    for( int y = 0; y < mapSize.y; y++ ) {
        alives.push_back( std::vector<bool>() );

        for( int x = 0; x < mapSize.x; x++ ) {
            alives.back().push_back( false );

            if( CRNG::percentRandom() < 0.4 ) {
                alives.back().back() = true;
            }
        }
    }

    for( int i = 0; i < 3; i++ ) {
        doSimStep( alives, mapSize );
    }

    // Setup tile base
    // Tileset for tiles is based on map type
    for( int y = 0; y < mapSize.y; y++ ) {
        for( int x = 0; x < mapSize.x; x++ ) {
            CTile tile;

            tile.setSize( tileSize );
            tile.setPos( sf::Vector2f( x * tileSize.x, y * tileSize.y ) );

            // Clip for tileset texture
            sf::IntRect clip( 0, 0, 64, 64 );

            // If tile position is wall
            if( x == 0 || x == mapSize.x - 1 || y == 0 || y == 1 || y == mapSize.y - 1 ) {
                // Move clip to correct position on tileset texture
                clip.left   = clip.width * 5;

                if( x == 0 ) {
                    if( y == 0 ) {
                        clip.top    += clip.height;
                    }
                    else if( y == mapSize.y - 1 ) {
                        clip.left   += clip.width;
                        clip.top    += clip.height;
                    }
                    else {
                        clip.left   += clip.width * 3;
                    }
                }
                else if( x == mapSize.x - 1 ) {
                    if( y == 0 ) {
                        clip.left   += clip.width * 2;
                        clip.top    += clip.height;
                    }
                    else if( y == mapSize.y - 1 ) {
                        clip.left   += clip.width * 3;
                        clip.top    += clip.height;
                    }
                    else {
                        clip.left   += clip.width * 1;
                    }
                }

                if( y == 0 ) {
                    if( x != 0 && x != mapSize.x - 1 ) {
                        clip.left   += clip.width * 4;
                    }
                }
                else if( y == 1 ) {
                    if( x != 0 && x != mapSize.x - 1 ) {
                        clip.top    += clip.height;
                        clip.left   += clip.width * 4;
                    }
                }
                else if( y == mapSize.y - 1 ) {
                    if( x != 0 && x != mapSize.x - 1 ) {
                        clip.left   += clip.width * 2;
                    }
                }

                // The tile is wall so make it collideable
                tile.setSolid();
            }
            // Otherwise if tile is ground
            else {
                if( alives[ y ][ x ] ) {
                    int bitmask = countBitmask( alives, sf::Vector2u( x, y ), mapSize );

                    int tilesetRow = floorf( bitmask / 4 );

                    clip.left   = clip.width * ( bitmask - tilesetRow * 4 );
                    clip.top    = clip.height * tilesetRow;
                }
                else {
                    clip.left   = clip.width * 4;
                }
            }

            CSpriteWrapper sprite;

            sprite.setTexture( tilesetID );
            sprite.setClip( clip );
            sprite.setSize( tileSize );

            tile.addSprite( sprite );

            layer->addTile( tile );
        }
    }

    CMap* map = new CMap();
    map->addTileLayer( layer );
    map->setSize( Util::multiplyVectors<float>( mapSize, tileSize ) );

    return map;
}

void CMapGenerator::createObjects( lua_State* state, int index ) {
    assert( lua_istable( state, index ) );

    for( lua_pushnil( state ); lua_next( state, index ) != 0; lua_pop( state, 1 ) ) {
        size_t minAmount    = 1;
        size_t maxAmount    = 0;

        // Get minimum amount of objects
        lua_getfield( state, - 1, "min" );
        if( lua_isinteger( state, - 1 ) ) {
            minAmount   = lua_tointeger( state, - 1 );
        }

        // Get maximum amount of objects
        lua_getfield( state, - 2, "max" );
        if( lua_isinteger( state, - 1 ) ) {
            maxAmount   = lua_tointeger( state, - 1 );
        }

        // Get object table
        lua_getfield( state, - 3, "table" );
        assert( lua_istable( state, - 1 ) );

        size_t amount = minAmount;

        // If there is maximum amount -> Randomize amount
        if( maxAmount != 0 ) {
            amount = CRNG::iRandom( minAmount, maxAmount );
        }

        // Spawn objects
        for( ; amount > 0; amount-- ) {
            CGame::EntitySystem.objectFromLuaTable( state, lua_gettop( state ) );
        }

        lua_pop( state, 3 );
    }
}

int CMapGenerator::countAlives( doubleVecBool alives, const sf::Vector2u& index, const sf::Vector2f& mapSize ) {
    int counter = 0;

    for( int y = -1; y <= 1; y++ ) {
        for( int x = -1; x <= 1; x++ ) {
            int xNb = index.x + x;
            int yNb = index.y + y;

            if( x == 0 && y == 0 ) {
            }
            else if( xNb < 0 || yNb < 0 || xNb > mapSize.x - 1 || yNb > mapSize.y - 1 ) {
                counter++;
            }
            else if( alives[ yNb ][ xNb ] ) {
                counter++;
            }
        }
    }

    return counter;
}

int CMapGenerator::countBitmask( doubleVecBool alives, const sf::Vector2u& index, const sf::Vector2f& mapSize ) {
    int bitmask = 0;

    if( (int)index.x - 1 < 0 ) {
        bitmask += 1;
    }
    else {
        if( alives[ index.y ][ (int)index.x - 1 ] ) {
            bitmask += 1;
        }
    }

    if( index.y + 1 > mapSize.y - 1 ) {
        bitmask += 2;
    }
    else {
        if( alives[ index.y + 1 ][ index.x ] ) {
            bitmask += 2;
        }
    }

    if( index.x + 1 > mapSize.x - 1 ) {
        bitmask += 4;
    }
    else {
        if( alives[ index.y ][ index.x + 1 ] ) {
            bitmask += 4;
        }
    }

    if( (int)index.y - 1 < 0 ) {
        bitmask += 8;
    }
    else {
        if( alives[ (int)index.y - 1 ][ index.x ] ) {
            bitmask += 8;
        }
    }

    return bitmask;
}

void CMapGenerator::doSimStep( doubleVecBool& alives, const sf::Vector2f& mapSize ) {
    doubleVecBool newAlive = alives;
    int deathLimit  = 2;
    int birthLimit  = 3;

    for( int y = 0; y < mapSize.y; y++ ) {
        for( int x = 0; x < mapSize.x; x++ ) {
            int nbs = countAlives( alives, sf::Vector2u( x, y ), mapSize );

            if( alives[ y ][ x ] ) {
                if( nbs < deathLimit ) {
                    newAlive[ y ][ x ] = false;
                }
                else {
                    newAlive[ y ][ x ] = true;
                }
            }
            else {
                if( nbs > birthLimit ) {
                    newAlive[ y ][ x ] = true;
                }
                else {
                    newAlive[ y ][ x ] = false;
                }
            }
        }
    }

    alives = newAlive;
}
