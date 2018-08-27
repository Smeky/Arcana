#ifndef CPARTICLESYSTEM_H
#define CPARTICLESYSTEM_H

#include <list>

#include <CParticle.h>

struct lua_State;
class CDimensional;
class CEntity;
class CParticleEffect;
class CParticleEmitter;

class CParticleSystem {
private:
    typedef CParticleEmitter Emitter;

public:
                    CParticleSystem     ();
                   ~CParticleSystem     ();

    void            close               ();

    void            update              ( const sf::Time& delta );

    void            spawnParticle       ( const ParticleData& data, const sf::Vector2f& poo );


    size_t          effectFromLuaTable  ( lua_State* state, int index, const CEntity* parent = nullptr );

    void            removeEffect        ( size_t ID );

public:
    static int      luaCreateEffect     ( lua_State* state );
    static int      luaCreateParticle   ( lua_State* state );

private:
    size_t          getNewID            ();

    void            addToZBuffer        ();

    Emitter*        createEmitter       ( lua_State* state, int index, const CEntity* parent = nullptr );

    ParticleData    particleDataFromLua ( lua_State* state, int index );
    ParticleData    setupSingleParticle ( lua_State* state, int index );

    void            setupParticleData   ( ParticleData& data );
    void            calcRange           ( float& value, float range );
    void            calcRange           ( int& value, int range );

private:
    size_t                              m_activeCounter;
    size_t                              m_idCounter;

    static const size_t                 m_maxParticles  = 1024;

    CParticle                           m_particles[ m_maxParticles ];

    std::vector<CParticleEffect*>       m_effects;
};

#endif // CPARTICLESYSTEM_H
