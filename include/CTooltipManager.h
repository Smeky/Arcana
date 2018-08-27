#ifndef CTOOLTIPMANAGER_H
#define CTOOLTIPMANAGER_H

#include "SFML/Graphics.hpp"

class   CTooltip;
struct  lua_State;

class CTooltipManager {
public:
                    CTooltipManager     ();
                   ~CTooltipManager     ();

    void            handleInput         ( const sf::Event& event );
    void            update              ();
    void            render              ( sf::RenderTarget& window, sf::RenderStates states ) const;

    void            createFromLuaTable  ( lua_State* state, int index );
    void            destroyTooltip      ();
    bool            hasTooltip          () const;

public:
    static int      luaCreateTooltip    ( lua_State* state );
    static int      luaDestroyTooltip   ( lua_State* state );
    static int      luaHasTooltip       ( lua_State* state );

private:
    CTooltip*       m_tooltip;
    sf::FloatRect   m_mouseBox;

};

#endif // CTOOLTIPMANAGER_H
