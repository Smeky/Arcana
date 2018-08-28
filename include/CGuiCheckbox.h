#ifndef CGUICHECKBOX_H
#define CGUICHECKBOX_H

#include <CGuiElement.h>

class CGuiCheckbox : public CGuiElement {
public:
    enum ButtonSoundType {
        ONCLICK,
        ONMOUSEOVER,
        TOTAL           // Keep last
    };

public:
                            CGuiCheckbox        ();

            void            handleInput         ( sf::Event event );
            void            update              ( const sf::Time& delta );
            void            draw                ( sf::RenderTarget& window, sf::RenderStates states ) const;

            /** Texture */
            size_t          addButtonTexture    ( lua_State* state, int index, bool* result = nullptr );
            bool            setButtonTexture    ( size_t spriteIndex );
            size_t          addCheckTexture     ( lua_State* state, int index, bool* result = nullptr );
            bool            setCheckTexture     ( size_t spriteIndex );

            /** Sound */
            void            setSound            ( ButtonSoundType type, const std::string& ID );

            /** Locked */
            void            setLocked           ( bool locked = true );
            bool            isLocked            () const;

            bool            isMouseover         () const;

            /** Checked */
            void            setChecked          ( bool checked = true );
            void            reverseChecked      ();
            bool            isChecked           () const;

    static  CGuiCheckbox*   setupFromTable      ( lua_State* state, int index, bool* result, const CGuiElement* parent = nullptr );

public:
    static  int             luaAddButtonTexture ( lua_State* state );
    static  int             luaSetButtonTexture ( lua_State* state );
    static  int             luaAddCheckTexture  ( lua_State* state );
    static  int             luaSetCheckTexture  ( lua_State* state );
    static  int             luaSetSound         ( lua_State* state );
    static  int             luaSetLocked        ( lua_State* state );
    static  int             luaIsLocked         ( lua_State* state );
    static  int             luaSetChecked       ( lua_State* state );
    static  int             luaIsChecked        ( lua_State* state );

private:
            CSpriteWrapper* getSprite           ( size_t ID );

            void            playSound           ( ButtonSoundType type );

private:
    size_t              m_spriteButtonID;
    size_t              m_spriteCheckID;
    sf::Vector2u        m_buttonClipSize;

    std::string         m_sounds[ ButtonSoundType::TOTAL ];

    bool                m_locked;
    bool                m_mouseover;
    bool                m_pressed;
    bool                m_clicked;
    bool                m_checked;

    bool                m_wasPressed;
};

#endif // CGUICHECKBOX_H
