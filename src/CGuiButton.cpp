#include "CGuiButton.h"

#include <CGame.h>
#include <Utility.h>
#include <LuaWrapper.h>

#include <iostream>
using namespace std;

CGuiButton::CGuiButton()
: m_spriteButtonID  ( 0 )
, m_locked          ( false )
, m_keySet          ( false )
, m_mouseover       ( false )
, m_pressed         ( false )
, m_clicked         ( false )
, m_rPressed        ( false )
, m_rClicked        ( false )
, m_wasPressed      ( false )
{
    m_type  = GuiType::GUI_BUTTON;

    for( int i = 0; i < LuaFunction::FUNC_COUNTER; i++ ) {
        m_luaFuncs[ i ]     = 0;
    }
}

void CGuiButton::handleInput( sf::Event event ) {
    if( !m_active || m_locked ) return;

    for( auto it : m_childs ) {
        it->handleInput( event );
    }

    if( m_mouseover ) {
        if( event.type == sf::Event::MouseButtonPressed ) {
            switch( event.mouseButton.button ) {
            case sf::Mouse::Left:   m_pressed   = true; CGame::InterfaceSystem.callLuaFunction( m_luaFuncs[ LuaFunction::FUNC_ONPRESS ],  this ); break;
            case sf::Mouse::Right:  m_rPressed  = true; CGame::InterfaceSystem.callLuaFunction( m_luaFuncs[ LuaFunction::FUNC_ONRPRESS ], this ); break;
            default: break;
            }
        }
    }

    if( m_keySet ) {
        if( event.type == sf::Event::KeyPressed ) {
            if( isHotkey( event.key.code ) ) {
                m_pressed = true;

                CGame::InterfaceSystem.callLuaFunction( m_luaFuncs[ LuaFunction::FUNC_ONPRESS ], this );
            }
        }
        else if( event.type == sf::Event::KeyReleased ) {
            if( isHotkey( event.key.code ) ) {
                m_pressed = false;
                m_clicked = true;
            }
        }
    }

    if( event.type == sf::Event::MouseMoved ) {
        bool wasMouseover = m_mouseover;

        if( getBox().contains( event.mouseMove.x, event.mouseMove.y ) ) {
            m_mouseover = true;

            if( !wasMouseover ) {
                CGame::InterfaceSystem.callLuaFunction( m_luaFuncs[ LuaFunction::FUNC_ONMOUSEENTER ], this );
            }
        }
        else {
            m_mouseover = false;

            if( wasMouseover ) {
                CGame::InterfaceSystem.callLuaFunction( m_luaFuncs[ LuaFunction::FUNC_ONMOUSELEAVE ], this );
            }
        }
    }
    else if( event.type == sf::Event::MouseButtonReleased ) {
        if( event.mouseButton.button == sf::Mouse::Left ) {
            if( m_pressed && m_mouseover ) {
                m_clicked = true;
            }

            m_pressed = false;
        }
        else if( event.mouseButton.button == sf::Mouse::Right ) {
            if( m_rPressed && m_mouseover ) {
                m_rClicked = true;
            }

            m_rPressed = false;
        }
    }
}

void CGuiButton::update( const sf::Time& delta ) {
    if( !m_active ) return;

    for( auto it : m_childs ) {
        it->update( delta );
    }

    if( !m_wasPressed && m_pressed ) {
        applyPressOffset();
    }
    else if( m_wasPressed && !m_pressed ) {
        resetPressOffset();
    }

    if( m_clicked ) {
        CGame::InterfaceSystem.callLuaFunction( m_luaFuncs[ LuaFunction::FUNC_ONCLICK ], this );

        if( !m_msg.empty() ) {

            CGame::MessageSystem.broadcast( new CMessage( m_msg ) );
        }

        resetPressOffset();

        m_clicked = false;
    }
    if( m_rClicked ) {
        CGame::InterfaceSystem.callLuaFunction( m_luaFuncs[ LuaFunction::FUNC_ONRCLICK ], this );

        if( !m_rMsg.empty() ) {

            CGame::MessageSystem.broadcast( new CMessage( m_rMsg ) );
        }

        resetPressOffset();

        m_rClicked = false;
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

void CGuiButton::draw( sf::RenderTarget& window, sf::RenderStates states ) const {
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
        sf::Transformable offsetTransform;
        sf::RenderStates offsetStates = states;

        if( m_pressed && it.getID() != m_spriteButtonID ) {
            offsetTransform.setPosition( m_pressOffset );
            offsetStates.transform *= offsetTransform.getTransform();
        }

        window.draw( it, offsetStates );
    }

    for( auto& it : m_texts ) {
        window.draw( it, states );
    }

    for( auto it : m_childs ) {
        window.draw( *it, states );
    }
}

bool CGuiButton::addHotkey( const sf::Keyboard::Key& key ) {
    if( isHotkey( key ) ) {
        std::cout << "Warning: adding hotkey ( KeyID: " << key << " ) to button that is already binded" << std::endl;

        return false;
    }
    else {
        m_hotkeys.push_back( key );

        m_keySet = true;

        return true;
    }
}

void CGuiButton::setMsg( const std::string& msg ) {
    m_msg   = msg;
}

void CGuiButton::setRMsg( const std::string& msg ) {
    m_rMsg  = msg;
}

void CGuiButton::registerFunction( LuaFunction type, size_t ID ) {
    m_luaFuncs[ type ]  = ID;
}

size_t CGuiButton::addButtonTexture( lua_State* state, int index, bool* result ) {
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

bool CGuiButton::setButtonTexture( size_t ID ) {
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

void CGuiButton::setPressOffset( const sf::Vector2f& offset ) {
    m_pressOffset   = offset;
}

void CGuiButton::setLocked( bool locked ) {
    m_locked    = locked;
}

bool CGuiButton::isLocked() const {
    return m_locked;
}

bool CGuiButton::isMouseover() const {
    return m_mouseover;
}

bool CGuiButton::isPressed() const {
    return m_pressed;
}

bool CGuiButton::isClicked() const {
    return m_clicked;
}

bool CGuiButton::isRPressed() const {
    return m_rPressed;
}

bool CGuiButton::isRClicked() const {
    return m_rClicked;
}

/** STATIC */

CGuiButton* CGuiButton::setupFromTable( lua_State* state, int index, bool* result, const CGuiElement* parent ) {
    CGuiButton* button = new CGuiButton();

    if( parent != nullptr ) {
        button->setParent( parent );
    }

    if( !lua_istable( state, index ) ) {
        cout << "Error: Unable to initialize gui element out of given value: " << lua_typename( state, lua_type( state, index ) ) << endl;

        if( result != nullptr ) {
            *result = false;
        }
    }
    else {
        lua_getfield( state, index, "pos" );
        if( lua_istable( state, - 1 ) ) {
            button->setPos( Util::vectorFromTable<float>( state, lua_gettop( state ) ) );
        }
        else {
            lua_getfield( state, index, "relPos" );
            if( lua_istable( state, - 1 ) ) {
                button->setRelativePos( Util::vectorFromTable<float>( state, lua_gettop( state ) ) );
            }

            lua_pop( state, 1 );
        }

        button->setSize( Util::vectorFromTable<float>   ( state, index, "size" ) );

        lua_getfield( state, index, "center" );
        if( lua_istable( state, - 1 ) ) {
            button->setCenter( Util::vectorFromTable<float>( state, lua_gettop( state ) ) );
        }

        lua_getfield( state, index, "posSize" );
        if( lua_istable( state, - 1 ) ) {
            button->setPosSize( Util::vectorFromTable<float>( state, lua_gettop( state ) ) );
        }

        bool active     = true;
        bool shown      = true;

        lWrapper::valueFromLuaTable<bool>( state, index, "active",  active );
        lWrapper::valueFromLuaTable<bool>( state, index, "shown",   shown );

        button->setActive( active );
        button->setShown( shown );

        lua_getfield( state, index, "textures" );
        if( lua_istable( state, - 1 ) ) {
            for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
                button->addTexture( state, lua_gettop( state ) );
            }
        }
        else {
            lua_getfield( state, index, "texture" );
            if( lua_isstring( state, - 1 ) ) {
                button->addTexture( state, lua_gettop( state ) );
            }

            lua_pop( state, 1 );
        }

        lua_getfield( state, index, "texts" );
        if( lua_istable( state, - 1 ) ) {
            for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
                button->addText( state, lua_gettop( state ) );
            }
        }
        else {
            lua_getfield( state, index, "text" );
            if( lua_istable( state, - 1 ) ) {
                button->addText( state, lua_gettop( state ) );
            }

            lua_pop( state, 1 );
        }

        lua_getfield( state, index, "childs" );
        if( lua_istable( state, - 1 ) ) {
            for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
                std::string type;
                lWrapper::stringFromLuaTable( state, lua_gettop( state ), "type", type );


                if( type == "default" ) {
                    CGuiElement* temp = nullptr;

                    temp = CGuiElement::setupFromTable( state, lua_gettop( state ), nullptr, button );

                    if( temp != nullptr ) {
                        button->addElement( temp );
                    }
                }
                else if( type == "button" ) {
                    CGuiButton* temp = nullptr;

                    temp = CGuiButton::setupFromTable( state, lua_gettop( state ), nullptr, button );

                    if( temp != nullptr ) {
                        button->addElement( temp );
                    }
                }
            }
        }

        /** Button specific */

        bool locked = false;
        lWrapper::valueFromLuaTable<bool>( state, index, "locked", locked );
        button->setLocked( locked );

        lua_getfield( state, index, "buttonTex" );
        if( !lua_isnil( state, - 1 ) ) {
            button->addButtonTexture( state, lua_gettop( state ) );
        }

        std::string msg;
        std::string rMsg;

        lWrapper::stringFromLuaTable( state, index, "msg",     msg );
        lWrapper::stringFromLuaTable( state, index, "rMsg",    rMsg );

        button->setMsg( msg );
        button->setRMsg( rMsg );

        lua_getfield( state, index, "hotkeys" );
        if( lua_istable( state, - 1 ) ) {
            for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
                button->addHotkey( (sf::Keyboard::Key)lua_tointeger( state, - 1 ) );
            }
        }

        button->setPressOffset( Util::vectorFromTable<float>( state, index, "offset" ) );

        lua_getfield( state, index, "functions" );
        if( lua_istable( state, - 1 ) ) {
            for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
                std::string key;

                lua_pushvalue( state, - 2 );
                if( lua_isstring( state, - 1 ) ) {
                    key = lua_tostring( state, - 1 );
                }

                lua_pop( state, 1 );

                lua_pushvalue( state, - 1 );

                if( !key.empty() ) {
                         if( key == "onClick" )     button->registerFunction( LuaFunction::FUNC_ONCLICK,    CGame::InterfaceSystem.registerLuaFunction( state ) );
                    else if( key == "onRClick" )    button->registerFunction( LuaFunction::FUNC_ONRCLICK,   CGame::InterfaceSystem.registerLuaFunction( state ) );
                    else if( key == "onPress" )     button->registerFunction( LuaFunction::FUNC_ONPRESS,    CGame::InterfaceSystem.registerLuaFunction( state ) );
                    else if( key == "onRPress" )    button->registerFunction( LuaFunction::FUNC_ONRPRESS,   CGame::InterfaceSystem.registerLuaFunction( state ) );
                    else if( key == "onMouseenter" )button->registerFunction( LuaFunction::FUNC_ONMOUSEENTER,CGame::InterfaceSystem.registerLuaFunction( state ) );
                    else if( key == "onMouseleave" )button->registerFunction( LuaFunction::FUNC_ONMOUSELEAVE,CGame::InterfaceSystem.registerLuaFunction( state ) );
                }
            }
        }

        lua_pop( state, 9 );
    }

    return button;
}

/** LUA FUNCTIONS */

int CGuiButton::luaAddHotkey( lua_State* state ) {
    int argc = lua_gettop( state );

    bool result = false;

    if( argc == 3 ) {
        CGuiButton* button = (CGuiButton*)lua_touserdata( state, argc - 1 );

        if( button->getType() == GuiType::GUI_BUTTON ) {
            result = button->addHotkey( (sf::Keyboard::Key)lua_tointeger( state, argc ) );
        }
        else {
            std::cout   << "Error: Wrong gui element type. Expected "
                        << CGuiElement::guiTypeToString( GuiType::GUI_BUTTON )
                        << ", got: " << CGuiElement::guiTypeToString( button->getType() )
                        << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to add hotkey to button. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    lua_pushboolean( state, result );

    return 1;
}

int CGuiButton::luaSetMsg( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc == 3 ) {
        CGuiButton* button = (CGuiButton*)lua_touserdata( state, argc - 1 );

        if( button->getType() == GuiType::GUI_BUTTON ) {
            button->setMsg( lua_tostring( state, argc ) );
        }
        else {
            std::cout   << "Error: Wrong gui element type. Expected "
                        << CGuiElement::guiTypeToString( GuiType::GUI_BUTTON )
                        << ", got: " << CGuiElement::guiTypeToString( button->getType() )
                        << std::endl;
        }

    }
    else {
        std::cout << "Error: Unable to set msg to button. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    return 0;
}

int CGuiButton::luaSetRMsg( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc == 3 ) {
        CGuiButton* button = (CGuiButton*)lua_touserdata( state, argc - 1 );

        if( button->getType() == GuiType::GUI_BUTTON ) {
            button->setRMsg( lua_tostring( state, argc ) );
        }
        else {
            std::cout   << "Error: Wrong gui element type. Expected "
                        << CGuiElement::guiTypeToString( GuiType::GUI_BUTTON )
                        << ", got: " << CGuiElement::guiTypeToString( button->getType() )
                        << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to set right msg to button. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    return 0;
}

int CGuiButton::luaAddButtonTexture( lua_State* state ) {
    int argc = lua_gettop( state );

    size_t result = 0;

    if( argc != 3 ) {
        cout << "Error: Unable to add button sprite to button. Wrong amount of arguments" << endl;
    }
    else {
        if( lua_islightuserdata( state, argc - 1 ) ) {
            CGuiButton* button = (CGuiButton*)lua_touserdata( state, argc - 1 );

            if( button->getType() == GuiType::GUI_BUTTON ) {
                result = button->addButtonTexture( state, argc );
            }
            else {
                std::cout   << "Error: Wrong gui element type. Expected "
                            << CGuiElement::guiTypeToString( GuiType::GUI_BUTTON )
                            << ", got: " << CGuiElement::guiTypeToString( button->getType() )
                            << std::endl;
            }
        }
    }

    lua_pop( state, argc );

    lua_pushinteger( state, result );

    return 1;
}

int CGuiButton::luaSetButtonTexture( lua_State* state ) {
    int argc = lua_gettop( state );

    bool result = false;

    if( argc == 3 ) {
        CGuiButton* button = (CGuiButton*)lua_touserdata( state, argc - 1 );

        if( button->getType() == GuiType::GUI_BUTTON ) {
            result = button->setButtonTexture( lua_tointeger( state, argc ) );
        }
        else {
            std::cout   << "Error: Wrong gui element type. Expected "
                        << CGuiElement::guiTypeToString( GuiType::GUI_BUTTON )
                        << ", got: " << CGuiElement::guiTypeToString( button->getType() )
                        << std::endl;
        }
    }

    lua_pop( state, argc );

    lua_pushboolean( state, result );

    return 1;
}

int CGuiButton::luaSetLocked( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc == 3 ) {
        CGuiButton* button = (CGuiButton*)lua_touserdata( state, argc - 1 );

        if( button->getType() == GuiType::GUI_BUTTON ) {
            button->setLocked( lua_toboolean( state, argc ) );
        }
        else {
            std::cout   << "Error: Wrong gui element type. Expected "
                        << CGuiElement::guiTypeToString( GuiType::GUI_BUTTON )
                        << ", got: " << CGuiElement::guiTypeToString( button->getType() )
                        << std::endl;
        }
    }

    lua_pop( state, argc );

    return 0;
}

int CGuiButton::luaIsLocked( lua_State* state ) {
    int argc = lua_gettop( state );

    bool result = false;

    if( argc == 2 ) {
        CGuiButton* button = (CGuiButton*)lua_touserdata( state, argc - 1 );

        if( button->getType() == GuiType::GUI_BUTTON ) {
            result = button->isLocked();
        }
        else {
            std::cout   << "Error: Wrong gui element type. Expected "
                        << CGuiElement::guiTypeToString( GuiType::GUI_BUTTON )
                        << ", got: " << CGuiElement::guiTypeToString( button->getType() )
                        << std::endl;
        }
    }

    lua_pop( state, argc );

    lua_pushboolean( state, result );

    return 1;
}

int CGuiButton::luaAddFunction( lua_State* state ) {
    int argc = lua_gettop( state );

    bool success = false;

    if( argc == 4 ) {
        if( lua_islightuserdata( state, argc - 2 ) ) {
            CGuiButton* button  = (CGuiButton*)lua_touserdata( state, argc - 2 );

            if( button->getType() == GuiType::GUI_BUTTON ) {
                std::string key     = lua_tostring( state, argc - 1 );

                if( lua_isfunction( state, argc ) ) {
                    success = true;

                         if( key == "onClick" )     button->registerFunction( LuaFunction::FUNC_ONCLICK,    CGame::InterfaceSystem.registerLuaFunction( state ) );
                    else if( key == "onRClick" )    button->registerFunction( LuaFunction::FUNC_ONRCLICK,   CGame::InterfaceSystem.registerLuaFunction( state ) );
                    else if( key == "onPress" )     button->registerFunction( LuaFunction::FUNC_ONPRESS,    CGame::InterfaceSystem.registerLuaFunction( state ) );
                    else if( key == "onRPress" )    button->registerFunction( LuaFunction::FUNC_ONRPRESS,   CGame::InterfaceSystem.registerLuaFunction( state ) );
                    else if( key == "onMouseenter" )button->registerFunction( LuaFunction::FUNC_ONMOUSEENTER,CGame::InterfaceSystem.registerLuaFunction( state ) );
                    else if( key == "onMouseleave" )button->registerFunction( LuaFunction::FUNC_ONMOUSELEAVE,CGame::InterfaceSystem.registerLuaFunction( state ) );
                    else success = false;
                }
                else {
                    std::cout << "Error: Unable to add button function. Function is missing" << std::endl;
                }
            }
            else {
                std::cout   << "Error: Wrong gui element type. Expected "
                            << CGuiElement::guiTypeToString( GuiType::GUI_BUTTON )
                            << ", got: " << CGuiElement::guiTypeToString( button->getType() )
                            << std::endl;
            }
        }
        else {
            std::cout << "Error: Unable to add button funcction. Light user data expecter, got " << lua_typename( state, lua_type( state, argc - 1 ) ) << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to add button function. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    lua_pushboolean( state, success );

    return 1;
}

int CGuiButton::luaSetPressOffset( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::Vector2f    offset;
    CGuiButton*     button  = nullptr;

    if( argc == 3 ) {
        button      = (CGuiButton*)lua_touserdata( state, argc - 1 );
        offset      = Util::vector2fFromLuaTable( state, argc );
    }
    else if( argc == 4 ) {
        button      = (CGuiButton*)lua_touserdata( state, argc - 2 );
        offset.x    = lua_tonumber( state, argc - 1 );
        offset.y    = lua_tonumber( state, argc );
    }

    if( button != nullptr ) {
        button->setPressOffset( offset );
    }

    lua_pop( state, argc );

    return 0;
}

/** PRIVATE */

bool CGuiButton::isHotkey( const sf::Keyboard::Key& hotkey ) {
    for( auto it : m_hotkeys ) {
        if( it == hotkey ) {
            return true;
        }
    }

    return false;
}

CSpriteWrapper* CGuiButton::getSprite( size_t ID ) {
    if( m_spriteButtonID != 0 ) {
        for( auto& it : m_sprites ) {
            if( it.getID() == ID ) {
                return &it;
            }
        }
    }

    return nullptr;
}

void CGuiButton::applyPressOffset() {
    for( auto& it : m_sprites ) {
//        if( it.getID() != m_spriteButtonID ) {
            sf::IntRect clip = it.getClip();

            clip.width  -= m_pressOffset.x;
            clip.height -= m_pressOffset.y;

            it.setClip( clip );
//        }
    }
}

void CGuiButton::resetPressOffset() {
    for( auto& it : m_sprites ) {
        if( it.getID() != m_spriteButtonID ) {
            it.resetClip();
        }
    }
}
