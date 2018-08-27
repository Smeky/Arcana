#ifndef CTESTING_H
#define CTESTING_H

#include <lua.hpp>

class CTesting {
public:

    static  void        setup           ( lua_State* state );

            void        talk            ( const char* text );

public:
    static  int         luaNew          ( lua_State* state );
    static  int         luaDestructor   ( lua_State* state );
    static  int         luaTalk         ( lua_State* state );

private:

};

#endif // CTESTING_H
