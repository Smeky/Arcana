#ifndef CINTERFACESYSTEM_H
#define CINTERFACESYSTEM_H

#include "SFML/Graphics.hpp"
#include "SFML/System/Time.hpp"
#include <unordered_map>

#include "CTooltipManager.h"
#include "CSpriteWrapper.h"
#include "CTextWrapper.h"

struct lua_State;

class CWindow;
class CWidget;
class CTextWidget;
class CButtonImage;
class CButtonAbility;
class CTrackHealth;
class CTrackResource;
class CEffect;

class CGuiElement;

class CInterfaceSystem {
private:
    typedef std::vector<size_t> Size_ts;
    typedef std::unordered_map<size_t,std::vector<int>> FunctionIDs;

public:
                    CInterfaceSystem        ();
                   ~CInterfaceSystem        ();

    void            close                   ();

    void            handleInput             ( const sf::Event& event );
    void            update                  ( const sf::Time& delta );
    void            render                  ( sf::RenderTarget& window, sf::RenderStates states );

    size_t          registerWindow          ( CWindow* window );
    size_t          registerSprite          ( CSpriteWrapper sprite );
    size_t          registerText            ( CTextWrapper text );

    void            deleteSprite            ( size_t ID );
    void            deleteText              ( size_t ID );

    void            windowReverseShown      ( size_t ID );
    void            windowReverseActive     ( size_t ID );
    void            setWindowShown          ( size_t ID, bool shown );
    void            setWindowActive         ( size_t ID, bool active );

    bool            isWindowShown           ( size_t ID );
    bool            isWindowActive          ( size_t ID );

    bool            hasMousefocus           ();

    CTextWidget*    createTextWidgetFromLua ( lua_State* state, int index );
    CButtonImage*   createButtonFromTable   ( lua_State* state, int index );
    CButtonAbility* createAbilityFromTable  ( lua_State* state, int index );
    CTrackHealth*   createHpTrackFromTable  ( lua_State* state, int index );
    CTrackResource* createRpTrackFromTable  ( lua_State* state, int index );

    Size_ts         getWindowWidgetsID      ( size_t ID );
    CWindow*        getWindow               ( size_t ID );
    CWidget*        getWidget               ( size_t windowID, size_t widgetID );

    void            createEffectIcon        ( const CEffect* effect );

    void            setWidgetBgTexture      ( size_t windowID, size_t widgetID, const std::string& texture );
    void            setWidgetFrontTexture   ( size_t windowID, size_t widgetID, const std::string& texture );
    void            setWidgetMiddleTexture  ( size_t windowID, size_t widgetID, const std::string& texture );

    // New element design
    void            registerElement         ( CGuiElement* element );
    void            callLuaFunction         ( size_t functionID, void* element );
    void            removeLuaFunction       ( size_t functionID );
    size_t          registerLuaFunction     ( lua_State* state );

public:
    CTooltipManager TooltipManager;

public:
    static  int     luaClose                ( lua_State* state );
    static  int     luaCreateWindow         ( lua_State* state );
    static  int     luaCreateSprite         ( lua_State* state );
    static  int     luaCreateText           ( lua_State* state );
    static  int     luaCreateInventory      ( lua_State* state );
    static  int     luaCreateCastbar        ( lua_State* state );

    /** Element - new interface design - work in progress */
    static  int     luaNewElement           ( lua_State* state );
    static  int     luaNewButton            ( lua_State* state );
    static  int     luaNewCheckbox          ( lua_State* state );
    static  int     luaNewSlider            ( lua_State* state );
    static  int     luaNewDropbox           ( lua_State* state );

    static  int     luaDeleteSprite         ( lua_State* state );
    static  int     luaDeleteText           ( lua_State* state );

    static  int     luaWindowReverseShown   ( lua_State* state );
    static  int     luaWindowReverseActive  ( lua_State* state );
    static  int     luaSetWindowShown       ( lua_State* state );
    static  int     luaSetWindowActive      ( lua_State* state );

    static  int     luaIsWindowShown        ( lua_State* state );
    static  int     luaIsWindowActive       ( lua_State* state );

    static  int     luaHasMousefocus        ( lua_State* state );

    static  int     luaGetWindowWidgetsID   ( lua_State* state );
    static  int     luaSetWidgetBgTexture       ( lua_State* state );
    static  int     luaSetWidgetMiddleTexture   ( lua_State* state );
    static  int     luaSetWidgetFrontTexture    ( lua_State* state );

    static  int     luaGetWidgetBox         ( lua_State* state );

private:
    size_t          getNewID                ();
    size_t          getNewFuncID            ();

private:
    size_t          m_IDCounter;
    size_t          m_funcIDCounter;

    FunctionIDs     m_luaFunctions;

    std::vector<CWindow*>       m_windows;
    std::vector<CSpriteWrapper> m_sprites;
    std::vector<CTextWrapper>   m_texts;
    std::vector<CGuiElement*>   m_elements;
};

#endif // CINTERFACESYSTEM_H
