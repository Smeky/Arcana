#include "CPlayer.h"

#include "CGame.h"

void CPlayer::update( const sf::Time& delta ) {
    m_moveSpeed = m_moveDirSpeed * delta.asSeconds();

    CCharacter::update( delta );

    m_moveDirSpeed.x    = 0;
    m_moveDirSpeed.y    = 0;
}

void CPlayer::moveLeft() {
    m_moveDirSpeed.x -= m_moveVelocity;

    m_moveDirTrack[ LEFT ] = true;
}

void CPlayer::moveRight() {
    m_moveDirSpeed.x += m_moveVelocity;

    m_moveDirTrack[ RIGHT ] = true;
}

void CPlayer::moveUp() {
    m_moveDirSpeed.y -= m_moveVelocity;

    m_moveDirTrack[ UP ] = true;
}

void CPlayer::moveDown() {
    m_moveDirSpeed.y += m_moveVelocity;

    m_moveDirTrack[ DOWN ] = true;
}

CInventory& CPlayer::getInventory() {
    return m_inventory;
}

void CPlayer::onStatChange() {
    CGame::MessageSystem.broadcast( new CMessage( "PlayerStatUpdate" ) );
}

void CPlayer::onNewAbility( size_t index ) {
    // Used in abilitybar.lua for setting up new button
    CGame::ScriptSystem.fireEventWithInt( "PlayerNewAbility", 1, index );
}

void CPlayer::onAbilityCast( size_t index ) {
    CGame::ScriptSystem.fireEventWithInt( "PlayerAbilityCasted", 1, index );
}

void CPlayer::updateDirOfAnimation( float* angle ) {
//    if( !m_attTimer.isTime() ) return;
    if( angle != nullptr ) {
        CEntity::updateDirOfAnimation( &m_castAngle );

        return;
    }

    for( size_t i = 0; i < 4; i++ ) {
        if( m_prevMoveDirTrack[ i ] != m_moveDirTrack[ i ] ) {
            // If key was released
            if( !m_moveDirTrack[ i ] ) {
                auto it = std::find( m_moveDirBuffer.begin(), m_moveDirBuffer.end(), i );

                if( it != m_moveDirBuffer.end() ) {
                    m_moveDirBuffer.erase( it );
                }
            }
            // If key was pressed
            else {
                m_moveDirBuffer.push_back( i );
            }

            m_prevMoveDirTrack[ i ] = m_moveDirTrack[ i ];
        }
    }

    // Update animation
    if( !m_moveDirBuffer.empty() ) {
        if( m_moveDirBuffer.size() > 2 ) {
            size_t dirIndex;

            if( m_moveDirSpeed.x < 0 ) {
                dirIndex = LEFT;
            }
            else if( m_moveDirSpeed.x > 0 ) {
                dirIndex = RIGHT;
            }

            if( m_moveDirSpeed.y < 0 ) {
                dirIndex = UP;
            }
            else if( m_moveDirSpeed.y > 0 ) {
                dirIndex = DOWN;
            }

            if( dirIndex != m_animation.getRowIndex() ) {
                m_animation.setRowIndex( dirIndex );
            }
        }
        else if( m_moveDirBuffer.size() != 2 ) {
            if( m_moveDirBuffer.front() != m_animation.getRowIndex() ) {
                m_animation.setRowIndex( m_moveDirBuffer.front() );
            }
        }
    }

    for( size_t i = 0; i < 4; i++ ) {
        m_moveDirTrack[ i ] = false;
    }
}
