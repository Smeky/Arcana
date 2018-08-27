#include "CCombatTextSystem.h"

#include "CGame.h"
#include "CCombatText.h"
#include "Utility.h"
#include "CRNG.h"

CCombatTextSystem::~CCombatTextSystem() {
    close();
}

void CCombatTextSystem::close() {
    while( !m_combatTexts.empty() ) delete m_combatTexts.back(), m_combatTexts.pop_back();
}

void CCombatTextSystem::update( const sf::Time& delta ) {
    for( auto it = m_combatTexts.begin(); it != m_combatTexts.end(); ) {
        if( (*it)->isDespawn() ) {
            delete *it;

            it = m_combatTexts.erase( it );
        }
        else {
            (*it)->update( delta );

            it++;
        }
    }
}

void CCombatTextSystem::render( sf::RenderTarget& window, sf::RenderStates states ) const {
    for( auto it : m_combatTexts ) {
        window.draw( *it, states );
    }
}

void CCombatTextSystem::createCombatText( const sf::Vector2f& pos, int number, CombatTextType type ) {
    CCombatText*    combatText  = new CCombatText();
    CTextWrapper    textWrapper;

    sf::Vector2f randomPos;
    randomPos.x     = CRNG::iRandom( pos.x - 15, pos.x + 15 );
    randomPos.y     = CRNG::iRandom( pos.y - 15, pos.y + 15 );

    textWrapper.setPos( randomPos );
    textWrapper.setFont( "FONT_ARIAL" );
    textWrapper.setString( Util::intToString( number ) );

    if( type == CombatTextType::COMBATTEXT_DMGNORMAL ) {
        textWrapper.setCharSize( 14 );
        textWrapper.setColor( sf::Color::White );
    }
    else if( type == CombatTextType::COMBATTEXT_DMGCRIT ) {
        textWrapper.setCharSize( 16 );
        textWrapper.setColor( sf::Color::Red );
    }

    textWrapper.setOrigin( OriginPosition::ORIGIN_CENTER );

    combatText->setTextWrapper( textWrapper );
    combatText->setLifetime( 2.0 );
    combatText->setMoveVelocity( 15 );
    combatText->setFadeVelocity( 550 );

    m_combatTexts.push_back( combatText );
}
