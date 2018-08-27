#include "LuaWrapper.h"

const char* lWrapper::keyFromLuaTable( lua_State* state, int index ) {
    lua_pushvalue( state, index );

    const char* key = lua_tostring( state, - 1 );

    lua_pop( state, 1 );

    return key;
}

bool lWrapper::stringFromLuaTable( lua_State* state, int index, const char* name, std::string& value ) {
    lua_getfield( state, index, name );

    bool result;

    if( !lua_isnil( state, - 1 ) ) {
        value   = lua_tostring( state, - 1 );

        result  = true;
    }
    else {
        result  = false;
    }

    lua_pop( state, 1 );

    return result;
}

bool lWrapper::stringFromLuaTable( lua_State* state, int index, int fieldIndex, std::string& value ) {
    lua_rawgeti( state, index, fieldIndex );

    bool result;

    if( !lua_isnil( state, - 1 ) ) {
        value   = lua_tostring( state, - 1 );

        result  = true;
    }
    else {
        result  = false;
    }

    lua_pop( state, 1 );

    return result;
}
