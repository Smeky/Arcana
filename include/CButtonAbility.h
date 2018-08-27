#ifndef CBUTTONABILITY_H
#define CBUTTONABILITY_H

#include "CButtonImage.h"
#include "CMessageListener.h"

/**
    Button that shows players ability
    cooldown
*/

class CButtonAbility : public CButtonImage, public CMessageListener {
public:
                    CButtonAbility  ();

    void            handleMsg       ( CMessage* msg );

    void            update          ( sf::Time time );

    void            setAbilityIndex ( size_t index );

private:
    size_t          m_abilityIndex;
    bool            m_cooldownProgress;
};

#endif // CBUTTONABILITY_H
