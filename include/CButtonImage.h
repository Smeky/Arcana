#ifndef CBUTTONIMAGE_H
#define CBUTTONIMAGE_H

#include "CButton.h"
#include "CTextWrapper.h"
#include "CSpriteWrapper.h"

class CButtonImage : public CButton {
public:
                    CButtonImage        ();

    virtual void    update              ( sf::Time delta );
    virtual void    draw                ( sf::RenderTarget& window, sf::RenderStates states ) const;

    void            setBgTexture        ( const std::string& textureID );
    void            setMiddleTexture    ( const std::string& textureID );
    void            setButtonTexture    ( const std::string& textureID );
    void            setFrontTexture     ( const std::string& textureID );
    void            setTextWrapper      ( const CTextWrapper& textWrapper );

    void            setMidDisplay       ( bool display = true );
    void            setTopDisplay       ( bool display = true );
    void            setButtonAboveMid   ( bool above = true );

protected:
    virtual void    onParentSet         ();
    virtual void    onSizeChange        ();

protected:
    CSpriteWrapper  m_spriteButton;
    CSpriteWrapper  m_spriteBg;
    CSpriteWrapper  m_spriteMid;
    CSpriteWrapper  m_spriteTop;
    CTextWrapper    m_text;

    bool            m_midDisplay;
    bool            m_topDisplay;
    bool            m_buttonAboveMid;   // If button image should be drawn above middle texture

    sf::Vector2f    m_clipSize;

};


#endif // CBUTTONIMAGE_H
