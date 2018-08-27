#ifndef CENTITY_H
#define CENTITY_H

#include "CDimensional.h"
#include "CAnimation.h"
#include "CMessageListener.h"

class CAiComponent;
struct lua_State;

enum Direction {
    LEFT,
    UP,
    RIGHT,
    DOWN
};

enum LuaFunction {
    FUNC_ONEVENT,
    FUNC_ONUPDATE,
    FUNC_ONHIT,
    FUNC_ONINTERACT,
    FUNC_ONDAMAGETAKEN,
    FUNC_ONDEATH,
    FUNC_ONDESPAWN,
    FUNC_ONENEMYHIT,
    FUNC_TOTAL      // Keep last
};

enum EntityType {
    TYPE_ENTITY,
    TYPE_CHARACTER,
    TYPE_PROJECTILE,
};


class CEntity : public CDimensional, public CMessageListener {
public:
                        CEntity             ();
    virtual            ~CEntity             ();

    virtual void        handleMsg           ( CMessage* msg );

    virtual void        update              ( const sf::Time& delta );
    virtual void        draw                ( sf::RenderTarget& window, sf::RenderStates states ) const;

            /** Type */
            EntityType  getType             () const;

            /** ID */
            void        setID               ( size_t ID );
            size_t      getID               () const;

            /** Despawn */
            void        setDespawn          ( bool despawn = true );
            bool        isDespawn           () const;

            /** Solid */
            void        setSolid            ( bool solid = true );
            bool        isSolid             () const;

            /** Collideable */
            void        setCollideable      ( bool collideable = true );
            bool        isCollideable       () const;

            /** Movement velocity */
            void        setMoveVelocity     ( float velocity );
            float       getMoveVelocity     () const;

            /** Movement angle*/
            void        setMoveAngle        ( float angle );
            float       getMoveAngle        () const;

            /** Movement speed */
            void        setMoveSpeed        ( const sf::Vector2f& speed );

            void        setAiComponent      ( CAiComponent* component );

            /** Lua Function */
            void        setFunctionID       ( LuaFunction type, size_t ID );
            void        addEventFunction    ( std::string event, size_t ID );
            size_t      getFunctionID       ( LuaFunction type ) const;
            void        delFunctionID       ( LuaFunction type );

            void        onEvent             ( std::string event );
            void        onUpdate            ( float delta );
            void        onHit               ();
            void        onInteraction       ();
            void        onDamageTaken       ( size_t sourceID, int damage );
            void        onDeath             ();
            void        onDespawn           ();

            /** Animation manipulation */
            void        setTexture          ( const std::string& textureID );
            void        centerTexture       ();
            void        setupAnimationFromLuaTable ( lua_State* state, int index );

            CAnimation& getAnimation        ();

            /** Particle effect */
            void        setParticleEffectID ( size_t ID );

            Direction   getFacingDir        ( float angle );

protected:
    virtual void        doMove              ();

    virtual void        updateAnimation     ( const sf::Time& delta );

    virtual void        updateDirOfAnimation( float* angle = nullptr );

            // Note: Calculates side correctly, just returns wrong number
            Direction   getSideOfCollision  ( const sf::FloatRect& first, const sf::FloatRect& second );

protected:
    EntityType          m_type;
    size_t              m_ID;
    bool                m_despawn;

    CAnimation          m_animation;

    bool                m_solid;
    bool                m_collideable;

    CAiComponent*       m_aiComponent;

    float               m_moveVelocity;
    float               m_moveAngle;
    sf::Vector2f        m_moveSpeed;

    size_t              m_funcIDs[ FUNC_TOTAL ];
    std::map<std::string,size_t>    m_onEventFuncIds;

    size_t              m_particleEffectID;
};

#endif // CENTITY_H
