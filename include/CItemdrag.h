#ifndef CITEMDRAG_H
#define CITEMDRAG_H

#include "CGame.h"

class CItemdrag : public sf::Drawable {
public:
                    CItemdrag       ( size_t itemIndex, const sf::Vector2f& pos, const sf::Vector2f& offset, const std::string& textureID );

    void            update          ();
    void            draw            ( sf::RenderTarget& window, sf::RenderStates states ) const;

    size_t          getItemIndex    () const;

private:
    size_t          m_itemIndex;
    sf::Vector2f    m_pos;
    sf::Sprite      m_itemSprite;

};

#endif // CITEMDRAG_H
