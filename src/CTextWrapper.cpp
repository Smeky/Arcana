#include "CTextWrapper.h"

#include <lua.hpp>

#include <CGame.h>
#include <Utility.h>

#include <iostream>
using namespace std;

CTextWrapper::CTextWrapper()
: m_origin      ( OriginPosition::ORIGIN_LEFT )
, m_fadeAlpha   ( 255 )
{

}

void CTextWrapper::draw( sf::RenderTarget& window, sf::RenderStates states ) const {
    sf::Transformable transformable;

    transformable.setPosition( m_pos );

    states.transform *= transformable.getTransform();

    window.draw( m_text, states );
}

void CTextWrapper::setID( size_t ID ) {
    m_ID = ID;
}

size_t CTextWrapper::getID() const {
    return m_ID;
}

void CTextWrapper::setString( const std::string& text ) {
    m_text.setString( text );

    setOrigin( m_origin );
}

void CTextWrapper::setFont( const std::string& fontID ) {
    m_text.setFont( CGame::AssetManager.getFont( fontID ) );
}

void CTextWrapper::setCharSize( size_t size ) {
    m_text.setCharacterSize( size );
}

void CTextWrapper::setStyle( const sf::Text::Style& style ) {
    m_text.setStyle( style );
}

void CTextWrapper::setColor( const sf::Color& color ) {
    m_text.setColor( color );
}

void CTextWrapper::setOrigin( const OriginPosition& origin ) {
    sf::FloatRect bounds = m_text.getLocalBounds();

    switch( origin ) {
    case OriginPosition::ORIGIN_CENTER: m_text.setOrigin( (int)( bounds.width / 2 ), (int)( bounds.height / 2 ) ); break;
    case OriginPosition::ORIGIN_RIGHT:  m_text.setOrigin( (int)bounds.width, (int)m_text.getOrigin().y ); break;
    default: break;
    }

    m_origin    = origin;
}

std::string CTextWrapper::getString() const {
    return m_text.getString();
}

sf::FloatRect CTextWrapper::getLocalBounds() const {
    return m_text.getLocalBounds();
}

void CTextWrapper::fade( float amount ) {
    m_fadeAlpha -= amount;

    if( m_fadeAlpha < 0 ) {
        m_fadeAlpha = 0;
    }

    sf::Color color = m_text.getColor();

    color.a = m_fadeAlpha + 0.5;

    m_text.setColor( color );
}

bool CTextWrapper::isFaded() const {
    return m_text.getColor().a == 0;
}

/** STATIC */

CTextWrapper CTextWrapper::loadFromTable( lua_State* state, int index, bool* result ) {
    CTextWrapper text;

    if( lua_istable( state, index ) ) {
        lua_getfield( state, index, "font" );
        if( lua_isstring( state, - 1 ) ) {
            text.setFont( lua_tostring( state, - 1 ) );
        }

        lua_getfield( state, index, "pos" );
        if( lua_istable( state, - 1 ) ) {
            text.setPos( Util::vectorFromTable<float>( state, lua_gettop( state ) ) );
        }

        lua_getfield( state, index, "size" );
        if( lua_isinteger( state, - 1 ) ) {
            text.setCharSize( lua_tointeger( state, - 1 ) );
        }

        lua_getfield( state, index, "style" );
        if( lua_isinteger( state, - 1 ) ) {
            text.setStyle( (sf::Text::Style)lua_tointeger( state, - 1 ) );
        }
        lua_getfield( state, index, "color" );
        if( lua_isstring( state, - 1 ) ) {
            text.setColor( Util::colorFromLuaTable( state, lua_gettop( state ) ) );
        }

        lua_getfield( state, index, "origin" );
        if( lua_isinteger( state, - 1 ) ) {
            text.setOrigin( (OriginPosition)lua_tointeger( state, - 1 ) );
        }

        lua_getfield( state, index, "string" );
        if( lua_isstring( state, - 1 ) ) {
            text.setString( lua_tostring( state, - 1 ) );
        }

        lua_pop( state, 7 );

        if( result != nullptr ) {
            *result = true;
        }
    }
    else {
        if( result != nullptr ) {
            *result = false;
        }
    }

    return text;
}
