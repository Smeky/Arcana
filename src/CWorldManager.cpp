#include "CWorldManager.h"

#include "assert.h"
#include "lua.hpp"

#include "CGame.h"
#include "CEntity.h"
#include "CMapGenerator.h"
#include "CMap.h"
#include "CPlayer.h"
#include "Utility.h"
#include <LuaWrapper.h>
#include <CRNG.h>
#include <CMapGen.h>

#include "iostream"
using namespace std;

CWorldManager::CWorldManager()
: m_mapIDCounter        ( 1 )
, m_debugDisplaySpatial ( false )
{}

void CWorldManager::close() {
    CGame::ZBuffer.clean();
    CGame::EntitySystem.close();
    CombatTextSystem.close();
    ParticleSystem.close();
    CGame::UpgradeManager.close();

    while( !m_maps.empty() ) delete m_maps.back(), m_maps.pop_back();
}

void CWorldManager::update( const sf::Time& delta ) {
    if( CGame::SceneID != 2 || m_paused ) return;

    m_worldTime += delta;

    CGame::SpatialSystem.clearGrid();
    CGame::ZBuffer.clean();

    if( !m_maps.empty() ) {
        m_maps.back()->update( delta );
    }

    CGame::ScriptSystem.fireEvent( "UpdateWorld" );

    Camera.update( delta );
    CombatTextSystem.update( delta );
    ParticleSystem.update( delta );

    CGame::EntitySystem.update( delta );
}

void CWorldManager::render( sf::RenderTarget& window, sf::RenderStates states ) {
    if( CGame::SceneID != 2 ) return;

    states.transform *= Camera.getTransform();

    if( !m_maps.empty() ) {
        m_maps.back()->render( window, states );
    }

    CGame::ZBuffer.render( window, states );

    CGame::EntitySystem.render( window, states );

    if( !m_maps.empty() ) {
        m_maps.back()->renderTopLayer( window, states );
    }

    CombatTextSystem.render( window, states );

    if( m_debugDisplaySpatial ) {
        CGame::SpatialSystem.drawCells( window, states );
    }
}

void CWorldManager::setCameraSize( const sf::Vector2f& cameraSize ) {
    Camera.setSize( cameraSize );
}

size_t CWorldManager::createMap( lua_State* state, int index ) {
    if( !lua_istable( state, index ) ) return 0;

    m_paused    = false;

    CGame::EntitySystem.cleanUp();
    CGame::WorldManager.ParticleSystem.close();

    size_t ID = 0;

    lua_getfield( state, index, "map" );
    if( lua_istable( state, - 1 ) ) {
//        CMapGenerator generator = CMapGenerator();

//        CMap* map = generator.createFromLuaTable( state, lua_gettop( state ) );
        CMap* map = CMapGen::generateMap( state, lua_gettop( state ) );

        if( map != nullptr ) {
            ID = addMap( map );
        }
        else {
            std::cout << "Error: Failed to create map" << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to create map. Expected table, got " << lua_typename( state, lua_type( state, - 1 ) ) << std::endl;

        return ID;
    }

    lua_getfield( state, index, "objects" );
    if( lua_istable( state, - 1 ) ) {
        for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
            int     minAmount    = 1;
            int     maxAmount    = 0;


            lWrapper::valueFromLuaTable<int>( state, lua_gettop( state ), "min", minAmount );
            lWrapper::valueFromLuaTable<int>( state, lua_gettop( state ), "max", maxAmount );

            // Get object table
            lua_getfield( state, - 1, "table" );

            if( lua_istable( state, - 1 ) ) {
                int amount = minAmount;

                // If there is maximum amount -> Randomize amount
                if( maxAmount != 0 ) {
                    amount = CRNG::iRandom( minAmount, maxAmount );
                }

                // Spawn objects
                for( ; amount > 0; amount-- ) {
                    CGame::EntitySystem.objectFromLuaTable( state, lua_gettop( state ) );
                }
            }
            else {
                std::cout << "Error: Unable to create object. Expected table, got " << lua_typename( state, lua_type( state, - 1 ) ) << std::endl;
            }

            lua_pop( state, 1 );
        }
    }

    lua_pop( state, 2 );


    CGame::PathFinder.setup();

    return ID;
}

size_t CWorldManager::addMap( CMap* map ) {
    for( auto it : m_maps ) {
        if( it->getID() == map->getID() ) {
            std::cout << "Error: Trying to add map that is already being used!" << std::endl;
            return 0;
        }
    }

    size_t ID = getNewMapID();

    map->setID( ID );

    CGame::SpatialSystem.close();
    CGame::SpatialSystem.setup( map->getSize(), sf::Vector2f( 12, 12 ) );

    m_maps.push_back( map );

    return ID;
}

bool CWorldManager::enterMap( size_t ID ) {
    for( auto it : m_maps ) {
        if( it->getID() == ID ) {
            if( it != m_maps.back() ) {
                CMap* buffer = m_maps.back();

                m_maps.back() = it;

                it = buffer;
            }
        }
    }

    return false;
}

void CWorldManager::delMap( size_t ID ) {
    for( auto it = m_maps.begin(); it != m_maps.end(); it++ ) {
        if( (*it)->getID() == ID ) {
            delete *it;
            m_maps.erase( it );
            return;
        }
    }

    std::cout << "Warning: Trying to delete map that is not being used!" << std::endl;
}

sf::Vector2f CWorldManager::getMapSize() const {
    if( !m_maps.empty() ) {
        return m_maps.back()->getSize();
    }
    #ifdef DEBUG
    else {
        std::cout << "[D]Warning: Trying to get map size but maps are empty" << std::endl;
    }
    #endif // DEBUG

    return sf::Vector2f();
}

CMap* CWorldManager::getCurrentlyUsedMap() const {
    assert( !m_maps.empty() && "Trying to return map that doesn't exist" );

    return m_maps.back();
}

sf::Time CWorldManager::getWorldTime() const {
    return m_worldTime;
}

void CWorldManager::pauseGame() {
    m_paused    = !m_paused;
}

bool CWorldManager::isPaused() const {
    return m_paused;
}

void CWorldManager::displaySpatialSystem() {
    m_debugDisplaySpatial = !m_debugDisplaySpatial;
}

/** LUA FUNCTIONS */

int CWorldManager::luaClose( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    CGame::WorldManager.close();

    return 0;
}

int CWorldManager::luaCreateMap( lua_State* state ) {
    int argc = lua_gettop( state );

    size_t ID = 0;

    if( argc == 2 ) {
        ID = CGame::WorldManager.createMap( state, argc );
    }
    else {
        std::cout << "Error: Unable to create map. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    lua_pushinteger( state, ID );

    return 1;
}

int CWorldManager::luaEnterMap( lua_State* state ) {
    int argc = lua_gettop( state );

    bool result = false;

    if( argc == 2 ) {
        size_t ID = lua_tointeger( state, - 1 );

        result = CGame::WorldManager.enterMap( ID );
    }
    else {
        std::cout << "Error: Unable to enter map. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    lua_pushboolean( state, result );

    return 1;
}

int CWorldManager::luaSetCameraPos( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::Vector2f    pos;

    if( argc == 2 ) {
        pos     = Util::vectorFromTable<float>( state, argc );
    }
    else if( argc == 3 ) {
        pos.x   = lua_tonumber( state, - 2 );
        pos.y   = lua_tonumber( state, - 1 );
    }
    else {
        std::cout << "Error: Unable to set camera position. Wrong amount of arguments" << std::endl;
    }

    CGame::WorldManager.Camera.setPos( pos );

    lua_pop( state, argc );

    return 0;
}

int CWorldManager::luaSetCameraCenter( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::Vector2f    center;

    if( argc == 2 ) {
        center      = Util::vectorFromTable<float>( state, argc );
    }
    else if( argc == 3 ) {
        center.x    = lua_tonumber( state, - 2 );
        center.y    = lua_tonumber( state, - 1 );
    }
    else {
        std::cout << "Error: Unable to set camera center. Wrong amount of arguments" << std::endl;
    }

    CGame::WorldManager.Camera.setCenter( center );

    lua_pop( state, argc );

    return 0;
}

int CWorldManager::luaSetCameraSize( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 2 && lua_istable( state, - 1 ) );

    CGame::WorldManager.setCameraSize( Util::vector2fFromLuaTable( state, argc ) );

    lua_pop( state, argc );

    return 0;
}

int CWorldManager::luaGetCameraPos( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    sf::Vector2f cameraPos = CGame::WorldManager.Camera.getPos();

    lua_pushnumber( state, cameraPos.x );
    lua_pushnumber( state, cameraPos.y );

    return 2;
}

int CWorldManager::luaCameraMoveTo( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 3 && lua_istable( state, - 2 ) );

    sf::Vector2f    destination;
    float           factor;

    destination = Util::vector2fFromLuaTable( state, argc - 1 );
    factor      = lua_tonumber( state, - 1 );

    CGame::WorldManager.Camera.moveTo( destination, factor );

    lua_pop( state, argc );

    return 0;
}


int CWorldManager::luaCameraMoveCenterTo( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 3 && lua_istable( state, - 2 ) );

    sf::Vector2f    destination;
    float           factor;

    destination = Util::vector2fFromLuaTable( state, argc - 1 );
    factor      = lua_tonumber( state, - 1 );

    CGame::WorldManager.Camera.moveCenterTo( destination, factor );

    lua_pop( state, argc );

    return 0;
}

int CWorldManager::luaCameraGetOffset( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 2 && lua_istable( state, - 1 ) );

    sf::Vector2f pos    = Util::vector2fFromLuaTable( state, argc );
    sf::Vector2f offset = CGame::WorldManager.Camera.getOffset( pos );

    lua_pop( state, argc );

    lua_pushnumber( state, offset.x );
    lua_pushnumber( state, offset.y );

    return 2;
}

int CWorldManager::luaGetWorldTime( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    lua_pushnumber( state, CGame::WorldManager.getWorldTime().asSeconds() );

    return 1;
}

int CWorldManager::luaPauseGame( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    CGame::WorldManager.pauseGame();

    return 0;
}


int CWorldManager::luaIsGamePaused( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    bool paused = CGame::WorldManager.isPaused();

    lua_pushboolean( state, paused );

    return 1;
}

int CWorldManager::luaDisplaySpatialSystem( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    CGame::WorldManager.displaySpatialSystem();

    return 0;
}

int CWorldManager::luaDisplayMapGrid( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    CMap* map = CGame::WorldManager.getCurrentlyUsedMap();

    if( map != nullptr ) {
        map->debugDisplayGrid();
    }

    return 0;
}

/** LUA FUNCTIONS END */

//void CWorldManager::sortZBuffer( std::list<CDimensional*>& zBuffer ) {
//    zBuffer.sort( []( const CDimensional* first, const CDimensional* second ) {
//        assert( first != nullptr && second != nullptr );
//
//        return ( first->getPosSize().y < second->getPosSize().y );
//    } );
//}

size_t CWorldManager::getNewMapID() {
    m_mapIDCounter++;

    return m_mapIDCounter - 1;
}
