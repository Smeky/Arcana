#include "CWidgetEffect.h"

#include <CEffect.h>
#include <CGame.h>
#include <CPlayer.h>

CWidgetEffect::CWidgetEffect( const CEffect* effect )
: m_effect      ( effect )
{
    setMiddleTexture( m_effect->getData().textureID );
}

void CWidgetEffect::handleInput( sf::Event event ) {
    CButtonImage::handleInput( event );


}

void CWidgetEffect::update( sf::Time delta ) {
    if( m_effect == nullptr ) {
        m_delete = true;

        return;
    }

    if( m_rClicked ) {
        if( !m_effect->isInfinite() && m_effect->getData().type == EffectType::EFFECT_BUFF ) {
            std::string     ID      = m_effect->getData().ID;
            EffectSubType   type    = m_effect->getData().subType;

            CGame::EntitySystem.getPlayer()->delEffect( ID, type );

            m_delete = true;
        }
    }
}

