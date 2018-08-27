#ifndef CWINDOW_H
#define CWINDOW_H

#include "CDimensional.h"


class CWidget;

class CWindow : public CDimensional {
private:
    typedef std::vector<size_t> Size_ts;

public:
                        CWindow             ();
    virtual            ~CWindow             ();

    virtual void        handleInput         ( sf::Event event );
    virtual void        update              ( const sf::Time& delta );
    virtual void        draw                ( sf::RenderTarget& window, sf::RenderStates states = sf::RenderStates::Default ) const;

            size_t      getID               () const;
            void        setID               ( size_t ID );

    virtual Size_ts     getWidgetsID        () const;

    virtual void        addWidget           ( CWidget* widget );
            void        delWidget           ( CWidget* widget );
    virtual CWidget*    getWidget           ( size_t ID );

            void        setTexture          ( const std::string& textureID );
            void        setFrontTexture     ( const std::string& textureID );

            void        setWidgetBgTexture      ( size_t ID, const std::string& textureID );
            void        setWidgetMiddleTexture  ( size_t ID, const std::string& textureID );
            void        setWidgetFrontTexture   ( size_t ID, const std::string& textureID );

            void        setShown            ( bool shown = true );
            void        reverseShown        ();
            bool        isShown             () const;

            void        setActive           ( bool active = true );
            void        reverseActive       ();
            bool        isActive            () const;

            bool        hasMouseover        ( int x, int y );

            bool        isMouseover         () const;
            bool        hasMouseFocus       () const;

protected:
            void    onPosChange         ();

protected:
    bool            m_shown;
    bool            m_active;
    bool            m_mouseFocus;
    bool            m_mouseover;

    size_t          m_ID;

    sf::Sprite      m_bgSprite;
    sf::Sprite      m_frontSprite;

    std::vector<CWidget*>   m_widgets;

};

#endif // CWINDOW_H
