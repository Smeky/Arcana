#include "CWidget.h"

CWidget::CWidget()
: m_parent      ( nullptr )
, m_delete      ( false )
{}

CWidget::~CWidget() {
    m_parent = nullptr;
}

void CWidget::setID( size_t ID ) {
    m_ID = ID;
}

size_t CWidget::getID() const {
    return m_ID;
}

void CWidget::setParent( CWindow* window ) {
    m_parent = window;

    onParentSet();
}

void CWidget::updatePos() {
    setPos( m_pos + m_parent->getPos() );
}

bool CWidget::shouldDelete() const {
    return m_delete;
}

void CWidget::onPosChange() {
    m_pos.x = (int)( m_pos.x + 0.5 );
    m_pos.y = (int)( m_pos.y + 0.5 );
}
