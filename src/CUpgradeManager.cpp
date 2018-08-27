#include "CUpgradeManager.h"

#include <fstream>
#include <sstream>
#include <cstdlib>

#include <CGame.h>
#include <CPlayer.h>
#include <Utility.h>
#include <LuaWrapper.h>

#include <iostream>
using namespace std;

void CUpgradeManager::close() {
    save();

//    m_upgrades.clear();
}

bool CUpgradeManager::save() {
    std::ofstream file;

    file.open( "media/saves/upgrades.txt" );

    if( file.is_open() ) {
        for( auto& it : m_upgrades ) {
            std::string id  = "id=\"";
            id.append( it.ID );
            id.append( "\"" );

            std::string locked = " locked=\"";
            if( it.Locked ) {
                locked.append( "true" );
            }
            else {
                locked.append( "false" );
            }
            locked.append( "\"" );

            std::string level  = " level=\"";
            level.append( Util::uintToString( it.Level ) );
            level.append( "\"" );

            file << id;
            file << locked;
            file << level;
            file << endl;
        }

        file.close();
    }

    return true;
}

bool CUpgradeManager::load( lua_State* state, int index ) {
    if( !lua_istable( state, index ) ) {
        cout << "Error: Unable to load upgrades out of non-table value" << endl;

        return false;
    }

    for( lua_pushnil( state ); lua_next( state, index ) != 0; lua_pop( state, 1 ) ) {
        int tableIndex  = lua_gettop( state );

        if( lua_istable( state, tableIndex ) ) {
            UpgradeData data;

            lWrapper::stringFromLuaTable        ( state, tableIndex, "ID",          data.ID );
            lWrapper::stringFromLuaTable        ( state, tableIndex, "name",        data.Name );
            lWrapper::stringFromLuaTable        ( state, tableIndex, "description", data.Description );
            lWrapper::stringFromLuaTable        ( state, tableIndex, "texture",     data.TextureID );
            lWrapper::valueFromLuaTable<bool>   ( state, tableIndex, "locked",      data.Locked );
            lWrapper::valueFromLuaTable<size_t> ( state, tableIndex, "levelMax",    data.LevelMax );
            data.Pos    = Util::vectorFromTable<float>( state, tableIndex, "pos" );

            lua_getfield( state, tableIndex, "unlocks" );

            int unlocksIndex    = lua_gettop( state );

            if( lua_istable( state, unlocksIndex ) ) {
                lWrapper::stringFromLuaTable( state, unlocksIndex, 1, data.Unlocks[ 0 ] );
                lWrapper::stringFromLuaTable( state, unlocksIndex, 2, data.Unlocks[ 1 ] );
                lWrapper::stringFromLuaTable( state, unlocksIndex, 3, data.Unlocks[ 2 ] );
            }

            lua_getfield( state, tableIndex, "prices" );

            int pricesIndex     = lua_gettop( state );

            if( lua_istable( state, pricesIndex ) ) {
                for( size_t i = 0; i < data.LevelMax; i++ ) {
                    lua_rawgeti( state, pricesIndex, i + 1 );

                    size_t price    = lua_tointeger( state, - 1 );

                    data.Prices.push_back( price );

                    lua_pop( state, 1 );
                }
            }

            lua_pop( state, 2 );

            m_upgrades.push_back( data );
        }
    }

    return loadFromSave();
}

void CUpgradeManager::setup() {
    for( auto& it : m_upgrades ) {
        if( !it.Locked && it.Level > 0 ) {
            handleUpgrade( &it, true );
        }
    }
}

bool CUpgradeManager::unlock( const std::string& ID ) {
    if( ID.empty() ) return false;

    UpgradeData* data = getUpgrade( ID );

    if( data->Level < data->LevelMax ) {
        data->Level++;

        handleUpgrade( data );
    }

    if( data->Level == data->LevelMax ) {
        for( size_t i = 0; i < 3; i++ ) {
            if( !data->Unlocks[ i ].empty() ) {
                UpgradeData* child = getUpgrade( data->Unlocks[ i ] );

                if( child != nullptr ) {
                    child->Locked = false;
                }
            }
        }
    }

    return true;
}

bool CUpgradeManager::canUnlock( const std::string& ID ) {
    if( ID.empty() ) return false;

    UpgradeData* data = getUpgrade( ID );

    if( data == nullptr ) return false;

    return !data->Locked && data->Level < data->LevelMax;
}

size_t CUpgradeManager::getPrice( const std::string& ID ) {
    if( ID.empty() ) return 0;

    UpgradeData* data = getUpgrade( ID );

    if( data == nullptr ) return 0;

    return data->Prices[ data->Level ];
}

bool CUpgradeManager::loadFromSave() {
    std::ifstream    file( "media/saves/upgrades.txt" );

    if( file.is_open() ) {
        std::string line;

        while( std::getline( file, line ) ) {
            std::string id      = getElement( line, "id" );
            std::string locked  = getElement( line, "locked" );
            std::string level   = getElement( line, "level" );

            UpgradeData* data = getUpgrade( id );

            if( data != nullptr ) {
                if( locked == "false" ) {
                    data->Locked    = false;
                }
                else {
                    data->Locked    = true;
                }

                data->Level     = atoi( level.c_str() );
            }
        }

        file.close();
    }

    return true;
}

void CUpgradeManager::setupUpgTable( lua_State* state, const std::string& ID ) {
    if( ID.empty() ) return;

    UpgradeData* data = getUpgrade( ID );

    if( data == nullptr ) return;

    lua_createtable( state, 0, 10 );

    lua_pushstring( state, data->ID.c_str() );
    lua_setfield( state, - 2, "ID" );

    lua_pushstring( state, data->Name.c_str() );
    lua_setfield( state, - 2, "name" );

    lua_pushstring( state, data->Description.c_str() );
    lua_setfield( state, - 2, "description" );

    lua_pushstring( state, data->TextureID.c_str() );
    lua_setfield( state, - 2, "texture" );

    lua_createtable( state, 0, 3 );

    lua_pushstring( state, data->Unlocks[ 0 ].c_str() );
    lua_rawseti( state, - 2, 1 );
    lua_pushstring( state, data->Unlocks[ 1 ].c_str() );
    lua_rawseti( state, - 2, 2 );
    lua_pushstring( state, data->Unlocks[ 2 ].c_str() );
    lua_rawseti( state, - 2, 3 );

    lua_setfield( state, - 2, "unlocks" );

    lua_createtable( state, 0, data->Prices.size() );
    for( size_t j = 0; j < data->Prices.size(); j++ ) {
        lua_pushinteger( state, data->Prices[ j ] );
        lua_rawseti( state, - 2, j + 1 );
    }
    lua_setfield( state, - 2, "prices" );

    lua_createtable( state, 0, 2 );
    lua_pushnumber( state, data->Pos.x );
    lua_setfield( state, - 2, "x" );
    lua_pushnumber( state, data->Pos.y );
    lua_setfield( state, - 2, "y" );

    lua_setfield( state, - 2, "pos" );

    lua_pushinteger( state, data->Level );
    lua_setfield( state, - 2, "level" );

    lua_pushinteger( state, data->LevelMax );
    lua_setfield( state, - 2, "levelMax" );

    lua_pushboolean( state, data->Locked );
    lua_setfield( state, - 2, "locked" );
}

void CUpgradeManager::setupUpgsTable( lua_State* state ) {
    lua_createtable( state, 0, m_upgrades.size() );

    for( size_t i = 0; i < m_upgrades.size(); i++ ) {
        lua_createtable( state, 0, 10 );

        UpgradeData data = m_upgrades[ i ];

        lua_pushstring( state, data.ID.c_str() );
        lua_setfield( state, - 2, "ID" );

        lua_pushstring( state, data.Name.c_str() );
        lua_setfield( state, - 2, "name" );

        lua_pushstring( state, data.Description.c_str() );
        lua_setfield( state, - 2, "description" );

        lua_pushstring( state, data.TextureID.c_str() );
        lua_setfield( state, - 2, "texture" );

        lua_createtable( state, 0, 3 );

        lua_pushstring( state, data.Unlocks[ 0 ].c_str() );
        lua_rawseti( state, - 2, 1 );
        lua_pushstring( state, data.Unlocks[ 1 ].c_str() );
        lua_rawseti( state, - 2, 2 );
        lua_pushstring( state, data.Unlocks[ 2 ].c_str() );
        lua_rawseti( state, - 2, 3 );

        lua_setfield( state, - 2, "unlocks" );

        lua_createtable( state, 0, data.Prices.size() );
        for( size_t j = 0; j < data.Prices.size(); j++ ) {
            lua_pushinteger( state, data.Prices[ j ] );
            lua_rawseti( state, - 2, j + 1 );
        }
        lua_setfield( state, - 2, "prices" );

        lua_createtable( state, 0, 2 );
        lua_pushnumber( state, data.Pos.x );
        lua_setfield( state, - 2, "x" );
        lua_pushnumber( state, data.Pos.y );
        lua_setfield( state, - 2, "y" );

        lua_setfield( state, - 2, "pos" );

        lua_pushinteger( state, data.Level );
        lua_setfield( state, - 2, "level" );

        lua_pushinteger( state, data.LevelMax );
        lua_setfield( state, - 2, "levelMax" );

        lua_pushboolean( state, data.Locked );
        lua_setfield( state, - 2, "locked" );

        lua_rawseti( state, - 2, i + 1 );
    }
}

/** LUA FUNCTIONS */

int CUpgradeManager::luaLoad( lua_State* state ) {
    int argc = lua_gettop( state );

    bool success = false;

    if( argc != 2 ) {
        cout << "Error: Unable to load upgrades. Too many arguments" << endl;
    }
    else {
        success = CGame::UpgradeManager.load( state, argc );
    }

    lua_pop( state, argc );

    lua_pushboolean( state, success );

    return 1;
}

int CUpgradeManager::luaSetup( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    CGame::UpgradeManager.setup();

    return 0;
}

int CUpgradeManager::luaGetUpgrade( lua_State* state ) {
    int argc = lua_gettop( state );

    std::string ID;

    if( argc == 2 ) {
        if( lua_isstring( state, argc ) ) {
            ID = lua_tostring( state, argc );
        }
        else {
            std::cout << "Error: Unable to setup upgrade table. Expected String, got " << lua_typename( state, lua_type( state, argc ) ) << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to setup upgrade table. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    CGame::UpgradeManager.setupUpgTable( state, ID );

    return 1;
}

int CUpgradeManager::luaGetUpgrades( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    CGame::UpgradeManager.setupUpgsTable( state );

    return 1;
}

/** PRIVATE */

std::string CUpgradeManager::getElement( const std::string& input, const std::string& name ) {
    size_t pos = input.find( name );

    if( pos > input.size() ) {
        pos = input.size();
    }

    size_t first    = input.find( '\"', pos );
    size_t second   = input.find( '\"', first + 1 );

    return input.substr( first + 1, second - first - 1 );
}

UpgradeData* CUpgradeManager::getUpgrade( const std::string& ID ) {
    for( auto &it : m_upgrades ) {
        if( it.ID == ID ) {
            return &it;
        }
    }

    std::cout << "Error: Unable to find upgrade. Wrong ID: " << ID << endl;

    return nullptr;
}

void CUpgradeManager::handleUpgrade( const UpgradeData* data, bool init ) {
    const std::string ID = data->ID;

    size_t loopCount = 1;

    if( init ) {
        loopCount = data->Level;
    }

    for( size_t i = 0; i < loopCount; i++ ) {
        if( ID == "UPG_HEALTH_MAX" ) {
            CGame::EntitySystem.getPlayer()->addStatAndUpdate( StatIndex::STAT_HEALTH, 2, CalcOperation::CALC_PERCENTADD );
        }
    }
}
