#ifndef CHARACTERSTATS_H_INCLUDED
#define CHARACTERSTATS_H_INCLUDED

struct CharacterStats {
    /** Basic character stats */
    int         STR             = 0;
    int         DEX             = 0;
    int         INT             = 0;
    int         VIT             = 0;
    int         Armor           = 0;

    /** Regeneration */
    int         regenHealth     = 0;
    int         regenResource   = 0;

    /** Speed based */
    float       movementSpd     = 0;
    float       attackSpd       = 0;
    float       castingSpd      = 0;

    /** Damage based */
    int         damageMin       = 0;
    int         damageMax       = 0;
    float       critChance      = 0;
    float       critMultip      = 0;

    CharacterStats operator+( const CharacterStats& right ) {
        CharacterStats result;

        result.STR           =  this->STR            + right.STR;
        result.DEX           =  this->DEX            + right.DEX;
        result.INT           =  this->INT            + right.INT;
        result.VIT           =  this->VIT            + right.VIT;

        result.regenHealth   =  this->regenHealth    + right.regenHealth;
        result.regenResource =  this->regenResource  + right.regenResource;

        result.movementSpd   =  this->movementSpd    + right.movementSpd;
        result.attackSpd     =  this->attackSpd      + right.attackSpd;
        result.castingSpd    =  this->castingSpd     + right.castingSpd;

        result.damageMin     =  this->damageMin      + right.damageMin;
        result.damageMax     =  this->damageMax      + right.damageMax;
        result.critChance    =  this->critChance     + right.critChance;
        result.critMultip    =  this->critMultip     + right.critMultip;

        return result;
    }

    CharacterStats operator-( const CharacterStats& right ) {
        CharacterStats result;

        result.STR           =  this->STR            - right.STR;
        result.DEX           =  this->DEX            - right.DEX;
        result.INT           =  this->INT            - right.INT;
        result.VIT           =  this->VIT            - right.VIT;

        result.regenHealth   =  this->regenHealth    - right.regenHealth;
        result.regenResource =  this->regenResource  - right.regenResource;

        result.movementSpd   =  this->movementSpd    - right.movementSpd;
        result.attackSpd     =  this->attackSpd      - right.attackSpd;
        result.castingSpd    =  this->castingSpd     - right.castingSpd;

        result.damageMin     =  this->damageMin      - right.damageMin;
        result.damageMax     =  this->damageMax      - right.damageMax;
        result.critChance    =  this->critChance     - right.critChance;
        result.critMultip    =  this->critMultip     - right.critMultip;

        return result;
    }

    void operator+=( const CharacterStats& right ) {
        this->STR           +=  right.STR;
        this->DEX           +=  right.DEX;
        this->INT           +=  right.INT;
        this->VIT           +=  right.VIT;

        this->regenHealth   +=  right.regenHealth;
        this->regenResource +=  right.regenResource;

        this->movementSpd   +=  right.movementSpd;
        this->attackSpd     +=  right.attackSpd;
        this->castingSpd    +=  right.castingSpd;

        this->damageMin     +=  right.damageMin;
        this->damageMax     +=  right.damageMax;
        this->critChance    +=  right.critChance;
        this->critMultip    +=  right.critMultip;
    }

    void operator-=( const CharacterStats& right ) {
        this->STR           -=  right.STR;
        this->DEX           -=  right.DEX;
        this->INT           -=  right.INT;
        this->VIT           -=  right.VIT;

        this->regenHealth   -=  right.regenHealth;
        this->regenResource -=  right.regenResource;

        this->movementSpd   -=  right.movementSpd;
        this->attackSpd     -=  right.attackSpd;
        this->castingSpd    -=  right.castingSpd;

        this->damageMin     -=  right.damageMin;
        this->damageMax     -=  right.damageMax;
        this->critChance    -=  right.critChance;
        this->critMultip    -=  right.critMultip;
    }
};

#endif // CHARACTERSTATS_H_INCLUDED
