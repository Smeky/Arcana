#ifndef CINPUTSYSTEM_H
#define CINPUTSYSTEM_H

struct lua_State;

namespace Input {

int     luaIsKeyPressed     ( lua_State* state );

int     luaIsMousePressed   ( lua_State* state );
int     luaGetMousePos      ( lua_State* state );

}

#endif // CINPUTSYSTEM_H
