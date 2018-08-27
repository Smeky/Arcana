#include "CButtonAbility.h"

#include "CGame.h"
#include "CPlayer.h"
#include "CAbility.h"
#include "Utility.h"

#include "iostream"
using namespace std;

CButtonAbility::CButtonAbility()
: m_abilityIndex    ( 0 )
, m_cooldownProgress( false )
{}

void CButtonAbility::handleMsg( CMessage* msg ) {
    if( msg->getMsg() == "PlayerAbilityCasted" ) {
        size_t index = dynamic_cast<CTypedMessage<size_t>*>( msg )->getValue();

        if( index == m_abilityIndex ) {
            m_cooldownProgress = true;
        }
    }
}

void CButtonAbility::update( sf::Time delta ) {
    if( !m_cooldownProgress ) return;

    CPlayer* player = CGame::EntitySystem.getPlayer();

    if( player != nullptr ) {
        const CAbility* ability = player->getAbility( m_abilityIndex );

        if( ability != nullptr ) {
            if( ability->isCooldown() ) {
                m_text.setString( Util::decimalToString( ability->getStats().cooldown - ability->getCooldown() ) );
            }
            else {
                m_text.setString( "" );

                m_cooldownProgress = false;
            }
        }
    }
}

void CButtonAbility::setAbilityIndex( size_t index ) {
    m_abilityIndex = index;

    CPlayer* player = CGame::EntitySystem.getPlayer();

    if( player != nullptr ) {
        const CAbility* ability = player->getAbility( m_abilityIndex );

        if( ability != nullptr ) {
            m_spriteBg.setTexture( ability->getStats().textureID );
        }
    }
}
