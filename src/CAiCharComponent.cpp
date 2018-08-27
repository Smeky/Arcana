#include "CAiCharComponent.h"

#include "CGame.h"
#include "CCharacter.h"
#include "Utility.h"

CAiCharComponent::CAiCharComponent()
: m_parent      ( nullptr )
{}

void CAiCharComponent::setParent( CCharacter* parent ) {
    m_parent    = parent;
}

/** Ai ranger human */
void CAiRangerHuman::update( const sf::Time& delta ) {
    float moveSpeed = m_parent->getMoveVelocity() * delta.asSeconds();

    sf::Vector2f targetPos = CGame::EntitySystem.getPlayerCenter();

    float   targetDistance    = Util::lenghtByVectors( m_parent->getCenter(), targetPos );
    int     shootDistance     = 300;
    int     runAwayDistance   = 150;

    float targetAngle = Util::angleByTarget( m_parent->getCenter(), targetPos );

    sf::Vector2f movement;

    if( targetDistance < runAwayDistance ) {
        m_parent->setMoveAngle( targetAngle + Util::degreeToRad( 180 ) );

        movement = Util::moveByAngle( targetAngle - Util::degreeToRad( 180 ), moveSpeed );
    }
    else if( targetDistance >= shootDistance - 20 ) {
        m_parent->setMoveAngle( targetAngle );

        movement = Util::moveByAngle( targetAngle, moveSpeed );
    }
    else {
        m_parent->setMoveAngle( targetAngle );
    }

    m_parent->setMoveSpeed( movement );

    if( targetDistance < shootDistance ) {
        m_parent->performAttack( targetAngle );
    }
}
