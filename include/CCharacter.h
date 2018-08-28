#ifndef CCHARACTER_H
#define CCHARACTER_H

#include <CEntity.h>
#include <CharacterStats.h>
#include <CharacterFaction.h>
#include <CProgressiveStat.h>
#include <CCharStat.h>
#include <LootTable.h>

enum CharacterClass {
    NOCLASS,
    BARBARIAN,
    KNIGHT,
    RANGER,
    ASSASSIN,
    SORCERER,
    NECROMANCER
};

enum class StatIndex : int {
    STAT_HEALTH = 0,
    STAT_RESOURCE,
    STAT_STR,
    STAT_DEX,
    STAT_INT,
    STAT_VIT,
    STAT_ARMOR,
    STAT_REGENHEALTH,
    STAT_REGENRESOURCE,
    STAT_MOVEMENTSPD,
    STAT_ATTACKSPD,
    STAT_CASTINGSPD,
    STAT_DAMAGEMIN,
    STAT_DAMAGEMAX,
    STAT_CRITCHANCE,
    STAT_CRITMULTIP,
    STAT_EXPBONUS,

    STAT_COUNT          // Keep last
};

enum class EffectEventType : int {
    EEVENT_NONE         = - 1,
    EEVENT_ONDMG        = 0,
    EEVENT_ONHEAL,
    EEVENT_ONDMGCRIT,
    EEVENT_COUNT        // KEEP LAST
};

enum class DamageType : int;
enum class EffectSubType : int;

class CAiCharComponent;
class CAbility;
class CEffect;
class CEffectStat;
class CEffectEvent;

class CCharacter : public CEntity {
private:
    typedef std::vector<CAbility*>      Abilities;
    typedef std::vector<CEffect*>       Effects;
    typedef std::vector<CEffectStat*>   EffectsStat;
    typedef std::vector<CEffectEvent*>  EffectsEvent;

public:
                        CCharacter          ();
                       ~CCharacter          ();

    virtual void        update              ( const sf::Time& delta );

            void        setAiCharComponent  ( CAiCharComponent* component );

            /** Combat */
            void        performAttack       ( float angle );
            void        takeDamage          ( size_t sourceID, int damage, DamageType type );   // Temp, remove it
            int         calculateReductions ( int damage );

            /** Character based data */
            void        setBaseStats        ( const CharacterStats& stats );
            void        setClass            ( const CharacterClass& charClass );
            void        setFaction          ( const CharacterFaction& faction );

            void        addBaseStats        ( const CharacterStats& stats );
            void        removeBaseStats     ( const CharacterStats& stats );

            /** Character stat manipulation */
            size_t      addStat             ( StatIndex index, float value, CalcOperation operand );
            size_t      addStatAndUpdate    ( StatIndex index, float value, CalcOperation operand );
            void        delStat             ( StatIndex index, size_t ID );
            void        delStatAndUpdate    ( StatIndex index, size_t ID );
            void        updateStats         ();

    CharacterStats      getBaseStats        () const;
    CharacterStats      getModStats         () const;
    CharacterClass      getClass            () const;
    CharacterFaction    getFaction          () const;

            /** Experience */
            void        setLevel            ( size_t level );
            void        setExpOnKill        ( size_t amount );
            void        addExp              ( size_t amount );
            size_t      getExp              () const;
            size_t      getExpOnKill        () const;
            size_t      getExpToLevel       () const;
            size_t      getLevel            () const;

            /** Health and resource */
            void        setMaxHealth        ( int maximum );
            void        setMaxResource      ( int maximum );

            int         getMaximumHealth    () const;
            int         getCurrentHealth    () const;
            int         getMaximumResource  () const;
            int         getCurrentResource  () const;

            bool        isDead              () const;
            bool        isAttacking         () const;

            /** Abilities */
            int         addAbility          ( CAbility* ability );
            void        setAbility          ( size_t index, CAbility* ability );
            void        castAbility         ( size_t index, const sf::Vector2f& destination = sf::Vector2f() );
    const   CAbility*   getAbility          ( size_t index ) const;
            bool        isCasting           () const;
            CTimer      getCastingTimer     () const;

            /** Effects */
            void        addEffect           ( CEffect* effect );
            void        addEffectStat       ( CEffectStat* effect );
            void        addEffectEvent      ( CEffectEvent* effect, EffectEventType type );
            void        delEffect           ( const std::string& ID );
            void        delEffect           ( const std::string& ID, const EffectSubType& subtype );

            /** Loot */
            void        setLootTable        ( const lootTable& table );
            lootTable   getLootTable        () const;
            bool        hasLootTable        () const;

protected:
    virtual void        onStatChange        () {}   // Called whenever character stat is modified
            void        setupStats          ();

    virtual void        updateAnimation     ( const sf::Time& delta );

            /** Stat updates */
            void        updateMovementSpeed ();
            void        updateRegens        ( const sf::Time& delta );

            /** Experience */
            void        levelUp             ();
            // Add stats for new level
            void        calcLevelUpStats    ( int level = 1 );

            /** Offensive */
            void        updateAttack        ( const sf::Time& delta );

            /** Abilities */
            void        updateAbilities     ( const sf::Time& delta );
            void        updateCasting       ( const sf::Time& delta );
            void        finishCasting       ();
    virtual void        onNewAbility        ( size_t index ) {}
    virtual void        onAbilityCast       ( size_t index ) {}

            /** Effects */
            void        updateEffects       ( const sf::Time& delta );

protected:
    CAiCharComponent*   m_aiCharComponent;

    /** Character based data */
    CCharStat           m_stats[ (int)StatIndex::STAT_COUNT ];
    CharacterStats      m_baseStats;
    CharacterStats      m_modStats;

    CharacterClass      m_class;
    CharacterFaction    m_faction;

    /** Experience */
    size_t              m_expOnKill;
    size_t              m_exp;
    float               m_expFactor;
    size_t              m_expToLevel;
    size_t              m_level;

    /** Health and resource */
    CProgressiveStat    m_health;
    CProgressiveStat    m_resource;
    CTimer              m_regTimer;             // Tracks when hp/rp regeneration should be applied

    /** Offensive */
    CTimer              m_attTimer;             // Tracks delay between attacks

    /** Abilities */
    Abilities           m_abilities;
    CAbility*           m_castBuffer;
    CTimer              m_castTimer;
    sf::Vector2f        m_castDestionation;
    float               m_castAngle;

    /** Effects */
    Effects             m_effects;
    EffectsStat         m_effectsStat;
    EffectsEvent        m_effectsEvent[ 3 ];

    /** Loot */
    bool                m_lootSet;
    lootTable           m_lootTable;
};

#endif // CCHARACTER_H
