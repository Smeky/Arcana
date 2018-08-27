#ifndef CWORLDMANAGER_H
#define CWORLDMANAGER_H

#include <list>

#include <CCamera.h>
#include <CCombatTextSystem.h>
#include <CParticleSystem.h>
#include <CLootSystem.h>

struct lua_State;
class CMap;
class CEntity;

class CWorldManager {
private:
    typedef std::vector<CMap*>      Maps;

public:
    CCamera             Camera;
    CCombatTextSystem   CombatTextSystem;
    CParticleSystem     ParticleSystem;
    CLootSystem         LootSystem;

public:
                    CWorldManager           ();

    void            close                   ();

    void            update                  ( const sf::Time& delta );
    void            render                  ( sf::RenderTarget& window, sf::RenderStates states );

    void            setCameraSize           ( const sf::Vector2f& cameraSize );

    /** Map releated functions */
    size_t          createMap               ( lua_State* state, int index );
    size_t          addMap                  ( CMap* map );
    bool            enterMap                ( size_t ID );
    void            delMap                  ( size_t ID );
    sf::Vector2f    getMapSize              () const;
    CMap*           getCurrentlyUsedMap     () const;

    void            pauseGame               ();
    bool            isPaused                () const;

    void            displaySpatialSystem    ();

public:
    static int      luaClose                ( lua_State* state );
    static int      luaCreateMap            ( lua_State* state );
    static int      luaEnterMap             ( lua_State* state );

    static int      luaSetCameraSize        ( lua_State* state );
    static int      luaGetCameraPos         ( lua_State* state );

    static int      luaCameraMoveTo         ( lua_State* state );
    static int      luaCameraMoveCenterTo   ( lua_State* state );
    static int      luaCameraGetOffset      ( lua_State* state );

    static int      luaPauseGame            ( lua_State* state );
    static int      luaIsGamePaused         ( lua_State* state );

    static int      luaDisplaySpatialSystem ( lua_State* state );

private:
    size_t          getNewMapID             ();

private:
    size_t          m_mapIDCounter;
    Maps            m_maps;

    bool            m_paused;

    bool            m_debugDisplaySpatial;
};

#endif // CWORLDMANAGER_H
