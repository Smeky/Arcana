#include "CAiCharComponent.h"

#include "CGame.h"
#include <CPlayer.h>
#include "CCharacter.h"
#include "Utility.h"

#include <iostream>
using namespace std;

CAiCharComponent::CAiCharComponent()
: m_parent      ( nullptr )
{}

void CAiCharComponent::setParent( CCharacter* parent ) {
    m_parent    = parent;
}

/** Ai ranger human */
void CAiRangerHuman::update( const sf::Time& delta ) {
    CPlayer* player = CGame::EntitySystem.getPlayer();

    if( player == nullptr ) {
        return;
    }

    sf::Vector2f targetPos       = player->getCenter();

    float       targetDistance  = Util::lenghtByVectors( m_parent->getCenter(), targetPos );
    float       shootDistance   = 300;
    float       targetAngle     = Util::angleByTarget( m_parent->getCenter(), targetPos );

    if( targetDistance < shootDistance ) {
        if( targetDistance < shootDistance ) {
            m_parent->performAttack( targetAngle );
        }

        m_pathWaypoints.clear();
    }
    else if( targetDistance < 1000 && targetDistance > shootDistance ) {
        if( m_seekTarget ) {
            if( m_timerPathFind.update( delta ) ) {
                m_pathWaypoints = CGame::PathFinder.findPath( m_parent->getCenter(), targetPos );
            }
        }
        else {
            m_pathWaypoints = CGame::PathFinder.findPath( m_parent->getCenter(), targetPos );

            m_seekTarget    = true;

            m_timerPathFind.setLimit( 1.0 );
        }

        if( !m_pathWaypoints.empty() ) {
            targetPos   = m_pathWaypoints.back();
        }
    }
    else {
        if( m_seekTarget ) {
            m_pathWaypoints.clear();

            m_seekTarget = false;
        }

        return;
    }

    float moveSpeed = m_parent->getMoveVelocity() * delta.asSeconds();

//    int     shootDistance     = 300;
//    int     runAwayDistance   = 150;


    sf::Vector2f movement;

//    if( targetDistance < runAwayDistance ) {
//        m_parent->setMoveAngle( targetAngle + Util::degreeToRad( 180 ) );
//
//        movement = Util::moveByAngle( targetAngle - Util::degreeToRad( 180 ), moveSpeed );
//    }
//    else if( targetDistance >= shootDistance - 20 ) {
//        m_parent->setMoveAngle( targetAngle );
//
//        movement = Util::moveByAngle( targetAngle, moveSpeed );
//    }
//    else {
    if( !m_pathWaypoints.empty() ) {
            movement = Util::moveByAngle( targetAngle, moveSpeed );
            m_parent->setMoveAngle( targetAngle );
    //    }

        m_parent->setMoveSpeed( movement );


        while( m_pathWaypoints.empty() ) {
            sf::Vector2f relativePos = m_parent->getPos() - m_pathWaypoints.back();

            if( relativePos.x < 10 && relativePos.y < 10 ) {
                m_pathWaypoints.pop_back();
            }
            else {
                break;
            }
        }
    }

//    if( targetDistance < shootDistance ) {
//        m_parent->performAttack( targetAngle );
//    }
}
