#include "CAbilityManager.h"

#include "assert.h"
#include "lua.hpp"

#include "CGame.h"
#include "CAbility.h"

#include "iostream"
using namespace std;

CAbilityManager::CAbilityManager()
: m_idAbilityCounter    ( 0 )
, m_idFunctionCounter   ( 1 )
{}

CAbilityManager::~CAbilityManager() {
    close();
}

void CAbilityManager::close() {
    m_luaFunctions.clear();
}

void CAbilityManager::callLuaFunction( size_t functionID, CCharacter* parent, const sf::Vector2f& destination ) {
    // ID 0 is default value, not used
    if( functionID == 0 ) return;

    // Store main Lua state
    lua_State* state = CGame::ScriptSystem.getState();

    if( !m_luaFunctions[ functionID ].empty() ) {
        for( int i : m_luaFunctions[ functionID ] ) {
            // Get function by it's ID
            lua_rawgeti( state, LUA_REGISTRYINDEX, i );

            // Check if function was found
            if( lua_isfunction( state, - 1 ) ) {
                // Put castersID and destination position as function arguments
                lua_pushlightuserdata( state, parent );
                lua_pushnumber( state, destination.x );
                lua_pushnumber( state, destination.y );

                // Call the function and store the result
                int result = lua_pcall( state, 3, LUA_MULTRET, 0 );

                // If there was a problem, print it
                if( result != LUA_OK ) {
                    std::cout << "Error: " << lua_tostring( state, - 1 ) << std::endl;
                }
            }
            else {
                // If function was not found, clean up
                lua_pop( state, 1 );
            }
        }
    }
}

void CAbilityManager::removeLuaFunction( size_t functionID ) {
//    auto found = m_luaFunctions.find( functionID );
//
//    if( found != m_luaFunctions.end() ) {
//        lua_State* state = CGame::ScriptSystem.getState();
//
//        for( int i : found->second ) {
//            luaL_unref( state, LUA_REGISTRYINDEX, i );
//        }
//
//        m_luaFunctions.erase( found->first );
//    }
}

CAbility* CAbilityManager::createFromLua( lua_State* state, int index ) {
    // Make sure lua index is correct
    assert( lua_istable( state, index ) && "Unable to make ability from non-table value" );

    // Initialize ability
    CAbility* ability = new CAbility();

    // Make new specific ID for the ability
    ability->setID( getAbilityID() );

    // Initialize Ability stats
    AbilityStats stats;

    lua_getfield( state, index, "data" );
    if( lua_istable( state, - 1 ) ) {
        int dataIndex = lua_gettop( state );

        lua_getfield( state, dataIndex, "name" );
        if( !lua_isnil( state, - 1 ) ) {
            stats.name          = lua_tostring( state, - 1 );
        }

        lua_getfield( state, dataIndex, "description" );
        if( !lua_isnil( state, - 1 ) ) {
            stats.description   = lua_tostring( state, - 1 );
        }

        lua_getfield( state, dataIndex, "icon" );
        if( !lua_isnil( state, - 1 ) ) {
            stats.textureID     = lua_tostring( state, - 1 );
        }

        lua_getfield( state, dataIndex, "cost" );
        if( lua_isnumber( state, - 1 ) ) {
            stats.cost          = lua_tonumber( state, - 1 );
        }

        lua_getfield( state, dataIndex, "castTime" );
        if( lua_isnumber( state, - 1 ) ) {
            stats.castTime      = lua_tonumber( state, - 1 );
        }

        lua_getfield( state, dataIndex, "cooldown" );
        if( lua_isnumber( state, - 1 ) ) {
            stats.cooldown      = lua_tonumber( state, - 1 );
        }

        lua_pop( state, 6 );
    }

    // Store ability stats in the ability
    ability->setStats( stats );

    // Look for and store onCast function
    lua_getfield( state, index, "onCast" );


    if( lua_isfunction( state, - 1 ) ) {
        size_t ID = registerFunction( state );

        ability->setOnCastID( ID );
    }
    else {
        lua_pop( state, 1 );
    }

    lua_pop( state, 1 );

    return ability;
}

void CAbilityManager::abilityToTable( lua_State* state, const CAbility* ability ) {
    if( ability == nullptr ) {
        lua_pushnil( state );
    }
    else {
        AbilityStats stats = ability->getStats();

        lua_createtable( state, 0, 8 );

        lua_pushstring( state, stats.name.c_str() );
        lua_setfield( state, - 2, "name" );

        lua_pushstring( state, stats.description.c_str() );
        lua_setfield( state, - 2, "description" );

        lua_pushstring( state, stats.textureID.c_str() );
        lua_setfield( state, - 2, "texture" );

        lua_pushinteger( state, stats.cost );
        lua_setfield( state, - 2, "cost" );

        lua_pushnumber( state, stats.castTime );
        lua_setfield( state, - 2, "cast" );

        lua_pushnumber( state, stats.cooldown );
        lua_setfield( state, - 2, "maxCD" );

        lua_pushnumber( state, ability->getCooldown() );
        lua_setfield( state, - 2, "curCD" );

        lua_pushboolean( state, ability->isInstant() );
        lua_setfield( state, - 2, "instant" );
    }
}

/** PRIVATE */

size_t CAbilityManager::registerFunction( lua_State* state ) {
    int functionRef = luaL_ref( state, LUA_REGISTRYINDEX );

    size_t ID = getFunctionID();

    m_luaFunctions[ ID ].push_back( functionRef );

    return ID;
}

size_t CAbilityManager::getAbilityID() {
    m_idAbilityCounter++;

    return m_idAbilityCounter - 1;
}

size_t CAbilityManager::getFunctionID() {
    m_idFunctionCounter++;

    return m_idFunctionCounter - 1;
}
