#include "CDimensional.h"

#include <Utility.h>

CDimensional::CDimensional()
: m_rotAngle    ( 0.0 )
, m_zLayer      ( 10 )
{}

void CDimensional::setPos( const sf::Vector2f& pos ) {
    m_pos       = pos;

    onPosChange();
}

void CDimensional::setPos( float x, float y ) {
    m_pos.x     = x;
    m_pos.y     = y;

    onPosChange();
}

sf::Vector2f CDimensional::getPos() const {
    return m_pos;
}

void CDimensional::setPosRounded( const sf::Vector2f& pos ) {
    m_pos.x     = (int)pos.x;
    m_pos.y     = (int)pos.y;

    onPosChange();
}

void CDimensional::setPosRounded( float x, float y ) {
    m_pos.x     = (int)x;
    m_pos.y     = (int)y;

    onPosChange();
}

void CDimensional::setCenter( const sf::Vector2f& center ) {
    m_pos.x     = center.x - m_size.x / 2;
    m_pos.y     = center.y - m_size.y / 2;

    onPosChange();
}

void CDimensional::setCenter( float x, float y ) {
    m_pos.x     = x - m_size.x / 2;
    m_pos.y     = y - m_size.y / 2;

    onPosChange();
}

sf::Vector2f CDimensional::getCenter() const {
    return sf::Vector2f( m_pos.x + m_size.x / 2, m_pos.y + m_size.y / 2 );
}

void CDimensional::setSize( const sf::Vector2f& size ) {
    m_size = size;

    onSizeChange();
}

void CDimensional::setSize( float x, float y ) {
    m_size.x = x;
    m_size.y = y;

    onSizeChange();
}

sf::Vector2f CDimensional::getSize() const {
    return m_size;
}

void CDimensional::setPosSize( const sf::Vector2f& pos ) {
    m_pos.x = pos.x - m_size.x;
    m_pos.y = pos.y - m_size.y;

    onPosChange();
}

void CDimensional::setPosSize( float x, float y ) {
    m_pos.x = x - m_size.x;
    m_pos.y = y - m_size.y;

    onPosChange();
}

sf::Vector2f CDimensional::getPosSize() const {
    return sf::Vector2f( m_pos.x + m_size.x, m_pos.y + m_size.y );
}

void CDimensional::setBox( const sf::FloatRect& box ) {
    m_pos.x     = box.left;
    m_pos.y     = box.top;
    m_size.x    = box.width;
    m_size.y    = box.height;

    onPosChange();
    onSizeChange();
}

void CDimensional::setBox( const sf::Vector2f& pos, const sf::Vector2f& size ) {
    m_pos       = pos;
    m_size      = size;

    onPosChange();
    onSizeChange();
}

void CDimensional::setBox( float x, float y, float w, float h ) {
    m_pos.x     = x;
    m_pos.y     = y;
    m_size.x    = w;
    m_size.y    = h;

    onPosChange();
    onSizeChange();
}

void CDimensional::setRotation( float angle ) {
    m_rotAngle  = angle;
}

float CDimensional::getRotation() const {
    return m_rotAngle;
}

sf::FloatRect CDimensional::getBox() const {
    return sf::FloatRect( m_pos.x, m_pos.y, m_size.x, m_size.y );
}

void CDimensional::resetCollBox() {
    m_collBox.left      = 0;
    m_collBox.top       = 0;
    m_collBox.width     = m_size.x;
    m_collBox.height    = m_size.y;
}

void CDimensional::setCollBox( const sf::FloatRect& box ) {
    m_collBox           = box;
}

void CDimensional::setCollBox( const sf::Vector2f& pos, const sf::Vector2f& size ) {
    m_collBox.left      = pos.x;
    m_collBox.top       = pos.y;
    m_collBox.width     = size.x;
    m_collBox.height    = size.y;
}

void CDimensional::setCollBox( float x, float y, float w, float h ) {
    m_collBox.left      = x;
    m_collBox.top       = y;
    m_collBox.width     = w;
    m_collBox.height    = h;
}

sf::FloatRect CDimensional::getCollBox() const {
    return sf::FloatRect( m_pos.x + m_collBox.left, m_pos.y + m_collBox.top, m_collBox.width, m_collBox.height );
}

sf::Vector2f CDimensional::getCollBoxPos() const {
    return sf::Vector2f( m_collBox.left, m_collBox.top );
}

sf::Vector2f CDimensional::getCollBoxSize() const {
    return sf::Vector2f( m_collBox.width, m_collBox.height );
}

void CDimensional::move( const sf::Vector2f& movement ) {
    m_pos       += movement;

    onPosChange();
}

void CDimensional::move( float x, float y ) {
    m_pos.x     += x;
    m_pos.y     += y;

    onPosChange();
}

void CDimensional::resize( const sf::Vector2f& size ) {
    m_size      += size;

    onSizeChange();
}

void CDimensional::resize( float x, float y ) {
    m_size.x    += x;
    m_size.y    += y;

    onSizeChange();
}

void CDimensional::rotate( float rotation ) {
    m_rotAngle  += rotation;
}

void CDimensional::rotateCenter( float rotation ) {
    m_rotAngle += rotation;
}

sf::Transform CDimensional::getTransform() const {
    sf::Transformable transformable;

    transformable.setPosition( m_pos );

    return transformable.getTransform();
}

void CDimensional::setZLayer( int layer ) {
    m_zLayer    = layer;
}

void CDimensional::moveZLayer( int amount ) {
    m_zLayer    += amount;

    if( m_zLayer < 0 ) {
        m_zLayer = 0;
    }
    else if( m_zLayer > 20 ) {
        m_zLayer = 20;
    }
}

int CDimensional::getZLayer() const {
    return m_zLayer;
}
