#include "CCombatTextSystem.h"

#include "CGame.h"
#include "CCombatText.h"
#include <CCharacter.h>
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
        textWrapper.setStyle( sf::Text::Bold );
    }

    textWrapper.setOrigin( OriginPosition::ORIGIN_CENTER );

    combatText->setTextWrapper( textWrapper );
    combatText->setLifetime( 2.0 );
    combatText->setMoveVelocity( 15 );
    combatText->setFadeVelocity( 550 );

    m_combatTexts.push_back( combatText );
}

void CCombatTextSystem::createCombatText( const CCharacter* character, int number, DamageType dmgType ) {
    CombatTextType textType;

    switch( dmgType ) {
    case DamageType::DMG_NORMAL:    textType = CombatTextType::COMBATTEXT_DMGNORMAL; break;
    case DamageType::DMG_CRIT:      textType = CombatTextType::COMBATTEXT_DMGCRIT; break;
    default:                        textType = CombatTextType::COMBATTEXT_DMGNORMAL; break;
    }

    createCombatText( sf::Vector2f( character->getCenter().x, character->getPos().y ), number, textType );
}
