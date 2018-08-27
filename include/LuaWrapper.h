#ifndef LUAWRAPPER_H
#define LUAWRAPPER_H

#include <lua.hpp>
#include <typeinfo>
#include <string>

namespace lWrapper {

/**
    brief:  gets field from lua table at given index, casts it to class C and overwrites template argument T
    param:  state       - lua state
    param:  index       - index of the table inside lua stack
    param:  name        - name of field inside table
    param:  value       - reference to value that should be overwritten
    return: returns true in case overwriting was successful, false otherwise
*/
template<class C, typename T>
bool valueFromLuaTable( lua_State* state, int index, const char* name, T& value ) {
    lua_getfield( state, index, name );

    // In case value is missing
    if( lua_isnil( state, - 1 ) ) {
        lua_pop( state, 1 );

        return false;
    }
    else if( typeid( C ) == typeid( int ) ) {
        value   = (C)lua_tointeger( state, - 1 );
    }
    else if( typeid( C ) == typeid( float ) ) {
        value   = (C)lua_tonumber( state, - 1 );
    }
    else if( typeid( C ) == typeid( bool ) ) {
        value   = (C)lua_toboolean( state, - 1 );
    }
    else if( typeid( C ) == typeid( std::string ) ) {
        value   = (C)*lua_tostring( state, - 1 );
    }
    else {
        value   = (C)lua_tonumber( state, - 1 );
    }

    lua_pop( state, 1 );

    return true;
}

template<class C, typename T>
bool valueFromLuaTable( lua_State* state, int index, int fieldIndex, T& value ) {
    lua_rawgeti( state, index, fieldIndex );

    // In case value is missing
    if( lua_isnil( state, - 1 ) ) {
        lua_pop( state, 1 );

        return false;
    }
    else if( typeid( C ) == typeid( int ) ) {
        value   = (C)lua_tointeger( state, - 1 );
    }
    else if( typeid( C ) == typeid( float ) ) {
        value   = (C)lua_tonumber( state, - 1 );
    }
    else if( typeid( C ) == typeid( bool ) ) {
        value   = (C)lua_toboolean( state, - 1 );
    }
    else if( typeid( C ) == typeid( std::string ) ) {
        value   = (C)*lua_tostring( state, - 1 );
    }
    else {
        value   = (C)lua_tonumber( state, - 1 );
    }

    lua_pop( state, 1 );

    return true;
}

/**
    brief:  equivalent function to valueFromLuaTable but for strings only
*/
bool stringFromLuaTable( lua_State* state, int index, const char* name, std::string& value );
bool stringFromLuaTable( lua_State* state, int index, int fieldIndex, std::string& value );

/**
    brief:  duplicates string at given index and returns it as a string. This function is used not to confuse lua_next by using lua_tostring on key
    param:  state       - lua state
    param:  index       - index of key inside lua stack
    return: key name at given index
*/
const char* keyFromLuaTable( lua_State* state, int index );

}

#endif // LUAWRAPPER_H
