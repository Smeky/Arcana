#ifndef CENTITYSYSTEM_H
#define CENTITYSYSTEM_H

#include "vector"
#include "unordered_map"
#include "list"

#include "SFML/Graphics.hpp"
#include "SFML/System/Time.hpp"


enum class CharacterFaction : int;

enum class DamageType : int {
    DMG_NORMAL,
    DMG_CRIT,
    DMG_REFLECT,
//    DMG_SPELL_NORMAL,
//    DMG_SPELL_CRIT,
};

struct lua_State;
struct CharacterStats;
struct ProjectileStats;
class CDimensional;
class CPlayer;
class CEntity;
class CCharacter;
class CProjectile;
class CInventory;
class CAbility;
class CEffect;
class CAiComponent;

typedef std::vector<const CDimensional*>  C_Dimensionals;

class CEntitySystem {
private:
    typedef std::vector<CCharacter*>    Characters;
    typedef std::vector<CEntity*>       Entities;
    typedef std::vector<CProjectile*>   Projectiles;

public:
                    CEntitySystem       ();
                   ~CEntitySystem       ();

    void            close               ();
    void            cleanUp             ();

    void            handleInput         ( sf::Event event );
    void            update              ( const sf::Time& delta );
    void            render              ( sf::RenderTarget& window, sf::RenderStates states );

    /** Spatial system */
    C_Dimensionals  getNearbySolid      ( const CDimensional* dim );
    bool            checkSphereForColl  ( const sf::Vector2f& pos, float radius );
    bool            isPosFree           ( const sf::Vector2f& pos, const sf::Vector2f& size );

    /** Damage dealing */
    // Old, ID based
    void            attackTarget        ( size_t sourceID, size_t targetID, int damage, DamageType type = DamageType::DMG_NORMAL );
    // New
    void            attackTarget        ( CCharacter* target, CCharacter* source = nullptr, int damage = 0, DamageType type = DamageType::DMG_NORMAL );
    void            attackTarget        ( size_t sourceID, size_t targetID, const ProjectileStats& stats );

    void            playerInteract      ();
    void            interactWithObject  ( size_t ID );

    static int      calculateDamage     ( int min, int max, float critChance, float critMultip, DamageType& type );

    /** Entity type */
    bool            isCharacter         ( size_t ID );

    /** Character faction */
    bool            compareFaction      ( CharacterFaction first, size_t second );
    bool            compareFaction      ( size_t first, size_t second );

    /** Despawns */
    void            despawnEntity       ( size_t ID );
    void            despawnEnemy        ( size_t ID );
    void            despawnObject       ( size_t ID );
    void            despawnProjectile   ( size_t ID );


    /** Player */
    CPlayer*        getPlayer           ();
    CInventory&     getInventory        ();

    sf::Vector2f    getPlayerPos        ();
    sf::Vector2f    getPlayerSize       ();
    sf::Vector2f    getPlayerCenter     ();

    void            addAbilityToPlayer  ( CAbility* ability );

    /** Characters */
    CCharacter*     getCharacter        ( size_t ID );
    CharacterStats  getCharacterStats   ( size_t ID );
   CharacterFaction getCharacterFaction ( size_t ID );

    /** Entities */
    void            setEntityPos        ( size_t ID, const sf::Vector2f& pos );
    void            setEntityCenter     ( size_t ID, const sf::Vector2f& center );
    void            setEntitySize       ( size_t ID, const sf::Vector2f& center );

    sf::Vector2f    getEntityPos        ( size_t ID );
    sf::Vector2f    getEntityCenter     ( size_t ID );
    sf::Vector2f    getEntitySize       ( size_t ID );

    void            setEntityPosRandom  ( CEntity* entity );

    CEntity*        getEntityByID       ( size_t ID );

    /** Loading entities from lua tables */
    size_t          playerFromLuaTable  ( lua_State* state, int index );
    size_t          enemyFromLuaTable   ( lua_State* state, int index );
    size_t          objectFromLuaTable  ( lua_State* state, int index );
    size_t          bulletFromLuatable  ( lua_State* state, int index );

    /** Loading ai from lua table */
    CAiComponent*   loadAiFromLauTable  ( lua_State* state, int index );

    /** Registration of entities */
    size_t          registerPlayer      ();
    size_t          registerEnemy       ( CCharacter* enemy );
    size_t          registerObject      ( CEntity* object );
    size_t          registerProjectile  ( CProjectile* projectile );

    /** Effects */
    void            applyEffect         ( size_t characterID, CEffect* effect );

    /** Debug */
    void            setDisplayEntityBox ();
    void            setDisplayEntityStat();

    /** Manage lua functions */
    void            callLuaFunction     ( size_t entityID, size_t functionID );
    void            callLuaFunctionLUD  ( size_t entityID, size_t functionID, void* lightUserData );
    void            callLuaFunctionInt  ( size_t entityID, size_t functionID, int argc, ... );
    void            callLuaFunctionFloat( size_t entityID, size_t functionID, int argc, ... );
    void            removeLuaFunctions  ( size_t ID );

public:
    /** New Lua functions */
    /** Temp */
    static int      luaGetLightUserData ( lua_State* state );

    /** Dimension set */
    static int      luaSetPos           ( lua_State* state );
    static int      luaSetCenter        ( lua_State* state );
    static int      luaSetSize          ( lua_State* state );
    static int      luaSetPosSize       ( lua_State* state );
    static int      luaSetPosRandom     ( lua_State* state );

    /** Dimension get */
    static int      luaGetPos           ( lua_State* state );
    static int      luaGetCenter        ( lua_State* state );
    static int      luaGetSize          ( lua_State* state );
    static int      luaGetPosSize       ( lua_State* state );

    /** Sprite */
    static int      luaSetClip          ( lua_State* state );
    static int      luaGetClip          ( lua_State* state );

    /** Characters */
    static int      luaGetHealth        ( lua_State* state );
    static int      luaGetResource      ( lua_State* state );
    static int      luaGetExperience    ( lua_State* state );
    static int      luaAddAbility       ( lua_State* state );
    static int      luaSetAbility       ( lua_State* state );
    static int      luaGetAbility       ( lua_State* state );
    static int      luaDealDamage       ( lua_State* state );

    /** Effects */
    static int      luaAddEffect        ( lua_State* state );

    /** Utility */
    static int      luaGetFaction       ( lua_State* state );
    static int      luaCompareFaction   ( lua_State* state );
    static int      luaGetNearby        ( lua_State* state );
    static int      luaGetNearbyChars   ( lua_State* state );

    /** Color */
    static int      luaSetColor         ( lua_State* state );
    static int      luaGetColor         ( lua_State* state );
    static int      luaAddColor         ( lua_State* state );
    static int      luaDelColor         ( lua_State* state );

    /** Particles       - Needs a good way of removing effects ( id based might be complicated ) */
//    static int      luaAddParticles     ( lua_State* state );
//    static int      luaDelParticles     ( lua_State* state );

    /** New Lua functions end */

    /** Utility */
    static int      luaIsPosFree        ( lua_State* state );

    static int      luaCreatePlayer     ( lua_State* state );
    static int      luaCreateEnemy      ( lua_State* state );
    static int      luaCreateObject     ( lua_State* state );
    static int      luaCreateProjectile ( lua_State* state );

    /** Player */
    static int      luaPlayerMoveLeft   ( lua_State* state );
    static int      luaPlayerMoveRight  ( lua_State* state );
    static int      luaPlayerMoveUp     ( lua_State* state );
    static int      luaPlayerMoveDown   ( lua_State* state );
    static int      luaPlayerAttack     ( lua_State* state );
    static int      luaPlayerCastAbility( lua_State* state );

    static int      luaSetPlayerPos     ( lua_State* state );
    static int      luaGetPlayerPos     ( lua_State* state );
    static int      luaGetPlayerSize    ( lua_State* state );
    static int      luaGetPlayerCenter  ( lua_State* state );

    /** Characters */
    static int      luaGetCharacterStats( lua_State* state );

    /** Entities */
    static int      luaGetEntityPos     ( lua_State* state );
    static int      luaGetEntityCenter  ( lua_State* state );
    static int      luaGetEntitySize    ( lua_State* state );

    static int      luaSetEntityPos     ( lua_State* state );
    static int      luaSetEntityCenter  ( lua_State* state );
    static int      luaSetEntitySize    ( lua_State* state );

    /** Interaction */
    static int      luaPlayerInteract   ( lua_State* state );
    static int      luaInteractObject   ( lua_State* state );

    /** Despawns */
    static int      luaDespawnEntity    ( lua_State* state );
    static int      luaDespawnEnemy     ( lua_State* state );
    static int      luaDespawnObject    ( lua_State* state );
    static int      luaDespawnProjectile( lua_State* state );

    /** Effects */
    static int      luaAddEffectToPlayer( lua_State* state );
    static int    luaDelEffectFromPlayer( lua_State* state );

    static int      luaDisplayEntityBox ( lua_State* state );
    static int      luaDisplayEntityStat( lua_State* state );

private:
    size_t          getNewID            ();
    size_t          getNewFunctionID    ();

    void            addToSpatialSystem  ();
    void            addToZBuffer        ();

    void            loadBaseEntityData  ( lua_State* state, int index, CEntity* entity );
    void            loadCharacterData   ( lua_State* state, int index, CCharacter* character );
    void            loadProjectileData  ( lua_State* state, int index, CProjectile* projectile );

    /** Combat */
    void            handleAttack        ( size_t targetID, size_t sourceID, int damage, DamageType type );
    void            handleAttack        ( size_t targetID, int damage, DamageType, int sourceType );

    /** Debug */
    void            displayEntityBox    ( sf::RenderTarget& window, sf::RenderStates states );
    void            displayEntityStat   ( sf::RenderTarget& window, sf::RenderStates states );

    bool            randomEntityPosition( CEntity* entity );
    bool            checkIfPosIsFree    ( const sf::FloatRect& box );

    size_t          registerLuaFunction ( lua_State* state );

    CEntity*        getEntity           ( size_t ID );
    CEntity*        getEntityByMouse    ( const sf::Vector2i& pos );

private:
    size_t          m_IDCounter;
    size_t          m_funcIDCounter;

    CPlayer*        m_player;
    Characters      m_enemies;
    Entities        m_objects;
    Projectiles     m_projectiles;

    std::unordered_map<size_t,std::vector<size_t>>    m_luaFunctions;

    bool            m_debugDispEntityBox;
    bool            m_debugDispEntityStat;
    const CEntity*  m_debugEntityStat;
};

#endif // CENTITYSYSTEM_H
