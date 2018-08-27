#ifndef CPROJECTILE_H
#define CPROJECTILE_H

#include "CEntity.h"
#include "CGame.h"
#include "CharacterFaction.h"

struct ProjectileStats {
    int                 dmgMin      = 0;
    int                 dmgMax      = 0;
    float               critChance  = 0;
    float               critMultip  = 0;
    CharacterFaction    faction     = CharacterFaction::FACTION_NEUTRAL;
};

enum ProjectileFuncs {
    PROJFUNC_ONENEMYHIT,
    PROJFUNC_TOTAL  // Keep last
};

class CProjectile : public CEntity {
public:
                    CProjectile     ();

    void            update          ( const sf::Time& delta );
    void            draw            ( sf::RenderTarget& window, sf::RenderStates states ) const;

    void            setCollCircle   ( const Circle& circle );
    Circle          getCollCircle   () const;

    void            setFlyDistance  ( float distance );
    float           getFlyDistance  () const;

    void            setStats        ( const ProjectileStats& stats );
    ProjectileStats getStats        () const;

    void            setProjFunc     ( ProjectileFuncs type, size_t ID );

private:
    void            doMove          ();
    bool            hitTarget       ( size_t targetID );

    void            onEnemyHit      ( void* enemy );

private:
    Circle          m_collCircle;

    float           m_flyDistance;
    float           m_maxDistance;

    size_t          m_projFuncIDs[ PROJFUNC_TOTAL ];

    ProjectileStats m_stats;
};

#endif // CPROJECTILE_H
