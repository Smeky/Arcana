#include "CParticleEmitter.h"

#include <CGame.h>
#include <CRNG.h>
#include <CEntity.h>

#include <iostream>
using namespace std;

CParticleEmitter::CParticleEmitter()
: m_active      ( true )
, m_parent      ( nullptr )
{
    //ctor
}

void CParticleEmitter::update( const sf::Time& delta ) {
    if( !m_active ) return;

    if( m_data.lifetimeType == PELifetimeType::PELIFE_TIMED ) {
        if( m_lifeTimer.update( delta ) ) {
            m_active = false;

            return;
        }
    }
    else {
        if( m_parent == nullptr ) {
            m_active = false;

            return;
        }
    }

    if( m_emissionTimer.update( delta ) ) {
        if( m_data.posType == PEPositionType::PEPOS_PARENT ) {
            if( m_parent != nullptr ) {
                CGame::WorldManager.ParticleSystem.spawnParticle( setupParticleData(), m_parent->getCenter() );
            }
        }
        else if( m_data.posType == PEPositionType::PEPOS_DEFAULT ) {
            CGame::WorldManager.ParticleSystem.spawnParticle( setupParticleData(), m_data.position );
        }

        m_emissionTimer.reset();
    }
}

void CParticleEmitter::setParent( const CEntity* parent ) {
    m_parent    = parent;
}

void CParticleEmitter::setData( const PEmitterData& data ) {
    m_data      = data;

    m_lifeTimer.setLimit( data.lifetime );
    m_emissionTimer.setLimit( data.emissionRate );
}

void CParticleEmitter::setParticleData( const ParticleData& data ) {
    m_particleData  = data;
}

bool CParticleEmitter::isActive() const {
    return m_active;
}

ParticleData CParticleEmitter::setupParticleData() {
    ParticleData particleData = m_particleData;

    if( m_data.posType == PEPositionType::PEPOS_DEFAULT ) {
        particleData.pos    = m_data.position;
    }
    else if( m_data.posType == PEPositionType::PEPOS_PARENT ) {
        if( m_parent != nullptr ) {
            particleData.pos    = m_parent->getCenter();
        }
    }

    if( m_data.zLayerType == PEZLayerType::PZLAYER_PARENT ) {
        if( m_parent != nullptr ) {
            particleData.zLayer     = m_parent->getZLayer();
        }
    }
    else if( m_data.zLayerType == PEZLayerType::PZLAYER_RANDOM ) {
        int layer = particleData.zLayer;

        if( m_parent != nullptr ) {
            layer = m_parent->getZLayer();
        }

        calcRange( layer, particleData.zLayerRange );

        particleData.zLayer = layer;
    }
    else if( m_data.zLayerType == PEZLayerType::PZLAYER_FRONT ) {
        if( m_parent != nullptr ) {
            particleData.zLayer = m_parent->getZLayer() + 1;
        }
    }
    else if( m_data.zLayerType == PEZLayerType::PZLAYER_BEHIND ) {
        if( m_parent != nullptr ) {
            particleData.zLayer = m_parent->getZLayer() - 1;
        }
    }

    if( m_data.emissionAngleType == PEEmissionAngleType::EMANGLE_RANDOM ) {
        particleData.moveAngle  = 360 * CRNG::percentRandom();
    }
    else if( m_data.emissionAngleType == PEEmissionAngleType::EMANGLE_RANDOM ) {
        particleData.moveAngle  = m_data.emissionAngle;
    }

    calcRange( particleData.moveVelocity,       particleData.moveVelocityRange );
    calcRange( particleData.moveAngle,          particleData.moveAngleRange );
    calcRange( particleData.moveAccel,          particleData.moveAccelRange );
    calcRange( particleData.moveDistance,       particleData.moveDistanceRange );
    calcRange( particleData.rotationVel,        particleData.rotationVelRange );
    calcRange( particleData.rotationAccel,      particleData.rotationAccelRange );
    calcRange( particleData.lifetime,           particleData.lifetimeRange );
    calcRange( particleData.scalingVel,         particleData.scalingVelRange );
    calcRange( particleData.opacityVelRange,    particleData.opacityVelRange );

    return particleData;
}

void CParticleEmitter::calcRange( float& value, float range ) {
    if( range != 0.0 ) {
        value += range - ( range * 2 ) * CRNG::percentRandom();
    }
}

void CParticleEmitter::calcRange( int& value, int range ) {
    if( range != 0 ) {
        value = CRNG::iRandom( value - range, value + range );
    }
}
