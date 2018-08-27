#include "CTooltipManager.h"

#include "cassert"
#include "lua.hpp"

#include "CGame.h"
#include "CTooltip.h"
#include "Utility.h"

#include "sstream"
#include "iostream"
using namespace std;

CTooltipManager::CTooltipManager()
: m_tooltip     ( nullptr )
{}

CTooltipManager::~CTooltipManager() {
    destroyTooltip();
}

void CTooltipManager::handleInput( const sf::Event& event ) {
//    if( event.type == sf::Event::MouseMoved ) {
//        if( !m_mouseBox.contains( event.mouseMove.x, event.mouseMove.y ) ) {
//            destroyTooltip();
//        }
//    }
}

void CTooltipManager::update() {
    if( hasTooltip() ) {
        sf::Vector2i mousePos = sf::Mouse::getPosition( CGame::GameWindow );

        if( !m_mouseBox.contains( mousePos.x, mousePos.y ) ) {
            destroyTooltip();
        }
    }
}

void CTooltipManager::render( sf::RenderTarget& window, sf::RenderStates states ) const {
    if( m_tooltip != nullptr ) {
        window.draw( *m_tooltip, states );
    }
}

void CTooltipManager::createFromLuaTable( lua_State* state, int index ) {
    destroyTooltip();


    if( !lua_istable( state, index ) ) {
        std::cout << "Error: Unable to create tooltip. Value at given index is not table!" << std::endl;
        return;
    }

    m_tooltip = new CTooltip();

    /** Position */
    lua_getfield( state, index, "mouseBox" );
    assert( lua_istable( state, - 2 ) && "MouseBox table is missing" );
    m_mouseBox  = Util::floatrectFromLuaTable( state, lua_gettop( state ) );

    m_tooltip->setPos( m_mouseBox.left + m_mouseBox.width + 3, m_mouseBox.top );

    /** Width */
    lua_getfield( state, index, "width" );
    if( lua_isnumber( state, - 1 ) ) {
        m_tooltip->setWidth( lua_tonumber( state, - 1 ) );
    }

    /** Texture background */
    lua_getfield( state, index, "textureBg" );
    assert( !lua_isnil( state, - 1 ) && "TextureBg value is missing" );
    m_tooltip->setBgTexture( lua_tostring( state, - 1 ) );

    /** Texture icon */
    lua_getfield( state, index, "icon" );
    if( !lua_isnil( state, - 1 ) ) {
        m_tooltip->setBgTexture( lua_tostring( state, - 1 ) );
    }

    lua_getfield( state, index, "texts" );
    if( !lua_isnil( state, - 1 ) ) {
        for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
            CTextWrapper text = Util::textWrapperFromLuaTable( state, lua_gettop( state ) );

            m_tooltip->addText( text );
        }
    }


    lua_pop( state, 6 );
}

void CTooltipManager::destroyTooltip() {
    if( m_tooltip != nullptr ) {
        delete m_tooltip;
        m_tooltip = nullptr;
    }
}

bool CTooltipManager::hasTooltip() const {
    return m_tooltip != nullptr;
}

/** LUA FUNCTIONS */

int CTooltipManager::luaCreateTooltip( lua_State* state ) {
    int argc = lua_gettop( state );

    CGame::InterfaceSystem.TooltipManager.createFromLuaTable( state, argc );

    lua_pop( state, argc );

    return 0;
}

int CTooltipManager::luaDestroyTooltip( lua_State* state ) {
    int argc = lua_gettop( state );


    CGame::InterfaceSystem.TooltipManager.destroyTooltip();

    lua_pop( state, argc );

    return 0;
}

int CTooltipManager::luaHasTooltip( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    bool hasTooltip = CGame::InterfaceSystem.TooltipManager.hasTooltip();

    lua_pushboolean( state, hasTooltip );

    return 1;
}


