#include "CGuiDropbox.h"

#include <lua.hpp>

#include <Utility.h>
#include <LuaWrapper.h>

#include <iostream>
using namespace std;


CGuiDropbox::CGuiDropbox()
: m_selected        ( 0 )
, m_mouseover       ( false )
, m_pressed         ( false )
, m_clicked         ( false )
, m_wasPressed      ( false )
, m_open            ( false )
{}

void CGuiDropbox::handleInput( sf::Event event ) {
    if( !m_active ) return;

    for( auto it : m_childs ) {
        it->handleInput( event );
    }

    if( m_mouseover ) {
        if( event.type == sf::Event::MouseButtonPressed ) {
            if( event.mouseButton.button == sf::Mouse::Left ) {
                m_pressed = true;
            }
        }
    }

    if( event.type == sf::Event::MouseMoved ) {
        bool wasMouseover = m_mouseover;

        if( getBox().contains( event.mouseMove.x, event.mouseMove.y ) ) {
            m_mouseover = true;

            if( !wasMouseover ) {
//                playSound( CGuiSlider::ONMOUSEOVER );
            }
        }
        else {
            if( m_open ) {
                for( auto& it : m_elements ) {
                    if( it.getBox().contains( event.mouseMove.x, event.mouseMove.y ) ) {
                        it.mouseover = true;
                    }
                    else {
                        it.mouseover = false;
                    }
                }
            }

            m_mouseover = false;
        }
    }
    else if( event.type == sf::Event::MouseButtonReleased ) {
        if( event.mouseButton.button == sf::Mouse::Left ) {
            if( m_pressed && m_mouseover ) {
                m_clicked = true;
            }

            bool inDropbox = false;

            if( m_open ) {
                for( size_t index = 0; index < m_elements.size(); index++ ) {
                    if( m_elements[ index ].mouseover ) {
                        m_elements[ index ].selected    = true;
                        m_elements[ index ].mouseover   = false;

                        inDropbox   = true;
                    }
                }
            }

            if( !inDropbox ) {
                m_open      = false;
            }

            m_pressed   = false;
        }
    }
}

void CGuiDropbox::update( const sf::Time& delta ) {
    if( !m_active ) return;

    for( auto it : m_childs ) {
        it->update( delta );
    }

    if( m_clicked ) {
        m_open  = !m_open;

//        playSound( CGuiButton::ONCLICK );

        m_clicked = false;
    }

    if( m_spriteDropboxID != 0 ) {
        CSpriteWrapper* sprite = getSprite( m_spriteDropboxID );

        if( sprite != nullptr ) {
            if( m_pressed ) {
                sprite->setClip( sf::IntRect( m_buttonClipSize.x * 2, 0, m_buttonClipSize.x, m_buttonClipSize.y ) );
            }
            else if( m_mouseover ) {
                sprite->setClip( sf::IntRect( m_buttonClipSize.x * 1, 0, m_buttonClipSize.x, m_buttonClipSize.y ) );
            }
            else {
                sprite->setClip( sf::IntRect( 0, 0, m_buttonClipSize.x, m_buttonClipSize.y ) );
            }
        }
    }

    m_wasPressed    = m_pressed;
}

void CGuiDropbox::draw( sf::RenderTarget& window, sf::RenderStates states ) const {
    if( !m_shown ) return;

    if( m_parent == nullptr ) {
        states.transform    *= getTransform();
    }
    else {
        sf::Transformable transformable;
        transformable.setPosition( getRelativePos() );
        states.transform    *= transformable.getTransform();
    }

    for( auto& it : m_sprites ) {
        window.draw( it, states );
    }

    for( auto& it : m_texts ) {
        window.draw( it, states );
    }

    for( auto it : m_childs ) {
        window.draw( *it, states );
    }

    if( m_open ) {
        for( auto& it : m_elements ) {
            window.draw( it, states );
        }
    }
}

size_t CGuiDropbox::addDropboxTexture( lua_State* state, int index, bool* result ) {
    bool success;

    CSpriteWrapper sprite = CSpriteWrapper::loadFromTable( state, index, &success );

    if( success ) {
        m_buttonClipSize    = sprite.getTexture()->getSize();
        m_buttonClipSize.x  = m_buttonClipSize.x / 3;

        sprite.setClip( sf::IntRect( 0, 0, m_buttonClipSize.x, m_buttonClipSize.y ) );

        sprite.setSize( m_size );
        sprite.setID( getNewID() );

        m_sprites.push_back( sprite );

        m_spriteDropboxID   = sprite.getID();

        return sprite.getID();
    }
    else {
        return 0;
    }
}

bool CGuiDropbox::setDropboxTexture( size_t ID ) {
    for( const auto& it : m_sprites ) {
        if( it.getID() == ID ) {
            m_spriteDropboxID   = ID;

            m_buttonClipSize    = it.getTexture()->getSize();
            m_buttonClipSize.x  = m_buttonClipSize.x / 3;

            return true;
        }
    }

    return false;
}

void CGuiDropbox::addElement( const CDropboxElement& element ) {
    CDropboxElement temp = element;

    sf::Vector2f pos    = m_pos;
    sf::Vector2f size   = m_size;

    pos.y   += m_elements.size() * m_size.y;

    temp.setBox( pos, size );

    m_elements.push_back( temp );
}

void CGuiDropbox::setSelected( size_t index ) {
    if( index < m_elements.size() ) {
        m_selected  = index;

        m_elements[ index ].selected = true;
    }
}

size_t CGuiDropbox::getSelected() const {
    return m_selected;
}

CGuiDropbox* CGuiDropbox::setupFromTable( lua_State* state, int index, bool* result, const CGuiElement* parent ) {
    CGuiDropbox* dropbox = new CGuiDropbox();

    bool success = CGuiElement::loadFromTable( state, index, dropbox, parent );

    if( success ) {
        lua_getfield( state, index, "dropboxTex" );
        if( !lua_isnil( state, - 1 ) ) {
            dropbox->addDropboxTexture( state, lua_gettop( state ) );
        }

        lua_getfield( state, index, "elements" );
        if( lua_istable( state, - 1 ) ) {
            sf::Vector2f pos    = dropbox->getPos();
            sf::Vector2f size   = dropbox->getSize();

            for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
                CDropboxElement element = CDropboxElement::setupFromTable( state, lua_gettop( state ) );

                pos.y   += size.y;

                element.setBox( pos, size );

                dropbox->addElement( element );
            }
        }

        size_t selected = 0;
        lWrapper::valueFromLuaTable<size_t>( state, index, "selected", selected );
        dropbox->setSelected( selected );

//
//        lua_getfield( state, index, "sounds" );
//        if( lua_istable( state, - 1 ) ) {
//            for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
//                // Copy key
//                lua_pushvalue( state, - 2 );
//
//                const std::string key = lua_tostring( state, - 1 );
//
//                lua_pop( state, 1 );
//
//                if( key == "onClick" ) {
//                    dropbox->setSound( CGuiButton::ONCLICK, lua_tostring( state, - 1 ) );
//                }
//                else if( key == "onMouseover" ) {
//                    dropbox->setSound( CGuiButton::ONMOUSEOVER, lua_tostring( state, - 1 ) );
//                }
//            }
//        }

        lua_pop( state, 2 );
    }

    if( result != nullptr ) {
        *result = success;
    }

    return dropbox;
}

/** LUA */

int CGuiDropbox::luaAddDropElement( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc == 3 ) {
        if( lua_islightuserdata( state, argc - 1 ) ) {
            CGuiDropbox* dropbox = (CGuiDropbox*)lua_touserdata( state, argc - 1 );

            if( dropbox != nullptr ) {
                dropbox->addElement( CDropboxElement::setupFromTable( state, argc ) );
            }
            else {
                std::cout << "Error: Unable to add dropbox element. Got nullptr" << std::endl;
            }
        }
        else {
            std::cout << "Error: Unable to add dropbox element. Light user data expected" << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to add dropbox element. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    return 0;
}

int CGuiDropbox::luaSetSelected( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc == 3 ) {
        if( lua_islightuserdata( state, argc - 1 ) ) {
            CGuiDropbox* dropbox = (CGuiDropbox*)lua_touserdata( state, argc - 1 );

            if( dropbox != nullptr ) {
                dropbox->setSelected( lua_tointeger( state, argc ) );
            }
            else {
                std::cout << "Error: Unable to set selected. Got nullptr" << std::endl;
            }
        }
        else {
            std::cout << "Error: Unable to set selected. Light user data expected" << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to set selected. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    return 0;
}

int CGuiDropbox::luaGetSelected( lua_State* state ) {
    int argc = lua_gettop( state );

    int index = - 1;

    if( argc == 2 ) {
        if( lua_islightuserdata( state, argc ) ) {
            CGuiDropbox* dropbox = (CGuiDropbox*)lua_touserdata( state, argc );

            if( dropbox != nullptr ) {
                index = dropbox->getSelected();
            }
            else {
                std::cout << "Error: Unable to get selected. Got nullptr" << std::endl;
            }
        }
        else {
            std::cout << "Error: Unable to get selected. Light user data expected" << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to get selected. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    lua_pushinteger( state, index );

    return 1;
}

/** PRIVATE */

CSpriteWrapper* CGuiDropbox::getSprite( size_t ID ) {
    if( ID != 0 ) {
        for( auto& it : m_sprites ) {
            if( it.getID() == ID ) {
                return &it;
            }
        }
    }

    return nullptr;

}
