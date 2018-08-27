#ifndef CWINDOWCASTBAR_H
#define CWINDOWCASTBAR_H

#include "CWindow.h"
#include "CSpriteWrapper.h"
#include "CTextWrapper.h"

class CWindowCastbar : public CWindow {
public:

    void            update          ( const sf::Time& delta );
    void            draw            ( sf::RenderTarget& window, sf::RenderStates states ) const;

    void            setCBBorder     ( const CSpriteWrapper& spriteWrapper );
    void            setCBBackground ( const CSpriteWrapper& spriteWrapper );
    void            setCBProgress   ( const CSpriteWrapper& spriteWrapper );
    void            setCastTimeText ( const CTextWrapper& textWrapper );

private:
    CSpriteWrapper  m_borderSprite;
    CSpriteWrapper  m_bgSprite;
    CSpriteWrapper  m_progSprite;
    CTextWrapper    m_castText;
};

#endif // CWINDOWCASTBAR_H
