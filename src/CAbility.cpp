#include "CAbility.h"

#include "CGame.h"
#include "CCharacter.h"

CAbility::CAbility()
: m_parent              ( nullptr )
, m_onCastFunctionID    ( 0 )
{}

CAbility::~CAbility() {
    CGame::AbilityManager.removeLuaFunction( m_onCastFunctionID );
}

void CAbility::update( const sf::Time& delta ) {
    m_cdTimer.update( delta );
}

void CAbility::setID( size_t ID ) {
    m_ID    = ID;
}

size_t CAbility::getID() const {
    return m_ID;
}

void CAbility::setParent( CCharacter* parent ) {
    m_parent = parent;
}

void CAbility::setStats( const AbilityStats& stats ) {
    m_stats = stats;

    m_cdTimer.setLimit(     stats.cooldown );
    m_cdTimer.setProgress(  stats.cooldown );
}

AbilityStats CAbility::getStats() const {
    return m_stats;
}

void CAbility::setOnCastID( size_t ID ) {
    m_onCastFunctionID = ID;
}

bool CAbility::isInstant() const {
    return m_stats.castTime == 0.0;
}

float CAbility::getCooldown() const {
    return m_cdTimer.getProgress().asSeconds();
}

bool CAbility::hasCooldown() const {
    return m_stats.cooldown != 0.0;
}

bool CAbility::isCooldown() const {
    return !m_cdTimer.isTime();
}

void CAbility::onCast( const sf::Vector2f& destination ) {
    CGame::AbilityManager.callLuaFunction( m_onCastFunctionID, m_parent, destination );

    if( hasCooldown() ) {
        m_cdTimer.reset();
    }
}
