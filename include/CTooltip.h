#ifndef CTOOLTIP_H
#define CTOOLTIP_H

#include "CSpriteWrapper.h"
#include "CTextWrapper.h"

class CTooltip : public CDimensional {
private:
    typedef std::vector<CTextWrapper>   Texts;

public:
                    CTooltip        ();

    void            draw            ( sf::RenderTarget& window, sf::RenderStates states ) const;

    void            setWidth        ( float width );
    void            setBgTexture    ( const std::string& textureID );
    void            addText         ( const CTextWrapper& textWrapper );

private:
    void            onSizeChange    ();

    void            pushText        ( const CTextWrapper& textWrapper );
    void            fitText         ( const CTextWrapper& textWrapper );
    void            updateHeight    ( const CTextWrapper& textWrapper );

private:
    CSpriteWrapper  m_bgSprite;
    Texts           m_texts;
    float           m_textOffset;

};

#endif // CTOOLTIP_H
