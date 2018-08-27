#include "CEffect.h"

#include <CGame.h>
#include <CCharacter.h>

CEffect::CEffect()
: m_active      ( true )
, m_infinite    ( false )
, m_parent      ( nullptr )
{
    for( size_t i = 0; i < 3; i++ ) {
        m_funcIDs[ i ] = 0;
    }
}

void CEffect::update( const sf::Time& delta ) {
    if( !m_infinite ) {
        if( m_liveTimer.update( delta ) ) {
            m_active = false;
        }
    }

    if( hasTicks() ) {
        if( m_tickTimer.update( delta ) ) {
            onAction();

            m_tickTimer.reset();
        }
    }
}

void CEffect::setData( const EffectData& data ) {
    m_data  = data;

    if( data.liveTime == 0.0 ) {
        m_infinite  = true;
    }
    else {
        m_liveTimer.setLimit( data.liveTime );
    }

    if( data.tickTime != 0.0 ) {
        m_tickTimer.setLimit( data.tickTime );
    }
}

EffectData CEffect::getData() const {
    return m_data;
}

void CEffect::setParent( CCharacter* parent ) {
    m_parent    = parent;
}

void CEffect::setFunctionID( EffectFunctionType type, size_t ID ) {
    m_funcIDs[ type ]   = ID;
}

void CEffect::onAplication() {
    if( m_data.color != sf::Color::White ) {
        m_parent->getAnimation().getSprite().addColorModul( m_data.color );
    }

    size_t functionID = m_funcIDs[ EffectFunctionType::EFUNC_APLICATION ];

    if( functionID != 0 ) {
        CGame::EffectManager.callLuaFunction( functionID, m_parent );
    }
}

void CEffect::onAction() {
    size_t functionID = m_funcIDs[ EffectFunctionType::EFUNC_ACTION ];

    if( functionID != 0 ) {
        CGame::EffectManager.callLuaFunction( functionID, m_parent );
    }
}

void CEffect::onRemove() {
    if( m_data.color != sf::Color::White ) {
        m_parent->getAnimation().getSprite().delColorModul( m_data.color );
    }

    size_t functionID = m_funcIDs[ EffectFunctionType::EFUNC_REMOVE ];

    if( functionID != 0 ) {
        CGame::EffectManager.callLuaFunction( functionID, m_parent );
    }
}

CTimer CEffect::getLiveTimer() const {
    return m_liveTimer;
}

CTimer CEffect::getTickTimer() const {
    return m_tickTimer;
}

bool CEffect::hasTicks() const {
    return m_data.tickTime != 0.0;
}

bool CEffect::isActive() const {
    return m_active;
}

bool CEffect::isInfinite() const {
    return m_infinite;
}

/** CEffectStat */

void CEffectStat::setStatData( const EffectStatData& data ) {
    m_statData  = data;
}

void CEffectStat::doStatAdjustment() {
    m_statModID = m_parent->addStatAndUpdate( m_statData.statIndex, m_statData.value, m_statData.operation );
}

void CEffectStat::removeStatAdjustment() {
    m_parent->delStatAndUpdate( m_statData.statIndex, m_statModID );
}

/** CEffectEvent */

void CEffectEvent::onEventAction( int argc, ... ) {
    std::vector<float>  vecArguments;

    va_list arguments;
    va_start( arguments, argc );

    for( int i = 0; i < argc; i++ ) {
        int num = va_arg( arguments, int );

        vecArguments.push_back( num );
    }

    va_end( arguments );

    CGame::EffectManager.callLuaFunction( m_funcIDs[ EFUNC_ACTION ], m_parent, vecArguments );
}
