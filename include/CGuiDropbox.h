#ifndef CGUIDROPBOX_H
#define CGUIDROPBOX_H

#include <vector>

#include <CGuiElement.h>
#include <CDropboxElement.h>

class CGuiDropbox : public CGuiElement {
private:
    typedef std::vector<CDropboxElement>        DropboxElements;

public:
                            CGuiDropbox             ();

            void            handleInput             ( sf::Event event );
            void            update                  ( const sf::Time& delta );
            void            draw                    ( sf::RenderTarget& window, sf::RenderStates states ) const;

            /** Dropbox specific */
            size_t          addDropboxTexture       ( lua_State* state, int index, bool* result = nullptr );
            bool            setDropboxTexture       ( size_t ID );

            void            addElement              ( const CDropboxElement& element );
            void            setSelected             ( size_t index );
            size_t          getSelected             () const;

    static  CGuiDropbox*    setupFromTable          ( lua_State* state, int index, bool* result = nullptr, const CGuiElement* parent = nullptr );

public:
    static  int             luaAddDropElement       ( lua_State* state );
    static  int             luaSetSelected          ( lua_State* state );
    static  int             luaGetSelected          ( lua_State* state );

private:
            CSpriteWrapper* getSprite               ( size_t ID );

private:
    size_t              m_spriteDropboxID;
    sf::Vector2u        m_buttonClipSize;

    DropboxElements     m_elements;

    size_t              m_selected;

    bool                m_mouseover;
    bool                m_pressed;
    bool                m_clicked;
    bool                m_wasPressed;

    bool                m_open;
};

#endif // CGUIDROPBOX_H
