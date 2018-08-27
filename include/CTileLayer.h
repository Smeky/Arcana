#ifndef CTILELAYER_H
#define CTILELAYER_H

#include <CTile.h>

class CTileLayer {
private:
    typedef std::vector<CTile> Tiles;

public:
                    CTileLayer      ();
                   ~CTileLayer      ();

    void            update          ( const sf::Time& delta, const sf::IntRect& indexBox );
    void            render          ( sf::RenderTarget& window, sf::RenderStates states, const sf::IntRect& indexBox ) const;

    void            setSize         ( const sf::Vector2f& size );
    void            addTile         ( const CTile& tile );

    void            addSprite       ( size_t index, const CSpriteWrapper& spriteWrapper );
    void            setSolid        ( size_t index, bool solid = true );

    const CTile*    getTile         ( size_t index ) const;

private:
    Tiles           m_tiles;

    sf::Vector2f    m_size;

    sf::Vector2f    m_tileSize;

};

#endif // CTILELAYER_H
