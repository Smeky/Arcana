#include "CGuiCheckbox.h"

#include <Utility.h>
#include <LuaWrapper.h>

#include <iostream>

CGuiCheckbox::CGuiCheckbox()
: m_spriteButtonID  ( 0 )
, m_spriteCheckID   ( 0 )
, m_locked          ( false )
, m_mouseover       ( false )
, m_pressed         ( false )
, m_clicked         ( false )
, m_checked         ( false )
, m_wasPressed      ( false )
{
    m_type  = GuiType::GUI_CHECKBOX;
}

void CGuiCheckbox::handleInput( sf::Event event ) {
    if( !m_active || m_locked ) return;

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
                playSound( CGuiCheckbox::ONMOUSEOVER );
            }
        }
        else {
            m_mouseover = false;
        }
    }
    else if( event.type == sf::Event::MouseButtonReleased ) {
        if( event.mouseButton.button == sf::Mouse::Left ) {
            if( m_pressed && m_mouseover ) {
                m_clicked = true;
            }

            m_pressed = false;
        }
    }
}

void CGuiCheckbox::update( const sf::Time& delta ) {
    if( !m_active ) return;

    for( auto it : m_childs ) {
        it->update( delta );
    }

    if( m_clicked ) {
        playSound( CGuiCheckbox::ONCLICK );

        reverseChecked();

        m_clicked = false;
    }

    if( m_spriteButtonID != 0 ) {
        CSpriteWrapper* sprite = getSprite( m_spriteButtonID );

        if( sprite != nullptr ) {
            // Update texture clip rectangle based on button state
            if( m_locked ) {
                sprite->setClip( sf::IntRect( m_buttonClipSize.x * 3, 0, m_buttonClipSize.x, m_buttonClipSize.y ) );
            }
            else if( m_pressed ) {
                sprite->setClip( sf::IntRect( m_buttonClipSize.x * 2, 0, m_buttonClipSize.x, m_buttonClipSize.y ) );
            }
            else if( m_mouseover ) {
                sprite->setClip( sf::IntRect( m_buttonClipSize.x * 1, 0, m_buttonClipSize.x, m_buttonClipSize.y ) );
            }
            else {
                sprite->setClip( sf::IntRect( m_buttonClipSize.x * 0, 0, m_buttonClipSize.x, m_buttonClipSize.y ) );
            }
        }
    }

    m_wasPressed    = m_pressed;
}

void CGuiCheckbox::draw( sf::RenderTarget& window, sf::RenderStates states ) const {
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
        if( it.getID() == m_spriteCheckID ) {
            if( m_checked ) {
                window.draw( it, states );
            }
        }
        else {
            window.draw( it, states );
        }
    }

    for( auto& it : m_texts ) {
        window.draw( it, states );
    }

    for( auto it : m_childs ) {
        window.draw( *it, states );
    }
}

size_t CGuiCheckbox::addButtonTexture( lua_State* state, int index, bool* result ) {
    bool success;

    CSpriteWrapper sprite = CSpriteWrapper::loadFromTable( state, index, &success );

    if( success ) {
        m_buttonClipSize    = sprite.getTexture()->getSize();
        m_buttonClipSize.x  = m_buttonClipSize.x / 4;

        sprite.setClip( sf::IntRect( 0, 0, m_buttonClipSize.x, m_buttonClipSize.y ) );

        sprite.setSize( m_size );
        sprite.setID( getNewID() );

        m_sprites.push_back( sprite );

        m_spriteButtonID    = sprite.getID();

        return sprite.getID();
    }
    else {
        return 0;
    }
}

bool CGuiCheckbox::setButtonTexture( size_t ID ) {
    for( const auto& it : m_sprites ) {
        if( it.getID() == ID ) {
            m_spriteButtonID = ID;

            m_buttonClipSize    = it.getTexture()->getSize();
            m_buttonClipSize.x  = m_buttonClipSize.x / 4;

            return true;
        }
    }

    return false;
}

size_t CGuiCheckbox::addCheckTexture( lua_State* state, int index, bool* result ) {
    bool success;

    CSpriteWrapper sprite = CSpriteWrapper::loadFromTable( state, index, &success );

    if( success ) {
        sprite.setSize( m_size );
        sprite.setID( getNewID() );

        m_sprites.push_back( sprite );

        m_spriteCheckID = sprite.getID();

        return sprite.getID();
    }
    else {
        return 0;
    }
}

bool CGuiCheckbox::setCheckTexture( size_t ID ) {
    for( const auto& it : m_sprites ) {
        if( it.getID() == ID ) {
            m_spriteCheckID = ID;

            return true;
        }
    }

    return false;
}

void CGuiCheckbox::setSound( ButtonSoundType type, const std::string& ID ) {
    m_sounds[ type ]    = ID;
}

void CGuiCheckbox::setLocked( bool locked ) {
    m_locked    = locked;
}

bool CGuiCheckbox::isLocked() const {
    return m_locked;
}

bool CGuiCheckbox::isMouseover() const {
    return m_mouseover;
}

void CGuiCheckbox::setChecked( bool checked ) {
    m_checked   = checked;
}

void CGuiCheckbox::reverseChecked() {
    m_checked   = !m_checked;
}

bool CGuiCheckbox::isChecked() const {
    return m_checked;
}

CGuiCheckbox* CGuiCheckbox::setupFromTable( lua_State* state, int index, bool* result, const CGuiElement* parent ) {
    CGuiCheckbox* checkbox = new CGuiCheckbox();

    bool success = CGuiElement::loadFromTable( state, index, checkbox, parent );

    if( success ) {
        bool locked = false;
        lWrapper::valueFromLuaTable<bool>( state, index, "locked", locked );
        checkbox->setLocked( locked );

        bool checked = false;
        lWrapper::valueFromLuaTable<bool>( state, index, "checked", checked );
        checkbox->setChecked( checked );

        lua_getfield( state, index, "checkboxTex" );
        if( !lua_isnil( state, - 1 ) ) {
            checkbox->addButtonTexture( state, lua_gettop( state ) );
        }

        lua_getfield( state, index, "checkTex" );
        if( !lua_isnil( state, - 1 ) ) {
            checkbox->addCheckTexture( state, lua_gettop( state ) );
        }

        lua_getfield( state, index, "sounds" );
        if( lua_istable( state, - 1 ) ) {
            for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
                // Copy key
                lua_pushvalue( state, - 2 );

                const std::string key = lua_tostring( state, - 1 );

                lua_pop( state, 1 );

                if( key == "onClick" ) {
                    checkbox->setSound( CGuiCheckbox::ONCLICK, lua_tostring( state, - 1 ) );
                }
                else if( key == "onMouseover" ) {
                    checkbox->setSound( CGuiCheckbox::ONMOUSEOVER, lua_tostring( state, - 1 ) );
                }
            }
        }

        lua_pop( state, 3 );
    }

    if( result != nullptr ) {
        *result = success;
    }

    return checkbox;
}

/** LUA */

int CGuiCheckbox::luaAddButtonTexture( lua_State* state ) {
    int argc = lua_gettop( state );

    size_t result = 0;

    if( argc != 3 ) {
        std::cout << "Error: Unable to add button sprite to checkbox. Wrong amount of arguments" << std::endl;
    }
    else {
        if( lua_islightuserdata( state, argc - 1 ) ) {
            CGuiCheckbox* checkbox = (CGuiCheckbox*)lua_touserdata( state, argc - 1 );

            if( checkbox->getType() == GuiType::GUI_CHECKBOX ) {
                result = checkbox->addButtonTexture( state, argc );
            }
            else {
                std::cout   << "Error: Wrong gui element type. Expected "
                            << CGuiElement::guiTypeToString( GuiType::GUI_CHECKBOX )
                            << ", got: " << CGuiElement::guiTypeToString( checkbox->getType() )
                            << std::endl;
            }
        }
    }

    lua_pop( state, argc );

    lua_pushinteger( state, result );

    return 1;
}

int CGuiCheckbox::luaSetButtonTexture( lua_State* state ) {
    int argc = lua_gettop( state );

    bool result = false;

    if( argc == 3 ) {
        CGuiCheckbox* checkbox = (CGuiCheckbox*)lua_touserdata( state, argc - 1 );

        if( checkbox->getType() == GuiType::GUI_CHECKBOX ) {
            result = checkbox->setButtonTexture( lua_tointeger( state, argc ) );
        }
        else {
            std::cout   << "Error: Wrong gui element type. Expected "
                        << CGuiElement::guiTypeToString( GuiType::GUI_CHECKBOX )
                        << ", got: " << CGuiElement::guiTypeToString( checkbox->getType() )
                        << std::endl;
        }
    }

    lua_pop( state, argc );

    lua_pushboolean( state, result );

    return 1;
}

int CGuiCheckbox::luaAddCheckTexture( lua_State* state ) {
    int argc = lua_gettop( state );

    size_t result = 0;

    if( argc != 3 ) {
        std::cout << "Error: Unable to add button sprite to checkbox. Wrong amount of arguments" << std::endl;
    }
    else {
        if( lua_islightuserdata( state, argc - 1 ) ) {
            CGuiCheckbox* checkbox = (CGuiCheckbox*)lua_touserdata( state, argc - 1 );

            if( checkbox->getType() == GuiType::GUI_CHECKBOX ) {
                result = checkbox->addCheckTexture( state, argc );
            }
            else {
                std::cout   << "Error: Wrong gui element type. Expected "
                            << CGuiElement::guiTypeToString( GuiType::GUI_CHECKBOX )
                            << ", got: " << CGuiElement::guiTypeToString( checkbox->getType() )
                            << std::endl;
            }
        }
    }

    lua_pop( state, argc );

    lua_pushinteger( state, result );

    return 1;
}

int CGuiCheckbox::luaSetCheckTexture( lua_State* state ) {
    int argc = lua_gettop( state );

    bool result = false;

    if( argc == 3 ) {
        CGuiCheckbox* checkbox = (CGuiCheckbox*)lua_touserdata( state, argc - 1 );

        if( checkbox->getType() == GuiType::GUI_CHECKBOX ) {
            result = checkbox->setCheckTexture( lua_tointeger( state, argc ) );
        }
        else {
            std::cout   << "Error: Wrong gui element type. Expected "
                        << CGuiElement::guiTypeToString( GuiType::GUI_CHECKBOX )
                        << ", got: " << CGuiElement::guiTypeToString( checkbox->getType() )
                        << std::endl;
        }
    }

    lua_pop( state, argc );

    lua_pushboolean( state, result );

    return 1;
}

int CGuiCheckbox::luaSetSound( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc == 4 ) {
        if( lua_islightuserdata( state, argc - 1 ) ) {
            CGuiCheckbox*   checkbox    = (CGuiCheckbox*)lua_touserdata( state, argc - 2 );
            std::string     ID          = lua_tostring( state, argc - 1 );
            ButtonSoundType type        = (ButtonSoundType)lua_tointeger( state, argc );

            if( checkbox != nullptr ) {
                checkbox->setSound( type, ID );
            }
        }
        else {
            std::cout << "Error: Unable to set sound to checkbox. Light user data expected" << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to set sound to checkbox. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    return 0;
}

int CGuiCheckbox::luaSetLocked( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc == 3 ) {
        CGuiCheckbox* checkbox = (CGuiCheckbox*)lua_touserdata( state, argc - 1 );

        if( checkbox->getType() == GuiType::GUI_CHECKBOX ) {
            checkbox->setLocked( lua_toboolean( state, argc ) );
        }
        else {
            std::cout   << "Error: Wrong gui element type. Expected "
                        << CGuiElement::guiTypeToString( GuiType::GUI_CHECKBOX )
                        << ", got: " << CGuiElement::guiTypeToString( checkbox->getType() )
                        << std::endl;
        }
    }

    lua_pop( state, argc );

    return 0;
}

int CGuiCheckbox::luaIsLocked( lua_State* state ) {
    int argc = lua_gettop( state );

    bool result = false;

    if( argc == 2 ) {
        CGuiCheckbox* checkbox = (CGuiCheckbox*)lua_touserdata( state, argc );

        if( checkbox->getType() == GuiType::GUI_CHECKBOX ) {
            result = checkbox->isLocked();
        }
        else {
            std::cout   << "Error: Wrong gui element type. Expected "
                        << CGuiElement::guiTypeToString( GuiType::GUI_CHECKBOX )
                        << ", got: " << CGuiElement::guiTypeToString( checkbox->getType() )
                        << std::endl;
        }
    }

    lua_pop( state, argc );

    lua_pushboolean( state, result );

    return 1;
}

int CGuiCheckbox::luaSetChecked( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc == 3 ) {
        CGuiCheckbox* checkbox = (CGuiCheckbox*)lua_touserdata( state, argc - 1 );

        if( checkbox->getType() == GuiType::GUI_CHECKBOX ) {
            checkbox->setChecked( lua_toboolean( state, argc ) );
        }
        else {
            std::cout   << "Error: Wrong gui element type. Expected "
                        << CGuiElement::guiTypeToString( GuiType::GUI_CHECKBOX )
                        << ", got: " << CGuiElement::guiTypeToString( checkbox->getType() )
                        << std::endl;
        }
    }

    lua_pop( state, argc );

    return 0;
}

int CGuiCheckbox::luaIsChecked( lua_State* state ) {
    int argc = lua_gettop( state );

    bool result = false;

    if( argc == 2 ) {
        CGuiCheckbox* checkbox = (CGuiCheckbox*)lua_touserdata( state, argc );

        if( checkbox != nullptr ) {
            if( checkbox->getType() == GuiType::GUI_CHECKBOX ) {
                result = checkbox->isChecked();
            }
            else {
                std::cout   << "Error: Wrong gui element type. Expected "
                            << CGuiElement::guiTypeToString( GuiType::GUI_CHECKBOX )
                            << ", got: " << CGuiElement::guiTypeToString( checkbox->getType() )
                            << std::endl;
            }
        }

    }

    lua_pop( state, argc );

    lua_pushboolean( state, result );

    return 1;
}

/** PRIVATE */

CSpriteWrapper* CGuiCheckbox::getSprite( size_t ID ) {
    if( ID != 0 ) {
        for( auto& it : m_sprites ) {
            if( it.getID() == ID ) {
                return &it;
            }
        }
    }

    return nullptr;
}

void CGuiCheckbox::playSound( ButtonSoundType type ) {
    CGame::AudioSystem.playSound( m_sounds[ type ], CAudioSystem::INTERFACE );
}
