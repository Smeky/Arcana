#ifndef CPARTICLE_H
#define CPARTICLE_H

#include <CDimensional.h>
#include <CTimer.h>

enum class RotationDir : int {
    ROTATION_RANDOM,
    ROTATION_LEFT,
    ROTATION_RIGHT,
};

enum class ParticleSlowType : int {
    PSLOW_NONE,
    PSLOW_FULLLIFE,
    PSLOW_HALFLIFE,
};

enum class ParticleScaleType : int {
    PSCALE_NONE,
    PSCALE_SHRINK,
    PSCALE_DILATE,
};

enum class ParticleScaleTime : int {
    PSCALETIME_FULLLIFE,
    PSCALETIME_HALFLIFE,
};

enum class ParticleOpType : int {
    POPACITY_NONE,
    POPACITY_TONULL,
    POPACITY_TOFULL,
};

enum class ParticleOpTime : int {
    POPTIME_FULLLIFE,
    POPTIME_HALFLIFE,
};

/**
    - Range based variable will be randomized between value - range to value + range
      when particle data are set
*/
struct ParticleData {
    sf::Vector2f        pos;
    sf::Vector2f        size;
    std::string         textureID;
    int                 zLayer              = 10;
    int                 zLayerRange         = 0;
    float               moveVelocity        = 0.0;
    float               moveVelocityRange   = 0.0;
    float               moveAccel           = 0.0;  // Movement acceleration
    float               moveAccelRange      = 0.0;
    float               moveAngle           = 0.0;
    float               moveAngleRange      = 0.0;
    ParticleSlowType    moveSlowType        = ParticleSlowType::PSLOW_NONE;
    float               moveSlowVel         = 0.0;
    float               moveSlowRange       = 0.0;
    float               moveDistance        = 0.0;
    float               moveDistanceRange   = 0.0;
    RotationDir         rotationDirection   = RotationDir::ROTATION_RANDOM;
    float               rotationVel         = 0.0;
    float               rotationVelRange    = 0.0;
    float               rotationAccel       = 0.0;  // Rotation acceleration
    float               rotationAccelRange  = 0.0;  // Rotation acceleration
    float               lifetime            = 0.0;
    float               lifetimeRange       = 0.0;
    ParticleScaleType   scalingType         = ParticleScaleType::PSCALE_NONE;
    ParticleScaleTime   scalingTime         = ParticleScaleTime::PSCALETIME_FULLLIFE;
    float               scalingVel          = 0.0;
    float               scalingVelRange     = 0.0;
    sf::Color           color               = sf::Color::White;
    float               opacity             = 255;
    ParticleOpType      opacityType         = ParticleOpType::POPACITY_NONE;
    ParticleOpTime      opacityTime         = ParticleOpTime::POPTIME_FULLLIFE;
    float               opacityVel          = 0.0;
    float               opacityVelRange     = 0.0;
};

class CParticle : public CDimensional {
public:
                    CParticle       ();

    void            update          ( const sf::Time& delta );
    void            draw            ( sf::RenderTarget& window, sf::RenderStates states ) const;

    void            setData         ( const ParticleData& data );

    void            setActive       ( bool active );
    bool            isActive        () const;

private:
    void            updateMovement  ( const sf::Time& delta );
    void            updateSlow      ( const sf::Time& delta );
    void            updateRotation  ( const sf::Time& delta );
    void            updateLifetime  ( const sf::Time& delta );
    void            updateScaling   ( const sf::Time& delta );
    void            updateOpacity   ( const sf::Time& delta );

    void            calcVertexes    ();
    void            calcColor       ();
    void            fixMoveVel      (); // Check if movement velocity is below 0 and if so, fixes it to 0

private:
    bool            m_active;

    sf::VertexArray m_bounds;
    sf::Texture*    m_texture;

    ParticleData    m_data;
    CTimer          m_lifeTimer;

    float           m_opacity;

    bool            m_liveByDistance;
    float           m_movedDistance;

};

#endif // CPARTICLE_H
