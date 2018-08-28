#ifndef CGUIBUTTON_H
#define CGUIBUTTON_H

#include <CGuiElement.h>

class CGuiButton : public CGuiElement {
public:
    enum ButtonSoundType {
        ONCLICK,
        ONMOUSEOVER,
        TOTAL           // Keep last
    };

private:
    typedef std::vector<sf::Keyboard::Key>  Hotkeys;

    enum LuaFunction {
        FUNC_ONCLICK,
        FUNC_ONRCLICK,
        FUNC_ONPRESS,
        FUNC_ONRPRESS,
        FUNC_ONMOUSEENTER,
        FUNC_ONMOUSELEAVE,
        FUNC_COUNTER        // KEEP LAST
    };

public:
                            CGuiButton          ();
    virtual                ~CGuiButton          () {}

            void            handleInput         ( sf::Event event );
            void            update              ( const sf::Time& delta );
            void            draw                ( sf::RenderTarget& window, sf::RenderStates states ) const;

            bool            addHotkey           ( const sf::Keyboard::Key& hotkey );
            void            setMsg              ( const std::string& msg );
            void            setRMsg             ( const std::string& msg );

            void            registerFunction    ( LuaFunction type, size_t ID );

            size_t          addButtonTexture    ( lua_State* state, int index, bool* result = nullptr );
            bool            setButtonTexture    ( size_t spriteIndex );
            void            setPressOffset      ( const sf::Vector2f& offset );

            void            setSound            ( ButtonSoundType type, const std::string& ID );

            void            setLocked           ( bool locked = true );
            bool            isLocked            () const;

            void            setSelected         ( bool selected = true );
            bool            isSelected          () const;

            bool            isMouseover         () const;
            bool            isPressed           () const;
            bool            isClicked           () const;
            bool            isRPressed          () const;
            bool            isRClicked          () const;

    static  CGuiButton*     setupFromTable      ( lua_State* state, int index, bool* result = nullptr, const CGuiElement* parent = nullptr );

public:
    static  int             luaAddHotkey        ( lua_State* state );
    static  int             luaSetMsg           ( lua_State* state );
    static  int             luaSetRMsg          ( lua_State* state );
    static  int             luaAddButtonTexture ( lua_State* state );
    static  int             luaSetButtonTexture ( lua_State* state );
    static  int             luaSetSound         ( lua_State* state );
    static  int             luaSetLocked        ( lua_State* state );
    static  int             luaIsLocked         ( lua_State* state );
    static  int             luaAddFunction      ( lua_State* state );
    static  int             luaSetPressOffset   ( lua_State* state );
    static  int             luaSetSelected      ( lua_State* state );
    static  int             luaIsSelected       ( lua_State* state );

protected:
            bool            isHotkey            ( const sf::Keyboard::Key& hotkey );

            CSpriteWrapper* getSprite           ( size_t ID );

            void            applyPressOffset    ();
            void            resetPressOffset    ();

            void            playSound           ( ButtonSoundType type );

protected:
    size_t                  m_spriteButtonID;
    sf::Vector2u            m_buttonClipSize;
    sf::Vector2f            m_pressOffset;
    int                     m_luaFuncs[ LuaFunction::FUNC_COUNTER ];

    Hotkeys                 m_hotkeys;

    std::string             m_msg;
    std::string             m_rMsg;

    std::string             m_sounds[ ButtonSoundType::TOTAL ];

    bool                    m_locked;
    bool                    m_keySet;
    bool                    m_mouseover;
    bool                    m_pressed;
    bool                    m_clicked;
    bool                    m_rPressed;
    bool                    m_rClicked;

    bool                    m_selected;

    bool                    m_wasPressed;
};

#endif // CGUIBUTTON_H
