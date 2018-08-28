#include "CCharacter.h"

#include <assert.h>

#include <CGame.h>
#include <CAiCharComponent.h>
#include <CAbility.h>
#include <CEffect.h>
#include <LuaWrapper.h>
#include <Utility.h>

#include <iostream>
using namespace std;

CCharacter::CCharacter()
: m_aiCharComponent ( nullptr )
, m_class           ( CharacterClass::NOCLASS )
, m_faction         ( CharacterFaction::FACTION_NEUTRAL )
, m_expOnKill       ( 0 )
, m_exp             ( 0 )
, m_expFactor       ( 1.0 )
, m_expToLevel      ( 20 )
, m_level           ( 1 )
, m_castBuffer      ( nullptr )
, m_castAngle       ( 0 )
, m_lootSet         ( false )
{
    m_type      = EntityType::TYPE_CHARACTER;

    setupStats();

    // Set hp/rp regeneration to one second
    m_regTimer.setLimit( 1.0 );
}

CCharacter::~CCharacter() {
    if( m_aiCharComponent != nullptr ) {
        delete m_aiCharComponent;
        m_aiCharComponent = nullptr;
    }

    m_castBuffer = nullptr;

    while( !m_abilities.empty() )       delete m_abilities.back(),      m_abilities.pop_back();
    while( !m_effects.empty() )         delete m_effects.back(),        m_effects.pop_back();
    while( !m_effectsStat.empty() )     delete m_effectsStat.back(),    m_effectsStat.pop_back();
}

void CCharacter::update( const sf::Time& delta ) {
    if( m_aiCharComponent != nullptr ) {
        m_aiCharComponent->update( delta );
    }

    if( m_castBuffer == nullptr ) {
        CEntity::update( delta );
    }

    updateAttack( delta );
    updateAbilities( delta );
    updateEffects( delta );

    // Hp/Rp regeneration
    updateRegens( delta );
}

void CCharacter::setAiCharComponent( CAiCharComponent* component ) {
    component->setParent( this );

    m_aiCharComponent   = component;
}

void CCharacter::performAttack( float angle ) {
    if( !m_attTimer.isTime() ) return;

    lua_State* state = CGame::ScriptSystem.getState();

    lua_createtable( state, 0, 4 );

    lua_pushstring( state, "x" );
    lua_pushnumber( state, getCenter().x );
    lua_settable( state, - 3 );

    lua_pushstring( state, "y" );
    lua_pushnumber( state, getCenter().y );
    lua_settable( state, - 3 );

    lua_pushstring( state, "angle" );
    lua_pushnumber( state, angle );
    lua_settable( state, - 3 );

    lua_pushstring( state, "distance" );
    lua_pushnumber( state, 300 );
    lua_settable( state, - 3 );

    int statIndex = lua_gettop( state );

    lua_pushinteger( state, m_modStats.damageMin );
    lua_setfield( state, statIndex, "dmgMin" );

    lua_pushinteger( state, m_modStats.damageMax );
    lua_setfield( state, statIndex, "dmgMax" );

    lua_pushnumber( state, m_modStats.critChance );
    lua_setfield( state, statIndex, "critChance" );

    lua_pushnumber( state, m_modStats.critMultip );
    lua_setfield( state, statIndex, "critMultip" );

    lua_pushinteger( state, (int)m_faction );
    lua_setfield( state, statIndex, "faction" );

    CGame::ScriptSystem.fireEventWithTable( "SpawnProjectile", lua_gettop( state ) );

    lua_pop( state, 1 );

    int facingDir = getFacingDir( angle ) + 4;

    if( (int)m_animation.getRowIndex() != facingDir ) {
        m_animation.setRowIndex( facingDir );
    }

    m_animation.setFactorFrameTime( m_attTimer.getLimit().asSeconds() / 1 );

    m_attTimer.reset();
}

void CCharacter::takeDamage( size_t sourceID, int damage, DamageType type ) {
    for( auto it : m_effectsEvent[ (int)EffectEventType::EEVENT_ONDMG ] ) {
//        CScriptSystem::ArgType args[ 2 ];
//        args[ 0 ] = CScriptSystem::ARG_INT;
//        args[ 1 ] = CScriptSystem::ARG_INT;

        it->onEventAction( CGame::EntitySystem.getEntityByID( sourceID ), damage );
    }

    if( !m_health.remove( damage ) || m_health.isEmpty() ) {
        m_despawn = true;
    }
}

int CCharacter::calculateReductions( int damage ) {
    // Temporary empty, no reductions are implemented yet

    return damage;
}

void CCharacter::setBaseStats( const CharacterStats& stats ) {
    m_baseStats = stats;
    m_modStats  = stats;

    setupStats();

    updateMovementSpeed();
    m_attTimer.setLimitOnly( m_modStats.attackSpd );
    m_attTimer.setProgress( m_modStats.attackSpd );
}

void CCharacter::setClass( const CharacterClass& charClass ) {
    m_class     = charClass;
}

void CCharacter::setFaction( const CharacterFaction& faction ) {
    m_faction   = faction;
}

void CCharacter::addBaseStats( const CharacterStats& stats ) {
    m_baseStats += stats;
    m_modStats  += stats;

    setupStats();

    updateMovementSpeed();
    m_attTimer.setLimitOnly( m_modStats.attackSpd );
    m_attTimer.setProgress( m_modStats.attackSpd );
}

void CCharacter::removeBaseStats( const CharacterStats& stats ) {
    m_baseStats -= stats;
    m_modStats  -= stats;

    setupStats();

    updateMovementSpeed();
    m_attTimer.setLimitOnly( m_modStats.attackSpd );
    m_attTimer.setProgress( m_modStats.attackSpd );
}

size_t CCharacter::addStat( StatIndex index, float value, CalcOperation operand ) {
    size_t ID = m_stats[ (int)index ].addModifier( value, operand );

    return ID;
}

size_t CCharacter::addStatAndUpdate( StatIndex index, float value, CalcOperation operand ) {
    size_t ID = m_stats[ (int)index ].addModifier( value, operand );

    updateStats();

    return ID;
}

void CCharacter::delStat( StatIndex index, size_t ID ) {
    m_stats[ (int)index ].delModifier( ID );
}

void CCharacter::delStatAndUpdate( StatIndex index, size_t ID ) {
    m_stats[ (int)index ].delModifier( ID );

    updateStats();
}

void CCharacter::updateStats() {
    /** Independent stats */
    m_modStats.STR              = m_stats[ 2 ].getResult();
    m_modStats.DEX              = m_stats[ 3 ].getResult();
    m_modStats.INT              = m_stats[ 4 ].getResult();
    m_modStats.VIT              = m_stats[ 5 ].getResult();
    m_modStats.Armor            = m_stats[ 6 ].getResult();

    /** Update bonus stats for dependencies */
    m_stats[ 0 ].setBonus( m_modStats.VIT );

    if( m_class == CharacterClass::SORCERER || m_class == CharacterClass::NECROMANCER ) {
        m_stats[ 1 ].setBonus( m_modStats.INT );
    }

    m_stats[ 7 ].setBonus( m_modStats.STR );

    if( m_class == CharacterClass::RANGER || m_class == CharacterClass::ASSASSIN ) {
        m_stats[ 8 ].setBonus( m_modStats.DEX );
    }
    else if( m_class == CharacterClass::SORCERER || m_class == CharacterClass::NECROMANCER ) {
        m_stats[ 8 ].setBonus( m_modStats.INT );
    }

    if( m_class == CharacterClass::RANGER || m_class == CharacterClass::ASSASSIN ) {
        m_stats[ 9 ].setBonus( m_modStats.DEX );
    }

    m_stats[ 10 ].setBonus( m_modStats.DEX );

    if( m_class == CharacterClass::BARBARIAN || m_class == CharacterClass::KNIGHT ) {
        m_stats[ 12 ].setBonus( m_modStats.STR );
        m_stats[ 13 ].setBonus( m_modStats.STR );
    }
    else if( m_class == CharacterClass::RANGER || m_class == CharacterClass::ASSASSIN ) {
        m_stats[ 12 ].setBonus( m_modStats.DEX );
        m_stats[ 13 ].setBonus( m_modStats.DEX );
    }
    else if( m_class == CharacterClass::SORCERER || m_class == CharacterClass::NECROMANCER ) {
        m_stats[ 12 ].setBonus( m_modStats.INT );
        m_stats[ 13 ].setBonus( m_modStats.INT );
    }

    m_stats[ 14 ].setBonus( m_modStats.DEX );
    m_stats[ 15 ].setBonus( m_modStats.INT );

    m_stats[ 16 ].setBase( m_expFactor );

    /** Dependent stats */
    m_health.setMaximumFactor(    m_stats[ 0 ].getResult() );
    m_resource.setMaximumFactor(  m_stats[ 1 ].getResult() );

    m_modStats.regenHealth      = m_stats[ 7 ].getResult();
    m_modStats.regenResource    = m_stats[ 8 ].getResult();
    m_modStats.movementSpd      = m_stats[ 9 ].getResult();
    m_modStats.attackSpd        = m_stats[ 10 ].getResult();
    m_modStats.damageMin        = m_stats[ 12 ].getResult();
    m_modStats.damageMax        = m_stats[ 13 ].getResult();
    m_modStats.critChance       = m_stats[ 14 ].getResult();
    m_modStats.critMultip       = m_stats[ 15 ].getResult();

    updateMovementSpeed();
    m_attTimer.setLimitOnly( m_modStats.attackSpd );

    onStatChange();
}

CharacterStats CCharacter::getBaseStats() const {
    return m_baseStats;
}

CharacterStats CCharacter::getModStats() const {
    return m_modStats;
}

CharacterClass CCharacter::getClass() const {
    return m_class;
}

CharacterFaction CCharacter::getFaction() const {
    return m_faction;
}

void CCharacter::setLevel( size_t level ) {
    int difference = (int)level - (int)m_level;

    m_level     = level;

    calcLevelUpStats( difference );
}

void CCharacter::setExpOnKill( size_t amount ) {
    m_expOnKill = amount;
}

void CCharacter::addExp( size_t amount ) {
    m_exp += ( (float)amount * m_stats[ 16 ].getResult() );

    if( m_exp >= m_expToLevel ) {
        levelUp();
    }
}

size_t CCharacter::getExp() const {
    return m_exp;
}

size_t CCharacter::getExpOnKill() const {
    return m_expOnKill;
}

size_t CCharacter::getExpToLevel() const {
    return m_expToLevel;
}

size_t CCharacter::getLevel() const {
    return m_level;
}

void CCharacter::setMaxHealth( int maximum ) {
    m_health.setMaximum( maximum );
    m_health.setCurrent( maximum );

    setupStats();
}

void CCharacter::setMaxResource( int maximum ) {
    m_resource.setMaximum( maximum );
    m_resource.setCurrent( maximum );

    setupStats();
}

int CCharacter::getMaximumHealth() const {
    return m_health.getMaximum();
}

int CCharacter::getCurrentHealth() const {
    return m_health.getCurrent();
}

int CCharacter::getMaximumResource() const {
    return m_resource.getMaximum();
}

int CCharacter::getCurrentResource() const {
    return m_resource.getCurrent();
}

bool CCharacter::isDead() const {
    return m_health.isEmpty();
}

bool CCharacter::isAttacking() const {
    return !m_attTimer.isTime();
}

int CCharacter::addAbility( CAbility* ability ) {
    if( ability == nullptr ) return - 1;

    ability->setParent( this );

    m_abilities.push_back( ability );

    onNewAbility( m_abilities.size() - 1 );

    return m_abilities.size() - 1;
}

void CCharacter::setAbility( size_t index, CAbility* ability ) {
    if( index >= m_abilities.size() || ability == nullptr ) return;

    ability->setParent( this );

    m_abilities[ index ] = ability;

    onNewAbility( index );
}

void CCharacter::castAbility( size_t index, const sf::Vector2f& destination ) {
    // Make sure index is not wrong or character is not casting anything atm
         if( index >= m_abilities.size() )          return;
    else if( m_castBuffer != nullptr )              return;
    else if( m_abilities[ index ]->isCooldown() )   return;

    m_castBuffer        = m_abilities[ index ];
    m_castDestionation  = destination;
    m_castAngle         = Util::angleByTarget( getCenter(), destination );

    if( m_resource.canRemove( m_castBuffer->getStats().cost ) ) {
        // If ability is instant, cast it immediately
        if( m_castBuffer->isInstant() ) {
            finishCasting();
        }
        else {
            m_castTimer.setLimit( m_castBuffer->getStats().castTime );
        }

        m_animation.resetIndex();
        updateDirOfAnimation( &m_castAngle );
    }
}

const CAbility* CCharacter::getAbility( size_t index ) const {
    if( index >= m_abilities.size() ) return nullptr;

    return m_abilities[ index ];
}

bool CCharacter::isCasting() const {
    return m_castBuffer != nullptr;
}

CTimer CCharacter::getCastingTimer() const {
    return m_castTimer;
}

void CCharacter::addEffect( CEffect* effect ) {
    if( effect == nullptr ) return;

    effect->setParent( this );
    effect->onAplication();

    m_effects.push_back( effect );
}

void CCharacter::addEffectStat( CEffectStat* effect ) {
    if( effect == nullptr ) return;

    effect->setParent( this );
    effect->doStatAdjustment();
    effect->onAplication();

    m_effectsStat.push_back( effect );
}

void CCharacter::addEffectEvent( CEffectEvent* effect, EffectEventType type ) {
    if( effect == nullptr || type == EffectEventType::EEVENT_NONE ) return;

    effect->setParent( this );
    effect->onAplication();

    m_effectsEvent[ (int)type ].push_back( effect );
}

void CCharacter::delEffect( const std::string& ID ) {
    for( auto it = m_effects.begin(); it != m_effects.end(); it++ ) {
        if( (*it)->getData().ID == ID ) {
            (*it)->onRemove();

            delete *it;

            m_effects.erase( it );

            return;
        }
    }

    for( auto it = m_effectsStat.begin(); it != m_effectsStat.end(); it++ ) {
        if( (*it)->getData().ID == ID ) {
            (*it)->onRemove();
            (*it)->removeStatAdjustment();

            delete *it;

            m_effectsStat.erase( it );

            return;
        }
    }

    for( int i = 0; i < (int)EffectEventType::EEVENT_COUNT; i++ ) {
        for( auto it = m_effectsEvent[ i ].begin(); it != m_effectsEvent[ i ].end(); it++ ) {
            if( (*it)->getData().ID == ID ) {
                (*it)->onRemove();

                delete *it;

                m_effectsEvent[ i ].erase( it );

                return;
            }
        }
    }
}

void CCharacter::delEffect( const std::string& ID, const EffectSubType& subtype ) {
    if( subtype == EffectSubType::EFFECT_DEFAULT ) {
        for( auto it = m_effects.begin(); it != m_effects.end(); it++ ) {
            if( (*it)->getData().ID == ID ) {
                (*it)->onRemove();

                delete *it;

                m_effects.erase( it );

                return;
            }
        }
    }
    else if( subtype == EffectSubType::EFFECT_STAT ) {
        for( auto it = m_effectsStat.begin(); it != m_effectsStat.end(); it++ ) {
            if( (*it)->getData().ID == ID ) {
                (*it)->onRemove();
                (*it)->removeStatAdjustment();

                delete *it;

                m_effectsStat.erase( it );

                return;
            }
        }
    }
    else if( subtype == EffectSubType::EFFECT_EVENT ) {
        for( int i = 0; i < (int)EffectEventType::EEVENT_COUNT; i++ ) {
            for( auto it = m_effectsEvent[ i ].begin(); it != m_effectsEvent[ i ].end(); it++ ) {
                if( (*it)->getData().ID == ID ) {
                    (*it)->onRemove();

                    delete *it;

                    m_effectsEvent[ i ].erase( it );

                    return;
                }
            }
        }
    }
}

void CCharacter::setLootTable( const lootTable& table ) {
    m_lootTable     = table;

    m_lootSet       = true;
}

lootTable CCharacter::getLootTable() const {
    return m_lootTable;
}

bool CCharacter::hasLootTable() const {
    return m_lootSet;
}

/** PRIVATE */

void CCharacter::setupStats() {
    for( size_t i = 0; i < (size_t)StatIndex::STAT_COUNT; i++ ) {
        m_stats[ i ] = CCharStat();
    }

    std::function<float(float value)> moveSpdFormula = []( float value ){ return 250 * ( 1 / ( 1 + powf( 1.008, - 1 * value ) ) - 0.5 ) * 2; };

    /** Health  */
    // STR based class has higher vit potential
    if( m_class == CharacterClass::BARBARIAN || m_class == CharacterClass::KNIGHT ) {
        m_stats[ 0 ].setCalcFunctionBonus( []( float value, float bonus ){ return (int)( ( value + bonus * 4 )+ 0.5 ); } );
    }
    else {
        m_stats[ 0 ].setCalcFunctionBonus( []( float value, float bonus ){ return (int)( ( value + bonus * 2 )+ 0.5 ); } );
    }

    m_stats[ 0 ].setBonus( m_baseStats.VIT );

    /** Resource */
    // Only INT based classes have bonus to resource
    if( m_class == CharacterClass::SORCERER || m_class == CharacterClass::NECROMANCER ) {
        m_stats[ 1 ].setCalcFunctionBonus( []( float value, float bonus ){ return (int)( ( value + bonus ) + 0.5 ); } );
        m_stats[ 1 ].setBonus( m_baseStats.INT );
    }
    else {
        m_stats[ 1 ].setCalcFunction( []( float value ){ return (int)( value + 0.5 ); } );
    }

    /** Character base stats */
    m_stats[ 2 ].setCalcFunction( []( float value ){ return (int)( value + 0.5 ); } );  // STR
    m_stats[ 3 ].setCalcFunction( []( float value ){ return (int)( value + 0.5 ); } );  // DEX
    m_stats[ 4 ].setCalcFunction( []( float value ){ return (int)( value + 0.5 ); } );  // INT
    m_stats[ 5 ].setCalcFunction( []( float value ){ return (int)( value + 0.5 ); } );  // VIT
    m_stats[ 6 ].setCalcFunction( []( float value ){ return (int)( value + 0.5 ); } );  // Armor

    /** Health regen */
    // STR based class has higher regen potential
    if( m_class == CharacterClass::BARBARIAN || m_class == CharacterClass::KNIGHT ) {
        m_stats[ 7 ].setCalcFunctionBonus( []( float value, float bonus ){ return (int)( ( value + bonus * 0.1 ) + 0.5 ); } );
    }
    else {
        m_stats[ 7 ].setCalcFunctionBonus( []( float value, float bonus ){ return (int)( ( value + bonus * 0.05 ) + 0.5 ); } );
    }

    m_stats[ 7 ].setBonus( m_baseStats.STR );

    /** Resource regen */
    // Only INT and DEX based classes have resource regen increased by base stat
    if( m_class == CharacterClass::SORCERER || m_class == CharacterClass::NECROMANCER ) {
        m_stats[ 8 ].setCalcFunctionBonus( []( float value, float bonus ){ return (int)( ( value + bonus * 0.1 ) + 0.5 ); } );
        m_stats[ 8 ].setBonus( m_baseStats.INT );
    }
    else if( m_class == CharacterClass::RANGER || m_class == CharacterClass::ASSASSIN ) {
        m_stats[ 8 ].setCalcFunctionBonus( []( float value, float bonus ){ return (int)( ( value + bonus * 0.1 ) + 0.5 ); } );
        m_stats[ 8 ].setBonus( m_baseStats.DEX );
    }
    else {
        m_stats[ 8 ].setCalcFunction( []( float value ){ return (int)( value + 0.5 ); } );
    }

    /** Movement speed */
    // DEX based class has small bonus from base stats
    if( m_class == CharacterClass::RANGER || m_class == CharacterClass::ASSASSIN ) {
        m_stats[ 9 ].setCalcFunctionBonus( []( float value, float bonus ){ return 250 * ( 1 / ( 1 + powf( 1.008, - 1 * ( value + bonus * 0.2 ) ) ) - 0.5 ) * 2; } );
        m_stats[ 9 ].setBonus( m_baseStats.DEX );
    }
    else {
        m_stats[ 9 ].setCalcFunction( []( float value ){ return 250 * ( 1 / ( 1 + powf( 1.008, - 1 * value ) ) - 0.5 ) * 2; } );
    }

    /** Attack speed */
    // DEX based class has higher bonus potential
    if( m_class == CharacterClass::RANGER || m_class == CharacterClass::ASSASSIN ) {
        m_stats[ 10 ].setCalcFunctionBonus( []( float value, float bonus ){ return 3.0 + ( 2.8 ) * ( 1 / ( 1 + powf( 1.008, - 1 * ( value + bonus ) ) ) - 0.5 ) * ( - 2 ); } );
    }
    else {
        m_stats[ 10 ].setCalcFunctionBonus( []( float value, float bonus ){ return 3.0 + ( 2.8 ) * ( 1 / ( 1 + powf( 1.008, - 1 * ( value + bonus * 0.5 ) ) ) - 0.5 ) * ( - 2 ); } );
    }

    m_stats[ 10 ].setBonus( m_baseStats.DEX );

    /** Casting speed */
    m_stats[ 11 ].setCalcFunction( []( float value ){ return value; } );  // TEMP

    /** Damage */
    m_stats[ 12 ].setCalcFunctionBonus( []( float value, float bonus ){ return (int)( ( value + bonus * 0.05 ) + 0.5 ); } );  // MIN DMG
    m_stats[ 13 ].setCalcFunctionBonus( []( float value, float bonus ){ return (int)( ( value + bonus * 0.05 ) + 0.5 ); } );  // MAX DMG

    if( m_class == CharacterClass::BARBARIAN || m_class == CharacterClass::KNIGHT ) {
        m_stats[ 12 ].setBonus( m_baseStats.STR );
        m_stats[ 13 ].setBonus( m_baseStats.STR );
    }
    else if( m_class == CharacterClass::RANGER || m_class == CharacterClass::ASSASSIN ) {
        m_stats[ 12 ].setBonus( m_baseStats.DEX );
        m_stats[ 13 ].setBonus( m_baseStats.DEX );
    }
    else if( m_class == CharacterClass::SORCERER || m_class == CharacterClass::NECROMANCER ) {
        m_stats[ 12 ].setBonus( m_baseStats.INT );
        m_stats[ 13 ].setBonus( m_baseStats.INT );
    }

    /** Critical hit chance */
    // DEX based class has higher dex potential
    if( m_class == CharacterClass::RANGER || m_class == CharacterClass::ASSASSIN ) {
        m_stats[ 14 ].setCalcFunctionBonus( []( float value, float bonus ){ return 100 * ( 1 / ( 1 + powf( 1.008, - 1 * ( value + bonus * 0.4 ) ) ) - 0.5 ) * 2; } );
        m_stats[ 14 ].setBonus( m_baseStats.DEX );
    }
    else {
        m_stats[ 14 ].setCalcFunctionBonus( []( float value, float bonus ){ return 100 * ( 1 / ( 1 + powf( 1.008, - 1 * ( value + bonus * 0.2 ) ) ) - 0.5 ) * 2; } );
        m_stats[ 14 ].setBonus( m_baseStats.DEX );
    }


    /** Critical hit multiplier */
    // DEX based class has higher dex potential
    if( m_class == CharacterClass::RANGER || m_class == CharacterClass::ASSASSIN ) {
        m_stats[ 15 ].setCalcFunctionBonus( []( float value, float bonus ){ return 5.0 * ( 1 / ( 1 + powf( 1.008, - 1 * ( value + bonus * 0.02 ) ) ) - 0.5 ) * 2; } );
        m_stats[ 15 ].setBonus( m_baseStats.INT );
    }
    else {
        m_stats[ 15 ].setCalcFunctionBonus( []( float value, float bonus ){ return 5.0 * ( 1 / ( 1 + powf( 1.008, - 1 * ( value + bonus * 0.01 ) ) ) - 0.5 ) * 2; } );
        m_stats[ 15 ].setBonus( m_baseStats.INT );
    }

    m_stats[ 16 ].setCalcFunction( []( float value ){ return value; } );

    m_stats[ 0 ].setBase(  m_health.getMaximum() );
    m_stats[ 1 ].setBase(  m_resource.getMaximum() );
    m_stats[ 2 ].setBase(  m_baseStats.STR );
    m_stats[ 3 ].setBase(  m_baseStats.DEX );
    m_stats[ 4 ].setBase(  m_baseStats.INT );
    m_stats[ 5 ].setBase(  m_baseStats.VIT );
    m_stats[ 6 ].setBase(  m_baseStats.Armor );
    m_stats[ 7 ].setBase(  m_baseStats.regenHealth );
    m_stats[ 8 ].setBase(  m_baseStats.regenResource );
    m_stats[ 9 ].setBase(  m_baseStats.movementSpd );
    m_stats[ 10 ].setBase( m_baseStats.attackSpd );
    m_stats[ 11 ].setBase( m_baseStats.castingSpd );
    m_stats[ 12 ].setBase( m_baseStats.damageMin );
    m_stats[ 13 ].setBase( m_baseStats.damageMax );
    m_stats[ 14 ].setBase( m_baseStats.critChance );
    m_stats[ 15 ].setBase( m_baseStats.critMultip );
    m_stats[ 16 ].setBase( m_expFactor );

    updateStats();
}

void CCharacter::updateAnimation( const sf::Time& delta ) {
    if( m_animation.isDirectional() ) {
        if( isAttacking() ) {
            m_animation.update( delta );
        }
        else if( !isCasting() ) {
            if( m_moveSpeed.x != 0 || m_moveSpeed.y != 0 ) {
                updateDirOfAnimation();
                m_animation.update( delta );
            }
            else {
                m_animation.resetIndex();
            }
        }
    }
    else {
        m_animation.update( delta );
    }
}

void CCharacter::updateMovementSpeed() {
    // m_moveVelocity = 500 / ( 1 + powf( 2.72, ( - 0.025 * m_modStats.movementSpd ) ) ) - 250;

    //      Result      base                                           mod
    // m_moveVelocity = 250 * ( 1 / ( 1 + powf( 1.008, - 1 * m_modStats.movementSpd ) ) - 0.5 ) * 2;
//    m_moveVelocity = m_stats[ (int)StatIndex::STAT_MOVEMENTSPD ].getResult();
    m_moveVelocity = m_modStats.movementSpd;

    m_animation.setFactorFrameTime( 100 / m_moveVelocity );
}

void CCharacter::updateRegens( const sf::Time& delta ) {
    m_regTimer.update( delta );

    if( m_regTimer.isTime() ) {
        m_health.add(   m_modStats.regenHealth );
        m_resource.add( m_modStats.regenResource );

        m_regTimer.reset();
    }
}

void CCharacter::levelUp() {
    m_level++;

    // Keep experience points that overflowed
    if( m_exp > m_expToLevel ) {
        m_exp = m_exp - m_expToLevel;
    }
    else {
        m_exp = 0;
    }

    // Increase required exp to level up
    m_expToLevel = m_expToLevel * 1.25;

    calcLevelUpStats();
}

void CCharacter::calcLevelUpStats( int level ) {
    if( m_class == CharacterClass::BARBARIAN ) {
        addStat( StatIndex::STAT_STR, 3 * level, CalcOperation::CALC_ADD );
        addStat( StatIndex::STAT_VIT, 1 * level, CalcOperation::CALC_ADD );
    }
    else if( m_class == CharacterClass::KNIGHT ) {
        addStat( StatIndex::STAT_STR, 1 * level, CalcOperation::CALC_ADD );
        addStat( StatIndex::STAT_VIT, 3 * level, CalcOperation::CALC_ADD );
    }

    if( m_class == CharacterClass::RANGER || m_class == CharacterClass::ASSASSIN ) {
        addStat( StatIndex::STAT_DEX, 3 * level, CalcOperation::CALC_ADD );
        addStat( StatIndex::STAT_VIT, 1 * level, CalcOperation::CALC_ADD );
    }

    if( m_class == CharacterClass::SORCERER || m_class == CharacterClass::NECROMANCER ) {
        addStat( StatIndex::STAT_INT, 3 * level, CalcOperation::CALC_ADD );
        addStat( StatIndex::STAT_VIT, 1 * level, CalcOperation::CALC_ADD );
    }

    updateStats();
}

void CCharacter::updateAttack( const sf::Time& delta ) {
    if( !m_attTimer.isTime() ) {
        m_attTimer.update( delta );

        // Fix movement animation speed
        if( m_attTimer.isTime() ) {
            m_animation.setFactorFrameTime( 100 / m_moveVelocity );
        }
    }
}

void CCharacter::updateAbilities( const sf::Time& delta ) {
    for( auto it : m_abilities ) {
        it->update( delta );
    }

    // Cast the ability ( if there is anything to cast )
    if( m_castBuffer != nullptr ) {
        updateCasting( delta );
    }
}

void CCharacter::updateCasting( const sf::Time& delta ) {
    m_castTimer.update( delta );

    if( m_castTimer.isTime() ) {
        finishCasting();
    }
}

void CCharacter::finishCasting() {
    // Double check if there is ability to cast
    if( m_castBuffer == nullptr ) return;

    if( m_resource.canRemove( m_castBuffer->getStats().cost ) ) {
        m_resource.remove( m_castBuffer->getStats().cost );

        m_castBuffer->onCast( m_castDestionation );

        // Call virtual function with casted ability index
        for( size_t i = 0; i < m_abilities.size(); i++ ) {
            if( m_castBuffer == m_abilities[ i ] ) {
                onAbilityCast( i );
            }
        }
    }

    m_castBuffer = nullptr;
}

void CCharacter::updateEffects( const sf::Time& delta ) {
    for( auto it = m_effects.begin(); it != m_effects.end(); ) {
        if( !(*it)->isActive() ) {
            (*it)->onRemove();

            delete *it;

            it = m_effects.erase( it );
        }
        else {
            (*it)->update( delta );

            it++;
        }
    }

    for( auto it = m_effectsStat.begin(); it != m_effectsStat.end(); ) {
        if( !(*it)->isActive() ) {
            (*it)->removeStatAdjustment();
            (*it)->onRemove();

            delete *it;

            it = m_effectsStat.erase( it );
        }
        else {
            (*it)->update( delta );

            it++;
        }
    }

    for( int i = 0; i < (int)EffectEventType::EEVENT_COUNT; i++ ) {
        for( auto it = m_effectsEvent[ i ].begin(); it != m_effectsEvent[ i ].end(); ) {
            if( !(*it)->isActive() ) {
                (*it)->onRemove();

                delete *it;

                it = m_effectsEvent[ i ].erase( it );
            }
            else {
                (*it)->update( delta );

                it++;
            }
        }
    }
}


