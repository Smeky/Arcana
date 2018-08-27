#ifndef CWIDGETEFFECT_H
#define CWIDGETEFFECT_H

#include <CButtonImage.h>

class CEffect;

class CWidgetEffect : public CButtonImage {
public:
                    CWidgetEffect       ( const CEffect* effect );

    void            handleInput         ( sf::Event event );
    void            update              ( sf::Time delta );
//    void            draw                ( sf::RenderTarget& window, sf::RenderStates states ) const;

private:
    const CEffect*  m_effect;

};

#endif // CWIDGETEFFECT_H
