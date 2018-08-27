#include "CAnimation.h"

#include <lua.hpp>

#include <CGame.h>
#include <Utility.h>

#include <iostream>
using namespace std;

CAnimation::CAnimation()
: m_index       ( 0 )
, m_clipsPerRow ( 1 )
, m_totalClips  ( 1 )
, m_rowIndex    ( 0 )
, m_delayFactor ( 1.0 )
, m_directional ( false )
, m_repeat      ( true )
{}

void CAnimation::setupFromLuaTable( lua_State* state, int index, const sf::Vector2f& size ) {
    if( lua_isnil( state, index ) ) {
        cout << "Error: Lua value at: " << index << " is not nil!" << endl;
        cout << "Unable to setup animation" << endl;
        return;
    }

    if( !lua_istable( state, index ) ) {
        std::string textureID = lua_tostring( state, - 1 );

        setupTexture( textureID, size );

        return;
    }

    lua_getfield( state, index, "texture" );
    if( !lua_isnil( state, - 1 ) ) {
        std::string textureID = lua_tostring( state, - 1 );

        m_sprite.setTexture( textureID );
    }

    lua_getfield( state, index, "clip" );
    if( !lua_isnil( state, - 1 ) ) {
        sf::IntRect clip = Util::intrectFromLuaTable( state, lua_gettop( state ) );

        m_baseClip  = clip;
        m_currClip  = clip;

        m_sprite.setClip( clip );
    }

    lua_getfield( state, index, "frameDelay" );
    if( lua_isnumber( state, - 1 ) ) {
        m_frameTimer.setLimit( lua_tonumber( state, - 1 ) );
    }

    lua_getfield( state, index, "directions" );
    if( lua_isboolean( state, - 1 ) ) {
        m_directional   = lua_toboolean( state, - 1 );
    }
    else {
        m_directional   = false;
    }

    lua_getfield( state, index, "doRepeat" );
    if( lua_isboolean( state, - 1 ) ) {
        m_repeat    = lua_toboolean( state, - 1 );
    }

    lua_pop( state, 5 );

    sf::Vector2u textureSize = m_sprite.getTexture()->getSize();

    // Calculate maximum clips in a single row
    m_clipsPerRow   = (float)textureSize.x / m_baseClip.width;

    // Calculate total amount of clips in image
    m_totalClips    = m_clipsPerRow * ( (float)textureSize.y / m_baseClip.height );

    // Reinitialize delay factor
    m_delayFactor   = 1.0;

    // Reset clip index
    m_index         = 0;
    m_rowIndex      = 0;

    // Set size of the sprite
    m_sprite.setSize( size );
}

void CAnimation::setupTexture( const std::string& textureID, const sf::Vector2f& size ) {
    m_sprite.setTexture( textureID );

    m_baseClip.width    = m_sprite.getTexture()->getSize().x;
    m_baseClip.height   = m_sprite.getTexture()->getSize().y;

    m_clipsPerRow   = 1;
    m_totalClips    = 1;

    m_index         = 0;
    m_rowIndex      = 0;

    m_sprite.setSize( size );

    storeDefaultColor();
}

void CAnimation::update( const sf::Time& delta ) {
    if( m_sprite.getTexture() == nullptr ) return;

    if( !m_repeat ) {
        if( m_index == m_totalClips - 1 ) {
            return;
        }
    }

    // If there is more than one clip
    if( m_totalClips > 1 ) {
        m_frameTimer.update( delta );

        if( m_frameTimer.isTime() ) {
            m_index++;

            // If animation is directinal, make sure that clip index doesn't go beyond maximum clip index in arow
            if( m_directional ) {
                if( m_index >= m_clipsPerRow ) {
                    m_index = 0;
                }
            }
            // If index is beyond last clip index, reset it
            else {
                if( m_index >= m_totalClips ) {
                    m_index = 0;
                }
            }

            // If animation is not directinal, calculate row index
            if( !m_directional ) {
                m_rowIndex      = floorf( m_index / m_clipsPerRow );
            }

            if( m_directional ) {
                m_currClip.left     = m_baseClip.left   + m_index * m_baseClip.width;
            }
            else {
                m_currClip.left     = m_baseClip.left   + ( m_index - ( m_rowIndex * m_clipsPerRow ) ) * m_baseClip.width;
            }

            m_currClip.top      = m_baseClip.top    + m_rowIndex * m_baseClip.height;

            m_sprite.setClip( m_currClip );

            m_frameTimer.reset();
        }
    }
}

void CAnimation::draw( sf::RenderTarget& window, sf::RenderStates states ) const {
    if( m_sprite.getTexture() == nullptr ) return;

    window.draw( m_sprite, states );
}

void CAnimation::setSize( const sf::Vector2f& size ) {
    if( m_sprite.getTexture() == nullptr ) return;

    m_sprite.setSize( size );
}

void CAnimation::setBaseClip( const sf::IntRect& clip ) {
    m_baseClip = clip;

    m_sprite.setClip( clip );
}

void CAnimation::setSpriteScale( const sf::Vector2f& size ) {
    m_sprite.setSize( size );
}

void CAnimation::setCenterSprite( const sf::Vector2f& size ) {
    m_sprite.setPos( size.x / 2, size.y / 2 );
}

void CAnimation::setCenterOrigin() {
    m_sprite.setOrigin( m_baseClip.width / 2, m_baseClip.height / 2 );
}

void CAnimation::resetIndex() {
    m_index     = 0;

    if( m_totalClips > 1 ) {
        if( m_directional ) {
            m_currClip.left     = m_baseClip.left   + m_index * m_baseClip.width;
        }
        else {
            m_currClip.left     = m_baseClip.left   + ( m_index - ( m_rowIndex * m_clipsPerRow ) ) * m_baseClip.width;
        }

        m_currClip.top      = m_baseClip.top    + m_rowIndex * m_baseClip.height;

        m_sprite.setClip( m_currClip );
    }
}

void CAnimation::setFrameTime( float seconds ) {
    m_frameTimer.setLimit( seconds );
}

void CAnimation::setFactorFrameTime( float factor ) {
    m_frameTimer.setLimitOnly( ( m_frameTimer.getLimit() / m_delayFactor ) * factor );

    m_delayFactor = factor;
}

void CAnimation::setRowIndex( const size_t& index ) {
    m_index     = 0;
    m_rowIndex  = index;

    m_currClip.left     = m_baseClip.left   + m_index * m_baseClip.width;
    m_currClip.top      = m_baseClip.top    + m_rowIndex * m_baseClip.height;

    m_sprite.setClip( m_currClip );
}

size_t CAnimation::getRowIndex() const {
    return m_rowIndex;
}

size_t CAnimation::getIndex() const {
    return m_index;
}

bool CAnimation::isDirectional() const {
    return m_directional;
}

void CAnimation::setColor( const sf::Color& color ) {
    m_sprite.setColor( color );

    storeDefaultColor();
}

void CAnimation::setColorModulation( const sf::Color& color ) {
    if( color == m_colorModulation || color == sf::Color::Transparent || color == sf::Color() ) return;

    resetDefaultColor();

    m_colorModulation   = color;

    m_sprite.setColor( m_colorDefault * m_colorModulation );
}

void CAnimation::delColorModulation( const sf::Color& color ) {
    if( color != m_colorModulation ) return;

    m_colorModulation   = sf::Color::Transparent;

    resetDefaultColor();
}

CSpriteWrapper& CAnimation::getSprite() {
    return m_sprite;
}

const CTimer& CAnimation::getTimer() {
    return m_frameTimer;
}

void CAnimation::storeDefaultColor() {
    m_colorDefault  = m_sprite.getColor();
}

void CAnimation::resetDefaultColor() {
    if( m_sprite.getColor() != m_colorDefault ) {
        m_sprite.setColor( m_colorDefault );
    }
}
