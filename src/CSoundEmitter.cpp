#include "CSoundEmitter.h"

#include <CEntity.h>
#include <CGame.h>
#include <Utility.h>

#include <SFML/Audio/Listener.hpp>

#include <iostream>
using namespace std;

CSoundEmitter::CSoundEmitter()
: m_parent      ( nullptr )
{}

CSoundEmitter::~CSoundEmitter() {
    m_sound.stop();
    m_sound.setLoop( false );
}

void CSoundEmitter::update() {
    if( m_parent != nullptr ) {
        sf::Vector2f parentPos  =   m_parent->getCenter();
        sf::Vector2f cameraPos  =   CGame::WorldManager.Camera.getPos();
        sf::Vector2f center     =   sf::Vector2f( CGame::GameWindow.getSize().x / 2, CGame::GameWindow.getSize().y / 2 );

        float distance      = Util::lenghtByVectors( parentPos - cameraPos, center );

        if( distance > 700 ) {
            distance = 700;
        }

        float hearFactor    = 1 - distance / 700;

        m_sound.setVolume( m_volume * hearFactor );
    }
}

void CSoundEmitter::setParent( const CEntity* parent ) {
    m_parent    = parent;
}

const CEntity* CSoundEmitter::getParent() const {
    return m_parent;
}

void CSoundEmitter::setBuffer( const sf::SoundBuffer& buffer ) {
    m_sound.setBuffer( buffer );
    m_sound.play();
    m_sound.setLoop( true );
}

void CSoundEmitter::setVolume( float volume ) {
    m_volume    = volume;
}

bool CSoundEmitter::isActive() const {
    return m_parent != nullptr;
}


