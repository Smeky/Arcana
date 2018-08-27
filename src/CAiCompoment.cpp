#include "CAiCompoment.h"

#include <CGame.h>
#include <CEntity.h>
#include <Utility.h>
#include <LuaWrapper.h>

CAiComponent::CAiComponent()
: m_parent  ( nullptr )
{}

void CAiComponent::setParent( CEntity* parent ) {
    m_parent    = parent;

    onParentSet();
}

/** Loop Movement */

CAiLoopMovement::CAiLoopMovement()
: m_toFirst         ( false )
, m_smoothVel       ( 0.0 )
, m_smoothTime      ( 0.0 )
, m_maxDistance     ( 0.0 )
, m_curDistance     ( 0.0 )
{}

void CAiLoopMovement::update( const sf::Time& delta ) {
    sf::Vector2f parentPos  = m_parent->getCenter();

    // In case movement loop should be smooth
    if( m_smoothVel != 0.0 ) {
        // Use correct movement direction based on loop direction
        if( m_toFirst ) m_smoothTime += delta.asSeconds();
        else            m_smoothTime -= delta.asSeconds();

        // Calculate smooth movement
        float v = m_smoothTime / m_smoothVel;
        v = v * v * ( 3 - 2 * v );
        parentPos.x = ( m_firstPos.x * v ) + ( m_secondPos.x * ( 1 - v ) );
        parentPos.y = ( m_firstPos.y * v ) + ( m_secondPos.y * ( 1 - v ) );

        // In case loop is over, reverse
        if( m_smoothTime >= m_smoothVel )   m_toFirst = false;
        else if( m_smoothTime <= 0 )        m_toFirst = true;
    }
    else {
        // Store correct destination pos based on loop direction
        sf::Vector2f destPos;
        m_toFirst ? destPos = m_firstPos : destPos = m_secondPos;

        // Calculate correct speed and angle
        float speed = m_parent->getMoveVelocity() * delta.asSeconds();
        float angle = Util::angleByTarget( parentPos, destPos );

        // Calculate movement
        parentPos += Util::moveByAngle( angle, speed );

        m_curDistance   += speed;

        // In case loop is over, reverse
        if( m_curDistance >= m_maxDistance ) {
            m_curDistance   = 0;
            m_toFirst       = !m_toFirst;
        }
    }

    // Move parent
    m_parent->setCenter( parentPos );
}

void CAiLoopMovement::loadDataFromLua( lua_State* state, int index ) {
    if( !lua_istable( state, index ) ) return;

    m_offsetPos     = Util::vectorFromTable<float>( state, index, "offsetPos" );

    m_maxDistance   = Util::lenghtByVectors( m_firstPos, m_secondPos );

    lWrapper::valueFromLuaTable<float>  ( state, index, "smoothVel",    m_smoothVel );
}

void CAiLoopMovement::onParentSet() {
    m_firstPos      = m_parent->getCenter();
    m_secondPos     = m_parent->getCenter() + m_offsetPos;
}
