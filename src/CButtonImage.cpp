#include "CButtonImage.h"

#include "CGame.h"
#include "Utility.h"

#include "iostream"
using namespace std;

CButtonImage::CButtonImage()
: m_midDisplay      ( true )
, m_topDisplay      ( true )
, m_buttonAboveMid  ( false )
{}

void CButtonImage::update( sf::Time delta ) {
    CButton::update();

    // Update texture clip rectangle based on button state
    if( !m_active ) {
        m_spriteButton.setClip( sf::IntRect( m_clipSize.x * 3, 0, m_clipSize.x, m_clipSize.y ) );
    }
    else if( m_pressed ) {
        m_spriteButton.setClip( sf::IntRect( m_clipSize.x * 2, 0, m_clipSize.x, m_clipSize.y ) );
    }
    else if( m_mouseover ) {
        m_spriteButton.setClip( sf::IntRect( m_clipSize.x * 1, 0, m_clipSize.x, m_clipSize.y ) );
    }
    else {
        m_spriteButton.setClip( sf::IntRect( m_clipSize.x * 0, 0, m_clipSize.x, m_clipSize.y ) );
    }
}

void CButtonImage::draw( sf::RenderTarget& window, sf::RenderStates states ) const {
    sf::Transformable transformable;

    transformable.setPosition( m_pos );

    window.draw( m_spriteBg,     transformable.getTransform() );

    if( !m_buttonAboveMid ) {
        window.draw( m_spriteButton, transformable.getTransform() );
    }

    if( m_midDisplay ) {
        window.draw( m_spriteMid,    transformable.getTransform() );
    }

    if( m_buttonAboveMid ) {
        window.draw( m_spriteButton, transformable.getTransform() );
    }

    if( m_topDisplay ) {
        window.draw( m_spriteTop,    transformable.getTransform() );
    }

    window.draw( m_text,         transformable.getTransform() );
}

void CButtonImage::setButtonAboveMid( bool above ) {
    m_buttonAboveMid = above;
}

void CButtonImage::setButtonTexture( const std::string& textureID ) {
    m_spriteButton.setTexture( textureID );

    m_clipSize.x    = m_spriteButton.getTexture()->getSize().x / 4;
    m_clipSize.y    = m_spriteButton.getTexture()->getSize().y;

    m_spriteButton.setClip( sf::IntRect( 0, 0, m_clipSize.x, m_clipSize.y ) );

    // Update size, so the clip size fits the button size
    m_spriteButton.setSize( m_size );
}

void CButtonImage::setBgTexture( const std::string& textureID ) {
    m_spriteBg.setTexture( textureID );
}

void CButtonImage::setMiddleTexture( const std::string& textureID ) {
    m_spriteMid.setTexture( textureID );
}

void CButtonImage::setFrontTexture( const std::string& textureID ) {
    m_spriteTop.setTexture( textureID );
}

void CButtonImage::setTextWrapper( const CTextWrapper& textWrapper ) {
    m_text          = textWrapper;
}

void CButtonImage::setMidDisplay( bool display ) {
    m_midDisplay    = display;
}

void CButtonImage::setTopDisplay( bool display ) {
    m_topDisplay    = display;
}

void CButtonImage::onParentSet() {
    m_pos += m_parent->getPos();
}

void CButtonImage::onSizeChange() {
    m_spriteButton.setSize( m_size );
    m_spriteBg.setSize( m_size );
    m_spriteMid.setSize( m_size );
    m_spriteTop.setSize( m_size );
}
