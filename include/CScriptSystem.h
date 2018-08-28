#ifndef CSCRIPTSYSTEM_H
#define CSCRIPTSYSTEM_H

#include "lua.hpp"
#include "string"
#include "unordered_map"
#include "vector"

class CScriptSystem {
public:
    enum ArgType {
        ARG_INT,
        ARG_FLOAT,
        ARG_BOOL,
        ARG_STRING,
        ARG_LUD
    };

public:
                    CScriptSystem           ();

    void            update                  ();

    void            prepareLuaState         ();
    void            closeLuaState           ();

    void            reloadLuaState          ();
    static int      luaReloadLuaState       ( lua_State* luaState );

    void            loadScript              ( const std::string& filePath );

    void            fireEvent               ( const std::string& eventID );
    void            fireEventWithString     ( const std::string& eventID, int argc, ... );
    void            fireEventWithInt        ( const std::string& eventID, int argc, ... );
    void            fireEventWithTable      ( const std::string& eventID, int index );

    /** Lua functions */
    size_t          registerFunc            ();
    void            callFunc                ( size_t ID );
    void            callFunc                ( size_t ID, ArgType type[], int argc, ... );

    static int      registerEvent           ( lua_State* luaState );

    void            goTroughTable           ();

    lua_State*      getState                ();

private:
    void            registerGlobalObject    ( const std::string& objectName, luaL_Reg* regs );

    size_t          getNewID                ();

private:
    lua_State*      m_luaState;

    static std::unordered_map<std::string,std::vector<size_t>> s_events;

    bool            m_reloadScrip;

    size_t          m_funcIDCounter;
    std::unordered_map<size_t,std::vector<int>>   m_funcs;
};

#endif // CSCRIPTSYSTEM_H
