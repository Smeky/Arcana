#include "CInterfaceSystem.h"

#include "cassert"
#include "lua.hpp"

#include "CGame.h"
#include "CWindow.h"
#include "CWindowInventory.h"
#include "CWindowCastbar.h"
#include "CWidgetEffect.h"
#include "CTextWidget.h"
#include "CButtonImage.h"
#include "CButtonAbility.h"
#include "CDisplayPlayerStat.h"
#include "CDisplayPlayerStat.h"
#include "CTrackHealth.h"
#include "CTrackResource.h"
#include "Utility.h"
#include <CGuiElement.h>
#include <CGuiButton.h>

#include "iostream"
using namespace std;

CInterfaceSystem::CInterfaceSystem()
: m_IDCounter       ( 0 )
, m_funcIDCounter   ( 1 )
{}

CInterfaceSystem::~CInterfaceSystem() {
    close();
}

void CInterfaceSystem::close() {
    while( !m_windows.empty() ) delete m_windows.back(), m_windows.pop_back();
    while( !m_elements.empty() ) delete m_elements.back(), m_elements.pop_back();

    m_sprites.clear();
    m_texts.clear();

    m_IDCounter = 0;
}

void CInterfaceSystem::handleInput( const sf::Event& event ) {
    TooltipManager.handleInput( event );

    for( auto it : m_windows ) {
        it->handleInput( event );
    }

    for( auto it : m_elements ) {
        it->handleInput( event );
    }
}

void CInterfaceSystem::update( const sf::Time& delta ) {
    TooltipManager.update();

    for( auto it : m_windows ) {
        it->update( delta );
    }

    for( auto it : m_elements ) {
        it->update( delta );
    }
}

void CInterfaceSystem::render( sf::RenderTarget& window, sf::RenderStates states ) {
    for( auto it : m_windows ) {
        window.draw( *it, states );
    }

    for( auto it : m_elements ) {
        window.draw( *it, states );
    }

    for( auto& it : m_sprites ) {
        window.draw( it, states );
    }

    for( auto& it : m_texts ) {
        window.draw( it, states );
    }

    TooltipManager.render( window, states );
}

size_t CInterfaceSystem::registerWindow( CWindow* window ) {
    size_t ID = getNewID();

    window->setID( ID );

    m_windows.push_back( window );

    return ID;
}

size_t CInterfaceSystem::registerSprite( CSpriteWrapper sprite ) {
    size_t ID = getNewID();

    sprite.setID( ID );

    m_sprites.push_back( sprite );

    return ID;
}

size_t CInterfaceSystem::registerText( CTextWrapper text ) {
    size_t ID = getNewID();

    text.setID( ID );

    m_texts.push_back( text );

    return ID;
}

void CInterfaceSystem::deleteSprite( size_t ID ) {
    for( auto it = m_sprites.begin(); it != m_sprites.end(); it++ ) {
        if( it->getID() == ID ) {
            m_sprites.erase( it );

            return;
        }
    }
}

void CInterfaceSystem::deleteText( size_t ID ) {
    for( auto it = m_texts.begin(); it != m_texts.end(); it++ ) {
        if( it->getID() == ID ) {
            m_texts.erase( it );

            return;
        }
    }
}

void CInterfaceSystem::windowReverseShown( size_t ID ) {
    for( auto it : m_windows ) {
        if( it->getID() == ID ) {
            it->reverseShown();

            return;
        }
    }
}

void CInterfaceSystem::windowReverseActive( size_t ID ) {
    for( auto it : m_windows ) {
        if( it->getID() == ID ) {
            it->reverseActive();

            return;
        }
    }
}

void CInterfaceSystem::setWindowShown( size_t ID, bool shown ) {
    for( auto it : m_windows ) {
        if( it->getID() == ID ) {
            it->setShown( shown );

            return;
        }
    }
}

void CInterfaceSystem::setWindowActive( size_t ID, bool active ) {
    for( auto it : m_windows ) {
        if( it->getID() == ID ) {
            it->setActive( active );

            return;
        }
    }
}

bool CInterfaceSystem::isWindowShown( size_t ID ) {
    for( auto it : m_windows ) {
        if( it->getID() == ID ) {
            return it->isShown();
        }
    }

    return false;
}

bool CInterfaceSystem::isWindowActive( size_t ID ) {
    for( auto it : m_windows ) {
        if( it->getID() == ID ) {
            return it->isActive();
        }
    }

    return false;
}

bool CInterfaceSystem::hasMousefocus() {
    for( auto it : m_windows ) {
        if( it->hasMouseFocus() ) {
            return true;
        }
    }

    return false;
}

CTextWidget* CInterfaceSystem::createTextWidgetFromLua( lua_State* state, int index ) {
    assert( lua_istable( state, index ) && "Value is not table" );

    CTextWidget* textWidget = new CTextWidget();

    textWidget->setTextWrapper( Util::textWrapperFromLuaTable( state, index ) );

    return textWidget;
}

CButtonImage* CInterfaceSystem::createButtonFromTable( lua_State* state, int index ) {
    assert( lua_istable( state, index ) && "Value is not table" );

    sf::Vector2f    pos;
    sf::Vector2f    size;
    std::string     textureID;
    std::string     textureBgID;
    std::string     textureFrontID;
    std::string     msgID;

    lua_getfield( state, index, "pos" );
    if( lua_istable( state, - 1 ) ) {
        pos     = Util::vector2fFromLuaTable( state, lua_gettop( state ) );
    }

    lua_getfield( state, index, "size" );
    if( lua_istable( state, - 1 ) ) {
        size    = Util::vector2fFromLuaTable( state, lua_gettop( state ) );
    }

    lua_getfield( state, index, "texture" );
    if( !lua_isnil( state, - 1 ) ) {
        textureID   = lua_tostring( state, - 1 );
    }

    lua_getfield( state, index, "textureBg" );
    if( !lua_isnil( state, - 1 ) ) {
        textureBgID   = lua_tostring( state, - 1 );
    }
    lua_getfield( state, index, "textureFront" );
    if( !lua_isnil( state, - 1 ) ) {
        textureFrontID = lua_tostring( state, - 1 );
    }

    lua_getfield( state, index, "msg" );
    if( !lua_isnil( state, - 1 ) ) {
        msgID   = lua_tostring( state, - 1 );
    }

    CButtonImage* button = new CButtonImage();

    lua_getfield( state, index, "text" );
    if( lua_istable( state, - 1 ) ) {
        CTextWrapper wrapper = Util::textWrapperFromLuaTable( state, lua_gettop( state ) );

        button->setTextWrapper( wrapper );
    }

    button->setID( getNewID() );
    button->setPos( pos );
    button->setSize( size );

    if( !textureID.empty() ) {
        button->setButtonTexture( textureID );
    }

    if( !textureBgID.empty() ) {
        button->setBgTexture( textureBgID );
    }

    if( !textureFrontID.empty() ) {
        button->setFrontTexture( textureFrontID );
    }

    if( !msgID.empty() ) {
        button->setMsg( msgID );
    }

    // Look for hotkeys
    lua_getfield( state, index, "hotkey" );

    // If there are any hotkeys
    if( !lua_isnil( state, - 1 ) ) {
        if( lua_istable( state, - 1 ) ) {
            // Go through all hotkey
            for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
                // Store hotkey
                int hotkey = lua_tointeger( state, - 1 );

                button->addHotkey( (sf::Keyboard::Key)hotkey );
            }
        }
        else {
            // Store hotkey
            int hotkey = lua_tointeger( state, - 1 );

            button->addHotkey( (sf::Keyboard::Key)hotkey );
        }
    }

    lua_pop( state, 8 );

    return button;
}

CButtonAbility* CInterfaceSystem::createAbilityFromTable( lua_State* state, int index ) {
    assert( lua_istable( state, index ) && "Value is not table" );

    sf::Vector2f    pos;
    sf::Vector2f    size;
    size_t          abilityIndex;
    std::string     textureID;
    std::string     textureBgID;
    std::string     textureFrontID;
    std::string     msgID;

    lua_getfield( state, index, "pos" );
    if( lua_istable( state, - 1 ) ) {
        pos     = Util::vector2fFromLuaTable( state, lua_gettop( state ) );
    }

    lua_getfield( state, index, "size" );
    if( lua_istable( state, - 1 ) ) {
        size    = Util::vector2fFromLuaTable( state, lua_gettop( state ) );
    }

    lua_getfield( state, index, "texture" );
    if( !lua_isnil( state, - 1 ) ) {
        textureID   = lua_tostring( state, - 1 );
    }

    lua_getfield( state, index, "textureBg" );
    if( !lua_isnil( state, - 1 ) ) {
        textureBgID   = lua_tostring( state, - 1 );
    }

    lua_getfield( state, index, "textureFront" );
    if( !lua_isnil( state, - 1 ) ) {
        textureFrontID = lua_tostring( state, - 1 );
    }

    lua_getfield( state, index, "index" );
    if( lua_isinteger( state, - 1 ) ) {
        abilityIndex    = lua_tointeger( state, - 1 );
    }

    lua_getfield( state, index, "msg" );
    if( !lua_isnil( state, - 1 ) ) {
        msgID   = lua_tostring( state, - 1 );
    }

    CButtonAbility* button = new CButtonAbility();

    lua_getfield( state, index, "text" );
    if( lua_istable( state, - 1 ) ) {
        CTextWrapper wrapper = Util::textWrapperFromLuaTable( state, lua_gettop( state ) );

        button->setTextWrapper( wrapper );
    }

    CGame::MessageSystem.subscribe( button, "PlayerAbilityCasted" );

    button->setID( getNewID() );
    button->setPos( pos );
    button->setSize( size );
    button->setAbilityIndex( abilityIndex );

    if( !textureID.empty() ) {
        button->setButtonTexture( textureID );
    }

    if( !textureBgID.empty() ) {
        button->setBgTexture( textureBgID );
    }

    if( !textureFrontID.empty() ) {
        button->setFrontTexture( textureFrontID );
    }

    if( !msgID.empty() ) {
        button->setMsg( msgID );
    }

    // Look for hotkeys
    lua_getfield( state, index, "hotkey" );

    // If there are any hotkeys
    if( !lua_isnil( state, - 1 ) ) {
        if( lua_istable( state, - 1 ) ) {
            // Go through all hotkey
            for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
                // Store hotkey
                int hotkey = lua_tointeger( state, - 1 );

                button->addHotkey( (sf::Keyboard::Key)hotkey );
            }
        }
        else {
            // Store hotkey
            int hotkey = lua_tointeger( state, - 1 );

            button->addHotkey( (sf::Keyboard::Key)hotkey );
        }
    }

    lua_pop( state, 9 );

    return button;
}

CTrackHealth* CInterfaceSystem::createHpTrackFromTable( lua_State* state, int index ) {
    assert( lua_istable( state, index ) && "Value is not table" );

    sf::Vector2f    pos;
    sf::Vector2f    size;
    std::string     textureBgID;
    std::string     textureProgID;

    lua_getfield( state, index, "pos" );
    if( lua_istable( state, - 1 ) ) {
        pos     = Util::vector2fFromLuaTable( state, lua_gettop( state ) );
    }

    lua_getfield( state, index, "size" );
    if( lua_istable( state, - 1 ) ) {
        size    = Util::vector2fFromLuaTable( state, lua_gettop( state ) );
    }

    lua_getfield( state, index, "textureBg" );
    if( !lua_isnil( state, - 1 ) ) {
        textureBgID   = lua_tostring( state, - 1 );
    }

    lua_getfield( state, index, "textureProg" );
    if( !lua_isnil( state, - 1 ) ) {
        textureProgID = lua_tostring( state, - 1 );
    }

    CTrackHealth* tracker = new CTrackHealth();

    tracker->setID( getNewID() );
    tracker->setPos( pos );
    tracker->setSize( size );

    if( !textureBgID.empty() ) {
        tracker->setBgTexture( textureBgID );
    }

    if( !textureProgID.empty() ) {
        tracker->setProgTexture( textureProgID );
    }

    lua_pop( state, 4 );

    return tracker;
}

CTrackResource* CInterfaceSystem::createRpTrackFromTable( lua_State* state, int index ) {
    assert( lua_istable( state, index ) && "Value is not table" );

    sf::Vector2f    pos;
    sf::Vector2f    size;
    std::string     textureBgID;
    std::string     textureProgID;

    lua_getfield( state, index, "pos" );
    if( lua_istable( state, - 1 ) ) {
        pos     = Util::vector2fFromLuaTable( state, lua_gettop( state ) );
    }

    lua_getfield( state, index, "size" );
    if( lua_istable( state, - 1 ) ) {
        size    = Util::vector2fFromLuaTable( state, lua_gettop( state ) );
    }

    lua_getfield( state, index, "textureBg" );
    if( !lua_isnil( state, - 1 ) ) {
        textureBgID   = lua_tostring( state, - 1 );
    }

    lua_getfield( state, index, "textureProg" );
    if( !lua_isnil( state, - 1 ) ) {
        textureProgID = lua_tostring( state, - 1 );
    }

    CTrackResource* tracker = new CTrackResource();

    tracker->setID( getNewID() );
    tracker->setPos( pos );
    tracker->setSize( size );

    if( !textureBgID.empty() ) {
        tracker->setBgTexture( textureBgID );
    }

    if( !textureProgID.empty() ) {
        tracker->setProgTexture( textureProgID );
    }

    lua_pop( state, 4 );

    return tracker;
}

std::vector<size_t> CInterfaceSystem::getWindowWidgetsID( size_t ID ) {
    for( auto it : m_windows ) {
        if( it->getID() == ID ) {
            return it->getWidgetsID();
        }
    }

    // If ID is wrong, return empty vector
    return std::vector<size_t>();
}

CWindow* CInterfaceSystem::getWindow( size_t ID ) {
    for( auto it : m_windows ) {
        if( it->getID() == ID ) {
            return it;
        }
    }

    cout << "Error: Unable to return window. Wrong item ID. ID: " << ID << endl;

    return nullptr;
}

CWidget* CInterfaceSystem::getWidget( size_t windowID, size_t widgetID ) {
    CWindow* window = getWindow( windowID );

    if( window == nullptr ) {
        return nullptr;
    }
    else {
        return window->getWidget( widgetID );
    }
}

void CInterfaceSystem::createEffectIcon( const CEffect* effect ) {
//    CWidgetEffect* widget   = new CWidgetEffect( effect );
}

void CInterfaceSystem::setWidgetBgTexture( size_t windowID, size_t widgetID, const std::string& textureID ) {
    for( auto it : m_windows ) {
        if( it->getID() == windowID ) {
            it->setWidgetBgTexture( widgetID, textureID );
        }
    }
}

void CInterfaceSystem::setWidgetMiddleTexture( size_t windowID, size_t widgetID, const std::string& textureID ) {
    for( auto it : m_windows ) {
        if( it->getID() == windowID ) {
            it->setWidgetMiddleTexture( widgetID, textureID );
        }
    }
}

void CInterfaceSystem::setWidgetFrontTexture( size_t windowID, size_t widgetID, const std::string& textureID ) {
    for( auto it : m_windows ) {
        if( it->getID() == windowID ) {
            it->setWidgetFrontTexture( widgetID, textureID );
        }
    }
}

void CInterfaceSystem::registerElement( CGuiElement* element ) {
    m_elements.push_back( element );
}

void CInterfaceSystem::callLuaFunction( size_t functionID, void* element ) {
    // ID 0 is default value, not used
    if( functionID == 0 ) return;

    // Store main Lua state
    lua_State* state = CGame::ScriptSystem.getState();

    if( !m_luaFunctions[ functionID ].empty() ) {
        for( int i : m_luaFunctions[ functionID ] ) {
            // Get function by it's ID
            lua_rawgeti( state, LUA_REGISTRYINDEX, i );

            // Check if function was found
            if( lua_isfunction( state, - 1 ) ) {
                // Put castersID and destination position as function arguments
                lua_pushlightuserdata( state, element );

                // Call the function and store the result
                int result = lua_pcall( state, 1, LUA_MULTRET, 0 );

                // If there was a problem, print it
                if( result != LUA_OK ) {
                    std::cout << "Error: " << lua_tostring( state, - 1 ) << std::endl;
                }
            }
            else {
                // If function was not found, clean up
                lua_pop( state, 1 );
            }
        }
    }
}

void CInterfaceSystem::removeLuaFunction( size_t functionID ) {

}

size_t CInterfaceSystem::registerLuaFunction( lua_State* state ) {
    int functionRef = luaL_ref( state, LUA_REGISTRYINDEX );

    size_t ID = getNewFuncID();

    m_luaFunctions[ ID ].push_back( functionRef );

    return ID;
}

/** LUA FUNCTIONS */

int CInterfaceSystem::luaClose( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    CGame::InterfaceSystem.close();

    return 0;
}

int CInterfaceSystem::luaCreateWindow( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::Vector2f    pos;
    sf::Vector2f    size;
    std::string     textureID;
    std::string     textureFrontID;

    lua_getfield( state, argc, "pos" );
    if( !lua_isnil( state, - 1 ) ) {
        pos = Util::vector2fFromLuaTable( state, lua_gettop( state ) );
    }

    lua_getfield( state, argc, "size" );
    if( !lua_isnil( state, - 1 ) ) {
        size = Util::vector2fFromLuaTable( state, lua_gettop( state ) );
    }

    lua_getfield( state, argc, "texture" );
    if( !lua_isnil( state, - 1 ) ) {
        textureID = lua_tostring( state, - 1 );
    }

    lua_getfield( state, argc, "textureFront" );
    if( !lua_isnil( state, - 1 ) ) {
        textureFrontID = lua_tostring( state, - 1 );
    }

    CWindow* window = new CWindow();

    window->setPos( pos );
    window->setSize( size );
    window->setTexture( textureID );

    if( !textureFrontID.empty() ) {
        window->setFrontTexture( textureFrontID );
    }

    lua_getfield( state, argc, "widgets" );

    if( !lua_isnil( state, - 1 ) ) {
        // Go through all widgets
        for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
            // Push type value to top of the stack
            lua_getfield( state, - 1, "type" );

            // Store the type as string
            std::string type = lua_tostring( state, - 1 );

            // Pop the copied key
            lua_pop( state, 1 );

            if( type == "TEXT" ) {
                window->addWidget( CGame::InterfaceSystem.createTextWidgetFromLua( state, lua_gettop( state ) ) );
            }
            else if( type == "BUTTON" ) {
                window->addWidget( CGame::InterfaceSystem.createButtonFromTable( state, lua_gettop( state ) ) );
            }
            else if( type == "BUTTON_ABILITY" ) {
                window->addWidget( CGame::InterfaceSystem.createAbilityFromTable( state, lua_gettop( state ) ) );
            }
            else if( type == "TRACK_HEALTH" ) {
                window->addWidget( CGame::InterfaceSystem.createHpTrackFromTable( state, lua_gettop( state ) ) );
            }
            else if( type == "TRACK_RESOURCE" ) {
                window->addWidget( CGame::InterfaceSystem.createRpTrackFromTable( state, lua_gettop( state ) ) );
            }
        }
    }


    lua_pop( state, argc + 5 );

    size_t ID = CGame::InterfaceSystem.registerWindow( window );

    lua_pushinteger( state, ID );

    return 1;
}

int CInterfaceSystem::luaCreateSprite( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( ( argc == 2 ) && "Wrong amount of arguments" );

    sf::FloatRect   box;
    sf::IntRect     clip;
    std::string     textureID;

    // Get box from table
    lua_getfield( state, argc, "box" );
    box = Util::floatrectFromLuaTable( state, lua_gettop( state ) );

    // Get box from table
    lua_getfield( state, argc, "clip" );
    clip = Util::intrectFromLuaTable( state, lua_gettop( state ) );

    // Get textureID
    lua_getfield( state, argc, "texture" );
    textureID = lua_tostring( state, - 1 );

    lua_pop( state, 3 );
    lua_pop( state, argc );

    CSpriteWrapper sprite;
    sprite.setTexture( textureID );
    sprite.setClip( clip );
    sprite.setBox( box );

    size_t ID = CGame::InterfaceSystem.registerSprite( sprite );

    lua_pushinteger( state, ID );

    return 1;
}

int CInterfaceSystem::luaCreateText( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( ( argc == 2 ) && "Wrong amount of arguments" );

    CTextWrapper text;

    // Get position
    lua_getfield( state, argc, "pos" );
    if( lua_istable( state, - 1 ) ) {
        text.setPos( Util::vector2fFromLuaTable( state, lua_gettop( state ) ) );
    }

    // Get text
    lua_getfield( state, argc, "text" );
    if( !lua_isnil( state, - 1 ) ) {
        text.setString( lua_tostring( state, - 1 ) );
    }

    // Get fontID
    lua_getfield( state, argc, "font" );
    if( !lua_isnil( state, - 1 ) ) {
        text.setFont( lua_tostring( state, - 1 ) );
    }

    // Get character size
    lua_getfield( state, argc, "size" );
    if( lua_isinteger( state, - 1 ) ) {
        text.setCharSize( lua_tointeger( state, - 1 ) );
    }

    // Get font style
    lua_getfield( state, argc, "style" );
    if( lua_isinteger( state, - 1 ) ) {
        text.setStyle( (sf::Text::Style)lua_tointeger( state, - 1 ) );
    }

    // Get color
    lua_getfield( state, argc, "color" );
    if( lua_istable( state, - 1 ) ) {
        text.setColor( Util::colorFromLuaTable( state, lua_gettop( state ) ) );
    }

    // Get origin
    lua_getfield( state, argc, "origin" );
    if( lua_isinteger( state, - 1 ) ) {
        text.setOrigin( (OriginPosition)lua_tointeger( state, - 1 ) );
    }

    lua_pop( state, argc + 6 );

    size_t ID = CGame::InterfaceSystem.registerText( text );

    lua_pushinteger( state, ID );

    return 1;
}

int CInterfaceSystem::luaCreateInventory( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::Vector2f    pos;
    sf::Vector2f    size;
    std::string     textureID;
    std::string     textureFrontID;

    lua_getfield( state, argc, "pos" );
    if( !lua_isnil( state, - 1 ) ) {
        pos = Util::vector2fFromLuaTable( state, lua_gettop( state ) );
    }

    lua_getfield( state, argc, "size" );
    if( !lua_isnil( state, - 1 ) ) {
        size = Util::vector2fFromLuaTable( state, lua_gettop( state ) );
    }

    lua_getfield( state, argc, "texture" );
    if( !lua_isnil( state, - 1 ) ) {
        textureID = lua_tostring( state, - 1 );
    }

    lua_getfield( state, argc, "textureFront" );
    if( !lua_isnil( state, - 1 ) ) {
        textureFrontID = lua_tostring( state, - 1 );
    }

    CWindowInventory* inventory = new CWindowInventory();

    inventory->setPos( pos );
    inventory->setSize( size );
    inventory->setTexture( textureID );

    CGame::MessageSystem.subscribe( inventory, "PlayerItemAdd" );
    CGame::MessageSystem.subscribe( inventory, "PlayerItemDel" );

    if( !textureFrontID.empty() ) {
        inventory->setFrontTexture( textureFrontID );
    }

    lua_getfield( state, argc, "widgets" );

    if( !lua_isnil( state, - 1 ) ) {
        // Go through all widgets
        for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
            // Push type value to top of the stack
            lua_getfield( state, - 1, "type" );

            // Store the type as string
            std::string type = lua_tostring( state, - 1 );

            // Pop the copied key
            lua_pop( state, 1 );

            if( type == "BUTTON" ) {
                inventory->addWidget( CGame::InterfaceSystem.createButtonFromTable( state, lua_gettop( state ) ) );
            }
            else if( type == "ITEMSLOT" ) {
                inventory->addItemslot( CGame::InterfaceSystem.createButtonFromTable( state, lua_gettop( state ) ) );
            }
        }
    }

    lua_pop( state, argc + 5 );

    inventory->addWidget( new CDisplayPlayerStat() );

    size_t ID = CGame::InterfaceSystem.registerWindow( inventory );

    lua_pushinteger( state, ID );

    return 1;
}

int CInterfaceSystem::luaCreateCastbar( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 2 && "Wrong amount of arguments" );

    CWindowCastbar* castbar = new CWindowCastbar();

    lua_getfield( state, argc, "pos" );
    if( lua_istable( state, - 1 ) ) {
        castbar->setPos( Util::vector2fFromLuaTable( state, lua_gettop( state ) ) );
    }

    lua_getfield( state, argc, "size" );
    if( lua_istable( state, - 1 ) ) {
        castbar->setSize( Util::vector2fFromLuaTable( state, lua_gettop( state ) ) );
    }

    lua_getfield( state, argc, "text" );
    if( lua_istable( state, - 1 ) ) {
        castbar->setCastTimeText( Util::textWrapperFromLuaTable( state, lua_gettop( state ) ) );
    }

    lua_getfield( state, argc, "textureBorder" );
    if( !lua_isnil( state, - 1 ) ) {
        CSpriteWrapper wrapper;

        wrapper.setTexture( lua_tostring( state, - 1 ) );

        castbar->setCBBorder( wrapper );
    }

    lua_getfield( state, argc, "textureBg" );
    if( !lua_isnil( state, - 1 ) ) {
        CSpriteWrapper wrapper;

        wrapper.setTexture( lua_tostring( state, - 1 ) );

        castbar->setCBBackground( wrapper );
    }

    lua_getfield( state, argc, "textureProg" );
    if( !lua_isnil( state, - 1 ) ) {
        CSpriteWrapper wrapper;

        wrapper.setTexture( lua_tostring( state, - 1 ) );

        castbar->setCBProgress( wrapper );
    }

    lua_pop( state, argc + 6 );

    size_t ID = CGame::InterfaceSystem.registerWindow( castbar );

    lua_pushinteger( state, ID );

    return 1;
}

int CInterfaceSystem::luaNewElement( lua_State* state ) {
    int argc = lua_gettop( state );

    CGuiElement*    parent      = nullptr;

    // Used if element should be made out of table
    bool            fromTable   = false;
    CGuiElement*    temp        = nullptr;

    // In case there is light user data of parent element or table
    if( argc == 2 ) {
        // In case parent was given as argument
        if( lua_islightuserdata( state, argc ) ) {
            parent  = (CGuiElement*)lua_touserdata( state, argc );
        }
        // In case table was given as argument
        else if( lua_istable( state, argc ) ) {
            fromTable   = true;

            temp = CGuiElement::setupFromTable( state, argc, nullptr, parent );
        }
    }
    // In case there are both parent and table
    else if( argc == 3 ) {
        if( lua_islightuserdata( state, argc - 1 ) ) {
            parent  = (CGuiElement*)lua_touserdata( state, argc - 1 );
        }
        if( lua_istable( state, argc ) ) {
            fromTable   = true;

            temp = CGuiElement::setupFromTable( state, argc, nullptr, parent );
        }
    }

    lua_pop( state, argc );

    CGuiElement* element;

    // If element should be made out of table
    if( fromTable ) {
        element = temp;
    }
    else {
        element = new CGuiElement();
    }

    // If element is child add it to parent, otherwise register it in interface system
    if( parent == nullptr ) {
        CGame::InterfaceSystem.registerElement( element );
    }
    else {
        parent->addElement( element );
    }

    lua_pushlightuserdata( state, element );

    return 1;
}

int CInterfaceSystem::luaNewButton( lua_State* state ) {
    int argc = lua_gettop( state );

    CGuiElement*    parent      = nullptr;

    // Used if element should be made out of table
    bool            fromTable   = false;
    CGuiButton*     temp        = nullptr;

    // In case there is light user data of parent element or table
    if( argc == 2 ) {
        // In case parent was given as argument
        if( lua_islightuserdata( state, argc ) ) {
            parent  = (CGuiElement*)lua_touserdata( state, argc );
        }
        // In case table was given as argument
        else if( lua_istable( state, argc ) ) {
            fromTable   = true;

            temp = CGuiButton::setupFromTable( state, argc, nullptr, parent );
        }
    }
    // In case there are both parent and table
    else if( argc == 3 ) {
        if( lua_islightuserdata( state, argc - 1 ) ) {
            parent  = (CGuiElement*)lua_touserdata( state, argc - 1 );
        }
        if( lua_istable( state, argc ) ) {
            fromTable   = true;

            temp = CGuiButton::setupFromTable( state, argc, nullptr, parent );
        }
    }

    lua_pop( state, argc );

    CGuiButton* button;

    // If element should be made out of table
    if( fromTable ) {
        button = temp;
    }
    else {
        button = new CGuiButton();
    }

    // If element is child add it to parent, otherwise register it in interface system
    if( parent == nullptr ) {
        CGame::InterfaceSystem.registerElement( button );
    }
    else {
        parent->addElement( button );
    }

    lua_pushlightuserdata( state, button );

    return 1;
}

int CInterfaceSystem::luaDeleteSprite( lua_State* state ) {
    int argc = lua_gettop( state );

    size_t ID = lua_tointeger( state, argc );

    CGame::InterfaceSystem.deleteSprite( ID );

    lua_pop( state, argc );

    return 0;
}

int CInterfaceSystem::luaDeleteText( lua_State* state ) {
    int argc = lua_gettop( state );

    size_t ID = lua_tointeger( state, argc );

    CGame::InterfaceSystem.deleteText( ID );

    lua_pop( state, argc );

    return 0;
}

int CInterfaceSystem::luaWindowReverseShown( lua_State* state ) {
    int argc = lua_gettop( state );

    size_t ID = lua_tointeger( state, argc );

    CGame::InterfaceSystem.windowReverseShown( ID );

    lua_pop( state, argc );

    return 0;
}

int CInterfaceSystem::luaWindowReverseActive( lua_State* state ) {
    int argc = lua_gettop( state );

    size_t ID = lua_tointeger( state, argc );

    CGame::InterfaceSystem.windowReverseActive( ID );

    lua_pop( state, argc );

    return 0;
}

int CInterfaceSystem::luaSetWindowShown( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 3 && "Wrong amount of arguments" );

    size_t  windowID    = lua_tointeger( state, - 2 );
    bool    shown       = lua_toboolean( state, - 1 );

    CGame::InterfaceSystem.setWindowShown( windowID, shown );

    lua_pop( state, argc );

    return 0;
}

int CInterfaceSystem::luaSetWindowActive( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 3 && "Wrong amount of arguments" );

    size_t  windowID    = lua_tointeger( state, - 2 );
    bool    active      = lua_toboolean( state, - 1 );

    CGame::InterfaceSystem.setWindowActive( windowID, active );

    lua_pop( state, argc );

    return 0;
}

int CInterfaceSystem::luaIsWindowShown( lua_State* state ) {
    int argc = lua_gettop( state );

    size_t ID = lua_tointeger( state, argc );

    lua_pop( state, argc );

    bool shown = CGame::InterfaceSystem.isWindowShown( ID );

    lua_pushboolean( state, shown );

    return 1;
}

int CInterfaceSystem::luaIsWindowActive( lua_State* state ) {
    int argc = lua_gettop( state );

    size_t ID = lua_tointeger( state, argc );

    lua_pop( state, argc );

    bool active = CGame::InterfaceSystem.isWindowActive( ID );

    lua_pushboolean( state, active );

    return 1;
}

int CInterfaceSystem::luaHasMousefocus( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    bool hasFocus = CGame::InterfaceSystem.hasMousefocus();

    lua_pushboolean( state, hasFocus );

    return 1;
}

int CInterfaceSystem::luaGetWindowWidgetsID( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 2 && "Wrong amount of arguments" );

    size_t windowID = lua_tointeger( state, - 1 );

    lua_pop( state, 2 );

    std::vector<size_t> IDs = CGame::InterfaceSystem.getWindowWidgetsID( windowID );

    lua_createtable( state, 0, IDs.size() );

    for( size_t i = 0; i < IDs.size(); i++ ) {
        lua_pushinteger( state, IDs[ i ] );
        lua_rawseti( state, - 2, i + 1 );
    }

    return 1;
}

int CInterfaceSystem::luaSetWidgetBgTexture( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 4 && "Wrong amount of arguments" );

    size_t      windowID;
    size_t      widgetID;
    std::string textureID;

    windowID    = lua_tointeger( state, - 3 );
    widgetID    = lua_tointeger( state, - 2 );
    textureID   = lua_tostring( state, - 1 );

    CGame::InterfaceSystem.setWidgetBgTexture( windowID, widgetID, textureID );

    lua_pop( state, argc );

    return 0;
}

int CInterfaceSystem::luaSetWidgetMiddleTexture( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 4 && "Wrong amount of arguments" );

    size_t      windowID;
    size_t      widgetID;
    std::string textureID;

    windowID    = lua_tointeger( state, - 3 );
    widgetID    = lua_tointeger( state, - 2 );
    textureID   = lua_tostring( state, - 1 );

    CGame::InterfaceSystem.setWidgetMiddleTexture( windowID, widgetID, textureID );

    lua_pop( state, argc );

    return 0;
}

int CInterfaceSystem::luaSetWidgetFrontTexture( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 4 && "Wrong amount of arguments" );

    size_t      windowID;
    size_t      widgetID;
    std::string textureID;

    windowID    = lua_tointeger( state, - 3 );
    widgetID    = lua_tointeger( state, - 2 );
    textureID   = lua_tostring( state, - 1 );

    CGame::InterfaceSystem.setWidgetFrontTexture( windowID, widgetID, textureID );

    lua_pop( state, argc );

    return 0;
}

int CInterfaceSystem::luaGetWidgetBox( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc != 3 ) {
        cout << "Error: Unable to return correct widget box. Wrong amount of arguments" << endl;
    }

    size_t windowID = lua_tointeger( state, - 2 );
    size_t widgetID = lua_tointeger( state, - 1 );

    lua_pop( state, argc );

    CWindow* window = CGame::InterfaceSystem.getWindow( windowID );
    CWidget* widget = nullptr;

    sf::FloatRect   box;

    if( window != nullptr ) {
        widget      = window->getWidget( widgetID );
    }

    if( widget != nullptr ) {
        box = widget->getBox();
    }

    lua_createtable( state, 0, 4 );

    lua_pushnumber( state, box.left );
    lua_rawseti( state, - 2, 1 );

    lua_pushnumber( state, box.top );
    lua_rawseti( state, - 2, 2 );

    lua_pushnumber( state, box.width );
    lua_rawseti( state, - 2, 3 );

    lua_pushnumber( state, box.height );
    lua_rawseti( state, - 2, 4 );

    return 1;
}

/** LUA FUNCTIONS END */

size_t CInterfaceSystem::getNewID() {
    m_IDCounter++;

    return m_IDCounter - 1;
}

size_t CInterfaceSystem::getNewFuncID() {
    m_funcIDCounter++;

    return m_funcIDCounter - 1;
}

