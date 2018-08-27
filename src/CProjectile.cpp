#include "CProjectile.h"

#include "CGame.h"
#include "Utility.h"
#include <CMap.h>

#include "iostream"
using namespace std;

CProjectile::CProjectile()
: CEntity::CEntity()
, m_flyDistance     ( 0 )
, m_maxDistance     ( 0 )
{
    m_type      = EntityType::TYPE_PROJECTILE;

    for( int i = 0; i < PROJFUNC_TOTAL; i++ ) {
        m_projFuncIDs[ i ] = 0;
    }
}

void CProjectile::update( const sf::Time& delta ) {
    float speed = m_moveVelocity * delta.asSeconds();

    m_flyDistance += speed;

    if( m_flyDistance >= m_maxDistance ) {
        m_despawn = true;
    }

    m_moveSpeed = Util::moveByAngle( m_moveAngle, speed );

    doMove();

    m_moveSpeed.x   = 0;
    m_moveSpeed.y   = 0;
}

void CProjectile::draw( sf::RenderTarget& window, sf::RenderStates states ) const {
    sf::Transformable transformable;

    transformable.setPosition( m_pos );
    transformable.setRotation( Util::radToDegree( m_moveAngle ) + 90 );

    states.transform *= transformable.getTransform();

    window.draw( m_animation, states );
}

void CProjectile::setCollCircle( const Circle& circle ) {
    m_collCircle = circle;
}

Circle CProjectile::getCollCircle() const {
    return m_collCircle;
}

void CProjectile::setFlyDistance( float distance ) {
    m_maxDistance = distance;
}

float CProjectile::getFlyDistance() const {
    return m_maxDistance;
}

void CProjectile::setStats( const ProjectileStats& stats ) {
    m_stats      = stats;
}

ProjectileStats CProjectile::getStats() const {
    return m_stats;
}

void CProjectile::setProjFunc( ProjectileFuncs type, size_t ID ) {
    m_projFuncIDs[ type ] = ID;
}

/** Private */

void CProjectile::doMove() {
    // Add m_movement to position
    m_pos += m_moveSpeed;

    // Copy collision circle ( so we can modify it for correct coll detection )

//    collCircle.x = getCenter().x;
//    collCircle.y = getCenter().y;

//    if( m_stats.type == ProjectType::ARROW ) {
        // Make the circle to be at the head of the projectile
        m_collCircle.x = m_pos.x + cosf( m_moveAngle ) * m_collCircle.r;
        m_collCircle.y = m_pos.y + sinf( m_moveAngle ) * m_collCircle.r;
//    }
    Circle collCircle = m_collCircle;

    CDimensional dim;
    dim.setPos( m_pos.x - m_collCircle.r, m_pos.y - m_collCircle.r );
    dim.setSize( m_collCircle.r * 2, m_collCircle.r * 2 );

    C_Dimensionals tiles = CGame::WorldManager.getCurrentlyUsedMap()->getNearby( &dim );

    for( auto it : tiles ) {
        if( Util::collCircleToRect( collCircle, it->getCollBox() ) ) {
            m_despawn = true;
            return;
        }
    }

    // Get nearby entities from spatial system
    CSpatialSystem::Cell entities = CGame::SpatialSystem.getNearby( this );

    // Go through all nearby entities
    for( auto it : entities ) {
        // Prevent using object that may not exist
        if( !it->isDespawn() ) {

            if( it->isSolid() || it->isCollideable() ) {
                sf::FloatRect collBox;

                bool character = CGame::EntitySystem.isCharacter( it->getID() );

                // Collision box for characters should be their whole box
                if( character ) {
                    collBox = it->getBox();
                }
                else {
                    collBox = it->getCollBox();
                }

                if( Util::collCircleToRect( collCircle, collBox ) ) {
                    if( character ) {
                        // False if target is of the same faction
                        if( hitTarget( it->getID() ) ) {
                            onEnemyHit( it );

                            m_despawn = true;
                            return;
                        }
                    }
                    else {
                        m_despawn = true;
                        return;
                    }
                }
            }
        } // If entity shouldn't despawn
    } // Entities loop
}

bool CProjectile::hitTarget( size_t targetID ) {
//    if( CGame::EntitySystem.isCharacter( targetID ) ) {
        if( !CGame::EntitySystem.compareFaction( m_stats.faction, targetID ) ) {
            if( m_stats.dmgMin != 0 ) {
                CGame::EntitySystem.attackTarget( m_ID, targetID, m_stats.dmgMin, DamageType::DMG_NORMAL );
            }

            // In case character has different faction
            return true;
        }
        else {
            // In case character has the same faction
            return false;
        }

//    }

    // In case target is not character
    return true;
}

void CProjectile::onEnemyHit( void* enemy ) {
    CGame::EntitySystem.callLuaFunctionLUD( m_ID, m_projFuncIDs[ PROJFUNC_ONENEMYHIT ], enemy );
}
