#include "CEntity.h"

#include "CGame.h"
#include "CAiCompoment.h"
#include "Utility.h"

#include "iostream"
using namespace std;

CEntity::CEntity()
: m_type            ( EntityType::TYPE_ENTITY )
, m_ID              ( 0 )
, m_despawn         ( false )
, m_solid           ( false )
, m_collideable     ( true )
, m_aiComponent     ( nullptr )
, m_moveVelocity    ( 0 )
, m_moveAngle       ( 0 )
, m_soundEmitter    ( false )
, m_particleEffectID( 0 )
{
    for( size_t i = 0; i < FUNC_TOTAL; i++ ) {
        m_funcIDs[ i ] = 0;
    }
}

CEntity::~CEntity() {
    for( size_t i = 0; i < FUNC_TOTAL; i++ ) {
        if( m_funcIDs[ i ] != 0 ) {
            CGame::EntitySystem.removeLuaFunctions( m_funcIDs[ i ] );
        }
    }

    for( auto it = m_onEventFuncIds.begin(); it != m_onEventFuncIds.end(); it++ ) {
        CGame::EntitySystem.removeLuaFunctions( it->second );
    }
    m_onEventFuncIds.clear();

    if( m_aiComponent != nullptr ) {
        delete m_aiComponent;
        m_aiComponent = nullptr;
    }

    if( m_soundEmitter ) {
        CGame::AudioSystem.destroyEmitter( this );
    }

    CGame::WorldManager.ParticleSystem.removeEffect( m_particleEffectID );
}

void CEntity::handleMsg( CMessage* msg ) {
    onEvent( msg->getMsg() );
}

void CEntity::update( const sf::Time& delta ) {
    if( m_aiComponent != nullptr ) {
        m_aiComponent->update( delta );
    }

    doMove();

    updateAnimation( delta );

    m_moveSpeed.x   = 0;
    m_moveSpeed.y   = 0;

    if( m_funcIDs[ FUNC_ONUPDATE ] != 0 ) {
        onUpdate( delta.asSeconds() );
    }
}

void CEntity::draw( sf::RenderTarget& window, sf::RenderStates states ) const {
    sf::Transformable transformable;

    // Rounded position for correct drawing ( float position makes pixel flickering )
    transformable.setPosition( (int)( m_pos.x + 0.5 ), (int)( m_pos.y + 0.5 ) );

    states.transform *= transformable.getTransform();


    window.draw( m_animation, states );
}

EntityType CEntity::getType() const {
    return m_type;
}

void CEntity::setID( size_t ID ) {
    m_ID    = ID;
}

size_t CEntity::getID() const {
    return m_ID;
}

void CEntity::setDespawn( bool despawn ) {
    m_despawn = despawn;
}

bool CEntity::isDespawn() const {
    return m_despawn;
}

void CEntity::setSolid( bool solid ) {
    m_solid = solid;
}

bool CEntity::isSolid() const {
    return m_solid;
}

void CEntity::setCollideable( bool collideable ) {
    m_collideable = collideable;
}

bool CEntity::isCollideable() const {
    return m_collideable;
}

void CEntity::setMoveVelocity( float velocity ) {
    m_moveVelocity = velocity;
}

float CEntity::getMoveVelocity() const {
    return m_moveVelocity;
}

void CEntity::setMoveAngle( float angle ) {
    m_moveAngle = angle;
}

float CEntity::getMoveAngle() const {
    return m_moveAngle;
}

void CEntity::setMoveSpeed( const sf::Vector2f& speed ) {
    m_moveSpeed     = speed;
}

void CEntity::setAiComponent( CAiComponent* component ) {
    if( component == nullptr ) return;

    component->setParent( this );

    m_aiComponent   = component;
}

void CEntity::setFunctionID( LuaFunction type, size_t ID ) {
    m_funcIDs[ type ] = ID;
}

void CEntity::addEventFunction( std::string event, size_t ID ) {
    m_onEventFuncIds.insert( std::make_pair( event, ID ) );
}

size_t CEntity::getFunctionID( LuaFunction type ) const {
    return m_funcIDs[ type ];
}

void CEntity::delFunctionID( LuaFunction type ) {
    m_funcIDs[ type ] = 0;
}

void CEntity::onEvent( std::string event ) {
    auto found = m_onEventFuncIds.find( event );

    if( found == m_onEventFuncIds.end() ) return;

    CGame::EntitySystem.callLuaFunction( m_ID, found->second );
}

void CEntity::onUpdate( float delta ) {
    CGame::EntitySystem.callLuaFunctionFloat( m_ID, m_funcIDs[ FUNC_ONUPDATE ], 1, delta );
}

void CEntity::onHit() {
    CGame::EntitySystem.callLuaFunction( m_ID, m_funcIDs[ FUNC_ONHIT ] );
}

void CEntity::onInteraction() {
    playSound( EntitySounds::ONINTERACT );

    CGame::EntitySystem.callLuaFunction( m_ID, m_funcIDs[ FUNC_ONINTERACT ] );
}

void CEntity::onDamageTaken( size_t sourceID, int damage ) {
    CGame::EntitySystem.callLuaFunctionInt( m_ID, m_funcIDs[ FUNC_ONDAMAGETAKEN ], 2, sourceID, damage );
}

void CEntity::onDeath() {
    playSound( EntitySounds::ONDEATH );

    CGame::EntitySystem.callLuaFunction( m_ID, m_funcIDs[ FUNC_ONDEATH ] );
}

void CEntity::onDespawn() {
    playSound( EntitySounds::ONDESPAWN );

    CGame::EntitySystem.callLuaFunction( m_ID, m_funcIDs[ FUNC_ONDESPAWN ] );
}

void CEntity::setSound( EntitySounds type, const std::string& ID ) {
    // In case type is wrong
    #ifndef DEBUG
        if( type >= EntitySounds::TOTAL ) {
            std::cout << "[D]Warning: Wrong Entity sound type: " << type << std::endl;
        }
    #endif // DEBUG

    m_sounds[ type ]    = ID;
}

void CEntity::setSoundEmitter( bool hasEmitter ) {
    m_soundEmitter   = hasEmitter;
}

bool CEntity::hasSoundEmitter() const {
    return m_soundEmitter;
}

void CEntity::setTexture( const std::string& textureID ) {
    m_animation.setupTexture( textureID, m_size );
}

void CEntity::centerTexture() {
    m_animation.setCenterSprite( m_size );
}

void CEntity::setupAnimationFromLuaTable( lua_State* state, int index ) {
    m_animation.setupFromLuaTable( state, index, m_size );
}

CAnimation& CEntity::getAnimation() {
    return m_animation;
}

void CEntity::setParticleEffectID( size_t ID ) {
    m_particleEffectID = ID;
}

Direction CEntity::getFacingDir( float angle ) {
    float faceAngle  = Util::radToDegree( angle );
    faceAngle  += 180 + 45;

    if( faceAngle > 360 ) {
        faceAngle -= 360;
    }

    return (Direction)( faceAngle / 90 );
}

void CEntity::doMove() {
    // Store map size
    sf::Vector2f mapSize = CGame::WorldManager.getMapSize();

    // Move by x
    m_pos.x += m_moveSpeed.x;

    // Check if player went outside the map
    if( m_pos.x < 0 ) { m_pos.x = 0; }
    else if( m_pos.x + m_size.x > mapSize.x ) { m_pos.x = mapSize.x - m_size.x; }

    // Calculate collision box
    sf::FloatRect collBox   = m_collBox;
    collBox.left    = m_pos.x + m_collBox.left;
    collBox.top     = m_pos.y + m_collBox.top;

    // Get all nearby entities from spatial sytem
    C_Dimensionals entities = CGame::EntitySystem.getNearbySolid( this );

    /** Horizontal movement */

    if( m_collideable ) {
        for( auto it : entities ) {
//            if( it->isSolid() ) {
                sf::FloatRect entityBox = it->getCollBox();

                if( collBox.intersects( entityBox ) ) {
                    // Make sure collision side is correct. This prevents rare glitch
                    int collSide = getSideOfCollision( collBox, entityBox );

                    if( collSide == 2 || collSide == 3 ) {
                        // Find side of collision and adjust movement
                        float side[ 2 ];
                        side[ 0 ] = entityBox.left  + entityBox.width   - collBox.left;
                        side[ 1 ] = ( collBox.left  + collBox.width )   - entityBox.left;

                        if( side[ 0 ] < side[ 1 ] ) {
                            m_pos.x = entityBox.left  + entityBox.width     - m_collBox.left;
                        }
                        else {
                            m_pos.x = entityBox.left  - ( m_collBox.left    + m_collBox.width );
                        }
                    }
                } // If there is collision
//            } // If is solid
        } // Entity loop
    } // If is collideable

    // Move by y and calculate new collision box
    m_pos.y += m_moveSpeed.y;

    // Check if player went outside the map
    if( m_pos.y < 0 ) { m_pos.y = 0; }
    else if( m_pos.y + m_size.y > mapSize.y ) { m_pos.y = mapSize.y - m_size.y; }

    // Update collision box
    collBox.left  = m_pos.x + m_collBox.left;
    collBox.top   = m_pos.y + m_collBox.top;

    /** Vertical movement */

    if( m_collideable ) {
        for( auto it : entities ) {
//            if( it->isSolid() ) {
                sf::FloatRect entityBox = it->getCollBox();

                if( collBox.intersects( entityBox ) ) {
                    // Make sure collision side is correct. This prevents rare glitch
//                    int collSide = getSideOfCollision( collBox, entityBox );

//                    if( collSide == 0 || collSide == 1 ) {
                        // Find side of collision and adjust movement
                        float side[ 2 ];
                        side[ 0 ] = entityBox.top   + entityBox.height  - collBox.top;
                        side[ 1 ] = ( collBox.top   + collBox.height )  - entityBox.top;

                        if( side[ 0 ] < side[ 1 ] ) {
                            m_pos.y = entityBox.top   + entityBox.height    - m_collBox.top;
                        }
                        else {
                            m_pos.y = entityBox.top   - ( m_collBox.top     + m_collBox.height );
                        }
//                    }
                } // If there is collision
//            } // If is solid
        } // Entity loop
    } // If is collideable
}

void CEntity::updateAnimation( const sf::Time& delta ) {
    m_animation.update( delta );
}

void CEntity::updateDirOfAnimation( float* angle ) {
    Direction dirIndex = getFacingDir( angle == nullptr ? m_moveAngle : *angle );

    if( m_animation.getRowIndex() != dirIndex ) {
        m_animation.setRowIndex( dirIndex );
    }
}

void CEntity::playSound( EntitySounds type ) {
    // In case type is wrong
    #ifndef DEBUG
        if( type >= EntitySounds::TOTAL ) {
            std::cout << "[D]Warning: Wrong Entity sound type: " << type << std::endl;
        }
    #endif // DEBUG

    CGame::AudioSystem.playSoundRelative( m_sounds[ type ], CAudioSystem::EFFECT, getCenter() );
}

Direction CEntity::getSideOfCollision( const sf::FloatRect& first, const sf::FloatRect& second ) {
    float side[ 4 ];
    side[ 0 ] = second.left     + second.width      - first.left;
    side[ 1 ] = ( second.left   + first.width )     - second.left;
    side[ 2 ] = second.top      + second.height     - first.top;
    side[ 3 ] = ( first.top     + first.height )    - second.top;

    int collSide =  0;
    int smallest =  side[ 0 ];

    for( int i = 1; i < 3; i++ ) {
        if( side[ i ] < smallest ) {
            collSide = i;
        }
    }

    switch( collSide ) {
    case 0: return Direction::RIGHT;    break;
    case 1: return Direction::LEFT;     break;
    case 2: return Direction::DOWN;     break;
    case 3: return Direction::UP;       break;
    default: break;
    }

    // Just to prevent pointless warnings
    return Direction::RIGHT;
}
