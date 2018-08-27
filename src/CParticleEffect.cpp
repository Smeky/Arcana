#include "CParticleEffect.h"

#include <CParticleEmitter.h>

CParticleEffect::CParticleEffect()
: m_ID      ( 0 )
, m_active  ( true )
{}

CParticleEffect::~CParticleEffect() {
    while( !m_emitters.empty() ) delete m_emitters.back(), m_emitters.pop_back();
}

void CParticleEffect::update( const sf::Time& delta ) {
    for( auto it = m_emitters.begin(); it != m_emitters.end(); ) {
        if( !(*it)->isActive() ) {
            delete *it;

            it = m_emitters.erase( it );

            if( m_emitters.empty() ) {
                m_active = false;
            }
        }
        else {
            (*it)->update( delta );

            it++;
        }
    }
}

void CParticleEffect::setID( size_t ID ) {
    m_ID        = ID;
}

size_t CParticleEffect::getID() const {
    return m_ID;
}

bool CParticleEffect::isActive() const {
    return m_active;
}

void CParticleEffect::addEmitter( CParticleEmitter* emitter ) {
    m_emitters.push_back( emitter );
}
