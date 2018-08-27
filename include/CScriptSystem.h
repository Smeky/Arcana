#ifndef CSCRIPTSYSTEM_H
#define CSCRIPTSYSTEM_H

#include "lua.hpp"
#include "string"
#include "unordered_map"
#include "vector"

class CScriptSystem {
public:
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

    static int      registerEvent           ( lua_State* luaState );

    void            goTroughTable           ();

    void            getFieldsFromTable      ( lua_State *state, int index, const char *fields, ...  );

    lua_State*      getState                ();

private:
    void            registerGlobalObject    ( const std::string& objectName, luaL_Reg* regs );

private:
    lua_State*      m_luaState;

    static std::unordered_map<std::string,std::vector<size_t>> s_events;

    bool            m_reloadScrip;
};

#endif // CSCRIPTSYSTEM_H
