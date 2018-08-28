#include "CParticle.h"

#include <Utility.h>

CParticle::CParticle()
: m_active          ( false )
, m_opacity         ( 255 )
, m_liveByDistance  ( false )
{
    m_bounds = sf::VertexArray( sf::Quads, 4 );
}

void CParticle::update( const sf::Time& delta ) {
    updateMovement( delta );

    if( m_data.moveSlowVel != 0.0 ) {
        updateSlow( delta );
    }

    updateRotation( delta );

    if( !m_liveByDistance ) {
        updateLifetime( delta );
    }

    updateScaling( delta );
    updateOpacity( delta );
}

void CParticle::draw( sf::RenderTarget& window, sf::RenderStates states ) const {
    sf::Transformable transformable;

    transformable.setPosition( m_pos );
    transformable.setOrigin( m_size.x / 2, m_size.y / 2 );
    transformable.setRotation( m_rotAngle );

    states.transform   *= transformable.getTransform();
    states.texture      = m_texture;

    window.draw( m_bounds, states );
}

void CParticle::setData( const ParticleData& data ) {
    m_data          = data;

    m_pos           = data.pos;
    m_size          = data.size;

    setZLayer( data.zLayer );

    m_active        = true;
    m_movedDistance = 0.0;

    if( m_data.opacityType == ParticleOpType::POPACITY_TOFULL ) {
        m_opacity   = 0;
    }
    else {
        m_opacity   = 255;
    }

    calcVertexes();
    calcColor();

    // Setup texture
    m_texture   = &CGame::AssetManager.getTexture( data.textureID );

    sf::Vector2u textureSize = m_texture->getSize();

    m_bounds[ 0 ].texCoords = sf::Vector2f( 0, 0 );
    m_bounds[ 1 ].texCoords = sf::Vector2f( textureSize.x, 0 );
    m_bounds[ 2 ].texCoords = sf::Vector2f( textureSize.x, textureSize.y );
    m_bounds[ 3 ].texCoords = sf::Vector2f( 0, textureSize.y );

    // Some specifications
    if( m_data.lifetime == 0.0 ) {
        m_liveByDistance = true;
    }
    else {
        m_lifeTimer.setLimit( m_data.lifetime );

        m_liveByDistance    = false;
    }
}

void CParticle::setActive( bool active ) {
    m_active    = active;
}

bool CParticle::isActive() const {
    return m_active;
}

/** PRIVATE */

void CParticle::updateMovement( const sf::Time& delta ) {
    if( m_data.moveVelocity != 0.0 ) {
        m_data.moveVelocity += m_data.moveAccel * delta.asSeconds();

        if( m_data.moveVelocity < 0.0 ) {
            m_data.moveVelocity = 0.0;
        }
    }

    float moveSpeed = delta.asSeconds() * m_data.moveVelocity;

    // Set position without invoking metamethods ( onPosChange() )
    m_pos += Util::moveByAngle( m_data.moveAngle, moveSpeed );

    if( m_liveByDistance ) {
        if( m_data.moveDistance != 0.0 ) {
            m_movedDistance += moveSpeed;

            if( m_movedDistance >= m_data.moveDistance ) {
                m_active = false;
            }
        }
    }
}

void CParticle::updateSlow( const sf::Time& delta ) {
    if( m_data.moveSlowType != ParticleSlowType::PSLOW_NONE ) {
        if( m_liveByDistance ) {
            switch( m_data.moveSlowType ) {
            case ParticleSlowType::PSLOW_FULLLIFE:
                if( m_data.moveVelocity != 0.0 ) {
                    m_data.moveVelocity -= m_data.moveSlowVel * delta.asSeconds();

                    if( m_data.moveVelocity < 0.0 ) {
                        m_active = false;
                    }
                }

                break;
            case ParticleSlowType::PSLOW_HALFLIFE:
                if( m_data.moveVelocity != 0.0 ) {
                    if( m_movedDistance > m_data.moveDistance / 2 ) {
                        m_data.moveVelocity -= m_data.moveSlowVel * delta.asSeconds();

                        if( m_data.moveVelocity < 0.0 ) {
                            m_active = false;
                        }
                    }
                }

                break;
            default: break;
            }
        }
        else {
            switch( m_data.moveSlowType ) {
            case ParticleSlowType::PSLOW_FULLLIFE:
                if( m_data.moveVelocity != 0.0 ) {
                    m_data.moveVelocity -= m_data.moveSlowVel * delta.asSeconds();

                    fixMoveVel();
                }

                break;
            case ParticleSlowType::PSLOW_HALFLIFE:
                if( m_data.moveVelocity != 0.0 ) {
                    if( m_lifeTimer.getProgress().asSeconds() > m_lifeTimer.getLimit().asSeconds() / 2 ) {
                        m_data.moveVelocity -= m_data.moveSlowVel * delta.asSeconds();

                        fixMoveVel();
                    }
                }

                break;
            default: break;
            }
        }
    }
}

void CParticle::updateRotation( const sf::Time& delta ) {
    if( m_data.rotationVel != 0.0 ) {
        m_rotAngle  += m_data.rotationVel * delta.asSeconds();
    }
}

void CParticle::updateLifetime( const sf::Time& delta ) {
    if( m_lifeTimer.update( delta ) ) {
        m_active = false;
    }
}

void CParticle::updateScaling( const sf::Time& delta ) {
    if( m_data.scalingType == ParticleScaleType::PSCALE_NONE ) return;

    float progress  = 0;
    float limit     = 1;

    if( m_liveByDistance ) {
        progress    = m_movedDistance;
        limit       = m_data.moveDistance;
    }
    else {
        progress  = m_lifeTimer.getProgress().asSeconds();
        limit     = m_lifeTimer.getLimit().asSeconds();
    }

    if( m_data.scalingVel != 0.0 ) {
        float speed = 0;

        if( m_data.scalingTime == ParticleScaleTime::PSCALETIME_FULLLIFE ) {
            speed = m_data.scalingVel * delta.asSeconds();
        }
        else if( m_data.scalingTime == ParticleScaleTime::PSCALETIME_HALFLIFE ) {
            if( progress >= limit / 2 ) {
                speed = m_data.scalingVel * delta.asSeconds();
            }
        }

        m_size.x = speed * m_data.size.x;
        m_size.y = speed * m_data.size.y;
    }
    else {
        float factor = 1;

        if( m_data.scalingTime == ParticleScaleTime::PSCALETIME_FULLLIFE ) {
            if( m_data.scalingType == ParticleScaleType::PSCALE_SHRINK ) {
                factor = 1 - progress / limit;
            }
            else if( m_data.scalingType == ParticleScaleType::PSCALE_DILATE ) {
                factor = progress / limit;
            }
        }
        else if( m_data.scalingTime == ParticleScaleTime::PSCALETIME_HALFLIFE ) {
            if( progress >= limit / 2 ) {
                if( m_data.scalingType == ParticleScaleType::PSCALE_SHRINK ) {
                    factor = 1 - ( ( progress - limit / 2 ) / ( limit / 2 ) );
                }
                else if( m_data.scalingType == ParticleScaleType::PSCALE_DILATE ) {
                    factor = ( ( progress - limit / 2 ) / ( limit / 2 ) );
                }
            }
        }

        m_size.x = factor * m_data.size.x;
        m_size.y = factor * m_data.size.y;
    }

    calcVertexes();
}

void CParticle::updateOpacity( const sf::Time& delta ) {
    if( m_data.opacityType == ParticleOpType::POPACITY_NONE ) return;

    float progress  = 0;
    float limit     = 1;

    if( m_liveByDistance ) {
        progress    = m_movedDistance;
        limit       = m_data.moveDistance;
    }
    else {
        progress  = m_lifeTimer.getProgress().asSeconds();
        limit     = m_lifeTimer.getLimit().asSeconds();
    }

    if( m_data.opacityVel != 0.0 ) {
        float speed = 0;

        if( m_data.opacityTime == ParticleOpTime::POPTIME_FULLLIFE ) {
            speed = m_data.opacityVel * delta.asSeconds();
        }
        else if( m_data.opacityTime == ParticleOpTime::POPTIME_HALFLIFE ) {
            if( progress >= limit / 2 ) {
                speed = m_data.opacityVel * delta.asSeconds();
            }
        }

        m_opacity += speed;
    }
    else {
        float factor = 1;

        if( m_data.opacityTime == ParticleOpTime::POPTIME_FULLLIFE ) {
            if( m_data.opacityType == ParticleOpType::POPACITY_TONULL ) {
                factor = 1 - progress / limit;
            }
            else if( m_data.opacityType == ParticleOpType::POPACITY_TOFULL ) {
                factor = progress / limit;
            }
        }
        else if( m_data.opacityTime == ParticleOpTime::POPTIME_HALFLIFE ) {
            if( progress >= limit / 2 ) {
                if( m_data.opacityType == ParticleOpType::POPACITY_TONULL ) {
                    factor = 1 - ( ( progress - limit / 2 ) / ( limit / 2 ) );
                }
                else if( m_data.opacityType == ParticleOpType::POPACITY_TOFULL ) {
                    factor = ( ( progress - limit / 2 ) / ( limit / 2 ) );
                }
            }
        }

        m_opacity   = factor * 255;
    }


    calcColor();
}

void CParticle::calcVertexes() {
    // Set new size to the vertex array
    // m_pos is excluded because position is being set through render states
    m_bounds[ 1 ].position = sf::Vector2f( m_size.x, 0 );
    m_bounds[ 2 ].position = sf::Vector2f( m_size.x, m_size.y );
    m_bounds[ 3 ].position = sf::Vector2f( 0, m_size.y );
}

void CParticle::calcColor() {
    sf::Color base = m_data.color;

    base.a  = (sf::Uint8)m_opacity;

    for( size_t i = 0; i < 4; i++ ) {
        m_bounds[ i ].color = base;
    }
}

void CParticle::fixMoveVel() {
    if( m_data.moveVelocity < 0.0 ) {
        m_data.moveVelocity = 0.0;
    }
}
