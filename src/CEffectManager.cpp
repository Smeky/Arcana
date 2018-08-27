#include "CEffectManager.h"

#include <lua.hpp>

#include <CGame.h>
#include <CCharacter.h>
#include <CEffect.h>
#include <Utility.h>
#include <LuaWrapper.h>

CEffectManager::CEffectManager()
: m_funcIDCounter       ( 1 )
{
    //ctor
}

void CEffectManager::createFromLuaTable( lua_State* state, int index, CCharacter* parent ) {
    if( !lua_istable( state, index ) ) return;

    EffectData data;

    lWrapper::stringFromLuaTable                ( state, index, "ID",           data.ID );
    lWrapper::stringFromLuaTable                ( state, index, "texture",      data.textureID );
    lWrapper::valueFromLuaTable<float>          ( state, index, "livetime",     data.liveTime );
    lWrapper::valueFromLuaTable<float>          ( state, index, "ticktime",     data.tickTime );
    lWrapper::valueFromLuaTable<EffectType>     ( state, index, "type",         data.type );
    lWrapper::valueFromLuaTable<EffectSubType>  ( state, index, "subtype",      data.subType );

    lua_getfield( state, index, "color" );
    if( lua_istable( state, - 1 ) ) {
        data.color  = Util::colorFromLuaTable( state, lua_gettop( state ) );
    }

    if( data.subType == EffectSubType::EFFECT_DEFAULT ) {
        CEffect* effect = new CEffect();
        effect->setData( data );

        lua_getfield( state, index, "functions" );
        if( lua_istable( state, - 1 ) ) {
            loadLuaFunctions( state, lua_gettop( state ), effect );
        }

        parent->addEffect( effect );
    }
    else if( data.subType == EffectSubType::EFFECT_STAT ) {
        CEffectStat* effect = new CEffectStat();
        effect->setData( data );

        EffectStatData statData;

        lua_getfield( state, index, "stat" );
        if( lua_istable( state, - 1 ) ) {
            int statIndex = lua_gettop( state );

            lWrapper::valueFromLuaTable<StatIndex>          ( state, statIndex, "index",            statData.statIndex );
            lWrapper::valueFromLuaTable<float>              ( state, statIndex, "value",            statData.value );
            lWrapper::valueFromLuaTable<CalcOperation>      ( state, statIndex, "operation",        statData.operation );
        }

        lua_pop( state, 1 );

        effect->setStatData( statData );

        lua_getfield( state, index, "functions" );
        if( lua_istable( state, - 1 ) ) {
            loadLuaFunctions( state, lua_gettop( state ), effect );
        }

        parent->addEffectStat( effect );
    }
    else if( data.subType == EffectSubType::EFFECT_EVENT ) {
        CEffectEvent* effect = new CEffectEvent();
        effect->setData( data );

        EffectEventType type    = EffectEventType::EEVENT_NONE;

        lWrapper::valueFromLuaTable<EffectEventType>( state, index, "event", type );

        lua_getfield( state, index, "functions" );
        if( lua_istable( state, - 1 ) ) {
            loadLuaFunctions( state, lua_gettop( state ), effect );
        }

        parent->addEffectEvent( effect, type );
    }

    lua_pop( state, 1 );


}

void CEffectManager::callLuaFunction( size_t functionID, CCharacter* parent, const std::vector<float>& arguments ) {
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
                // Push pointer to effects parent
                lua_pushlightuserdata( state, parent );

                // Push any additional arguments
                for( auto num : arguments ) {
                    lua_pushnumber( state, num );
                }

                // Call the function and store the result
                int result = lua_pcall( state, 1 + arguments.size(), LUA_MULTRET, 0 );

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

void CEffectManager::removeLuaFunction( size_t functionID ) {
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

/** PRIVATE */

size_t CEffectManager::getNewID() {
    m_funcIDCounter++;

    return m_funcIDCounter - 1;
}

void CEffectManager::loadLuaFunctions( lua_State* state, int index, CEffect* effect ) {
    lua_getfield( state, index, "onAplication" );
    if( lua_isfunction( state, - 1 ) ) {
        effect->setFunctionID( CEffect::EFUNC_APLICATION, registerFunction( state ) );
    }

    lua_getfield( state, index, "onAction" );
    if( lua_isfunction( state, - 1 ) ) {
        effect->setFunctionID( CEffect::EFUNC_ACTION, registerFunction( state ) );
    }

    lua_getfield( state, index, "onRemove" );
    if( lua_isfunction( state, - 1 ) ) {
        effect->setFunctionID( CEffect::EFUNC_REMOVE, registerFunction( state ) );
    }

    lua_pop( state, 3 );
}

size_t CEffectManager::registerFunction( lua_State* state ) {
    int functionRef = luaL_ref( state, LUA_REGISTRYINDEX );

    size_t ID = getNewID();

    m_luaFunctions[ ID ].push_back( functionRef );

    return ID;
}
