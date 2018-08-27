#ifndef CTEXTWIDGET_H
#define CTEXTWIDGET_H

#include "CWidget.h"
#include "CTextWrapper.h"

class CTextWidget : public CWidget {
public:
    void            draw            ( sf::RenderTarget& window, sf::RenderStates states ) const;

    void            setTextWrapper  ( const CTextWrapper& textWrapper );

private:
    CTextWrapper    m_text;
};

#endif // CTEXTWIDGET_H
