#ifndef CEFFECTMANAGER_H
#define CEFFECTMANAGER_H

#include <unordered_map>
#include <vector>
#include <stddef.h>

struct lua_State;
class CEffect;
class CCharacter;

class CEffectManager {
private:
    typedef std::unordered_map<size_t,std::vector<int>> FunctionIDs;

public:
                    CEffectManager      ();

    void            createFromLuaTable  ( lua_State* state, int index, CCharacter* parent );

    void            callLuaFunction     ( size_t functionID, CCharacter* parent, const std::vector<float>& arguments = std::vector<float>() );
    void            removeLuaFunction   ( size_t functionID );

private:
    size_t          getNewID            ();

    void            loadLuaFunctions    ( lua_State* state, int index, CEffect* effect );
    size_t          registerFunction    ( lua_State* state );

private:
    size_t          m_funcIDCounter;
    FunctionIDs     m_luaFunctions;


};

#endif // CEFFECTMANAGER_H
