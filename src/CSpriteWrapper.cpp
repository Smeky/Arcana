#include "CSpriteWrapper.h"

#include <CGame.h>
#include <Utility.h>

CSpriteWrapper::CSpriteWrapper()
: m_colorBase   ( sf::Color::White )
{}

CSpriteWrapper::CSpriteWrapper( const std::string& textureID ) {
    setTexture( textureID );
}

void CSpriteWrapper::draw( sf::RenderTarget& window, sf::RenderStates states ) const {
    sf::Transformable transformable;

    transformable.setPosition( m_pos );

    states.transform *= transformable.getTransform();

    window.draw( m_sprite, states );
}

void CSpriteWrapper::setTexture( const std::string& textureID ) {
    m_sprite.setTexture( CGame::AssetManager.getTexture( textureID ) );

    Util::resizeSprite( m_sprite, m_size );
}

void CSpriteWrapper::setClip( const sf::IntRect& clip ) {
    if( clip.width == 0 && clip.height == 0 ) return;

    m_sprite.setTextureRect( clip );
}

void CSpriteWrapper::setClip( const sf::FloatRect& clip ) {
    setClip( Util::convertRect<int>( clip ) );
}

sf::IntRect CSpriteWrapper::getClip() const {
    return m_sprite.getTextureRect();
}

void CSpriteWrapper::resetClip() {
    const sf::Texture* texture = m_sprite.getTexture();
    sf::Vector2u texSize;

    if( texture != nullptr ) {
        texSize = texture->getSize();
    }

    m_sprite.setTextureRect( sf::IntRect( 0, 0, texSize.x, texSize.y ) );
}

void CSpriteWrapper::setID( size_t ID ) {
    m_ID = ID;
}

size_t CSpriteWrapper::getID() const {
    return m_ID;
}

void CSpriteWrapper::setOrigin( const sf::Vector2f& origin ) {
    m_sprite.setOrigin( origin );
}

void CSpriteWrapper::setOrigin( float x, float y ) {
    m_sprite.setOrigin( x, y );
}

void CSpriteWrapper::setColor( const sf::Color& color ) {
    m_sprite.setColor( color );

    m_colorBase = color;
}

sf::Color CSpriteWrapper::getColor() const {
    return m_sprite.getColor();
}

void CSpriteWrapper::addColorModul( const sf::Color& color ) {
    m_colorModuls.push_back( color );

    updateColor();
}

void CSpriteWrapper::delColorModul( const sf::Color& color ) {
    for( auto it = m_colorModuls.begin(); it != m_colorModuls.end(); it++ ) {
        if( *it == color ) {
            m_colorModuls.erase( it );

            updateColor();

            return;
        }
    }
}

sf::FloatRect CSpriteWrapper::getLocalBounds() const {
    return m_sprite.getLocalBounds();
}

sf::FloatRect CSpriteWrapper::getGlobalBounds() const {
    return m_sprite.getGlobalBounds();
}

const sf::Texture* CSpriteWrapper::getTexture() const {
    return m_sprite.getTexture();
}

CSpriteWrapper CSpriteWrapper::loadFromTable( lua_State* state, int index, bool* result ) {
    CSpriteWrapper wrapper;

    if( lua_istable( state, index ) ) {
        lua_getfield( state, index, "pos" );
        if( lua_istable( state, - 1 ) ) {
            wrapper.setPos( Util::vector2fFromLuaTable( state, lua_gettop( state ) ) );
        }

        lua_getfield( state, index, "size" );
        if( lua_istable( state, - 1 ) ) {
            wrapper.setSize( Util::vector2fFromLuaTable( state, lua_gettop( state ) ) );
        }

        lua_getfield( state, index, "clip" );
        if( lua_istable( state, - 1 ) ) {
            wrapper.setClip( Util::floatrectFromLuaTable( state, lua_gettop( state ) ) );
        }

        lua_getfield( state, index, "texture" );
        if( lua_isstring( state, - 1 ) ) {
            wrapper.setTexture( lua_tostring( state, - 1 ) );
        }

        lua_pop( state, 4 );

        if( result != nullptr ) {
            *result = true;
        }
    }
    else if( lua_isstring( state, index ) ) {
        wrapper.setTexture( lua_tostring( state, index ) );

        if( result != nullptr ) {
            *result = true;
        }
    }
    else {
        if( result != nullptr ) {
            *result = false;
        }
    }

    return wrapper;
}

/** PRIVATE */

void CSpriteWrapper::onSizeChange() {
    Util::resizeSprite( m_sprite, m_size );
}

void CSpriteWrapper::updateColor() {
    sf::Color result;

    if( !m_colorModuls.empty() ) {
        result = m_colorModuls.back();
    }
    else {
        result = m_colorBase;
    }

    m_sprite.setColor( result );
}
