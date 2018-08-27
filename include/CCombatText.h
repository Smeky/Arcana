#ifndef CCOMBATTEXT_H
#define CCOMBATTEXT_H

#include "CDimensional.h"
#include "CTextWrapper.h"
#include "CTimer.h"

class CCombatText : public CDimensional {
public:
    void            update          ( const sf::Time& delta );
    void            draw            ( sf::RenderTarget& window, sf::RenderStates states ) const;

    void            setTextWrapper  ( const CTextWrapper& textWrapper );
    void            setLifetime     ( float seconds );
    void            setMoveVelocity ( float velocity );
    void            setFadeVelocity ( float velocity );

    bool            isDespawn       () const;

private:
    float           m_moveVelocity;
    float           m_fadeVelocity;

    CTimer          m_lifeTimer;
    CTextWrapper    m_text;

};

#endif // CCOMBATTEXT_H
