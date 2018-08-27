#include "CInputSystem.h"

#include "CGame.h"
#include "lua.hpp"

int Input::luaIsKeyPressed( lua_State* luaState ) {
    int argc    = lua_gettop( luaState );

    int keyID   = lua_tointeger( luaState, lua_gettop( luaState ) );

    lua_pop( luaState, argc );

    int result = sf::Keyboard::isKeyPressed( sf::Keyboard::Key( keyID ) );

    lua_pushboolean( luaState, result );

    return 1;
}

int Input::luaIsMousePressed( lua_State* luaState ) {
    int argc        = lua_gettop( luaState );

    int buttonID    = lua_tointeger( luaState, lua_gettop( luaState ) );

    lua_pop( luaState, argc );

    int result      = sf::Mouse::isButtonPressed( sf::Mouse::Button( buttonID ) );

    lua_pushboolean( luaState, result );

    return 1;
}

int Input::luaGetMousePos( lua_State* luaState ) {
    int argc = lua_gettop( luaState );

    lua_pop( luaState, argc );

    sf::Vector2i pos = sf::Mouse::getPosition( CGame::GameWindow );

    lua_pushnumber( luaState, pos.x );
    lua_pushnumber( luaState, pos.y );

    return 2;
}
