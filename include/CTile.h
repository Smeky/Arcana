#ifndef CTILE_H
#define CTILE_H

#include <CDimensional.h>
#include <CSpriteWrapper.h>

class CTile : public CDimensional {
private:
    typedef std::vector<CSpriteWrapper> Sprites;

public:
                CTile       ();

    void        update      ( const sf::Time& delta );
    void        draw        ( sf::RenderTarget& window, sf::RenderStates states ) const;

    void        addSprite   ( const CSpriteWrapper& spriteWrapper );
    void        clearSprites();

    void        setSolid    ( bool solid = true );
    bool        isSolid     () const;

private:
    Sprites     m_sprites;

    bool        m_solid;

};

#endif // CTILE_H
