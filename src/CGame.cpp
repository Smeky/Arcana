#include "CGame.h"

#include "cassert"
#include "typeinfo"

#include "CRNG.h"
#include <CEntity.h>

#include "iostream"
using namespace std;

CScriptSystem       CGame::ScriptSystem;
CInterfaceSystem    CGame::InterfaceSystem;
CMessageSystem      CGame::MessageSystem;
CEntitySystem       CGame::EntitySystem;
CSceneManager       CGame::SceneManager;
CAssetManager       CGame::AssetManager;
CWorldManager       CGame::WorldManager;
CSpatialSystem      CGame::SpatialSystem;
CZBuffer            CGame::ZBuffer;
CAbilityManager     CGame::AbilityManager;
CEffectManager      CGame::EffectManager;
CUpgradeManager     CGame::UpgradeManager;

sf::RenderWindow    CGame::GameWindow;
sf::Clock           CGame::GameClock;
size_t              CGame::SceneID;

bool                CGame::m_changeScene;

bool CGame::init() {
    setupWindow();

    ScriptSystem.prepareLuaState();

    ScriptSystem.fireEvent( "GameInit" );

    CRNG::makeSeed();

    m_displayStats.init();

    // Initialize clocks
    GameClock.restart();
    m_loopClock.restart();

    return true;
}

void CGame::close() {
    GameWindow.close();
}

void CGame::run() {
    while( GameWindow.isOpen() ) {
        handleInput();
        update();
        render();
    }
}

void CGame::changeScene() {
    CGame::m_changeScene = false;

    ScriptSystem.fireEventWithInt( "NewScene", 1, SceneID );
}

size_t CGame::getSceneID() {
    return SceneID;
}

/** LUA FUNCTIONS */

int CGame::luaCloseGame( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    CGame::close();

    return 0;
}

int CGame::luaGetWindowSize( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    lua_pushinteger( state, GameWindow.getSize().x );
    lua_pushinteger( state, GameWindow.getSize().y );

    return 2;
}

int CGame::luaGetGameTime( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    lua_pushnumber( state, CGame::GameClock.getElapsedTime().asSeconds() );

    return 1;
}

int CGame::luaChangeScene( lua_State* state ) {
    int argc = lua_gettop( state );

    CGame::SceneID = lua_tonumber( state, lua_gettop( state ) );

    CGame::m_changeScene = true;

    lua_pop( state, argc );

    return 0;
}

int CGame::luaGetSceneID( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    lua_pushinteger( state, CGame::SceneID );

    return 1;
}

int CGame::luaLoadScript( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc == 2 ) {
        CGame::ScriptSystem.loadScript( lua_tostring( state, - 1 ) );
    }
    else {
        cout << "Error: Wrong argument count - Unable to load script" << endl;
    }

    lua_pop( state, argc );

    return 0;
}

int CGame::luaSendMsg( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 2 && "Wrong amount of arguments" );

    std::string msg = lua_tostring( state, argc );

    CGame::MessageSystem.broadcast( new CMessage( msg ) );

    lua_pop( state, argc );

    return 0;
}

int CGame::luaSendDelayMsg( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 3 && "Wrong amount of arguments" );

    std::string msg     = lua_tostring( state, - 2 );
    float       delay   = lua_tonumber( state, - 1 );

    CGame::MessageSystem.broadcastDelayedMsg( new CMessage( msg ), delay );

    lua_pop( state, argc );

    return 0;
}

/** LUA FUNCTIONS END */

void CGame::setupWindow() {
    GameWindow.create( sf::VideoMode( 1280, 724 ), "Arcana", sf::Style::Close );
//    GameWindow.create( sf::VideoMode( 1680, 1024 ), "Arcana", sf::Style::Close );
    GameWindow.setKeyRepeatEnabled( false );
//    GameWindow.setFramerateLimit( 120 );
}

void CGame::handleInput() {
    sf::Event event;

    while( GameWindow.pollEvent( event ) ) {
        if( event.type == sf::Event::Closed ) {
            close();
        }
        else if( event.type == sf::Event::KeyPressed ) {
            ScriptSystem.fireEventWithInt( "InputKeyPressed", 1, event.key.code );
        }
        else if( event.type == sf::Event::KeyReleased ) {
            ScriptSystem.fireEventWithInt( "InputKeyReleased", 1, event.key.code );
        }

        InterfaceSystem.handleInput( event );
        EntitySystem.handleInput( event );
    }
}

void CGame::update() {
    sf::Time delta = m_loopClock.restart();

    if( GameWindow.hasFocus() ) {
        MessageSystem.update( delta );

        m_displayStats.update( delta );

        ScriptSystem.fireEvent( "Update" );

        ScriptSystem.update();
        InterfaceSystem.update( delta );
        WorldManager.update( delta );

        if( m_changeScene ) {
            changeScene();
        }
    }
}

void CGame::render() {
    GameWindow.clear( sf::Color::Black );

    sf::RenderStates states = sf::RenderStates::Default;

    WorldManager.render( GameWindow, states );
    InterfaceSystem.render( GameWindow, states );

    m_displayStats.render( GameWindow, states );

    GameWindow.display();
}
