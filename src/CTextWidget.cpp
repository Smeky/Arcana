#include "CTextWidget.h"

void CTextWidget::draw( sf::RenderTarget& window, sf::RenderStates states ) const {
    sf::Transformable transformable;

    transformable.setPosition( m_pos );

    states.transform *= transformable.getTransform();

    window.draw( m_text, states );
}

void CTextWidget::setTextWrapper( const CTextWrapper& textWrapper ) {
    m_text  = textWrapper;
}
