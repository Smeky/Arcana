#include "CEntitySystem.h"

#include "cassert"
#include <sstream>
#include "lua.hpp"

#include "CGame.h"
#include "CMap.h"
#include "CTile.h"
#include "CRNG.h"
#include "CPlayer.h"
#include "CCharacter.h"
#include "CharacterStats.h"
#include "CharacterFaction.h"
#include "CProjectile.h"
#include "CAiCompoment.h"
#include "CAiCharComponent.h"
#include "Utility.h"
#include <LuaWrapper.h>

#include "iostream"
using namespace std;

const std::string errorArgc = "Wrong amount of arguments.";

CEntitySystem::CEntitySystem()
: m_IDCounter           ( 1 )
, m_funcIDCounter       ( 1 )
, m_debugDispEntityBox  ( false )
, m_debugDispEntityStat ( false )
, m_debugEntityStat     ( nullptr )
{}

CEntitySystem::~CEntitySystem() {
    close();
}

void CEntitySystem::close() {
    while( !m_enemies.empty() ) delete m_enemies.back(), m_enemies.pop_back();
    while( !m_objects.empty() ) delete m_objects.back(), m_objects.back() = nullptr, m_objects.pop_back();
    while( !m_projectiles.empty() ) delete m_projectiles.back(), m_projectiles.pop_back();


    delete m_player;
    m_player = nullptr;

    m_luaFunctions.clear();

    m_IDCounter     = 1;
    m_funcIDCounter = 1;

    /** debug */
    m_debugEntityStat   = nullptr;
}

void CEntitySystem::cleanUp() {
    while( !m_enemies.empty() ) delete m_enemies.back(), m_enemies.pop_back();
    while( !m_objects.empty() ) delete m_objects.back(), m_objects.pop_back();
    while( !m_projectiles.empty() ) delete m_projectiles.back(), m_projectiles.pop_back();
}

void CEntitySystem::handleInput( sf::Event event ) {
    if( m_debugDispEntityStat ) {
        if( event.type == sf::Event::MouseButtonReleased ) {
            if( event.mouseButton.button == sf::Mouse::Left ) {
                sf::Vector2i mousePos = sf::Mouse::getPosition( CGame::GameWindow );
                mousePos.x += CGame::WorldManager.Camera.getPos().x;
                mousePos.y += CGame::WorldManager.Camera.getPos().y;

                m_debugEntityStat = getEntityByMouse( mousePos );
            }
        }
    }
}

void CEntitySystem::update( const sf::Time& delta ) {
    addToSpatialSystem();

    if( m_player != nullptr ) {
        m_player->update( delta );
    }

    for( auto it = m_enemies.begin(); it != m_enemies.end(); ) {
        if( (*it)->isDespawn() ) {
            (*it)->onDespawn();

            delete (*it);

            it = m_enemies.erase( it );
        }
        else {
            (*it)->update( delta );

            it++;
        }
    }

    for( auto it = m_objects.begin(); it != m_objects.end(); ) {
        if( (*it)->isDespawn() ) {
            (*it)->onDespawn();

            delete (*it);

            it = m_objects.erase( it );
        }
        else {
            (*it)->update( delta );

            it++;
        }
    }

    for( auto it = m_projectiles.begin(); it != m_projectiles.end(); ) {
        if( (*it)->isDespawn() ) {
            (*it)->onDespawn();

            delete (*it);

            it = m_projectiles.erase( it );
        }
        else {
            (*it)->update( delta );

            it++;
        }
    }

    addToZBuffer();
}

void CEntitySystem::render( sf::RenderTarget& window, sf::RenderStates states ) {
    if( m_debugDispEntityBox ) {
        displayEntityBox( window, states );
    }
    if( m_debugDispEntityStat ) {
        displayEntityStat( window, states );
    }
}

C_Dimensionals CEntitySystem::getNearbySolid( const CDimensional* dim ) {
    CSpatialSystem::Cell    entities    = CGame::SpatialSystem.getNearby( dim );
    C_Dimensionals          tiles       = CGame::WorldManager.getCurrentlyUsedMap()->getNearby( dim );

    C_Dimensionals          nearby;

    for( auto it : entities ) {
        if( it->isSolid() ) {
            nearby.push_back( it );
        }
    }

    for( auto it : tiles ) {
        nearby.push_back( it );
    }

    return nearby;
}

bool CEntitySystem::checkSphereForColl( const sf::Vector2f& pos, float radius ) {
    CDimensional dim;

    dim.setPos( pos.x - radius, pos.y - radius );
    dim.setSize( radius * 2, radius * 2 );

    CSpatialSystem::Cell    entities    = CGame::SpatialSystem.getNearby( &dim );

    for( auto it : entities ) {
        if( it->getBox().intersects( dim.getBox() ) ) {
            return true;
        }
    }

    C_Dimensionals tiles       = CGame::WorldManager.getCurrentlyUsedMap()->getNearby( &dim );

    for( auto it : tiles ) {
        if( it->getBox().intersects( dim.getBox() ) ) {
            return true;
        }
    }

    return false;
}

bool CEntitySystem::isPosFree( const sf::Vector2f& pos, const sf::Vector2f& size ) {
    CDimensional dim;

    dim.setPos( pos );
    dim.setSize( size );

    for( auto it : CGame::SpatialSystem.getNearby( &dim ) ) {
        if( it->getBox().intersects( dim.getBox() ) ) {
            return false;
        }
    }

    for( auto it : CGame::WorldManager.getCurrentlyUsedMap()->getNearby( &dim ) ) {
        if( it->getBox().intersects( dim.getBox() ) ) {
            return false;
        }
    }

    return true;
}

void CEntitySystem::attackTarget( size_t sourceID, size_t targetID, int damage, DamageType type ) {
    if( m_player->getID() == targetID ) {
        m_player->takeDamage( sourceID, damage, type );
        m_player->onDamageTaken( sourceID, damage );

        if( m_player->isDead() ) {
            m_player->onDeath();
        }

        return;
    }

    for( auto it : m_enemies ) {
        if( it->getID() == targetID ) {
            if( !it->isDead() ) {
                it->takeDamage( sourceID, damage, type );
                it->onDamageTaken( sourceID, damage );

                if( it->isDead() ) {
                    m_player->addExp( it->getExpOnKill() );
                    it->onDeath();

                    if( it->hasLootTable() ) {
                        CGame::WorldManager.LootSystem.createLoot( it );
                    }
                }
            }


            return;
        }
    }
}

void CEntitySystem::attackTarget( CCharacter* target, CCharacter* source, int damage, DamageType type ) {
    if( target == nullptr || target->getType() != TYPE_CHARACTER || damage == 0 ) return;

    // TEMP
    size_t sourceID = 0;
    if( source != nullptr ) {
        sourceID = source->getID();
    }

    if( m_player == target ) {
        m_player->takeDamage( sourceID, damage, type );
        m_player->onDamageTaken( sourceID, damage );

        if( m_player->isDead() ) {
            m_player->onDeath();
        }

        return;
    }

    if( !target->isDead() ) {
        target->takeDamage( sourceID, damage, type );
        target->onDamageTaken( sourceID, damage );

        if( target->isDead() ) {
            m_player->addExp( target->getExpOnKill() );
            target->onDeath();

            if( target->hasLootTable() ) {
                CGame::WorldManager.LootSystem.createLoot( target );
            }
        }
    }
}

void CEntitySystem::attackTarget( size_t sourceID, size_t targetID, const ProjectileStats& stats ) {
    DamageType type;

    int damage = calculateDamage( stats.dmgMin, stats.dmgMax, stats.critChance, stats.critMultip, type );

    if( damage != 0 ) {
        handleAttack( targetID, sourceID, damage, type );
    }
}

void CEntitySystem::playerInteract() {
    // Maximum distance for interaction
    float maximum   = 150;
    float closest   = maximum;

    CEntity* object = nullptr;

    for( auto it : m_objects ) {
        float distance = Util::lenghtByVectors( m_player->getCenter(), it->getCenter() );

        if( distance < maximum && distance < closest ) {
            object  = it;

            closest = distance;
        }
    }

    if( object != nullptr ) {
        object->onInteraction();
    }
}

void CEntitySystem::interactWithObject( size_t ID ) {
    for( auto it : m_objects ) {
        if( it->getID() == ID ) {
            it->onInteraction();

            return;
        }
    }
}

int CEntitySystem::calculateDamage( int min, int max, float critChance, float critMultip, DamageType& type ) {
    if( min == 0 || max == 0 ) return 0;

    int damage = CRNG::iRandom( min, max );

    if( critChance != 0.0 && critMultip != 0.0 ) {
        if( CRNG::percentRandom() * 100 < critChance ) {
            damage += 1 + (int)( damage * critMultip + 0.5 );

            type = DamageType::DMG_CRIT;
        }
        else {
            type = DamageType::DMG_NORMAL;
        }
    }

    return damage;
}

bool CEntitySystem::isCharacter( size_t ID ) {
    if( m_player->getID() == ID ) {
        return true;
    }

    for( auto it : m_enemies ) {
        if( it->getID() == ID ) {
            return true;
        }
    }

    return false;
}

bool CEntitySystem::compareFaction( CharacterFaction first, size_t second ) {
    if( m_player->getID() == second ) {
        if( m_player->getFaction() == first ) {
            return true;
        }
        else {
            return false;
        }
    }

    for( auto it : m_enemies ) {
        if( it->getID() == second ) {
            if( it->getFaction() == first ) {
                return true;
            }
            else {
                return false;
            }
        }
    }

    return false;
}

void CEntitySystem::despawnEntity( size_t ID ) {
    for( auto it : m_enemies ) {
        if( it->getID() == ID ) {
            it->setDespawn();

            return;
        }
    }

    for( auto it : m_objects ) {
        if( it->getID() == ID ) {
            it->setDespawn();

            return;
        }
    }

    for( auto it : m_projectiles ) {
        if( it->getID() == ID ) {
            it->setDespawn();

            return;
        }
    }
}

void CEntitySystem::despawnEnemy( size_t ID ) {
    for( auto it : m_enemies ) {
        if( it->getID() == ID ) {
            it->setDespawn();

            return;
        }
    }
}

void CEntitySystem::despawnObject( size_t ID ) {
    for( auto it : m_objects ) {
        if( it->getID() == ID ) {
            it->setDespawn();

            return;
        }
    }
}

void CEntitySystem::despawnProjectile( size_t ID ) {
    for( auto it : m_projectiles ) {
        if( it->getID() == ID ) {
            it->setDespawn();

            return;
        }
    }
}

CPlayer* CEntitySystem::getPlayer() {
    return m_player;
}

CInventory& CEntitySystem::getInventory() {
    return m_player->getInventory();
}

sf::Vector2f CEntitySystem::getPlayerPos() {
    return m_player != nullptr ? m_player->getPos() : sf::Vector2f();
}

sf::Vector2f CEntitySystem::getPlayerSize() {
    return m_player != nullptr ? m_player->getSize() : sf::Vector2f();
}

sf::Vector2f CEntitySystem::getPlayerCenter() {
    return m_player != nullptr ? m_player->getCenter() : sf::Vector2f();
}

void CEntitySystem::addAbilityToPlayer( CAbility* ability ) {
    m_player->addAbility( ability );
}

CCharacter* CEntitySystem::getCharacter( size_t ID ) {
    if( m_player != nullptr ) {
        if( m_player->getID() == ID ) {
            return m_player;
        }
    }

    for( auto it : m_enemies ) {
        if( it->getID() == ID ) {
            return it;
        }
    }

    return nullptr;
}

CharacterStats CEntitySystem::getCharacterStats( size_t ID ) {
    CCharacter* character = getCharacter( ID );

    if( character != nullptr ) {
        return character->getModStats();
    }

    return CharacterStats();
}

CharacterFaction CEntitySystem::getCharacterFaction( size_t ID ) {
    CCharacter* character = getCharacter( ID );

    if( character != nullptr ) {
        return character->getFaction();
    }

    return CharacterFaction::FACTION_NEUTRAL;
}

void CEntitySystem::setEntityPos( size_t ID, const sf::Vector2f& pos ) {
    CEntity* entity = getEntity( ID );

    if( entity != nullptr ) {
        entity->setPos( pos );
    }
}

void CEntitySystem::setEntityCenter( size_t ID, const sf::Vector2f& center ) {
    CEntity* entity = getEntity( ID );

    if( entity != nullptr ) {
        entity->setCenter( center );
    }
}

void CEntitySystem::setEntitySize( size_t ID, const sf::Vector2f& size ) {
    CEntity* entity = getEntity( ID );

    if( entity != nullptr ) {
        entity->setSize( size );
    }
}

sf::Vector2f CEntitySystem::getEntityPos( size_t ID ) {
    CEntity* entity = getEntity( ID );

    if( entity != nullptr ) {
        return entity->getPos();
    }

    // Return empty vector ( no entity was found )
    return sf::Vector2f();
}

sf::Vector2f CEntitySystem::getEntityCenter( size_t ID ) {
    CEntity* entity = getEntity( ID );

    if( entity != nullptr ) {
        return entity->getCenter();
    }

    // Return empty vector ( no entity was found )
    return sf::Vector2f();
}

sf::Vector2f CEntitySystem::getEntitySize( size_t ID ) {
    CEntity* entity = getEntity( ID );

    if( entity != nullptr ) {
        return entity->getSize();
    }

    // Return empty vector ( no entity was found )
    return sf::Vector2f();
}

void CEntitySystem::setEntityPosRandom( CEntity* entity ) {
    if( entity != nullptr ) {
        sf::Vector2f mapSize    = CGame::WorldManager.getCurrentlyUsedMap()->getSize();

        sf::Vector2f size = entity->getSize();
        sf::Vector2f pos;

        for( size_t i = 0; i < 100; i++ ) {
            pos.x   = CRNG::iRandom( 0, mapSize.x );
            pos.y   = CRNG::iRandom( 0, mapSize.y );

            if( checkIfPosIsFree( sf::FloatRect( pos.x, pos.y, size.x, size.y ) ) ) {
                entity->setPos( pos );

                return;
            }
        }

        #ifdef DEBUG
            std::cout << "[D]Warning: Set random entity position failed." << std::endl;
        #endif // DEBUG
    }
    else {
        std::cout << "Warning: Unable to set random entity position. Got nullptr" << std::endl;
    }
}

CEntity* CEntitySystem::getEntityByID( size_t ID ) {
    return getEntity( ID );
}

size_t CEntitySystem::playerFromLuaTable( lua_State* state, int index ) {
    assert( lua_istable( state, index ) );

    m_player = new CPlayer();

    loadBaseEntityData( state, index, m_player );
    loadCharacterData( state, index, m_player );

    return CGame::EntitySystem.registerPlayer();
}

size_t CEntitySystem::enemyFromLuaTable( lua_State* state, int index ) {
    assert( lua_istable( state, index ) );

    CCharacter* enemy = new CCharacter();

    loadBaseEntityData( state, index, enemy );
    loadCharacterData( state, index, enemy );
    loadCharacterData( state, index, enemy );

    enemy->setAiCharComponent( new CAiRangerHuman() );

    return CGame::EntitySystem.registerEnemy( enemy );
}

size_t CEntitySystem::objectFromLuaTable( lua_State* state, int index ) {
    assert( lua_istable( state, index ) );

    CEntity* object = new CEntity();

    loadBaseEntityData( state, index, object );
    size_t ID = CGame::EntitySystem.registerObject( object );

    lua_getfield( state, index, "onCreation" );
    if( lua_isfunction( state, - 1 ) ) {
        lua_pushinteger( state, ID );

        // Call the function and store the result
        int result = lua_pcall( state, 1, LUA_MULTRET, 0 );

        // If there was a problem, print it
        if( result != LUA_OK ) {
            std::cout << "Error: " << lua_tostring( state, - 1 ) << std::endl;
        }
    }
    else {
        lua_pop( state, 1 );
    }

    return ID;
}

size_t CEntitySystem::bulletFromLuatable( lua_State* state, int index ) {
    assert( lua_istable( state, index ) );

    CProjectile* projectile = new CProjectile();

    loadBaseEntityData( state, index, projectile );
    loadProjectileData( state, index, projectile );

    // Load collision circle ( used instead of collision box )
    lua_getfield( state, index, "collrad" );
    projectile->setCollCircle( Util::circleFromLuaTable( state, lua_gettop( state ) ) );

    // Center projectile sprite
    projectile->getAnimation().setCenterOrigin();

    return CGame::EntitySystem.registerProjectile( projectile );
}

CAiComponent* CEntitySystem::loadAiFromLauTable( lua_State* state, int index ) {
    if( !lua_istable( state, index ) ) return nullptr;

    CAiComponent* comp = nullptr;

    std::string ID;
    lWrapper::stringFromLuaTable( state, index, "ID", ID );

    if( ID == "LoopMovement" ) {
        comp = new CAiLoopMovement();
    }

    if( comp != nullptr ) {
        lua_getfield( state, index, "data" );

        comp->loadDataFromLua( state, lua_gettop( state ) );

        lua_pop( state, 1 );
    }

    return comp;
}

size_t CEntitySystem::registerPlayer() {
    size_t ID = getNewID();

    m_player->setID( ID );

    // Used by widget in Gui to display players stats ( init setup )
    CGame::MessageSystem.broadcast( new CMessage( "PlayerStatUpdate" ) );

    return ID;
}

size_t CEntitySystem::registerEnemy( CCharacter* enemy ) {
    size_t ID = getNewID();

    enemy->setID( ID );

    m_enemies.push_back( enemy );

    return ID;
}

size_t CEntitySystem::registerObject( CEntity* object ) {
    size_t ID = getNewID();

    object->setID( ID );

    m_objects.push_back( object );

    return ID;
}

size_t CEntitySystem::registerProjectile( CProjectile* projectile ) {
    size_t ID = getNewID();

    projectile->setID( ID );

    m_projectiles.push_back( projectile );

    return ID;
}

void CEntitySystem::applyEffect( size_t characterID, CEffect* effect ) {
    CCharacter* character = getCharacter( characterID );

    if( character != nullptr ) {
        character->addEffect( effect );
    }
}

void CEntitySystem::setDisplayEntityBox() {
    m_debugDispEntityBox = !m_debugDispEntityBox;
}

void CEntitySystem::setDisplayEntityStat() {
    m_debugDispEntityStat = !m_debugDispEntityStat;
}

void CEntitySystem::callLuaFunction( size_t entityID, size_t functionID ) {
    // ID 0 is default value, not used
    if( functionID == 0 ) return;

    // Store main Lua state
    lua_State* state = CGame::ScriptSystem.getState();

    if( !m_luaFunctions[ functionID ].empty() ) {
        for( int i : m_luaFunctions[ functionID ] ) {
            // Get function by it's ID
            lua_rawgeti( state, LUA_REGISTRYINDEX, i );

            // Check if function was found
            if( lua_isfunction( state, - 1 ) ) {
                // Put entity ID as function argument
                lua_pushinteger( state, entityID );

                // Call the function and store the result
                int result = lua_pcall( state, 1, LUA_MULTRET, 0 );

                // If there was a problem, print it
                if( result != LUA_OK ) {
                    std::cout << "Error: " << lua_tostring( state, - 1 ) << std::endl;
                }
            }
            else {
                // If function was not found, clean up
                lua_pop( state, 1 );
            }
        }
    }
}

void CEntitySystem::callLuaFunctionLUD( size_t entityID, size_t functionID, void* lightUserData ) {
    // ID 0 is default value, not used
    if( functionID == 0 ) return;

    // Store main Lua state
    lua_State* state = CGame::ScriptSystem.getState();

    if( !m_luaFunctions[ functionID ].empty() ) {
        for( int i : m_luaFunctions[ functionID ] ) {
            // Get function by it's ID
            lua_rawgeti( state, LUA_REGISTRYINDEX, i );

            // Check if function was found
            if( lua_isfunction( state, - 1 ) ) {
                // Put entity ID as function argument
                lua_pushinteger( state, entityID );
                lua_pushlightuserdata( state, lightUserData );

                // Call the function and store the result
                int result = lua_pcall( state, 2, LUA_MULTRET, 0 );

                // If there was a problem, print it
                if( result != LUA_OK ) {
                    std::cout << "Error: " << lua_tostring( state, - 1 ) << std::endl;
                }
            }
            else {
                // If function was not found, clean up
                lua_pop( state, 1 );
            }
        }
    }
}

void CEntitySystem::callLuaFunctionInt( size_t entityID, size_t functionID, int argc, ... ) {
    if( functionID == 0 ) return;

    va_list valist;
    va_start( valist, argc );

    lua_State* state = CGame::ScriptSystem.getState();

    if( !m_luaFunctions[ functionID ].empty() ) {
        for( int i : m_luaFunctions[ functionID ] ) {
            lua_rawgeti( state, LUA_REGISTRYINDEX, i );

            if( lua_isfunction( state, - 1 ) ) {
                // Push entity ID as first argument
                lua_pushinteger( state, entityID );

                for( int j = 0; j < argc; j++ ) {
                    int value = va_arg( valist, int );

                    lua_pushinteger( state, value );
                }

                int result = lua_pcall( state, argc + 1, LUA_MULTRET, 0 );

                if( result != LUA_OK ) {
                    std::cout << "Error: " << lua_tostring( state, - 1 ) << std::endl;
                }
            }
            else {
                lua_pop( state, 1 );
            }
        }
    }

    va_end( valist );
}

void CEntitySystem::callLuaFunctionFloat( size_t entityID, size_t functionID, int argc, ... ) {
    if( functionID == 0 ) return;

    va_list valist;
    va_start( valist, argc );

    lua_State* state = CGame::ScriptSystem.getState();

    if( !m_luaFunctions[ functionID ].empty() ) {
        for( int i : m_luaFunctions[ functionID ] ) {
            lua_rawgeti( state, LUA_REGISTRYINDEX, i );

            if( lua_isfunction( state, - 1 ) ) {
                // Push entity ID as first argument
                lua_pushnumber( state, entityID );

                for( int j = 0; j < argc; j++ ) {
                    double value = va_arg( valist, double );

                    lua_pushnumber( state, value );
                }

                int result = lua_pcall( state, argc + 1, LUA_MULTRET, 0 );

                if( result != LUA_OK ) {
                    std::cout << "Error: " << lua_tostring( state, - 1 ) << std::endl;
                }
            }
            else {
                lua_pop( state, 1 );
            }
        }
    }

    va_end( valist );
}

/** LUA FUNCTIONS */

int CEntitySystem::luaGetLightUserData( lua_State* state ) {
    int argc = lua_gettop( state );

    CEntity* entity = nullptr;

    if( argc == 2 ) {
        size_t ID = lua_tointeger( state, argc );

        entity = CGame::EntitySystem.getEntity( ID );
    }
    else {
        std::cout << "Error: Unable to get entity light user data. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    lua_pushlightuserdata( state, entity );

    return 1;
}

int CEntitySystem::luaSetPos( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::Vector2f    pos;
    CDimensional*   dim = nullptr;

    if( argc == 3 ) {
        dim     = (CDimensional*)lua_touserdata( state, argc - 1 );
        pos     = Util::vector2fFromLuaTable( state, argc );
    }
    else if( argc == 4 ) {
        dim     = (CDimensional*)lua_touserdata( state, argc - 2 );
        pos.x   = lua_tonumber( state, - 2 );
        pos.y   = lua_tonumber( state, - 1 );
    }

    if( dim != nullptr ) {
        dim->setPos( pos );
    }

    lua_pop( state, argc );

    return 0;
}

int CEntitySystem::luaSetCenter( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::Vector2f    pos;
    CDimensional*   dim = nullptr;

    if( argc == 3 ) {
        dim     = (CDimensional*)lua_touserdata( state, argc - 1 );
        pos     = Util::vector2fFromLuaTable( state, argc );
    }
    else if( argc == 4 ) {
        dim     = (CDimensional*)lua_touserdata( state, argc - 2 );
        pos.x   = lua_tonumber( state, - 2 );
        pos.y   = lua_tonumber( state, - 1 );
    }

    if( dim != nullptr ) {
        dim->setCenter( pos );
    }

    lua_pop( state, argc );

    return 0;
}

int CEntitySystem::luaSetSize( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::Vector2f    size;
    CDimensional*   dim     = nullptr;

    if( argc == 3 ) {
        dim     = (CDimensional*)lua_touserdata( state, argc - 1 );
        size    = Util::vector2fFromLuaTable( state, argc );
    }
    else if( argc == 4 ) {
        dim     = (CDimensional*)lua_touserdata( state, argc - 2 );
        size.x  = lua_tonumber( state, argc - 1 );
        size.y  = lua_tonumber( state, argc );
    }

    if( dim != nullptr ) {
        dim->setSize( size );
    }

    lua_pop( state, argc );

    return 0;
}

int CEntitySystem::luaSetPosSize( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::Vector2f    posSize;
    CDimensional*   dim = nullptr;

    if( argc == 3 ) {
        dim         = (CDimensional*)lua_touserdata( state, argc - 1 );
        posSize     = Util::vector2fFromLuaTable( state, argc );
    }
    else if( argc == 4 ) {
        dim     = (CDimensional*)lua_touserdata( state, argc - 2 );
        posSize.x   = lua_tonumber( state, - 2 );
        posSize.y   = lua_tonumber( state, - 1 );
    }

    if( dim != nullptr ) {
        dim->setPosSize( posSize );
    }

    lua_pop( state, argc );

    return 0;
}

int CEntitySystem::luaSetPosRandom( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc == 2 ) {
        if( lua_islightuserdata( state, argc ) ) {
            CEntity* entity = (CEntity*)lua_touserdata( state, argc );

            CGame::EntitySystem.setEntityPosRandom( entity );
        }
        else {
            std::cout << "Error: Unable to set random position. Light user data expected" << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to set random position. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    return 0;
}

int CEntitySystem::luaGetPos( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::Vector2f pos;

    if( argc != 2 ) {
        std::cout << "Error: Unable to return entity position. Wrong amount of arguments" << std::endl;
    }
    else {
        CDimensional* dim = (CDimensional*)lua_touserdata( state, argc );

        if( dim != nullptr ) {
            pos = dim->getPos();
        }
    }

    lua_pop( state, argc );

    lua_pushnumber( state, pos.x );
    lua_pushnumber( state, pos.y );

    return 2;
}

int CEntitySystem::luaGetCenter( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::Vector2f pos;

    if( argc != 2 ) {
        std::cout << "Error: Unable to return entity center. Wrong amount of arguments" << std::endl;
    }
    else {
        CDimensional* dim = (CDimensional*)lua_touserdata( state, argc );

        if( dim != nullptr ) {
            pos = dim->getCenter();
        }
    }

    lua_pop( state, argc );

    lua_pushnumber( state, pos.x );
    lua_pushnumber( state, pos.y );

    return 2;
}

int CEntitySystem::luaGetSize( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::Vector2f size;

    if( argc != 2 ) {
        std::cout << "Error: Unable to return element position. Wrong amount of arguments" << std::endl;
    }
    else {
        CDimensional* dim = (CDimensional*)lua_touserdata( state, argc );

        if( dim != nullptr ) {
            size = dim->getSize();
        }
    }

    lua_pop( state, argc );

    lua_pushnumber( state, size.x );
    lua_pushnumber( state, size.y );

    return 2;
}

int CEntitySystem::luaGetPosSize( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::Vector2f posSize;

    if( argc != 2 ) {
        std::cout << "Error: Unable to return element position. Wrong amount of arguments" << std::endl;
    }
    else {
        CDimensional* dim = (CDimensional*)lua_touserdata( state, argc );

        if( dim != nullptr ) {
            posSize = dim->getPosSize();
        }
    }

    lua_pop( state, argc );

    lua_pushnumber( state, posSize.x );
    lua_pushnumber( state, posSize.y );

    return 2;
}

int CEntitySystem::luaSetClip( lua_State* state ) {
    int argc = lua_gettop( state );

    CEntity*        entity  = nullptr;
    sf::IntRect     clip;

    if( argc == 3 ) {
        entity      = (CEntity*)lua_touserdata( state, argc - 1 );
        clip        = Util::convertRect<int>( Util::floatrectFromLuaTable( state, argc ) );
    }
    else if( argc == 6 ) {
        entity      = (CEntity*)lua_touserdata( state, argc - 4 );
        clip.left   = lua_tointeger( state, argc - 3 );
        clip.top    = lua_tointeger( state, argc - 2 );
        clip.width  = lua_tointeger( state, argc - 1 );
        clip.height = lua_tointeger( state, argc );
    }
    else {
        std::cout << "Error: Unable to set clip. Wrong amount of arguments" << std::endl;
    }

    if( entity != nullptr ) {
        entity->getAnimation().setBaseClip( clip );
    }

    lua_pop( state, argc );

    return 0;
}

int CEntitySystem::luaGetClip( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::IntRect box;

    if( argc == 2 ) {
        CEntity* entity = (CEntity*)lua_touserdata( state, argc );

        box = entity->getAnimation().getSprite().getClip();
    }
    else {
        std::cout << "Error: Unable to get clip. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    lua_pushnumber( state, box.left );
    lua_pushnumber( state, box.top );
    lua_pushnumber( state, box.width );
    lua_pushnumber( state, box.height );

    return 4;
}

int CEntitySystem::luaGetHealth( lua_State* state ) {
    int argc = lua_gettop( state );

    int maxHealth   = 0;
    int curHealth   = 0;

    if( argc == 2 ) {
        if( lua_islightuserdata( state, argc ) ) {
            CCharacter* character = (CCharacter*)lua_touserdata( state, argc );

            if( character != nullptr ) {
                maxHealth   = character->getMaximumHealth();
                curHealth   = character->getCurrentHealth();
            }
        }
        else {
            std::cout << "Error: Unable to get health. Light user data is missing" << std::cout;
        }
    }
    else {
        std::cout << "Error: Unable to get health. Wrong amount of arguments" << std::cout;
    }

    lua_pop( state, argc );

    lua_pushinteger( state, maxHealth );
    lua_pushinteger( state, curHealth );

    return 2;
}

int CEntitySystem::luaGetResource( lua_State* state ) {
    int argc = lua_gettop( state );

    int maxResource     = 0;
    int curResource     = 0;

    if( argc == 2 ) {
        if( lua_islightuserdata( state, argc ) ) {
            CCharacter* character = (CCharacter*)lua_touserdata( state, argc );

            if( character != nullptr ) {
                maxResource = character->getMaximumResource();
                curResource = character->getCurrentResource();
            }
        }
        else {
            std::cout << "Error: Unable to get resource. Light user data is missing" << std::cout;
        }
    }
    else {
        std::cout << "Error: Unable to get resource. Wrong amount of arguments" << std::cout;
    }

    lua_pop( state, argc );

    lua_pushinteger( state, maxResource );
    lua_pushinteger( state, curResource );

    return 2;
}

int CEntitySystem::luaGetExperience( lua_State* state ) {
    int argc = lua_gettop( state );

    int maxExperience   = 0;
    int curExperience   = 0;

    if( argc == 2 ) {
        if( lua_islightuserdata( state, argc ) ) {
            CCharacter* character = (CCharacter*)lua_touserdata( state, argc );

            if( character != nullptr ) {
                maxExperience = character->getExpToLevel();
                curExperience = character->getExp();
            }
        }
        else {
            std::cout << "Error: Unable to get experience. Expected Light user data" << std::cout;
        }
    }
    else {
        std::cout << "Error: Unable to get experience. Wrong amount of arguments" << std::cout;
    }

    lua_pop( state, argc );

    lua_pushinteger( state, maxExperience );
    lua_pushinteger( state, curExperience );

    return 2;
}

int CEntitySystem::luaAddAbility( lua_State* state ) {
    int argc = lua_gettop( state );

    int index = - 1;

    if( argc == 3 ) {
        if( lua_islightuserdata( state, argc - 1  ) ) {
            CCharacter* character = (CCharacter*)lua_touserdata( state, argc - 1 );

            if( character != nullptr ) {
                index = character->addAbility( CGame::AbilityManager.createFromLua( state, argc ) );
            }
            else {
                std::cout << "Error: Unable to add ability to character. Empty pointer" << std::endl;
            }
        }
        else {
            std::cout << "Error: Unable to add ability to character. Light user data is missing" << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to add ability to character. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    lua_pushinteger( state, index );

    return 1;
}

int CEntitySystem::luaSetAbility( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc == 4 ) {
        if( lua_islightuserdata( state, argc - 2  ) ) {
            CCharacter* character = (CCharacter*)lua_touserdata( state, argc - 2 );

            if( character != nullptr ) {
                size_t index = lua_tointeger( state, argc - 1 );

                character->setAbility( index, CGame::AbilityManager.createFromLua( state, argc ) );
            }
            else {
                std::cout << "Error: Unable to add ability to character. Empty pointer" << std::endl;
            }
        }
        else {
            std::cout << "Error: Unable to add ability to character. Light user data is missing" << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to add ability to character. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    return 0;
}

int CEntitySystem::luaGetAbility( lua_State* state ) {
    int argc = lua_gettop( state );

    const CAbility* ability = nullptr;

    if( argc == 3 ) {
        if( lua_islightuserdata( state, argc - 1 ) ) {
            CCharacter* character = (CCharacter*)lua_touserdata( state, argc - 1 );

            if( character != nullptr ) {
                size_t index = lua_tointeger( state, argc );

                ability = character->getAbility( index );
            }
            else {
                std::cout << "Error: Unable to get ability from character. Empty pointer" << std::endl;
            }
        }
        else {
            std::cout << "Error: Unable to get ability from character. Light user data is missing" << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to get ability from character. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    if( ability != nullptr ) {
        CGame::AbilityManager.abilityToTable( state, ability );
    }

    return 1;
}

int CEntitySystem::luaGetFaction( lua_State* state ) {
    int argc = lua_gettop( state );

    CharacterFaction faction    = CharacterFaction::FACTION_NEUTRAL;

    if( argc == 2 ) {
        if( lua_islightuserdata( state, argc ) ) {
            CEntity* temp       = (CEntity*)lua_touserdata( state, argc );

            if( temp != nullptr ) {
                EntityType type = temp->getType();

                if( type == EntityType::TYPE_CHARACTER ) {
                    CCharacter* character   = (CCharacter*)temp;

                    faction = character->getFaction();
                }
                else if( type == EntityType::TYPE_PROJECTILE ) {
                    CProjectile* projectile = (CProjectile*)temp;

                    faction = projectile->getStats().faction;
                }
            }
            else {
                std::cout << "Error: Unable to get faction. Got nullptr" << std::endl;
            }
        }
        else {
            std::cout << "Error: Unable to get faction. Light user data expected" << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to get faction. " << errorArgc << std::endl;
    }

    lua_pop( state, argc );

    lua_pushinteger( state, (int)faction );

    return 1;
}

int CEntitySystem::luaCompareFaction( lua_State* state ) {
    int argc = lua_gettop( state );

    bool result = false;

    if( argc == 3 ) {
        if( lua_islightuserdata( state, argc - 1 ) && lua_islightuserdata( state, argc ) ) {
            CCharacter* first       = (CCharacter*)lua_touserdata( state, argc - 1 );
            CCharacter* second      = (CCharacter*)lua_touserdata( state, argc );

            if( first != nullptr && second != nullptr ) {
                result = ( first->getFaction() == second->getFaction() );

                cout << (int)first->getFaction() << " " << (int)second->getFaction() << endl;
            }
            else {
                std::cout << "Error: Unable to compare faction. One of given args is nullptr" << std::endl;
            }
        }
        else {
            std::cout << "Error: Unable to compare faction. One of given args is not light user data" << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to compare faction. " << errorArgc << std::endl;
    }

    lua_pop( state, argc );

    lua_pushboolean( state, result );

    return 1;
}

int CEntitySystem::luaDealDamage( lua_State* state ) {
    int argc = lua_gettop( state );

    CCharacter*     target      = nullptr;
    CCharacter*     source      = nullptr;
    int             damage      = 0;

    if( argc == 3 ) {
        if( lua_islightuserdata( state, argc - 1 ) ) {
            target      = (CCharacter*)lua_touserdata( state, argc - 1 );
            damage      = lua_tointeger( state, argc );
        }
        else {
            std::cout << "Error: Unable to deal damage. Light user data expected" << std::endl;
        }
    }
    else if( argc == 4 ) {
        if( lua_islightuserdata( state, argc - 2 ) && lua_islightuserdata( state, argc - 1 ) ) {
            target      = (CCharacter*)lua_touserdata( state, argc - 2 );
            source      = (CCharacter*)lua_touserdata( state, argc - 1 );
            damage      = lua_tointeger( state, argc );
        }
        else {
            std::cout << "Error: Unable to deal damage. Light user data expected" << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to deal damage. " << errorArgc << std::endl;
    }

    lua_pop( state, argc );

    if( target != nullptr && damage != 0 ) {
        CGame::EntitySystem.attackTarget( target, source, damage );
    }

    return 0;
}

int CEntitySystem::luaAddEffect( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc == 3 ) {
        if( lua_islightuserdata( state, argc - 1 ) ) {
            CCharacter* character = (CCharacter*)lua_touserdata( state, argc - 1 );

            if( character != nullptr ) {
                if( character->getType() == TYPE_CHARACTER ) {
                    CGame::EffectManager.createFromLuaTable( state, argc, character );
                }
            }
        }
        else {
            std::cout << "Error: Unable to add effect. Light user data expected" << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to add effect. " << errorArgc << std::endl;
    }


    lua_pop( state, argc );

    return 0;
}

int CEntitySystem::luaGetNearby( lua_State* state ) {
    int argc = lua_gettop( state );

    CDimensional*   dim         = nullptr;
    float           radius      = 0;

    if( argc == 2 ) {
        if( lua_islightuserdata( state, argc - 1 ) ) {
            dim     = (CDimensional*)lua_touserdata( state, argc - 1 );
        }
        else {
            std::cout << "Error: Unable to get nearby entities. Light user data is missing" << std::endl;
        }
    }
    else if( argc == 3 ) {
        if( lua_islightuserdata( state, argc - 1 ) ) {
            dim     = (CDimensional*)lua_touserdata( state, argc - 1 );

            radius  = lua_tonumber( state, argc );
        }
        else {
            std::cout << "Error: Unable to get nearby entities. Light user data is missing" << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to get nearby entities. Wrong amount of arguments" << std::endl;
    }

    if( dim != nullptr ) {
        if( radius == 0.0 ) {
            CSpatialSystem::Cell nearby = CGame::SpatialSystem.getNearby( dim );

            lua_createtable( state, 0, nearby.size() );

            for( size_t i = 0; i < nearby.size(); i++ ) {
                lua_pushlightuserdata( state, nearby[ i ] );
                lua_rawseti( state, - 2, i + 1 );
            }
        }
        else {
            sf::Vector2f pos = dim->getCenter();
            sf::Vector2f size;

            pos.x -= radius;
            pos.y -= radius;

            size.x = radius * 2;
            size.y = radius * 2;

            CDimensional temp;
            temp.setPos( pos );
            temp.setSize( size );

            CSpatialSystem::Cell nearby = CGame::SpatialSystem.getNearby( &temp );

            lua_createtable( state, 0, nearby.size() );

            for( size_t i = 0; i < nearby.size(); i++ ) {
                lua_pushlightuserdata( state, nearby[ i ] );
                lua_rawseti( state, - 2, i + 1 );
            }
        }
    }
    else {
        lua_pushnil( state );
    }

    return 1;
}

int CEntitySystem::luaGetNearbyChars( lua_State* state ) {
    int argc = lua_gettop( state );

    CCharacter*     character   = nullptr;
    float           radius      = 0;

    if( argc == 2 ) {
        if( lua_islightuserdata( state, argc - 1 ) ) {
            character   = (CCharacter*)lua_touserdata( state, argc - 1 );
        }
        else {
            std::cout << "Error: Unable to get nearby entities. Light user data is missing" << std::endl;
        }
    }
    else if( argc == 3 ) {
        if( lua_islightuserdata( state, argc - 1 ) ) {
            character   = (CCharacter*)lua_touserdata( state, argc - 1 );

            radius      = lua_tonumber( state, argc );
        }
        else {
            std::cout << "Error: Unable to get nearby entities. Light user data is missing" << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to get nearby entities. Wrong amount of arguments" << std::endl;
    }

    if( character != nullptr ) {
        if( radius == 0.0 ) {
            CSpatialSystem::Cell nearby = CGame::SpatialSystem.getNearby( character );

            Entities nearbyChars;

            for( auto it : nearby ) {
                if( it->getType() == TYPE_CHARACTER ) {
                    nearbyChars.push_back( it );
                }
            }

            lua_createtable( state, 0, nearbyChars.size() );

            for( size_t i = 0; i < nearbyChars.size(); i++ ) {
                lua_pushlightuserdata( state, nearbyChars[ i ] );
                lua_rawseti( state, - 2, i + 1 );
            }
        }
        else {
            sf::Vector2f pos = character->getCenter();
            sf::Vector2f size;

            pos.x -= radius;
            pos.y -= radius;

            size.x = radius * 2;
            size.y = radius * 2;

            CDimensional dim;
            dim.setPos( pos );
            dim.setSize( size );

            CSpatialSystem::Cell nearby = CGame::SpatialSystem.getNearbySingle( &dim );

            Entities nearbyChars;

            for( auto it : nearby ) {
                if( it->getType() == TYPE_CHARACTER ) {
                    nearbyChars.push_back( it );
                }
            }

            lua_createtable( state, 0, nearbyChars.size() );

            for( size_t i = 0; i < nearbyChars.size(); i++ ) {
                lua_pushlightuserdata( state, nearbyChars[ i ] );
                lua_rawseti( state, - 2, i + 1 );
            }
        }
    }
    else {
        lua_pushnil( state );
    }

    return 1;
}

int CEntitySystem::luaSetColor( lua_State* state ) {
    int argc = lua_gettop( state );

    CEntity*    entity  = nullptr;
    sf::Color   color   = sf::Color::White;

    if( argc == 3 ) {
        if( lua_islightuserdata( state, argc - 1 ) ) {
            entity  = (CEntity*)lua_touserdata( state, argc - 1 );
            color   = Util::colorFromLuaTable( state, argc );
        }
        else {
            std::cout << "Error: Unable to set color. Light user data expected" << std::endl;
        }
    }
    else if( argc == 6 ) {
        if( lua_islightuserdata( state, argc - 4 ) ) {
            entity  = (CEntity*)lua_touserdata( state, argc - 4 );
            color.r = lua_tointeger( state, argc - 3 );
            color.g = lua_tointeger( state, argc - 2 );
            color.b = lua_tointeger( state, argc - 1 );
            color.a = lua_tointeger( state, argc );
        }
        else {
            std::cout << "Error: Unable to set color. Light user data expected" << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to set color. " << errorArgc << std::endl;
    }

    if( entity != nullptr ) {
        entity->getAnimation().getSprite().setColor( color );
    }

    return 0;
}

int CEntitySystem::luaGetColor( lua_State* state ) {
    int argc = lua_gettop( state );

    CEntity*    entity  = nullptr;

    if( argc == 2 ) {
        if( lua_islightuserdata( state, argc ) ) {
            entity  = (CEntity*)lua_touserdata( state, argc );
        }
        else {
            std::cout << "Error: Unable to get color. Light user data expected" << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to get color. " << errorArgc << std::endl;
    }

    lua_pop( state, argc );

    if( entity != nullptr ) {
        sf::Color color = entity->getAnimation().getSprite().getColor();

        lua_pushinteger( state, color.r );
        lua_pushinteger( state, color.g );
        lua_pushinteger( state, color.b );
        lua_pushinteger( state, color.a );

        return 4;
    }
    else {
        lua_pushnil( state );

        return 1;
    }
}

int CEntitySystem::luaAddColor( lua_State* state ) {
    int argc = lua_gettop( state );

    CEntity*    entity  = nullptr;
    sf::Color   color   = sf::Color::White;

    if( argc == 3 ) {
        if( lua_islightuserdata( state, argc - 1 ) ) {
            entity  = (CEntity*)lua_touserdata( state, argc - 1 );
            color   = Util::colorFromLuaTable( state, argc );
        }
        else {
            std::cout << "Error: Unable to add color. Light user data expected" << std::endl;
        }
    }
    else if( argc == 6 ) {
        if( lua_islightuserdata( state, argc - 4 ) ) {
            entity  = (CEntity*)lua_touserdata( state, argc - 4 );
            color.r = lua_tointeger( state, argc - 3 );
            color.g = lua_tointeger( state, argc - 2 );
            color.b = lua_tointeger( state, argc - 1 );
            color.a = lua_tointeger( state, argc );
        }
        else {
            std::cout << "Error: Unable to add color. Light user data expected" << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to add color. " << errorArgc << std::endl;
    }

    if( entity != nullptr ) {
        entity->getAnimation().getSprite().addColorModul( color );
    }

    return 0;
}

int CEntitySystem::luaDelColor( lua_State* state ) {
    int argc = lua_gettop( state );

    CEntity*    entity  = nullptr;
    sf::Color   color   = sf::Color::White;

    if( argc == 3 ) {
        if( lua_islightuserdata( state, argc - 1 ) ) {
            entity  = (CEntity*)lua_touserdata( state, argc - 1 );
            color   = Util::colorFromLuaTable( state, argc );
        }
        else {
            std::cout << "Error: Unable to del color. Light user data expected" << std::endl;
        }
    }
    else if( argc == 6 ) {
        if( lua_islightuserdata( state, argc - 4 ) ) {
            entity  = (CEntity*)lua_touserdata( state, argc - 4 );
            color.r = lua_tointeger( state, argc - 3 );
            color.g = lua_tointeger( state, argc - 2 );
            color.b = lua_tointeger( state, argc - 1 );
            color.a = lua_tointeger( state, argc );
        }
        else {
            std::cout << "Error: Unable to del color. Light user data expected" << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to del color. " << errorArgc << std::endl;
    }

    if( entity != nullptr ) {
        entity->getAnimation().getSprite().delColorModul( color );
    }

    return 0;
}

//int CEntitySystem::luaAddParticles( lua_State* state ) {
//    int argc = lua_gettop( state );
//
//    if( argc == 3 ) {
//        if( lua_islightuserdata( state, argc - 1 ) ) {
//            CEntity* entity = (CEntity*)lua_touserdata( state, argc - 1 );
//
//            CGame::WorldManager.ParticleSystem.effectFromLuaTable( state, argc, entity );
//        }
//        else {
//            std::cout << "Error: Unable to del color. Light user data expected" << std::endl;
//        }
//    }
//    else {
//        std::cout << "Error: Unable to add particles. " << errorArgc << std::endl;
//    }
//
//    lua_pop( state, argc );
//
//    return 0;
//}

int CEntitySystem::luaIsPosFree( lua_State* state ) {
    int argc = lua_gettop( state );

    bool result = false;

    if( argc == 5 ) {
        sf::FloatRect box;

        box.left    = lua_tonumber( state, argc - 3 );
        box.top     = lua_tonumber( state, argc - 2 );
        box.width   = lua_tonumber( state, argc - 1 );
        box.height  = lua_tonumber( state, argc );

        result = CGame::EntitySystem.checkIfPosIsFree( box );
    }
    else {
        std::cout << "Error: Unable to determine if pos is free. Wrong amount of arguments." << std::endl;
        std::cout << "Remember isPosFree() accepts x, y, w, h" << std::endl;
    }

    lua_pop( state, argc );

    lua_pushboolean( state, result );

    return 1;
}

int CEntitySystem::luaCreatePlayer( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 2 && lua_istable( state, - 1 ) );

    size_t ID = CGame::EntitySystem.playerFromLuaTable( state, argc );

    lua_pop( state, argc );

    lua_pushinteger( state, ID );

    return 1;
}

int CEntitySystem::luaCreateEnemy( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 2 && lua_istable( state, - 1 ) );

    size_t ID = CGame::EntitySystem.enemyFromLuaTable( state, argc );

    lua_pop( state, argc );

    lua_pushinteger( state, ID );

    return 1;
}

int CEntitySystem::luaCreateObject( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 2 && lua_istable( state, - 1 ) );

    size_t ID = CGame::EntitySystem.objectFromLuaTable( state, argc );

    lua_pop( state, argc );

    lua_pushinteger( state, ID );

    return 1;
}

int CEntitySystem::luaCreateProjectile( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 2 && lua_istable( state, - 1 ) );

    size_t ID = CGame::EntitySystem.bulletFromLuatable( state, argc );

    lua_pop( state, argc );

    lua_pushinteger( state, ID );

    return 1;
}

int CEntitySystem::luaPlayerMoveLeft( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    CGame::EntitySystem.getPlayer()->moveLeft();

    return 0;
}

int CEntitySystem::luaPlayerMoveRight( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    CGame::EntitySystem.getPlayer()->moveRight();

    return 0;
}

int CEntitySystem::luaPlayerMoveUp( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    CGame::EntitySystem.getPlayer()->moveUp();

    return 0;
}

int CEntitySystem::luaPlayerMoveDown( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    CGame::EntitySystem.getPlayer()->moveDown();

    return 0;
}

int CEntitySystem::luaPlayerAttack( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 2 && "Wrong amount of arguments" );

    float angle = lua_tonumber( state, - 1 );

    CGame::EntitySystem.getPlayer()->performAttack( angle );

    lua_pop( state, argc );

    return 0;
}

int CEntitySystem::luaPlayerCastAbility( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 3 && "Wrong amount of arguments" );

    size_t          index       = lua_tointeger( state, - 2 );
    sf::Vector2f    destination = Util::vector2fFromLuaTable( state, argc );

    CGame::EntitySystem.getPlayer()->castAbility( index, destination );

    lua_pop( state, argc );

    return 0;
}

int CEntitySystem::luaPlayerInteract( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    CGame::EntitySystem.playerInteract();

    return 0;
}

int CEntitySystem::luaInteractObject( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc != 2 ) {
        std::cout << "Error: Wrong amount of arguments - luaPlayerInteract" << std::endl;

        lua_pop( state, argc );

        return 0;
    }

    size_t ID = lua_tointeger( state, - 1 );

    CGame::EntitySystem.interactWithObject( ID );

    lua_pop( state, argc );

    return 0;
}

int CEntitySystem::luaDespawnEntity( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( lua_isinteger( state, - 1 ) && "ID is not integer" );
    size_t ID = lua_tointeger( state, - 1 );

    lua_pop( state, argc );

    CGame::EntitySystem.despawnEntity( ID );

    return 0;
}

int CEntitySystem::luaDespawnEnemy( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( lua_isinteger( state, - 1 ) && "ID is not integer" );
    size_t ID = lua_tointeger( state, - 1 );

    lua_pop( state, argc );

    CGame::EntitySystem.despawnEnemy( ID );

    return 0;
}

int CEntitySystem::luaDespawnObject( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( lua_isinteger( state, - 1 ) && "ID is not integer" );
    size_t ID = lua_tointeger( state, - 1 );

    lua_pop( state, argc );

    CGame::EntitySystem.despawnObject( ID );

    return 0;
}

int CEntitySystem::luaDespawnProjectile( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( lua_isinteger( state, - 1 ) && "ID is not integer" );
    size_t ID = lua_tointeger( state, - 1 );

    lua_pop( state, argc );

    CGame::EntitySystem.despawnProjectile( ID );

    return 0;
}

int CEntitySystem::luaSetPlayerPos( lua_State* state ) {
    int argc = lua_gettop( state );

    sf::Vector2f pos = Util::vector2fFromLuaTable( state, argc );

    CGame::EntitySystem.getPlayer()->setPos( pos );

    lua_pop( state, argc );

    return 0;
}

int CEntitySystem::luaGetPlayerPos( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    sf::Vector2f pos = CGame::EntitySystem.getPlayerPos();

    lua_pushnumber( state, pos.x );
    lua_pushnumber( state, pos.y );

    return 2;
}

int CEntitySystem::luaGetPlayerSize( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    sf::Vector2f size = CGame::EntitySystem.getPlayerSize();

    lua_pushnumber( state, size.x );
    lua_pushnumber( state, size.y );

    return 2;
}

int CEntitySystem::luaGetPlayerCenter( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    sf::Vector2f center = CGame::EntitySystem.getPlayerCenter();

    lua_pushnumber( state, center.x );
    lua_pushnumber( state, center.y );

    return 2;
}

/** LUA Character */
int CEntitySystem::luaGetCharacterStats( lua_State* state ) {
    int argc = lua_gettop( state );

    CCharacter* character = nullptr;

    if( argc == 2 ) {
        if( lua_islightuserdata( state, argc ) ) {
            character   = (CCharacter*)lua_touserdata( state, argc );
        }
        else {
            std::cout << "Error: Unable to get character stats. Light user data expected, got " << lua_typename( state, lua_type( state, argc ) ) << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to get character stats. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    if( character != nullptr ) {
        CharacterStats      stats   = character->getModStats();
        CharacterFaction    faction = character->getFaction();

        lua_createtable( state, 0, 17 );

        lua_pushinteger( state, stats.STR );
        lua_setfield( state, - 2, "str" );
        lua_pushinteger( state, stats.DEX );
        lua_setfield( state, - 2, "dex" );
        lua_pushinteger( state, stats.INT );
        lua_setfield( state, - 2, "int" );
        lua_pushinteger( state, stats.VIT );
        lua_setfield( state, - 2, "vit" );
        lua_pushinteger( state, stats.Armor );
        lua_setfield( state, - 2, "armor" );
        lua_pushinteger( state, stats.regenHealth );
        lua_setfield( state, - 2, "regHp" );
        lua_pushinteger( state, stats.regenResource );
        lua_setfield( state, - 2, "regRp" );
        lua_pushnumber( state, stats.movementSpd );
        lua_setfield( state, - 2, "moveSpd" );
        lua_pushnumber( state, stats.attackSpd );
        lua_setfield( state, - 2, "attSpd" );
        lua_pushnumber( state, stats.castingSpd );
        lua_setfield( state, - 2, "castSpd" );
        lua_pushinteger( state, stats.damageMin );
        lua_setfield( state, - 2, "dmgMin" );
        lua_pushinteger( state, stats.damageMax );
        lua_setfield( state, - 2, "dmgMax" );
        lua_pushnumber( state, stats.critChance );
        lua_setfield( state, - 2, "critChance" );
        lua_pushnumber( state, stats.critMultip );
        lua_setfield( state, - 2, "critMultip" );
        lua_pushinteger( state, (int)faction );
        lua_setfield( state, - 2, "faction" );
    }
    else {
        lua_pushnil( state );
    }

    return 1;
}

/** LUA Entities */

int CEntitySystem::luaGetEntityPos( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 2 && "Wrong amount of arguments" );

    size_t ID = lua_tointeger( state, argc );

    lua_pop( state, argc );

    sf::Vector2f pos = CGame::EntitySystem.getEntityPos( ID );

    lua_pushnumber( state, pos.x );
    lua_pushnumber( state, pos.y );

    return 2;
}

int CEntitySystem::luaGetEntityCenter( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 2 && "Wrong amount of arguments" );

    size_t ID = lua_tointeger( state, argc );

    lua_pop( state, argc );

    sf::Vector2f center = CGame::EntitySystem.getEntityCenter( ID );

    lua_pushnumber( state, center.x );
    lua_pushnumber( state, center.y );

    return 2;
}

int CEntitySystem::luaGetEntitySize( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 2 && "Wrong amount of arguments" );

    size_t ID = lua_tointeger( state, argc );

    lua_pop( state, argc );

    sf::Vector2f size = CGame::EntitySystem.getEntitySize( ID );

    lua_pushnumber( state, size.x );
    lua_pushnumber( state, size.y );

    return 2;
}

int CEntitySystem::luaSetEntityPos( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 3 && "Wrong amount of arguments" );

    size_t ID           = lua_tointeger( state, - 2 );
    sf::Vector2f pos    = Util::vector2fFromLuaTable( state, argc );

    CGame::EntitySystem.setEntityPos( ID, pos );

    lua_pop( state, argc );

    return 0;
}

int CEntitySystem::luaSetEntityCenter( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 3 && "Wrong amount of arguments" );

    size_t ID           = lua_tointeger( state, - 2 );
    sf::Vector2f center = Util::vector2fFromLuaTable( state, argc );

    CGame::EntitySystem.setEntityCenter( ID, center );

    lua_pop( state, argc );

    return 0;
}

int CEntitySystem::luaSetEntitySize( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 3 && "Wrong amount of arguments" );

    size_t ID           = lua_tointeger( state, - 2 );
    sf::Vector2f size   = Util::vector2fFromLuaTable( state, argc );

    CGame::EntitySystem.setEntitySize( ID, size );

    lua_pop( state, argc );

    return 0;
}

int CEntitySystem::luaAddEffectToPlayer( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 2 && "Wrong amount of arguments" );

    CCharacter* player      = CGame::EntitySystem.getPlayer();

    CGame::EffectManager.createFromLuaTable( state, argc, player );

    lua_pop( state, argc );

    return 0;
}

int CEntitySystem::luaDelEffectFromPlayer( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 2 && "Wrong amount of arguments" );

    std::string ID          = lua_tostring( state, argc );
    CCharacter* player      = CGame::EntitySystem.getPlayer();

    player->delEffect( ID );

    lua_pop( state, argc );

    return 0;
}

int CEntitySystem::luaDisplayEntityBox( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    CGame::EntitySystem.setDisplayEntityBox();

    return 0;
}

int CEntitySystem::luaDisplayEntityStat( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    CGame::EntitySystem.setDisplayEntityStat();

    return 0;
}

/** LUA FUNCTIONS END */

size_t CEntitySystem::getNewID() {
    m_IDCounter++;

    return m_IDCounter - 1;
}

size_t CEntitySystem::getNewFunctionID() {
    m_funcIDCounter++;

    return m_funcIDCounter - 1;
}

void CEntitySystem::addToSpatialSystem() {
    if( m_player != nullptr ) {
        CGame::SpatialSystem.addEntity( m_player );
    }

    for( auto it : m_enemies ) {
        CGame::SpatialSystem.addEntity( it );
    }

    for( auto it : m_objects ) {
        CGame::SpatialSystem.addEntity( it );
    }
}

void CEntitySystem::addToZBuffer() {
    CGame::ZBuffer.insert( m_player );

    for( auto it : m_enemies ) {
        CGame::ZBuffer.insert( it );
    }

    for( auto it : m_objects ) {
        CGame::ZBuffer.insert( it );
    }

    for( auto it : m_projectiles ) {
        CGame::ZBuffer.insert( it );
    }
}

void CEntitySystem::loadBaseEntityData( lua_State* state, int index, CEntity* entity ) {
    lua_getfield( state, index, "data" );
    if( lua_istable( state, - 1 ) ) {
        int dataIndex = lua_gettop( state );

        lua_getfield( state, dataIndex, "size" );
        if( !lua_isnil( state, - 1 ) ) {
            entity->setSize( Util::vector2fFromLuaTable( state, lua_gettop( state ) ) );
        }

        lua_getfield( state, dataIndex, "collbox" );
        if( lua_istable( state, - 1 ) ) {
            entity->setCollBox( Util::floatrectFromLuaTable( state, lua_gettop( state ) ) );
        }
        else {
            entity->setCollideable( false );
        }

        lua_getfield( state, dataIndex, "speed" );
        if( lua_isnumber( state, - 1 ) ) {
            entity->setMoveVelocity( lua_tonumber( state, - 1 ) );
        }

        lua_getfield( state, dataIndex, "solid" );
        if( lua_isboolean( state, - 1 ) ) {
            entity->setSolid( lua_toboolean( state, - 1 ) );
        }

        lua_getfield( state, dataIndex, "collideable" );
        if( lua_isboolean( state, - 1 ) ) {
            entity->setCollideable( lua_toboolean( state, - 1 ) );
        }

        lua_getfield( state, dataIndex, "texture" );
        if( !lua_isnil( state, - 1 ) ) {
            entity->setupAnimationFromLuaTable( state, lua_gettop( state ) );
        }

        lua_getfield( state, dataIndex, "zLayer" );
        if( lua_isinteger( state, - 1 ) ) {
            entity->setZLayer( lua_tointeger( state, - 1 ) );
        }

        lua_pop( state, 7 );
    }
    else {
        std::cout << "Warning: Can not make correct entity. Table \'data\' is missing" << std::endl;
    }

    lua_getfield( state, index, "angle" );
    if( lua_isnumber( state, - 1 ) ) {
        entity->setMoveAngle( lua_tonumber( state, - 1 ) );
    }

    lua_getfield( state, index, "pos" );
    if( !lua_isnil( state, - 1 ) ) {
        entity->setPos( Util::vector2fFromLuaTable( state, lua_gettop( state ) ) );
    }
    else {
        randomEntityPosition( entity );
    }

    lua_getfield( state, index, "effect" );
    if( !lua_isnil( state, - 1 ) ) {
        size_t effectID = CGame::WorldManager.ParticleSystem.effectFromLuaTable( state, lua_gettop( state ), entity );

        entity->setParticleEffectID( effectID );
    }

    lua_getfield( state, index, "soundEmitter" );
    if( lua_isstring( state, - 1 ) ) {
        CGame::AudioSystem.registerEmitter( entity, lua_tostring( state, - 1 ) );

        entity->setSoundEmitter( true );
    }

    lua_getfield( state, index, "sounds" );
    if( lua_istable( state, - 1 ) ) {
        for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
            // Copy the key
            lua_pushvalue( state, - 2 );

            std::string key     = lua_tostring( state, - 1 );
            std::string ID      = lua_tostring( state, - 2 );

            // Pop the copy of the key
            lua_pop( state, 1 );

            if( key == "onDespawn" ) {
                entity->setSound( EntitySounds::ONDESPAWN, ID );
            }
            else if( key == "onInteract" ) {
                entity->setSound( EntitySounds::ONDESPAWN, ID );
            }
            else if( key == "onDeath" ) {
                entity->setSound( EntitySounds::ONDESPAWN, ID );
            }
            else {
                std::cout << "Warning: Not supported key for entity sound: " << key << std::endl;
            }
        }
    }

    lua_getfield( state, index, "ai" );
    if( lua_istable( state, - 1 ) ) {
        entity->setAiComponent( loadAiFromLauTable( state, lua_gettop( state ) ) );
    }

    lua_getfield( state, index, "functions" );
    if( lua_istable( state, - 1 ) ) {
        for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; ) {
            // Copy the key to top of the stack - so we can be sure that original key is not modified by lua_tostring
            lua_pushvalue( state, - 2 );

            // Get the key
            std::string key = lua_tostring( state, - 1 );

            // Pop the copied key
            lua_pop( state, 1 );

            // Registers Lua function and pops it from the stack
            size_t ID = registerLuaFunction( state );

            if( key == "onHit" ) {
                entity->setFunctionID( LuaFunction::FUNC_ONHIT, ID );
            }
            else if( key == "onUpdate" ) {
                entity->setFunctionID( LuaFunction::FUNC_ONUPDATE, ID );
            }
            else if( key == "onInteract" ) {
                entity->setFunctionID( LuaFunction::FUNC_ONINTERACT, ID );
            }
            else if( key == "onDeath" ) {
                entity->setFunctionID( LuaFunction::FUNC_ONDEATH, ID );
            }
            else if( key == "onDespawn" ) {
                entity->setFunctionID( LuaFunction::FUNC_ONDESPAWN, ID );
            }
        }
    }

    lua_getfield( state, index, "listenTo" );
    if( lua_istable( state, - 1 ) ) {
        for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {

            std::string msgID;

            lua_getfield( state, - 1, "name" );
            if( !lua_isnil( state, - 1  ) ) {

                msgID = lua_tostring( state, - 1 );

                CGame::MessageSystem.subscribe( entity, msgID );
            }

            lua_getfield( state, - 2, "action" );

            size_t ID = registerLuaFunction( state );

            entity->addEventFunction( msgID, ID );

            lua_pop( state, 1 );
        }
    }

    lua_pop( state, 9 );
}

void CEntitySystem::loadCharacterData( lua_State* state, int index, CCharacter* character ) {
    lua_getfield( state, index, "charData" );

    // Check if table with character data exists
    if( !lua_istable( state, - 1 ) ) {
        std::cout << "Error: Unable to initialize character data. Table is missing" << std::endl;

        lua_pop( state, 1 );

        return;
    }
    else {
        // Update index to charData table
        index = lua_gettop( state );
    }

    // Initialize all character data
    CharacterClass      charClass       = CharacterClass::NOCLASS;
    CharacterFaction    charFaction     = CharacterFaction::FACTION_NEUTRAL;
    int                 charHealth      = 0;
    int                 charResource    = 0;
    size_t              charExpOnKill   = 0;
    CharacterStats      charStats;

    lua_getfield( state, index, "class" );
    if( lua_isinteger( state, - 1 ) ) {
        charClass   = (CharacterClass)lua_tointeger( state, - 1 );
    }

    lua_getfield( state, index, "faction" );
    if( lua_isinteger( state, - 1 ) ) {
        charFaction = (CharacterFaction)lua_tointeger( state, - 1 );
    }

    lua_getfield( state, index, "health" );
    if( lua_isinteger( state, - 1 ) ) {
        charHealth  = lua_tointeger( state, - 1 );
    }

    lua_getfield( state, index, "resource" );
    if( lua_isinteger( state, - 1 ) ) {
        charResource = lua_tointeger( state, - 1 );
    }

    lua_getfield( state, index, "experience" );
    if( lua_isinteger( state, - 1) ) {
        charExpOnKill= lua_tointeger( state, - 1 );
    }

    lua_getfield( state, index, "stats" );
    if( lua_istable( state, - 1 ) ) {
        int statTableIndex = lua_gettop( state );

        lua_getfield( state, statTableIndex, "str" );
        if( !lua_isnil( state, - 1 ) ) {
            charStats.STR = lua_tointeger( state, - 1 );
        }

        lua_getfield( state, statTableIndex, "dex" );
        if( !lua_isnil( state, - 1 ) ) {
            charStats.DEX = lua_tointeger( state, - 1 );
        }

        lua_getfield( state, statTableIndex, "int" );
        if( !lua_isnil( state, - 1 ) ) {
            charStats.INT = lua_tointeger( state, - 1 );
        }

        lua_getfield( state, statTableIndex, "vit" );
        if( !lua_isnil( state, - 1 ) ) {
            charStats.VIT = lua_tointeger( state, - 1 );
        }

        lua_getfield( state, statTableIndex, "armor" );
        if( !lua_isnil( state, - 1 ) ) {
            charStats.Armor = lua_tointeger( state, - 1 );
        }

        lua_getfield( state, statTableIndex, "regHealth" );
        if( !lua_isnil( state, - 1 ) ) {
            charStats.regenHealth = lua_tointeger( state, - 1 );
        }

        lua_getfield( state, statTableIndex, "regResource" );
        if( !lua_isnil( state, - 1 ) ) {
            charStats.regenResource = lua_tointeger( state, - 1 );
        }

        lua_getfield( state, statTableIndex, "moveSpd" );
        if( !lua_isnil( state, - 1 ) ) {
            charStats.movementSpd = lua_tonumber( state, - 1 );
        }

        lua_getfield( state, statTableIndex, "attSpd" );
        if( !lua_isnil( state, - 1 ) ) {
            charStats.attackSpd = lua_tonumber( state, - 1 );
        }

        lua_getfield( state, statTableIndex, "castSpd" );
        if( !lua_isnil( state, - 1 ) ) {
            charStats.castingSpd = lua_tonumber( state, - 1 );
        }

        lua_getfield( state, statTableIndex, "dmgMin" );
        if( !lua_isnil( state, - 1 ) ) {
            charStats.damageMin = lua_tointeger( state, - 1 );
        }

        lua_getfield( state, statTableIndex, "dmgMax" );
        if( !lua_isnil( state, - 1 ) ) {
            charStats.damageMax = lua_tointeger( state, - 1 );
        }

        lua_getfield( state, statTableIndex, "critChance" );
        if( !lua_isnil( state, - 1 ) ) {
            charStats.critChance = lua_tonumber( state, - 1 );
        }

        lua_getfield( state, statTableIndex, "critMultip" );
        if( !lua_isnil( state, - 1 ) ) {
            charStats.critMultip = lua_tonumber( state, - 1 );
        }

        lua_pop( state, 14 );
    }

    lua_getfield( state, index, "abilities" );
    if( lua_istable( state, - 1 ) ) {
        for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
            character->addAbility( CGame::AbilityManager.createFromLua( state, lua_gettop( state ) ) );
        }
    }

    lua_getfield( state, index, "loot" );
    if( lua_istable( state, - 1 ) ) {
        character->setLootTable( CGame::WorldManager.LootSystem.loadLootTableFromLua( state, lua_gettop( state ) ) );
    }

    character->setClass( charClass );
    character->setFaction( charFaction );
    character->setBaseStats( charStats );
    character->setMaxHealth( charHealth );
    character->setMaxResource( charResource );
    character->setExpOnKill( charExpOnKill );

    lua_pop( state, 9 );
}

void CEntitySystem::loadProjectileData( lua_State* state, int index, CProjectile* projectile ) {
    lua_getfield( state, index, "distance" );
    if( lua_isnumber( state, - 1 ) ) {
        float distance = lua_tonumber( state, - 1 );

        projectile->setFlyDistance( distance );
    }
    else {
        std::cout << "Error: Maximum fly distance for projectile is missing." << std::endl;
    }

    lua_getfield( state, index, "functions" );
    if( lua_istable( state, - 1 ) ) {
        for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
            // Copy the key so we don't mess up lua_next
            lua_pushvalue( state, - 2 );

            std::string key = lua_tostring( state, - 1 );

            // Pop the copy of the key
            lua_pop( state, 1 );

            // Copy function so we don't mess up lua_next
            lua_pushvalue( state, - 1 );

            if( key == "onEnemyHit" ) {
                projectile->setProjFunc( PROJFUNC_ONENEMYHIT, registerLuaFunction( state ) );
            }
            else {
                lua_pop( state, 1 );
            }
        }
    }

    lua_getfield( state, index, "stats" );

    // Check if table with character data exists
    if( !lua_istable( state, - 1 ) ) {
        std::cout << "Error: Unable to initialize projectile stats. Table is missing" << std::endl;

        lua_pop( state, 1 );

        return;
    }
    else {
        // Update index to charData table
        index = lua_gettop( state );
    }

    ProjectileStats stats;

    lWrapper::valueFromLuaTable<int>    ( state, index, "dmgMin",       stats.dmgMin );
    lWrapper::valueFromLuaTable<int>    ( state, index, "dmgMax",       stats.dmgMax );
    lWrapper::valueFromLuaTable<float>  ( state, index, "critChance",   stats.critChance );
    lWrapper::valueFromLuaTable<float>  ( state, index, "critMultip",   stats.critMultip );

    lua_getfield( state, index, "faction" );
    if( lua_isinteger( state, - 1 ) ) {
        stats.faction     = (CharacterFaction)lua_tointeger( state, - 1 );
    }

    projectile->setStats( stats );

    lua_pop( state, 4 );
}

/** LUA END */

void CEntitySystem::handleAttack( size_t targetID, size_t sourceID, int damage, DamageType type ) {
    if( m_player->getID() == targetID ) {
        int recalculatedDamage = m_player->calculateReductions( damage );

        m_player->takeDamage( sourceID, recalculatedDamage, type );
        m_player->onDamageTaken( sourceID, recalculatedDamage );

        CGame::WorldManager.CombatTextSystem.createCombatText( m_player, recalculatedDamage, type );

        if( m_player->isDead() ) {
            m_player->onDeath();
        }

        return;
    }


//    CGame::WorldManager.CombatTextSystem.createCombatText( sf::Vector2f( getCenter().x, m_pos.y ), recalculatedDamage, CombatTextType::COMBATTEXT_DMGNORMAL );

    for( auto it : m_enemies ) {
        if( it->getID() == targetID ) {
            if( !it->isDead() ) {
                int recalculatedDamage = it->calculateReductions( damage );

                it->takeDamage( sourceID, recalculatedDamage, type );
                it->onDamageTaken( sourceID, recalculatedDamage );

                CGame::WorldManager.CombatTextSystem.createCombatText( it, recalculatedDamage, type );

                if( it->isDead() ) {
                    m_player->addExp( it->getExpOnKill() );
                    it->onDeath();

                    if( it->hasLootTable() ) {
                        CGame::WorldManager.LootSystem.createLoot( it );
                    }
                }
            }

            return;
        }
    }
}

void CEntitySystem::displayEntityBox( sf::RenderTarget& window, sf::RenderStates states ) {
    sf::RectangleShape red;
    sf::RectangleShape blue;

    red.setFillColor( sf::Color::Transparent );
    red.setOutlineColor( sf::Color::Red );
    red.setOutlineThickness( 1.0 );

    blue = red;
    blue.setOutlineColor( sf::Color::Blue );

    sf::Vector2f pos;
    sf::Vector2f size;
    sf::Vector2f collPos;
    sf::Vector2f collSize;

    if( m_player != nullptr ) {
        pos         = m_player->getPos();
        size        = m_player->getSize();
        collPos     = m_player->getCollBoxPos() + pos;
        collSize    = m_player->getCollBoxSize();

        red.setPosition(    pos );
        red.setSize(        size );

        blue.setPosition(   collPos );
        blue.setSize(       collSize );

        window.draw( red, states );
        window.draw( blue, states );
    }

    for( auto it : m_enemies ) {
        pos         = it->getPos();
        size        = it->getSize();
        collPos     = it->getCollBoxPos() + pos;
        collSize    = it->getCollBoxSize();

        red.setPosition(    pos );
        red.setSize(        size );

        blue.setPosition(   collPos );
        blue.setSize(       collSize );

        window.draw( red, states );
        window.draw( blue, states );
    }

    for( auto it : m_objects ) {
        pos         = it->getPos();
        size        = it->getSize();
        collPos     = it->getCollBoxPos() + pos;
        collSize    = it->getCollBoxSize();

        red.setPosition(    pos );
        red.setSize(        size );

        blue.setPosition(   collPos );
        blue.setSize(       collSize );

        window.draw( red, states );
        window.draw( blue, states );
    }

    for( auto it : m_projectiles ) {
        sf::CircleShape cBlue;

        cBlue.setFillColor( sf::Color::Transparent );
        cBlue.setOutlineColor( sf::Color::Blue );
        cBlue.setOutlineThickness( 1.0 );

        sf::Vector2f    circlePos;
        float           circleRad;

        pos         = it->getPos();
        size        = it->getSize();
        circlePos.x = it->getCollCircle().x;
        circlePos.y = it->getCollCircle().y;
        circleRad   = it->getCollCircle().r;

        red.setPosition(    sf::Vector2f( pos.x - size.x / 2, pos.y - size.y / 2 ) );
        red.setSize(        size );

        cBlue.setPosition(  sf::Vector2f( circlePos.x - circleRad, circlePos.y - circleRad ) );
        cBlue.setRadius(    it->getCollCircle().r );

        window.draw( red, states );
        window.draw( cBlue, states );
    }
}

void CEntitySystem::displayEntityStat( sf::RenderTarget& window, sf::RenderStates states ) {
    if( m_debugEntityStat == nullptr ) return;

    sf::Vector2f entityPos = m_debugEntityStat->getPos();
    sf::Vector2f entitySize = m_debugEntityStat->getSize();

    /** Box */
    sf::RectangleShape box;
    box.setFillColor( sf::Color::Transparent );
    box.setOutlineColor( sf::Color::Yellow );
    box.setOutlineThickness( 1.0 );
    box.setPosition( entityPos.x - 2, entityPos.y - 2 );
    box.setSize( sf::Vector2f( entitySize.x + 4, entitySize.y + 4 ) );

    /** Memory Adress */
    sf::Text textAddess;
    textAddess.setPosition( 20, 120 );
    textAddess.setFont( CGame::AssetManager.getFont( "FONT_ARIAL" ) );
    textAddess.setCharacterSize( 12 );
    textAddess.setColor( sf::Color::White );

    std::ostringstream oss;
    oss << m_debugEntityStat;

    std::string addressString;
    addressString.append( "mem. address: " );
    addressString.append( oss.str() );

    textAddess.setString( addressString );

    /** Position */
    sf::Text textPos;
    textPos.setPosition( 20, 135 );
    textPos.setFont( CGame::AssetManager.getFont( "FONT_ARIAL" ) );
    textPos.setCharacterSize( 12 );
    textPos.setColor( sf::Color::White );

    std::string posString;
    posString.append( "x: " );
    posString.append( Util::intToString( entityPos.x ) );
    posString.append( "   y: " );
    posString.append( Util::intToString( entityPos.y ) );

    textPos.setString( posString );

    /** Size */
    sf::Text textSize;
    textSize.setPosition( 20, 150 );
    textSize.setFont( CGame::AssetManager.getFont( "FONT_ARIAL" ) );
    textSize.setCharacterSize( 12 );
    textSize.setColor( sf::Color::White );


    std::string sizeString;
    sizeString.append( "w: " );
    sizeString.append( Util::intToString( entitySize.x ) );
    sizeString.append( "   h: " );
    sizeString.append( Util::intToString( entitySize.y ) );

    textSize.setString( sizeString );

    /** Z layer */
    sf::Text textZ;
    textZ.setPosition( 20, 165 );
    textZ.setFont( CGame::AssetManager.getFont( "FONT_ARIAL" ) );
    textZ.setCharacterSize( 12 );
    textZ.setColor( sf::Color::White );


    std::string zString;
    zString.append( "z layer: " );
    zString.append( Util::intToString( m_debugEntityStat->getZLayer() ) );

    textZ.setString( zString );

    window.draw( textAddess );
    window.draw( textPos );
    window.draw( textSize );
    window.draw( textZ );
    window.draw( box, states );
}

bool CEntitySystem::randomEntityPosition( CEntity* entity ) {
    sf::Vector2f size = entity->getSize();
    sf::Vector2f pos;

    sf::Vector2f mapSize = CGame::WorldManager.getMapSize();

    // Prevent infinite loops
    for( size_t counter = 0; counter < 100; counter++ ) {
        // Randomize position
        pos.x = CRNG::iRandom( 0, mapSize.x );
        pos.y = CRNG::iRandom( 0, mapSize.y );

        sf::FloatRect box( pos.x, pos.y, size.x, size.y );

        if( checkIfPosIsFree( box ) ) {
            entity->setPos( pos );

            return true;
        }
    }

    // Randomize position
    pos.x = CRNG::iRandom( 0, mapSize.x );
    pos.y = CRNG::iRandom( 0, mapSize.y );

    sf::FloatRect box( pos.x, pos.y, size.x, size.y );

    entity->setPos( pos );

    return false;
}

bool CEntitySystem::checkIfPosIsFree( const sf::FloatRect& box ) {
    sf::Vector2f mapSize = CGame::WorldManager.getCurrentlyUsedMap()->getSize();

    if( !Util::isRectInside( box, sf::FloatRect( 0, 0, mapSize.x, mapSize.y ) ) ) {
        return false;
    }

    for( auto it : m_objects ) {
        if( it->getBox().intersects( box ) ) {
            return false;
        }
    }

    sf::Vector2f pos    = sf::Vector2f( box.left,    box.top );
    sf::Vector2f size   = sf::Vector2f( box.width,   box.height );

    auto tiles = CGame::WorldManager.getCurrentlyUsedMap()->getNearby( pos, size );

    for( auto it : tiles ) {
        if( it->getBox().intersects( box ) ) {
            return false;
        }
    }

    return true;
}

size_t CEntitySystem::registerLuaFunction( lua_State* state ) {
    int functionRef = luaL_ref( state, LUA_REGISTRYINDEX );

    size_t ID = getNewFunctionID();

    m_luaFunctions[ ID ].push_back( functionRef );

    return ID;
}

void CEntitySystem::removeLuaFunctions( size_t ID ) {
    auto found = m_luaFunctions.find( ID );

    if( found != m_luaFunctions.end() ) {
        lua_State* state = CGame::ScriptSystem.getState();

        for( int i : found->second ) {
            luaL_unref( state, LUA_REGISTRYINDEX, i );
        }

        m_luaFunctions.erase( found->first );
    }
}

CEntity* CEntitySystem::getEntity( size_t ID ) {
    if( m_player != nullptr ) {
        if( m_player->getID() == ID ) {
            return m_player;
        }
    }

    for( auto it : m_enemies ) {
        if( it->getID() == ID ) {
            return it;
        }
    }

    for( auto it : m_objects ) {
        if( it->getID() == ID ) {
            return it;
        }
    }

    for( auto it : m_projectiles ) {
        if( it->getID() == ID ) {
            return it;
        }
    }

    return nullptr;
}

CEntity* CEntitySystem::getEntityByMouse( const sf::Vector2i& pos ) {
    // Lazy to change it
    int x = pos.x;
    int y = pos.y;

    if( m_player != nullptr ) {
        if( m_player->getBox().contains( x, y ) ) {
            return m_player;
        }
    }

    for( auto it : m_enemies ) {
        if( it->getBox().contains( x, y ) ) {
            return it;
        }
    }

    for( auto it : m_objects ) {
        if( it->getBox().contains( x, y ) ) {
            return it;
        }
    }

    for( auto it : m_projectiles ) {
        if( it->getBox().contains( x, y ) ) {
            return it;
        }
    }

    return nullptr;
}
