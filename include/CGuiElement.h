#ifndef CGUIELEMENT_H
#define CGUIELEMENT_H

#include <string>
#include <vector>

#include <CDimensional.h>
#include <CSpriteWrapper.h>
#include <CTextWrapper.h>

struct lua_State;

class CGuiElement;

enum class GuiType : int {
    GUI_DEFAULT,
    GUI_BUTTON,
    GUI_CHECKBOX,
    GUI_SLIDER,
    GUI_COUNTER     // Keep last
};

class CGuiElement : public CDimensional {
private:
    typedef std::vector<CGuiElement*>   GuiElements;
    typedef std::vector<CSpriteWrapper> SpriteWrappers;
    typedef std::vector<CTextWrapper>   TextWrappers;

public:
                            CGuiElement         ();
    virtual                ~CGuiElement         ();

    virtual void            handleInput         ( sf::Event event );
    virtual void            update              ( const sf::Time& delta );
    virtual void            draw                ( sf::RenderTarget& window, sf::RenderStates states ) const;

            GuiType         getType             () const;
            void            setParent           ( const CGuiElement* parent );

            // Creates new sprite wrapper from either whole table or textureID
            // Returns index of created sprite, - 1 in case creation failed
            size_t          addTexture          ( lua_State* state, int index );
            bool            setTexture          ( lua_State* state, int index, size_t ID );

            size_t          addText             ( lua_State* state, int index );
            bool            setText             ( lua_State* state, int index, size_t ID );

            size_t          getTexIDByIndex     ( size_t index );
            size_t          getTextIDByIndex    ( size_t index );

            void            setRelativePos      ( const sf::Vector2f& pos );
            sf::Vector2f    getRelativePos      () const;
            sf::Vector2f    getAbsolutePos      () const;

            void            addElement          ( CGuiElement* element );

            void            setActive           ( bool active = true );
            bool            isActive            () const;
            bool            reverseActive       ();

            void            setShown            ( bool shown = true );
            bool            isShown             () const;
            bool            reverseShown        ();

    // Crate new element and loads everything out of given table
    static  CGuiElement*    setupFromTable      ( lua_State* state, int index, bool* result = nullptr, const CGuiElement* parent = nullptr );
    // Loads everything out of given table to the element
    static  bool            loadFromTable       ( lua_State* state, int index, CGuiElement* element, const CGuiElement* parent = nullptr );

    static  std::string     guiTypeToString     ( const GuiType& type );

public:
    static  int             luaSetPos           ( lua_State* state );
    static  int             luaSetCenter        ( lua_State* state );
    static  int             luaSetSize          ( lua_State* state );
    static  int             luaSetPosSize       ( lua_State* state );
    static  int             luaSetBox           ( lua_State* state );

    static  int             luaGetPos           ( lua_State* state );
    static  int             luaGetCenter        ( lua_State* state );
    static  int             luaGetSize          ( lua_State* state );
    static  int             luaGetPosSize       ( lua_State* state );
    static  int             luaGetBox           ( lua_State* state );

    static  int             luaMove             ( lua_State* state );
    static  int             luaResize           ( lua_State* state );

    static  int             luaAddTexture       ( lua_State* state );
    static  int             luaSetTexture       ( lua_State* state );
    static  int             luaGetTexIDByIndex  ( lua_State* state );
    static  int             luaAddText          ( lua_State* state );
    static  int             luaSetText          ( lua_State* state );
    static  int             luaGetTextIDByIndex ( lua_State* state );
    static  int             luaSetRelativePos   ( lua_State* state );
    static  int             luaGetRelativePos   ( lua_State* state );

    static  int             luaSetActive        ( lua_State* state );
    static  int             luaIsActive         ( lua_State* state );
    static  int             luaReverseActive    ( lua_State* state );
    static  int             luaSetShown         ( lua_State* state );
    static  int             luaIsShown          ( lua_State* state );
    static  int             luaReverseShown     ( lua_State* state );

protected:
            void            onSizeChange        ();

            size_t          getNewID            ();

protected:
    size_t                  m_IDCounter;
    GuiType                 m_type;

    const CGuiElement*      m_parent;
    GuiElements             m_childs;

    SpriteWrappers          m_sprites;
    TextWrappers            m_texts;

    bool                    m_active;
    bool                    m_shown;
};

#endif // CGUIELEMENT_H
