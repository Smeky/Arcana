#include "CGuiElement.h"

#include <lua.hpp>

#include <CGuiButton.h>
#include <CGuiCheckbox.h>
#include <CGuiSlider.h>
#include <Utility.h>
#include <LuaWrapper.h>

#include <iostream>
using namespace std;

CGuiElement::CGuiElement()
: m_IDCounter       ( 1 )
, m_parent          ( nullptr )
, m_active          ( true )
, m_shown           ( true )
{
    m_type  = GuiType::GUI_DEFAULT;
}

CGuiElement::~CGuiElement() {
    while( !m_childs.empty() ) delete m_childs.back(), m_childs.pop_back();
}

void CGuiElement::handleInput( sf::Event event ) {
    if( !m_active || !m_shown ) return;

    for( auto it : m_childs ) {
        it->handleInput( event );
    }
}

void CGuiElement::update( const sf::Time& delta ) {
    if( !m_active || !m_shown ) return;

    for( auto it : m_childs ) {
        it->update( delta );
    }
}

void CGuiElement::draw( sf::RenderTarget& window, sf::RenderStates states ) const {
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
}

GuiType CGuiElement::getType() const {
    return m_type;
}

void CGuiElement::setParent( const CGuiElement* parent ) {
    m_parent    = parent;
}

size_t CGuiElement::addTexture( lua_State* state, int index ) {
    bool success;

    CSpriteWrapper sprite = CSpriteWrapper::loadFromTable( state, index, &success );

    if( success ) {
        if( sprite.getSize().x == 0 && sprite.getSize().y == 0 ) {
            sprite.setSize( m_size );
        }

        sprite.setID( getNewID() );

        m_sprites.push_back( sprite );

        return sprite.getID();
    }
    else {
        return 0;
    }
}

bool CGuiElement::setTexture( lua_State* state, int index, size_t ID ) {
    if( ID == 0 || m_sprites.empty() ) {
        bool success;

        CSpriteWrapper sprite = CSpriteWrapper::loadFromTable( state, index, &success );

        if( success ) {
            if( sprite.getSize().x == 0 && sprite.getSize().y == 0 ) {
                sprite.setSize( m_size );
            }

            if( m_sprites.empty() ) {
                m_sprites.push_back( sprite );
            }
            else {
                m_sprites.back() = sprite;
            }

            return true;
        }
    }
    else {
        for( auto& it : m_sprites ) {
            if( it.getID() == ID ) {
                bool success;

                CSpriteWrapper sprite = CSpriteWrapper::loadFromTable( state, index, &success );

                if( success ) {
                    if( sprite.getSize().x == 0 && sprite.getSize().y == 0 ) {
                        sprite.setSize( m_size );
                    }

                    it = sprite;

                    return true;
                }
            }
        }
    }

    std::cout << "Warning: Unable to set texture to element. Wrong ID" << std::endl;

    return false;
}

size_t CGuiElement::addText( lua_State* state, int index ) {
    bool success;

    CTextWrapper text = CTextWrapper::loadFromTable( state, index, &success );

    if( success ) {
        text.setID( getNewID() );

        m_texts.push_back( text );

        return text.getID();
    }
    else {
        return 0;
    }
}

bool CGuiElement::setText( lua_State* state, int index, size_t ID ) {
    if( ID == 0 || m_texts.empty() ) {
        bool success;

        CTextWrapper text = CTextWrapper::loadFromTable( state, index, &success );

        if( success ) {
            if( m_texts.empty() ) {
                m_texts.push_back( text );
            }
            else {
                m_texts.back() = text;
            }

            return true;
        }
    }
    else {
        for( auto& it : m_texts ) {
            if( it.getID() == ID ) {
                bool success;

                CTextWrapper text = CTextWrapper::loadFromTable( state, index, &success );

                if( success ) {
                    it = text;

                    return true;
                }
            }
        }
    }


    std::cout << "Warning: Unable to set text to element. Wrong ID" << std::endl;

    return false;
}

size_t CGuiElement::getTexIDByIndex( size_t index ) {
    if( index >= m_sprites.size() ) return 0;

    return m_sprites[ index ].getID();
}

size_t CGuiElement::getTextIDByIndex( size_t index ) {
    if( index >= m_texts.size() ) return 0;

    return m_texts[ index ].getID();
}

void CGuiElement::setRelativePos( const sf::Vector2f& pos ) {
    sf::Vector2f newPos = pos;

    if( m_parent != nullptr ) {
        newPos += m_parent->getAbsolutePos();
    }

    setPos( newPos );
}

sf::Vector2f CGuiElement::getRelativePos() const {
    sf::Vector2f relPos = m_pos;

    if( m_parent != nullptr ) {
        relPos -= m_parent->getAbsolutePos();
    }

    return relPos;
}

sf::Vector2f CGuiElement::getAbsolutePos() const {
    sf::Vector2f absPos = getRelativePos();

    if( m_parent != nullptr ) {
        absPos += m_parent->getAbsolutePos();
    }

    return absPos;
}

void CGuiElement::addElement( CGuiElement* element ) {
    element->setParent( this );

    m_childs.push_back( element );
}

void CGuiElement::setActive( bool active ) {
    m_active    = active;
}

bool CGuiElement::isActive() const {
    return m_active;
}

bool CGuiElement::reverseActive() {
    m_active    = !m_active;

    return m_active;
}

void CGuiElement::setShown( bool shown ) {
    m_shown     = shown;
}

bool CGuiElement::isShown() const {
    return m_shown;
}

bool CGuiElement::reverseShown() {
    m_shown    = !m_shown;

    return m_shown;
}


CGuiElement* CGuiElement::setupFromTable( lua_State* state, int index, bool* result, const CGuiElement* parent ) {
    CGuiElement* element = new CGuiElement();

    bool success = loadFromTable( state, index, element, parent );

    if( result != nullptr ) {
        *result = success;
    }

    return element;
}

bool CGuiElement::loadFromTable( lua_State* state, int index, CGuiElement* element, const CGuiElement* parent ) {
    if( element == nullptr ) return false;

    if( parent != nullptr ) {
        element->setParent( parent );
    }

    if( !lua_istable( state, index ) ) {
        cout << "Error: Unable to initialize gui element out of given value: " << lua_typename( state, lua_type( state, index ) ) << endl;

        return false;
    }
    else {
        lua_getfield( state, index, "pos" );
        if( lua_istable( state, - 1 ) ) {
            element->setPos( Util::vectorFromTable<float>( state, lua_gettop( state ) ) );
        }
        else {
            lua_getfield( state, index, "relPos" );
            if( lua_istable( state, - 1 ) ) {
                element->setRelativePos( Util::vectorFromTable<float>( state, lua_gettop( state ) ) );
            }

            lua_pop( state, 1 );
        }

        element->setSize( Util::vectorFromTable<float>   ( state, index, "size" ) );

        lua_getfield( state, index, "center" );
        if( lua_istable( state, - 1 ) ) {
            element->setCenter( Util::vectorFromTable<float>( state, lua_gettop( state ) ) );
        }

        lua_getfield( state, index, "posSize" );
        if( lua_istable( state, - 1 ) ) {
            element->setPosSize( Util::vectorFromTable<float>( state, lua_gettop( state ) ) );
        }

        bool active     = true;
        bool shown      = true;

        lWrapper::valueFromLuaTable<bool>( state, index, "active",  active );
        lWrapper::valueFromLuaTable<bool>( state, index, "shown",   shown );

        element->setActive( active );
        element->setShown( shown );

        lua_getfield( state, index, "textures" );
        if( lua_istable( state, - 1 ) ) {
            for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
                element->addTexture( state, lua_gettop( state ) );
            }
        }
        else {
            lua_getfield( state, index, "texture" );
            if( lua_isstring( state, - 1 ) ) {
                element->addTexture( state, lua_gettop( state ) );
            }

            lua_pop( state, 1 );
        }

        lua_getfield( state, index, "texts" );
        if( lua_istable( state, - 1 ) ) {
            for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
                element->addText( state, lua_gettop( state ) );
            }
        }
        else {
            lua_getfield( state, index, "text" );
            if( lua_istable( state, - 1 ) ) {
                element->addText( state, lua_gettop( state ) );
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

                    temp = CGuiElement::setupFromTable( state, lua_gettop( state ), nullptr, element );

                    if( temp != nullptr ) {
                        element->addElement( temp );
                    }
                }
                else if( type == "button" ) {
                    CGuiButton* temp = nullptr;

                    temp = CGuiButton::setupFromTable( state, lua_gettop( state ), nullptr, element );

                    if( temp != nullptr ) {
                        element->addElement( temp );
                    }
                }
                else if( type == "checkbox" ) {
                    CGuiCheckbox* temp = nullptr;

                    temp = CGuiCheckbox::setupFromTable( state, lua_gettop( state ), nullptr, element );

                    if( temp != nullptr ) {
                        element->addElement( temp );
                    }
                }
                else if( type == "slider" ) {
                    CGuiSlider* temp = nullptr;

                    temp = CGuiSlider::setupFromTable( state, lua_gettop( state ), nullptr, element );

                    if( temp != nullptr ) {
                        element->addElement( temp );
                    }
                }
            }
        }

        lua_pop( state, 6 );
    }

    return true;
}

std::string CGuiElement::guiTypeToString( const GuiType& type ) {
    std::string result;

    switch( type ) {
    case GuiType::GUI_DEFAULT:  result = "Default";     break;
    case GuiType::GUI_BUTTON:   result = "Button";      break;
    case GuiType::GUI_CHECKBOX: result = "Checkbox";    break;
    default: result = "Unkown"; break;
    }

    return result;
}

/** LUA FUNCTIONS */


int CGuiElement::luaSetPos( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::Vector2f    pos;
    CDimensional*   dim = nullptr;

    if( argc == 3 ) {
        dim     = (CDimensional*)lua_touserdata( state, argc - 1 );
        pos     = Util::vector2fFromLuaTable( state, argc );
    }
    else if( argc == 4 ) {
        dim     = (CDimensional*)lua_touserdata( state, argc - 2 );
        pos.x   = lua_tonumber( state, - 2 );
        pos.y   = lua_tonumber( state, - 1 );
    }

    if( dim != nullptr ) {
        dim->setPos( pos );
    }

    lua_pop( state, argc );

    return 0;
}

int CGuiElement::luaSetCenter( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::Vector2f    pos;
    CDimensional*   dim = nullptr;

    if( argc == 3 ) {
        dim     = (CDimensional*)lua_touserdata( state, argc - 1 );
        pos     = Util::vector2fFromLuaTable( state, argc );
    }
    else if( argc == 4 ) {
        dim     = (CDimensional*)lua_touserdata( state, argc - 2 );
        pos.x   = lua_tonumber( state, - 2 );
        pos.y   = lua_tonumber( state, - 1 );
    }

    if( dim != nullptr ) {
        dim->setCenter( pos );
    }

    lua_pop( state, argc );

    return 0;
}

int CGuiElement::luaSetSize( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::Vector2f    size;
    CDimensional*   dim     = nullptr;

    if( argc == 3 ) {
        dim     = (CDimensional*)lua_touserdata( state, argc - 1 );
        size    = Util::vector2fFromLuaTable( state, argc );
    }
    else if( argc == 4 ) {
        dim     = (CDimensional*)lua_touserdata( state, argc - 2 );
        size.x  = lua_tonumber( state, argc - 1 );
        size.y  = lua_tonumber( state, argc );
    }

    if( dim != nullptr ) {
        dim->setSize( size );
    }

    lua_pop( state, argc );

    return 0;
}

int CGuiElement::luaSetPosSize( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::Vector2f    posSize;
    CDimensional*   dim = nullptr;

    if( argc == 3 ) {
        dim         = (CDimensional*)lua_touserdata( state, argc - 1 );
        posSize     = Util::vector2fFromLuaTable( state, argc );
    }
    else if( argc == 4 ) {
        dim     = (CDimensional*)lua_touserdata( state, argc - 2 );
        posSize.x   = lua_tonumber( state, - 2 );
        posSize.y   = lua_tonumber( state, - 1 );
    }

    if( dim != nullptr ) {
        dim->setPosSize( posSize );
    }

    lua_pop( state, argc );

    return 0;
}

int CGuiElement::luaSetBox( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::FloatRect   box;
    CDimensional*   dim = nullptr;

    if( argc == 3 ) {
        dim         = (CDimensional*)lua_touserdata( state, argc - 1 );
        box         = Util::floatrectFromLuaTable( state, argc );
    }
    else if( argc == 6 ) {
        dim         = (CDimensional*)lua_touserdata( state, argc - 4 );
        box.left    = lua_tonumber( state, argc - 3 );
        box.top     = lua_tonumber( state, argc - 2 );
        box.width   = lua_tonumber( state, argc - 1 );
        box.height  = lua_tonumber( state, argc );
    }
    else {
        std::cout << "Error: Unable to set element box. Wrong amount of arguments" << std::endl;
    }

    if( dim != nullptr ) {
        dim->setBox( box );
    }

    lua_pop( state, argc );

    return 0;
}

int CGuiElement::luaGetPos( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::Vector2f pos;

    if( argc != 2 ) {
        std::cout << "Error: Unable to return element position. Wrong amount of arguments" << std::endl;
    }
    else {
        CDimensional* dim = (CDimensional*)lua_touserdata( state, argc );

        if( dim != nullptr ) {
            pos = dim->getPos();
        }
    }

    lua_pop( state, argc );

    lua_pushnumber( state, pos.x );
    lua_pushnumber( state, pos.y );

    return 2;
}

int CGuiElement::luaGetCenter( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::Vector2f pos;

    if( argc != 2 ) {
        std::cout << "Error: Unable to return element center. Wrong amount of arguments" << std::endl;
    }
    else {
        CDimensional* dim = (CDimensional*)lua_touserdata( state, argc );

        if( dim != nullptr ) {
            pos = dim->getCenter();
        }
    }

    lua_pop( state, argc );

    lua_pushnumber( state, pos.x );
    lua_pushnumber( state, pos.y );

    return 2;
}

int CGuiElement::luaGetSize( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::Vector2f size;

    if( argc != 2 ) {
        std::cout << "Error: Unable to return element position. Wrong amount of arguments" << std::endl;
    }
    else {
        CDimensional* dim = (CDimensional*)lua_touserdata( state, argc );

        if( dim != nullptr ) {
            size = dim->getSize();
        }
    }

    lua_pop( state, argc );

    lua_pushnumber( state, size.x );
    lua_pushnumber( state, size.y );

    return 2;
}

int CGuiElement::luaGetPosSize( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::Vector2f posSize;

    if( argc != 2 ) {
        std::cout << "Error: Unable to return element position. Wrong amount of arguments" << std::endl;
    }
    else {
        CDimensional* dim = (CDimensional*)lua_touserdata( state, argc );

        if( dim != nullptr ) {
            posSize = dim->getPosSize();
        }
    }

    lua_pop( state, argc );

    lua_pushnumber( state, posSize.x );
    lua_pushnumber( state, posSize.y );

    return 2;
}

int CGuiElement::luaGetBox( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::FloatRect box;

    if( argc != 2 ) {
        std::cout << "Error: Unable to return element box. Wrong amount of arguments" << std::endl;
    }
    else {
        CDimensional* dim = (CDimensional*)lua_touserdata( state, argc );

        if( dim != nullptr ) {
            box = dim->getBox();
        }
    }

    lua_pop( state, argc );

    lua_pushnumber( state, box.left );
    lua_pushnumber( state, box.top );
    lua_pushnumber( state, box.width );
    lua_pushnumber( state, box.height );

    return 4;
}

int CGuiElement::luaMove( lua_State* state ) {
    int argc = lua_gettop( state );

    CDimensional*   dim     = nullptr;
    sf::Vector2f    movement;

    if( argc == 3 ) {
        dim         = (CDimensional*)lua_touserdata( state, argc - 1 );
        movement    = Util::vector2fFromLuaTable( state, argc );
    }
    else if( argc == 4 ) {
        dim         = (CDimensional*)lua_touserdata( state, argc - 2 );
        movement.x  = lua_tonumber( state, argc - 1 );
        movement.y  = lua_tonumber( state, argc );
    }

    if( dim != nullptr ) {
        dim->move( movement );
    }

    return 0;
}

int CGuiElement::luaResize( lua_State* state ) {
    int argc = lua_gettop( state );

    CDimensional*   dim     = nullptr;
    sf::Vector2f    size;

    if( argc == 3 ) {
        dim         = (CDimensional*)lua_touserdata( state, argc - 1 );
        size        = Util::vector2fFromLuaTable( state, argc );
    }
    else if( argc == 4 ) {
        dim         = (CDimensional*)lua_touserdata( state, argc - 2 );
        size.x      = lua_tonumber( state, argc - 1 );
        size.y      = lua_tonumber( state, argc );
    }

    if( dim != nullptr ) {
        dim->resize( size );
    }

    return 0;
}

int CGuiElement::luaGetTexIDByIndex( lua_State* state ) {
    int argc = lua_gettop( state );

    size_t ID = 0;

    if( argc == 3 ) {
        if( lua_islightuserdata( state, argc - 1 ) ) {
            CGuiElement* element    = (CGuiElement*)lua_touserdata( state, argc - 1 );
            size_t index            = lua_tointeger( state, argc );

            ID = element->getTexIDByIndex( index );
        }
        else {
            std::cout << "Error: Unable to get texture ID. Light user data is missing" << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to get texture ID. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    lua_pushinteger( state, ID );

    return 1;
}

int CGuiElement::luaGetTextIDByIndex( lua_State* state ) {
    int argc = lua_gettop( state );

    size_t ID = 0;

    if( argc == 3 ) {
        if( lua_islightuserdata( state, argc - 1 ) ) {
            CGuiElement* element    = (CGuiElement*)lua_touserdata( state, argc - 1 );
            size_t index            = lua_tointeger( state, argc );

            ID = element->getTextIDByIndex( index );
        }
        else {
            std::cout << "Error: Unable to get text ID. Light user data is missing" << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to get text ID. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    lua_pushinteger( state, ID );

    return 1;
}

int CGuiElement::luaAddTexture( lua_State* state ) {
    int argc = lua_gettop( state );

    size_t result = 0;

    if( argc != 3 ) {
        cout << "Error: Unable to add sprite to element. Wrong amount of arguments" << endl;
    }
    else {
        if( lua_islightuserdata( state, argc - 1 ) ) {
            CGuiElement* element = (CGuiElement*)lua_touserdata( state, argc - 1 );

            result = element->addTexture( state, argc );
        }
    }

    lua_pop( state, argc );

    lua_pushinteger( state, result );

    return 1;
}

int CGuiElement::luaSetTexture( lua_State* state ) {
    int argc = lua_gettop( state );

    bool success = false;

    if( argc == 4 ) {
        CGuiElement* element = (CGuiElement*)lua_touserdata( state, argc - 2 );

        size_t ID = lua_tointeger( state, argc );

        success = element->setTexture( state, argc - 1, ID );
    }
    else if( argc == 3 ) {
        CGuiElement* element = (CGuiElement*)lua_touserdata( state, argc - 1 );

        size_t ID = 0;

        success = element->setTexture( state, argc, ID );

    }
    else {
        std::cout << "Error: unable to set texture by ID. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    lua_pushboolean( state, success );

    return 1;
}

int CGuiElement::luaAddText( lua_State* state ) {
    int argc = lua_gettop( state );

    size_t result = 0;

    if( argc != 3 ) {
        cout << "Error: Unable to add text to element. Wrong amount of arguments" << endl;
    }
    else {
        if( lua_islightuserdata( state, argc - 1 ) ) {
            CGuiElement* element = (CGuiElement*)lua_touserdata( state, argc - 1 );

            result = element->addText( state, argc );
        }
    }

    lua_pop( state, argc );

    lua_pushinteger( state, result );

    return 1;
}

int CGuiElement::luaSetText( lua_State* state ) {
    int argc = lua_gettop( state );

    bool success = false;

    if( argc == 4 ) {
        CGuiElement* element = (CGuiElement*)lua_touserdata( state, argc - 2 );

        size_t ID = lua_tointeger( state, argc );

        success = element->setText( state, argc - 1, ID );
    }
    else if( argc == 3 ) {
        CGuiElement* element = (CGuiElement*)lua_touserdata( state, argc - 1 );

        size_t ID = 0;

        success = element->setText( state, argc, ID );

    }
    else {
        std::cout << "Error: unable to set text by ID. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    lua_pushboolean( state, success );

    return 1;
}

int CGuiElement::luaSetRelativePos( lua_State* state ) {
    int argc = lua_gettop( state );

    CGuiElement*    element;
    sf::Vector2f    pos;

    if( argc == 3 ) {
        element     = (CGuiElement*)lua_touserdata( state, argc - 1 );
        pos         = Util::vectorFromTable<float>( state, argc );
    }
    else if( argc == 4 ) {
        element     = (CGuiElement*)lua_touserdata( state, argc - 2 );
        pos.x       = lua_tonumber( state, argc - 1 );
        pos.y       = lua_tonumber( state, argc );
    }
    else {
        return 0;
    }

    element->setRelativePos( pos );

    lua_pop( state, argc );

    return 0;
}

int CGuiElement::luaGetRelativePos( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::Vector2f pos;

    if( argc != 2 ) {
        std::cout << "Error: Unable to return element relative pos. Wrong amount of arguments" << std::endl;
    }
    else {
        if( lua_islightuserdata( state, argc ) ) {
            CGuiElement* element = (CGuiElement*)lua_touserdata( state, argc );
            pos = element->getRelativePos();
        }
    }

    lua_pop( state, argc );

    lua_pushnumber( state, pos.x );
    lua_pushnumber( state, pos.y );

    return 2;
}

int CGuiElement::luaSetActive( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc == 3 ) {
        CGuiElement* element = (CGuiElement*)lua_touserdata( state, argc - 1 );
        element->setActive( lua_toboolean( state, argc ) );
    }
    else {
        std::cout << "Error: Unable to set active for element. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    return 0;
}

int CGuiElement::luaIsActive( lua_State* state ) {
    int argc = lua_gettop( state );

    bool active = false;

    if( argc == 2 ) {
        if( lua_islightuserdata( state, argc ) ) {
            CGuiElement* element = (CGuiElement*)lua_touserdata( state, argc );

            if( element != nullptr ) {
                active = element->isActive();
            }
        }
    }

    lua_pop( state, argc );

    lua_pushboolean( state, active );

    return 1;
}

int CGuiElement::luaReverseActive( lua_State* state ) {
    int argc = lua_gettop( state );

    bool active = false;

    if( argc == 2 ) {
        CGuiElement* element = (CGuiElement*)lua_touserdata( state, argc );
        active = element->reverseActive();
    }

    lua_pop( state, argc );

    lua_pushboolean( state, active );

    return 1;
}

int CGuiElement::luaSetShown( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc == 3 ) {
        CGuiElement* element = (CGuiElement*)lua_touserdata( state, argc - 1 );
        element->setShown( lua_toboolean( state, argc ) );
    }
    else {
        std::cout << "Error: Unable to set active for element. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    return 0;
}

int CGuiElement::luaIsShown( lua_State* state ) {
    int argc = lua_gettop( state );

    bool shown = false;

    if( argc == 2 ) {
        CGuiElement* element = (CGuiElement*)lua_touserdata( state, argc );
        shown = element->isShown();
    }

    lua_pop( state, argc );

    lua_pushboolean( state, shown );

    return 1;
}

int CGuiElement::luaReverseShown( lua_State* state ) {
    int argc = lua_gettop( state );

    bool shown = false;

    if( argc == 2 ) {
        CGuiElement* element = (CGuiElement*)lua_touserdata( state, argc );
        shown = element->reverseShown();
    }

    lua_pop( state, argc );

    lua_pushboolean( state, shown );

    return 1;
}

/** PRIVATE */

void CGuiElement::onSizeChange() {
    for( auto& it : m_sprites ) {
        it.setSize( m_size );
    }
}

size_t CGuiElement::getNewID() {
    m_IDCounter++;

    return m_IDCounter - 1;
}
