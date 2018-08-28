#include "CProgressiveStat.h"

CProgressiveStat::CProgressiveStat()
: m_maximum     ( 0 )
, m_current     ( 0 )
{}

bool CProgressiveStat::add( int amount ) {
    m_current += amount;

    if( m_current > m_maximum ) {
        m_current = m_maximum;

        return false;
    }

    return true;
}

bool CProgressiveStat::remove( int amount ) {
    m_current -= amount;

    if( m_current < 0 ) {
        m_current = 0;

        return false;
    }

    return true;
}

bool CProgressiveStat::canAdd( int amount ) const {
    return m_current + amount <= m_maximum;
}

bool CProgressiveStat::canRemove( int amount ) const {
    return m_current - amount >= 0;
}

void CProgressiveStat::setMaximum( int maximum ) {
    m_maximum   = maximum;
}

void CProgressiveStat::setMaximumFactor( int maximum ) {
    float factor = (float)m_current / m_maximum;

    m_maximum   = maximum;
    m_current   = maximum * factor;
}

void CProgressiveStat::setCurrent( int current ) {
    m_current   = current;
}

int CProgressiveStat::getMaximum() const {
    return m_maximum;
}

int CProgressiveStat::getCurrent() const {
    return m_current;
}

bool CProgressiveStat::isEmpty() const {
    return m_current == 0;
}
