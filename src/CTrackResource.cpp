#include "CTrackResource.h"

#include "CGame.h"
#include "CPlayer.h"
#include "Utility.h"

#include "iostream"
using namespace std;

CTrackResource::CTrackResource()
: m_prevCurrent ( 0 )
, m_prevTotal   ( 0 )
{
    m_text.setFont( "FONT_ARIAL" );
    m_text.setCharSize( 16 );
    m_text.setOrigin( OriginPosition::ORIGIN_CENTER );
}

void CTrackResource::update( sf::Time delta ) {
    CPlayer* player = CGame::EntitySystem.getPlayer();

    int resourceTotal       = player->getMaximumResource();
    int resourceCurrent     = player->getCurrentResource();

    if( m_prevCurrent != resourceCurrent || m_prevTotal != resourceTotal ) {
        updateTracker( resourceCurrent, resourceTotal );
    }
}

void CTrackResource::draw( sf::RenderTarget& window, sf::RenderStates states ) const {
    sf::Transformable transformable;

    transformable.setPosition( m_pos );

    states.transform *= transformable.getTransform();

    window.draw( m_bgSpite,     states );
    window.draw( m_progSprite,  states );
    window.draw( m_text,        states );
}

void CTrackResource::setBgTexture( const std::string& textureID ) {
    m_bgSpite.setTexture( CGame::AssetManager.getTexture( textureID ) );
    Util::resizeSprite( m_bgSpite, m_size );
}

void CTrackResource::setProgTexture( const std::string& textureID ) {
    m_progSprite.setTexture( CGame::AssetManager.getTexture( textureID ) );
    Util::resizeSprite( m_progSprite, m_size );
}

/** PRIVATE */

void CTrackResource::onSizeChange() {
    CWidget::onSizeChange();

    m_text.setPosRounded( m_size.x / 2, m_size.y / 2 - 2 );

    Util::resizeSprite( m_bgSpite, m_size );
    Util::resizeSprite( m_progSprite, m_size );
}

void CTrackResource::updateTracker( int current, int total ) {
    /** Text */
    std::string text;

    text.append( Util::intToString( current ) );
    text.append( " / " );
    text.append( Util::intToString( total ) );

    m_text.setString( text );
    m_text.setOrigin( OriginPosition::ORIGIN_CENTER );

    /** Sprite */
    float factor = (float)current / total;

    sf::Vector2u size  = m_progSprite.getTexture()->getSize();

    m_progSprite.setTextureRect( sf::IntRect( 0, size.y - size.y * factor, size.x, size.y * factor ) );
    m_progSprite.setPosition( 0, size.y - size.y * factor );

    m_prevCurrent   = current;
    m_prevTotal     = total;
}
