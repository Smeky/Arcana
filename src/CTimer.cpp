#include "CTimer.h"

CTimer::CTimer()
{}

bool CTimer::update( const sf::Time& delta ) {
    if( isTime() ) {
        return true;
    }
    else {
        m_progress += delta;

        if( m_progress > m_limit ) {
            m_progress = m_limit;
        }

        return false;
    }
}

void CTimer::setLimit( const sf::Time& limit ) {
    m_limit = limit;

    reset();
}

void CTimer::setLimit( float limit ) {
    m_limit = sf::seconds( limit );

    reset();
}

void CTimer::setLimitOnly( const sf::Time& limit ) {
    m_limit = limit;
}

void CTimer::setLimitOnly( float limit ) {
    m_limit = sf::seconds( limit );
}

void CTimer::setProgress( const sf::Time& progress ) {
    m_progress = progress;
}

void CTimer::setProgress( float progress ) {
    m_progress = sf::seconds( progress );
}

sf::Time CTimer::getRemaining() const {
    if( isTime() ) {
        return sf::Time::Zero;
    }

    return m_limit - m_progress;
}

sf::Time CTimer::getLimit() const {
    return m_limit;
}

sf::Time CTimer::getProgress() const {
    return m_progress;
}

void CTimer::reset() {
    m_progress = sf::Time::Zero;
}

bool CTimer::isTime() const {
    return m_progress >= m_limit;
}
