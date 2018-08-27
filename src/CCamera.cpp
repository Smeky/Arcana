#include "CCamera.h"

#include "CGame.h"

void CCamera::update( const sf::Time& delta ) {
    if( m_pos != m_destination ) {
        // Move the camera
        m_pos += ( m_destination - m_pos ) * ( m_moveFactor * delta.asSeconds() );

        sf::Vector2f difference = m_pos - m_destination;

        if( difference.x < 0.5 && difference.x > - 0.5 ) m_pos.x = m_destination.x;
        if( difference.y < 0.5 && difference.y > - 0.5 ) m_pos.y = m_destination.y;

        // Store map size
        sf::Vector2f mapSize = CGame::WorldManager.getMapSize();

        // Make sure camera doesn't go outside map
        if( m_pos.x < 0 ) { m_pos.x = 0; }
        else if( getPosSize().x > mapSize.x ) { m_pos.x = mapSize.x - m_size.x; }

        if( m_pos.y < 0 ) { m_pos.y = 0; }
        else if( getPosSize().y > mapSize.y ) { m_pos.y = mapSize.y - m_size.y; }
    }
}

void CCamera::moveTo( const sf::Vector2f& destination, float factor ) {
    m_destination   = destination;
    m_moveFactor    = factor;
}

void CCamera::moveCenterTo( const sf::Vector2f& destination, float factor ) {
    m_destination   = destination - getRelCenter();
    m_moveFactor    = factor;
}

sf::Vector2f CCamera::getOffset( const sf::Vector2f& pos ) {
    return pos - m_pos;
}

sf::Vector2f CCamera::getRelCenter() const {
    return sf::Vector2f( m_size.x / 2, m_size.y / 2 );
}

sf::Transform CCamera::getTransform() {
    sf::Transformable transformable;

    transformable.setPosition( -(int)( m_pos.x + 0.5 ), -(int)( m_pos.y + 0.5 ) );

    return transformable.getTransform();
}
