#include "CWindowCastbar.h"

#include "CGame.h"
#include "CPlayer.h"
#include "Utility.h"

void CWindowCastbar::update( const sf::Time& delta ) {
    CPlayer* player = CGame::EntitySystem.getPlayer();

         if( player == nullptr )    return;
    else if( !player->isCasting() ) return;

    CTimer castTimer = player->getCastingTimer();

    float factor = castTimer.getProgress().asSeconds() / castTimer.getLimit().asSeconds();

    sf::FloatRect bounds = m_bgSprite.getLocalBounds();

    bounds.width *= factor;

    m_progSprite.setClip( bounds );

    // Update cast text
    m_castText.setString( Util::decimalToString( castTimer.getLimit().asSeconds() - castTimer.getProgress().asSeconds() ) );
}

void CWindowCastbar::draw( sf::RenderTarget& window, sf::RenderStates states ) const {
    CPlayer* player = CGame::EntitySystem.getPlayer();

         if( player == nullptr )    return;
    else if( !player->isCasting() ) return;

    CWindow::draw( window, states );

    sf::Transformable transformable;

    transformable.setPosition( m_pos );

    states.transform *= transformable.getTransform();

    window.draw( m_borderSprite,    states );
    window.draw( m_bgSprite,        states );
    window.draw( m_progSprite,      states );
    window.draw( m_castText,        states );
}

void CWindowCastbar::setCBBorder( const CSpriteWrapper& spriteWrapper ) {
    m_borderSprite  = spriteWrapper;
    m_borderSprite.setSize( m_size );
}

void CWindowCastbar::setCBBackground( const CSpriteWrapper& spriteWrapper ) {
    m_bgSprite      = spriteWrapper;
    m_bgSprite.setSize( m_size );
}

void CWindowCastbar::setCBProgress( const CSpriteWrapper& spriteWrapper ) {
    m_progSprite    = spriteWrapper;
    m_progSprite.setSize( m_size );
}

void CWindowCastbar::setCastTimeText( const CTextWrapper& textWrapper ) {
    m_castText      = textWrapper;
}
