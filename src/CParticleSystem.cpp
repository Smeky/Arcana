#include "CParticleSystem.h"

#include <cassert>

#include <CGame.h>
#include <CParticleEffect.h>
#include <CParticleEmitter.h>
#include <CEntity.h>
#include <Utility.h>
#include <LuaWrapper.h>
#include <CRNG.h>

#include "iostream"
using namespace std;

CParticleSystem::CParticleSystem()
: m_activeCounter   ( 0 )
, m_idCounter       ( 1 )
{
    for( size_t i = 0; i < m_maxParticles; i++ ) {
        m_particles[ i ] = CParticle();
    }
}

CParticleSystem::~CParticleSystem() {
    close();
}

void CParticleSystem::close() {
    // Deactivate all particles
    for( size_t i = 0; i < m_maxParticles; i++ ) {
        m_particles[ i ].setActive( false );
    }

    // Reset counter
    m_activeCounter = 0;

    while( !m_effects.empty() ) delete m_effects.back(), m_effects.pop_back();
}

void CParticleSystem::update( const sf::Time& delta ) {
    for( auto it = m_effects.begin(); it != m_effects.end(); ) {
        if( !(*it)->isActive() ) {
            delete *it;
            it = m_effects.erase( it );
        }
        else {
            (*it)->update( delta );

            it++;
        }
    }

    for( size_t i = 0; i < m_activeCounter; ) {
        // If particle is not active anymore, swap it with the last one in array
        if( !m_particles[ i ].isActive() ) {
            m_particles[ i ] = m_particles[ m_activeCounter - 1 ];

            m_activeCounter--;
        }
        else {
            m_particles[ i ].update( delta );

            i++;
        }
    }

    addToZBuffer();
}

void CParticleSystem::spawnParticle( const ParticleData& data, const sf::Vector2f& pos ) {
    // Make sure not to create too many particles
    if( m_activeCounter < m_maxParticles - 1 ) {
        m_particles[ m_activeCounter ].setData( data );
        m_particles[ m_activeCounter ].setPos( pos );

        m_activeCounter++;
    }
}

size_t CParticleSystem::effectFromLuaTable( lua_State* state, int index, const CEntity* parent ) {
    if( !lua_istable( state, index ) ) {
        std::cout << "Error: Unable to create particle effect out of non-table value" << std::endl;

        return 0;
    }

    CParticleEffect* effect = new CParticleEffect();

    effect->setID( getNewID() );

    for( lua_pushnil( state ); lua_next( state, index ) != 0; lua_pop( state, 1 ) ) {
        effect->addEmitter( createEmitter( state, lua_gettop( state ), parent ) );
    }

    m_effects.push_back( effect );

    return effect->getID();
}

void CParticleSystem::removeEffect( size_t ID ) {
    if( ID == 0 ) return;

    for( auto it = m_effects.begin(); it != m_effects.end(); it++ ) {
        if( (*it)->getID() == ID ) {
            delete *it;

            m_effects.erase( it );

            return;
        }
    }
}

/** LUA FUNCTIONS */

int CParticleSystem::luaCreateEffect( lua_State* state ) {
    int argc    = lua_gettop( state );

    assert( argc == 2 && "Wrong amount of arguments" );

    size_t ID = CGame::WorldManager.ParticleSystem.effectFromLuaTable( state, argc );

    lua_pop( state, argc );

    lua_pushinteger( state, ID );

    return 1;
}

int CParticleSystem::luaCreateParticle( lua_State* state ) {
    int argc = lua_gettop( state );

    bool            success = false;

    std::vector<ParticleData> data;

    sf::Vector2f    pos;
    int             amount  = 1;

    if( argc == 5 ) {
        amount  = lua_tointeger( state, argc );

        for( ; amount > 0; amount-- ) {
            data.push_back( CGame::WorldManager.ParticleSystem.setupSingleParticle( state, argc - 3 ) );
        }

        pos.x   = lua_tonumber( state, argc - 2 );
        pos.y   = lua_tonumber( state, argc - 1 );


        success = true;
    }
    else if( argc == 4 ) {
        data.push_back( CGame::WorldManager.ParticleSystem.setupSingleParticle( state, argc - 2 ) );

        pos.x   = lua_tonumber( state, argc - 1 );
        pos.y   = lua_tonumber( state, argc );

        success = true;
    }
    else {
        std::cout << "Error: Unable to create particle. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    if( success ) {
        for( auto& it : data ) {
            CGame::WorldManager.ParticleSystem.spawnParticle( it, pos );
        }
    }

    return 0;
}

/** PRIVATE */


size_t CParticleSystem::getNewID() {
    m_idCounter++;

    return m_idCounter - 1;
}

void CParticleSystem::addToZBuffer() {
    for( size_t i = 0; i < m_activeCounter; i++ ) {
        if( m_particles[ i ].isActive() ) {
            CGame::ZBuffer.insert( &m_particles[ i ] );
        }
    }
}

CParticleEmitter* CParticleSystem::createEmitter( lua_State* state, int index, const CEntity* parent ) {
    if( !lua_istable( state, index ) ) {
        std::cout << "Error: Unable to create emitter out of non-table value" << std::endl;

        return nullptr;
    }

    PEmitterData    emitterData;
    ParticleData    particleData;
    bool            parentBased = false;

    lWrapper::valueFromLuaTable<float>          ( state, index, "emission",     emitterData.emissionRate );
    lWrapper::valueFromLuaTable<bool>           ( state, index, "parent",       parentBased );
    lWrapper::valueFromLuaTable<PELifetimeType> ( state, index, "lifeType",     emitterData.lifetimeType );
    lWrapper::valueFromLuaTable<float>          ( state, index, "lifetime",     emitterData.lifetime );
    lWrapper::valueFromLuaTable<PEZLayerType>   ( state, index, "zLayerType",   emitterData.zLayerType );
    lWrapper::valueFromLuaTable<PEPositionType> ( state, index, "posType",      emitterData.posType );

    if( emitterData.posType == PEPositionType::PEPOS_DEFAULT ) {
        lua_getfield( state, index, "pos" );

        emitterData.position       = Util::vector2fFromLuaTable( state, lua_gettop( state ) );

        lua_pop( state, 1 );
    }
    else if( emitterData.posType == PEPositionType::PEPOS_PARENT ) {
        if( parent != nullptr ) {
            emitterData.position   = parent->getCenter();
        }
    }

    lua_getfield( state, index, "particle" );
    if( !lua_isnil( state, - 1 ) ) {
        particleData                = particleDataFromLua( state, lua_gettop( state ) );
    }

    lua_pop( state, 1 );

    CParticleEmitter* emitter = new CParticleEmitter();
    emitter->setData( emitterData );
    emitter->setParticleData( particleData );

    if( parentBased && parent != nullptr ) {
        emitter->setParent( parent );
    }

    return emitter;
}

ParticleData CParticleSystem::particleDataFromLua( lua_State* state, int index ) {
    ParticleData data;

    // In case table is missing
    if( !lua_istable( state, index ) ) {
        #ifdef DEBUG
            cout << "Error: Unable to create particle out of non-table value" << endl;
        #endif // DEBUG

        return data;
    }

    lua_getfield( state, index, "size" );
    data.size   = Util::vector2fFromLuaTable( state, lua_gettop( state ) );
    lua_pop( state, 1 );

    lua_getfield( state, index, "texture" );
    if( !lua_isnil( state, - 1 ) ) {
        data.textureID  = lua_tostring( state, - 1 );
    }
    lua_pop( state, 1 );

    // Z Layer
    lWrapper::valueFromLuaTable<int>              ( state, index, "zLayer",           data.zLayer );
    lWrapper::valueFromLuaTable<int>              ( state, index, "zLayerRange",      data.zLayerRange );

    // Movement velocity
    lWrapper::valueFromLuaTable<float>            ( state, index, "moveVel",          data.moveVelocity );
    lWrapper::valueFromLuaTable<float>            ( state, index, "moveVelRange",     data.moveVelocityRange );
    lWrapper::valueFromLuaTable<float>            ( state, index, "moveAccel",        data.moveAccel );
    lWrapper::valueFromLuaTable<float>            ( state, index, "moveAccelRange",   data.moveAccelRange );

    // Movement angle
    lWrapper::valueFromLuaTable<float>            ( state, index, "moveAngle",        data.moveAngle );
    lWrapper::valueFromLuaTable<float>            ( state, index, "moveAngleRange",   data.moveAngleRange );

    // Slow
    lWrapper::valueFromLuaTable<ParticleSlowType> ( state, index, "slowType",         data.moveSlowType );
    lWrapper::valueFromLuaTable<float>            ( state, index, "slowVel",          data.moveSlowVel );
    lWrapper::valueFromLuaTable<float>            ( state, index, "slowRange",        data.moveSlowRange );

    // Movement distance
    lWrapper::valueFromLuaTable<float>            ( state, index, "distance",         data.moveDistance );
    lWrapper::valueFromLuaTable<float>            ( state, index, "distanceRange",    data.moveDistanceRange );

    // Rotation
    lWrapper::valueFromLuaTable<RotationDir>      ( state, index, "rotDirection",     data.rotationDirection );
    lWrapper::valueFromLuaTable<float>            ( state, index, "rotVel",           data.rotationVel );
    lWrapper::valueFromLuaTable<float>            ( state, index, "rotVelRange",      data.rotationVelRange );
    lWrapper::valueFromLuaTable<float>            ( state, index, "rotAccel",         data.rotationAccel );
    lWrapper::valueFromLuaTable<float>            ( state, index, "rotAccelRange",    data.rotationAccelRange );

    // Lifetime
    lWrapper::valueFromLuaTable<float>            ( state, index, "lifetime",         data.lifetime );
    lWrapper::valueFromLuaTable<float>            ( state, index, "lifetimeRange",    data.lifetimeRange );

    // Scaling
    lWrapper::valueFromLuaTable<ParticleScaleType>( state, index, "scalingType",      data.scalingType );
    lWrapper::valueFromLuaTable<ParticleScaleTime>( state, index, "scalingTime",      data.scalingTime );
    lWrapper::valueFromLuaTable<float>            ( state, index, "scalingVel",       data.scalingVel );
    lWrapper::valueFromLuaTable<float>            ( state, index, "scalingVelRange",  data.scalingVelRange );

    // Color
    lua_getfield( state, index, "color" );
    if( !lua_isnil( state, - 1  ) ) {
        data.color  = Util::colorFromLuaTable( state, lua_gettop( state ) );
    }
    else {
        lua_getfield( state, index, "opacity" );
        if( lua_isnumber( state, - 1 ) ) {
            data.opacity    = lua_tonumber( state, - 1 );
        }

        lua_pop( state, 1 );
    }

    lua_pop( state, 1 );

    lWrapper::valueFromLuaTable<ParticleOpType>   ( state, index, "opacityType",      data.opacityType );
    lWrapper::valueFromLuaTable<ParticleOpTime>   ( state, index, "opacityTime",      data.opacityTime );
    lWrapper::valueFromLuaTable<float>            ( state, index, "opacityVel",       data.opacityVel );
    lWrapper::valueFromLuaTable<float>            ( state, index, "opacityVelRange",  data.opacityVelRange );

    return data;
}

ParticleData CParticleSystem::setupSingleParticle( lua_State* state, int index ) {
    ParticleData data;

    lua_getfield( state, index, "particle" );

    data    = particleDataFromLua( state, lua_gettop( state ) );

    PEEmissionAngleType angleType = PEEmissionAngleType::EMANGLE_RANDOM;

    lWrapper::valueFromLuaTable<PEEmissionAngleType> ( state, index, "angleType", angleType );

    if( angleType == PEEmissionAngleType::EMANGLE_RANDOM ) {
        data.moveAngle  = 360 * CRNG::percentRandom();
    }
    else if( angleType == PEEmissionAngleType::EMANGLE_RANDOM ) {
        float angle = 0;

        lWrapper::valueFromLuaTable<float>( state, index, "angle", angle );

        data.moveAngle  = angle;
    }

    setupParticleData( data );

    lua_pop( state, 1 );

    return data;
}

void CParticleSystem::setupParticleData( ParticleData& data ) {
    calcRange( data.moveVelocity,       data.moveVelocityRange );
    calcRange( data.moveAngle,          data.moveAngleRange );
    calcRange( data.moveAccel,          data.moveAccelRange );
    calcRange( data.moveDistance,       data.moveDistanceRange );
    calcRange( data.rotationVel,        data.rotationVelRange );
    calcRange( data.rotationAccel,      data.rotationAccelRange );
    calcRange( data.lifetime,           data.lifetimeRange );
    calcRange( data.scalingVel,         data.scalingVelRange );
    calcRange( data.opacityVelRange,    data.opacityVelRange );
}

void CParticleSystem::calcRange( float& value, float range ) {
    if( range != 0.0 ) {
        value += range - ( range * 2 ) * CRNG::percentRandom();
    }
}

void CParticleSystem::calcRange( int& value, int range ) {
    if( range != 0 ) {
        value = CRNG::iRandom( value - range, value + range );
    }
}
