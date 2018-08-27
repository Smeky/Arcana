#ifndef CABILITY_H
#define CABILITY_H

#include "string"
#include "SFML/System/Vector2.hpp"
#include "CTimer.h"

struct AbilityStats {
    std::string     name;
    std::string     description;
    std::string     textureID;
    int             cost            = 0;
    float           castTime        = 0.0;
    float           cooldown        = 0.0;
};

class CCharacter;

class CAbility {
public:
                    CAbility            ();
                   ~CAbility            ();

    void            update              ( const sf::Time& delta );

    void            setID               ( size_t ID );
    size_t          getID               () const;

    void            setParent           ( CCharacter* parent );

    void            setStats            ( const AbilityStats& stats );
    AbilityStats    getStats            () const;

    void            setOnCastID         ( size_t ID );

    bool            isInstant           () const;

    void            onCast              ( const sf::Vector2f& destination = sf::Vector2f() );

    float           getCooldown         () const;
    bool            hasCooldown         () const;
    bool            isCooldown          () const;

private:
    size_t          m_ID;
    CCharacter*     m_parent;
    AbilityStats    m_stats;
    CTimer          m_cdTimer;

    size_t          m_onCastFunctionID;
};

#endif // CABILITY_H
