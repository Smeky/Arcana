#include "CDisplayStats.h"

#include "CGame.h"
#include "Utility.h"

#include "iostream"
using namespace std;

void CDisplayStats::init() {
    m_updateTimer.setLimit( 0.5 );

    /** FPS setup */
    m_FPS.setPos( 20, 20 );
    m_FPS.setFont( "FONT_ARIAL" );
    m_FPS.setColor( sf::Color::White );
    m_FPS.setCharSize( 12 );

    m_FPSAverage    = 0;

    /** Lua memory setup */
    m_LuaMemory.setPos( 20, 35 );
    m_LuaMemory.setFont( "FONT_ARIAL" );
    m_LuaMemory.setColor( sf::Color::White );
    m_LuaMemory.setCharSize( 12 );
}

void CDisplayStats::update( const sf::Time& delta ) {
    m_updateTimer.update( delta );

    // Calculate average FPS
    m_FPSAverage = ( 1.0f / delta.asSeconds() ) * 0.1 + m_FPSAverage * 0.9;

    if( m_updateTimer.isTime() ) {
        updateFPS();
        updateLuaMemory();

        m_updateTimer.reset();
    }
}

void CDisplayStats::render( sf::RenderTarget& window, sf::RenderStates states ) const {
    window.draw( m_FPS,         states );
    window.draw( m_LuaMemory,   states );
}

void CDisplayStats::updateFPS() {
    std::string FPS = "FPS: ";
    FPS.append( Util::intToString( (int)m_FPSAverage ) );

    m_FPS.setString( FPS );
}

void CDisplayStats::updateLuaMemory() {
    int memoryUsage = lua_gc( CGame::ScriptSystem.getState(), LUA_GCCOUNT, 0 );

    std::string memory = "Lua memory: ";
    memory.append( Util::intToString( memoryUsage ) );
    memory.append( " KB" );

    m_LuaMemory.setString( memory );
}
