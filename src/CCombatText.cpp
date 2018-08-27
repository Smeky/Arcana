#include "CCombatText.h"

#include "iostream"
using namespace std;

void CCombatText::update( const sf::Time& delta ) {
    // Update life timer
    m_lifeTimer.update( delta );

    if( m_lifeTimer.isTime() ) {
        m_text.fade( m_fadeVelocity * delta.asSeconds() );
    }

    // Move up
    move( 0, - ( m_moveVelocity * delta.asSeconds() ) );
}

void CCombatText::draw( sf::RenderTarget& window, sf::RenderStates states ) const {
    sf::Transformable transformable;

    transformable.setPosition( m_pos );

    states.transform *= transformable.getTransform();

    window.draw( m_text, states );
}

void CCombatText::setTextWrapper( const CTextWrapper& textWrapper ) {
    m_text  = textWrapper;
}

void CCombatText::setLifetime( float seconds ) {
    m_lifeTimer.setLimit( seconds );
}

void CCombatText::setMoveVelocity( float velocity ) {
    m_moveVelocity  = velocity;
}

void CCombatText::setFadeVelocity( float velocity ) {
    m_fadeVelocity  = velocity;
}

bool CCombatText::isDespawn() const {
    return m_text.isFaded();
}
