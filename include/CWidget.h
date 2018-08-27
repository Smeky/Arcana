#ifndef CWIDGET_H
#define CWIDGET_H

#include "CWindow.h"

class CWidget : public CDimensional {
public:
                    CWidget         ();
    virtual        ~CWidget         ();

    virtual void    handleInput     ( sf::Event event ) {}
    virtual void    update          ( sf::Time delta ) {}
    virtual void    draw            ( sf::RenderTarget& window, sf::RenderStates states ) const {}

    virtual void    setBgTexture    ( const std::string& textureID ) {}
    virtual void    setMiddleTexture( const std::string& textureID ) {}
    virtual void    setFrontTexture ( const std::string& textureID ) {}

            void    setID           ( size_t ID );
            size_t  getID           () const;

            void    setParent       ( CWindow* window );

            void    updatePos       ();

            bool    shouldDelete    () const;

protected:
    virtual void    onParentSet     () {}

    virtual void    onPosChange     ();

protected:
    size_t          m_ID;

    CWindow*        m_parent;

    bool            m_delete;       // In case of true, widget will be deleted
};

#endif // CWIDGET_H
