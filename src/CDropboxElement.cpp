#include "CDropboxElement.h"

#include <lua.hpp>

#include <Utility.h>
#include <LuaWrapper.h>

#include <iostream>
using namespace std;

CDropboxElement::CDropboxElement()
: selected      ( false )
, mouseover     ( false )
{}

void CDropboxElement::draw( sf::RenderTarget& window, sf::RenderStates states ) const {
    states.transform *= getTransform();

    window.draw( sprite, states );
}

void CDropboxElement::updateSprite() {
    sf::Vector2u size = sprite.getTexture()->getSize();

    if( selected ) {
        sprite.setClip( sf::IntRect( size.x * 2, 0, size.x, size.y ) );
    }
    else if( mouseover ) {
        sprite.setClip( sf::IntRect( size.x * 1, 0, size.x, size.y ) );
    }
    else {
        sprite.setClip( sf::IntRect( 0, 0, size.x, size.y ) );
    }
}

CDropboxElement CDropboxElement::setupFromTable( lua_State* state, int index ) {
    CDropboxElement element;

    if( lua_istable( state, index ) ) {
        lua_getfield( state, index, "texture" );
        if( !lua_isnil( state, - 1 ) ) {
            element.sprite = CSpriteWrapper::loadFromTable( state, lua_gettop( state ) );
        }

        lua_pop( state, 1 );
    }
    else {
        std::cout << "Error: Unable to setup dropbox element out of non-table value" << std::endl;
    }

    return element;
}


