#ifndef CEFFECT_H
#define CEFFECT_H

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Color.hpp>
#include <string>

#include <CTimer.h>
#include <CCharStat.h>
#include <CCharacter.h>

enum class EffectType : int {
    EFFECT_BUFF,
    EFFECT_DEBUFF
};

enum class EffectSubType : int {
    EFFECT_DEFAULT,
    EFFECT_STAT,
    EFFECT_EVENT
};

struct EffectData {
    float           liveTime    = 0.0;
    float           tickTime    = 0.0;
    EffectType      type        = EffectType::EFFECT_BUFF;
    EffectSubType   subType     = EffectSubType::EFFECT_DEFAULT;
    std::string     ID;
    std::string     textureID;
    sf::Color       color       = sf::Color::White;
};

struct EffectStatData {
    StatIndex       statIndex       = StatIndex::STAT_HEALTH;
    float           value           = 0.0;
    CalcOperation   operation       = CalcOperation::CALC_ADD;
};

class CCharacter;

class CEffect {
public:
    enum EffectFunctionType {
        EFUNC_APLICATION,
        EFUNC_ACTION,
        EFUNC_REMOVE,
    };

public:
                    CEffect         ();

    void            update          ( const sf::Time& delta );

    void            setData         ( const EffectData& data );
    EffectData      getData         () const;

    void            setParent       ( CCharacter* parent );

    void            setFunctionID   ( EffectFunctionType type, size_t ID );

    void            onAplication    ();
    void            onAction        ();
    void            onRemove        ();

    CTimer          getLiveTimer    () const;
    CTimer          getTickTimer    () const;

    bool            hasTicks        () const;
    bool            isActive        () const;
    bool            isInfinite      () const;

protected:
    bool            m_active;
    bool            m_infinite;

    EffectData      m_data;
    CCharacter*     m_parent;
    size_t          m_funcIDs[ 3 ];

    CTimer          m_liveTimer;
    CTimer          m_tickTimer;
};

class CEffectStat : public CEffect {
public:
    void            setStatData         ( const EffectStatData& data );

    void            doStatAdjustment    ();
    void            removeStatAdjustment();

private:
    EffectStatData  m_statData;
    size_t          m_statModID;

};

class CEffectEvent : public CEffect {
public:
    void            onEventAction       ( int argc, ... );
};


#endif // CEFFECT_H
