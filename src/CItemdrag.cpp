#include "CItemdrag.h"

#include "Utility.h"

CItemdrag::CItemdrag( size_t itemIndex, const sf::Vector2f& pos, const sf::Vector2f& offset, const std::string& textureID )
: m_itemIndex       ( itemIndex )
, m_pos             ( pos )
{
    m_itemSprite.setTexture( CGame::AssetManager.getTexture( textureID ) );
    m_itemSprite.setOrigin( offset );
}

void CItemdrag::update() {
    m_pos = Util::convertVector<float>( sf::Mouse::getPosition( CGame::GameWindow ) );
}

void CItemdrag::draw( sf::RenderTarget& window, sf::RenderStates states ) const {
    sf::Transformable transformable;

    transformable.setPosition( m_pos );

    states.transform *= transformable.getTransform();

    window.draw( m_itemSprite, states );
}

size_t CItemdrag::getItemIndex() const {
    return m_itemIndex;
}
