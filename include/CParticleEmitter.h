#ifndef CPARTICLEEMITTER_H
#define CPARTICLEEMITTER_H

#include <CParticle.h>


/**
    Particle emitter lifetime types: ( how long should the emitter be alive )
        TIMED       - Lifetime based on timer only
        PARENT      - Lifetime based on lifetime of the parent
*/
enum class PELifetimeType : int {
    PELIFE_TIMED,
    PELIFE_PARENT,
};

/**
    Particle emitter position specification types:
        DEFAULT     - Position is specified inside emitter data table
        PARENT      - Position is based on parents center position
*/
enum class PEPositionType : int {
    PEPOS_DEFAULT,
    PEPOS_PARENT,
};

/**
    Particle emitter z-layer types:
        DEFAULT     - Normal z-buffering ( same layer as parent )
        RANDOM      - Z-layer randomized in range ( around parents z-layer )
        FRONT       - Z-layer above parent ( by one )
        BEHIND      - Z-layer behind parent ( by one )
*/
enum class PEZLayerType : int {
    PZLAYER_DEFAULT,
    PZLAYER_PARENT,
    PZLAYER_RANDOM,
    PZLAYER_FRONT,
    PZLAYER_BEHIND,
};

enum class PEEmissionAngleType : int {
    EMANGLE_RANDOM,
    EMANGLE_STRAIGHT,
};

struct PEmitterData {
    float               emissionRate        = 0.0;
    float               radius              = 0.0;

    sf::Vector2f        offset;
    PEPositionType      posType             = PEPositionType::PEPOS_DEFAULT;
    sf::Vector2f        position;

    PELifetimeType      lifetimeType        = PELifetimeType::PELIFE_TIMED;
    float               lifetime            = 0.0;

    PEZLayerType        zLayerType          = PEZLayerType::PZLAYER_DEFAULT;

    PEEmissionAngleType emissionAngleType   = PEEmissionAngleType::EMANGLE_RANDOM;
    float               emissionAngle       = 0.0;
};

class CEntity;

class CParticleEmitter {
public:
                    CParticleEmitter    ();
                   ~CParticleEmitter    () {}

    void            update              ( const sf::Time& delta );

    void            setParent           ( const CEntity* parent );

    void            setData             ( const PEmitterData& data );
    void            setParticleData     ( const ParticleData& data );

    bool            isActive            () const;

private:
    ParticleData    setupParticleData   ();
    void            calcRange           ( float& value, float   range );
    void            calcRange           ( int&   value, int     range );

private:
    bool            m_active;

    const CEntity*  m_parent;
    PEmitterData    m_data;
    ParticleData    m_particleData;

    CTimer          m_lifeTimer;
    CTimer          m_emissionTimer;
};

#endif // CPARTICLEEMITTER_H
