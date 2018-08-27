#ifndef CABILITYMANAGER_H
#define CABILITYMANAGER_H

#include "unordered_map"
#include "vector"
#include "cstring"
#include "SFML/System/Vector2.hpp"

class CAbility;
class CCharacter;

struct lua_State;

class CAbilityManager {
private:
    typedef std::unordered_map<size_t,std::vector<int>> FunctionIDs;

public:
                    CAbilityManager     ();
                   ~CAbilityManager     ();

    void            close               ();

    void            callLuaFunction     ( size_t functionID, CCharacter* parent, const sf::Vector2f& destination );
    void            removeLuaFunction   ( size_t functionID );

    CAbility*       createFromLua       ( lua_State* state, int index );

    void            abilityToTable      ( lua_State* state, const CAbility* ability );

private:
    size_t          registerFunction    ( lua_State* state );

    size_t          getAbilityID        ();
    size_t          getFunctionID       ();

private:
    size_t          m_idAbilityCounter;
    size_t          m_idFunctionCounter;
    FunctionIDs     m_luaFunctions;

};

#endif // CABILITYMANAGER_H
