#include "CTesting.h"

#include <iostream>
using namespace std;

//CTesting::CTesting()
//{
//    //ctor
//}

void CTesting::setup( lua_State* state ) {
    luaL_Reg testFuncs[] {
        { "new",    CTesting::luaNew },
        { "talk",   CTesting::luaTalk },
        { "__gc",   CTesting::luaDestructor },
        { NULL, NULL }
    };

    luaL_newmetatable( state, "luaL_Test" );

    lua_pushvalue( state, - 1 );

    lua_setfield( state, - 2, "__index" );

    luaL_setfuncs( state, testFuncs, 0 );

    lua_setglobal( state, "Test" );
}

void CTesting::talk( const char* text ) {
    cout << "My text: " << text << endl;
}

int CTesting::luaNew( lua_State* state ) {
    CTesting* testing = (CTesting*)lua_newuserdata( state, sizeof( CTesting ) );
    testing = new CTesting();

    luaL_getmetatable( state, "luaL_Test" );

    lua_setmetatable( state, - 2 );

    return 1;
}

int CTesting::luaDestructor( lua_State* state ) {
    CTesting* testing = (CTesting*)luaL_checkudata( state, lua_gettop( state ), "luaL_Test" );
    delete testing;

    return 0;
}

int CTesting::luaTalk( lua_State* state ) {
    CTesting* testing = (CTesting*)luaL_checkudata(state, lua_gettop( state ) - 1, "luaL_Test");

    const char* text = lua_tostring( state, lua_gettop( state ) );

    testing->talk( text );

    lua_pop( state, lua_gettop( state ) );

    return 0;
}
