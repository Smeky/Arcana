#include "CAudioSystem.h"

#include <lua.hpp>

#include <CSoundEmitter.h>
#include <CGame.h>
#include <Utility.h>

#include <iostream>
using namespace std;

CAudioSystem::CAudioSystem()
: m_activeCounter   ( 0 )
, m_music           ( nullptr )
{
    for( float& volume : m_volume ) {
        volume = 100;
    }

    for( bool& enabled : m_enable ) {
        enabled = true;
    }
}

void CAudioSystem::update() {
    for( size_t i = 0; i < m_activeCounter; ) {
        // If sound is done, swap it
        if( m_sounds[ i ].getStatus() == sf::Sound::Stopped ) {
            m_sounds[ i ].setLoop( false );
            m_sounds[ i ] = m_sounds[ m_activeCounter - 1 ];

            m_activeCounter--;
        }
        else {
            i++;
        }
    }

    if( m_music != nullptr ) {
        if( m_music->getStatus() == sf::Music::Stopped ) {
            stopMusic();
        }
    }

    for( auto it = m_emitters.begin(); it != m_emitters.end(); ) {
        if( !(*it)->isActive() ) {
            delete *it;

            it = m_emitters.erase( it );
        }
        else {
            (*it)->update();

            it++;
        }
    }
}

void CAudioSystem::stopAll() {
    for( size_t i = 0; i < m_activeCounter; i++ ) {
        m_sounds[ i ].stop();
    }

    m_activeCounter = 0;

    stopMusic();
}

void CAudioSystem::pauseAll() {
    for( size_t i = 0; i < m_activeCounter; i++ ) {
        m_sounds[ i ].pause();
    }

    stopMusic();
}

void CAudioSystem::resumeAll() {
    for( size_t i = 0; i < m_activeCounter; i++ ) {
        m_sounds[ i ].play();
    }

    playMusic();
}

void CAudioSystem::setVolume( float volume, AudioCategory category ) {
    m_volume[ category ] = volume;

    if( category == AudioCategory::EFFECT ) {
        for( auto it : m_emitters ) {
            it->setVolume( getVolume( category ) );
        }
    }

    updateMusic();
}

float CAudioSystem::getVolume( AudioCategory category ) const {
    return m_volume[ category ];
}

void CAudioSystem::setEnabled( bool enabled, AudioCategory category ) {
    m_enable[ category ] = enabled;

    updateMusic();
}

void CAudioSystem::reverseEnabled( AudioCategory category ) {
    m_enable[ category ] = !m_enable[ category ];

    updateMusic();
}

bool CAudioSystem::isEnabled( AudioCategory category ) const {
    return m_enable[ category ];
}

void CAudioSystem::playSound( const std::string& ID, AudioCategory category ) {
    if( ID.empty() ) return;
    if( m_activeCounter >= MAX_SOUNDS ) return;
    if( !m_enable[ category ] ) return;
    if( !m_enable[ AudioCategory::DEFAULT ] ) return;

    sf::SoundBuffer& buffer = CGame::AssetManager.getSound( ID );

    m_sounds[ m_activeCounter ].setBuffer( buffer );
    m_sounds[ m_activeCounter ].setVolume( getVolume( category ) );
    m_sounds[ m_activeCounter ].play();

    m_activeCounter++;
}

void CAudioSystem::playSoundRelative( const std::string& ID, AudioCategory category, const sf::Vector2f& pos ) {
    if( ID.empty() ) return;
    if( m_activeCounter >= MAX_SOUNDS ) return;
    if( !m_enable[ category ] ) return;
    if( !m_enable[ AudioCategory::DEFAULT ] ) return;

    sf::SoundBuffer& buffer = CGame::AssetManager.getSound( ID );

    sf::Vector2f windowCenter   = sf::Vector2f( CGame::GameWindow.getSize().x / 2, CGame::GameWindow.getSize().y / 2 );
    sf::Vector2f relativePos    = CGame::WorldManager.Camera.getOffset( pos );

    float distance   = Util::lenghtByVectors( windowCenter, relativePos );

    if( distance > 700 ) {
        distance = 700;
    }

    float hearFactor = 1 - distance / 700;

    m_sounds[ m_activeCounter ].setBuffer( buffer );
    m_sounds[ m_activeCounter ].setVolume( hearFactor * getVolume( category ) );
    m_sounds[ m_activeCounter ].play();

    m_activeCounter++;
}

void CAudioSystem::playMusic( const std::string& ID ) {
    if( !ID.empty() ) {
        auto found = m_musicMap.find( ID );

        if( found != m_musicMap.end() ) {
            m_music = new sf::Music();

            m_music->openFromFile( found->second );
            m_music->setVolume( getVolume( AudioCategory::MUSIC ) );

            if( m_enable[ AudioCategory::MUSIC ] && m_enable[ AudioCategory::DEFAULT ] ) {
                m_music->play();
            }
        }
    }
    else {
        if( m_music != nullptr ) {
            m_music->play();
        }
    }
}

void CAudioSystem::stopMusic() {
    if( m_music != nullptr ) {
        m_music->stop();

//        delete m_music;
//        m_music = nullptr;
    }
}

void CAudioSystem::pauseMusic() {
    if( m_music != nullptr ) {
        m_music->pause();
    }
}

void CAudioSystem::registerEmitter( const CEntity* entity, const std::string& ID ) {
    if( m_emitters.size() >= MAX_EMITTERS ) {
        #ifndef DEBUG
            std::cout << "[D]Warning: Exceeded emitter limit with: " << ID << std::endl;
        #endif

        return;
    }

    sf::SoundBuffer& buffer = CGame::AssetManager.getSound( ID );

    CSoundEmitter* emitter = new CSoundEmitter();

    emitter->setParent( entity );
    emitter->setBuffer( buffer );
    emitter->setVolume( getVolume( AudioCategory::EFFECT ) );

    m_emitters.push_back( emitter );
}

void CAudioSystem::destroyEmitter( const CEntity* parent ) {
    for( auto it = m_emitters.begin(); it != m_emitters.end(); it++ ) {
        if( (*it)->getParent() == parent ) {
            delete *it;

            m_emitters.erase( it );

            return;
        }
    }
}

void CAudioSystem::loadMusic( const std::string& filepath, const std::string& ID ) {
    m_musicMap.insert( std::make_pair( ID, filepath ) );
}

/** Lua */

int CAudioSystem::luaLoadMusic( lua_State* state ) {
    int argc = lua_gettop( state );

    std::string folderpath = lua_tostring( state, lua_gettop( state ) - 1 );

    for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
        lua_getfield( state, - 1, "ID" );

        std::string ID          = lua_tostring( state, lua_gettop( state ) );

        lua_pop( state, 1 );

        lua_getfield( state, - 1, "File" );

        std::string filename    = lua_tostring( state, lua_gettop( state ) );

        lua_pop( state, 1 );

        std::string filepath = folderpath;
        filepath.append( filename );

        CGame::AudioSystem.loadMusic( filepath, ID );
    }

    lua_pop( state, argc );

    return 0;
}

int CAudioSystem::luaStopAll( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    CGame::AudioSystem.stopAll();

    return 0;
}

int CAudioSystem::luaPauseAll( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    CGame::AudioSystem.pauseAll();

    return 0;
}

int CAudioSystem::luaResumeAll( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    CGame::AudioSystem.resumeAll();

    return 0;
}

int CAudioSystem::luaSetVolume( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc == 3 ) {
        float           volume      = lua_tonumber( state, argc - 1 );
        AudioCategory   category    = (AudioCategory)lua_tointeger( state, argc );

        CGame::AudioSystem.setVolume( volume, category );
    }
    else {
        std::cout << "Error: Unable to set volume. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    return 0;
}

int CAudioSystem::luaGetVolume( lua_State* state ) {
    int argc = lua_gettop( state );

    float   volume = 0;

    if( argc == 2 ) {
        AudioCategory category = (AudioCategory)lua_tointeger( state, argc );

        volume  = CGame::AudioSystem.getVolume( category );
    }
    else {
        std::cout << "Error: Unable to get volume. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    lua_pushnumber( state, volume );

    return 1;
}

int CAudioSystem::luaSetEnabled( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc == 3 ) {
        bool            enabled     = lua_toboolean( state, argc - 1 );
        AudioCategory   category    = (AudioCategory)lua_tointeger( state, argc );

        CGame::AudioSystem.setEnabled( enabled, category );
    }
    else {
        std::cout << "Error: Unable to set enabled. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    return 0;
}

int CAudioSystem::luaReverseEnabled( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc == 2 ) {
        AudioCategory   category    = (AudioCategory)lua_tointeger( state, argc );

        CGame::AudioSystem.reverseEnabled( category );
    }
    else {
        std::cout << "Error: Unable to reverse enabled. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    return 0;
}

int CAudioSystem::luaIsEnabled( lua_State* state ) {
    int argc = lua_gettop( state );

    bool enabled = false;

    if( argc == 2 ) {
        AudioCategory category = (AudioCategory)lua_tointeger( state, argc );

        enabled = CGame::AudioSystem.isEnabled( category );
    }
    else {
        std::cout << "Error: Unable to get enabled. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    lua_pushboolean( state, enabled );

    return 1;
}

int CAudioSystem::luaPlaySound( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc == 3 ) {
        const std::string ID    = lua_tostring( state, argc - 1 );

        AudioCategory category  = (AudioCategory)lua_tointeger( state, argc );

        CGame::AudioSystem.playSound( ID, category );
    }
    else {
        std::cout << "Error: Unable to play sound. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    return 0;
}

int CAudioSystem::luaPlayMusic( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc == 2 ) {
        const std::string ID = lua_tostring( state, argc );

        CGame::AudioSystem.playMusic( ID );
    }
    else {
        std::cout << "Error: Unable to play music. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    return 0;
}

int CAudioSystem::luaStopMusic( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    CGame::AudioSystem.stopMusic();

    return 0;
}

int CAudioSystem::luaPauseMusic( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    CGame::AudioSystem.pauseMusic();

    return 0;
}

/** PRIVATE */

float CAudioSystem::getVolume( AudioCategory category ) {
    // Volume factor based on master volume
    float factor = m_volume[ AudioCategory::DEFAULT ] / 100;

    return m_volume[ category ] * factor;
}

void CAudioSystem::updateMusic() {
    if( m_music != nullptr ) {
        if( !m_enable[ AudioCategory::MUSIC ] ) {
            stopMusic();
        }
        else {
            if( m_music->getStatus() == sf::Music::Stopped ) {
                m_music->play();
            }

            m_music->setVolume( getVolume( AudioCategory::MUSIC ) );
        }
    }
}
