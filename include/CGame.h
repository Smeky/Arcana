#ifndef CGAME_H
#define CGAME_H

#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"

#include "CScriptSystem.h"
#include "CInterfaceSystem.h"
#include "CMessageSystem.h"
#include "CEntitySystem.h"
#include "CSceneManager.h"
#include "CAssetManager.h"
#include "CWorldManager.h"
#include "CSpatialSystem.h"
#include "CZBuffer.h"
#include "CDisplayStats.h"
#include "CAbilityManager.h"
#include "CEffectManager.h"
#include "CUpgradeManager.h"

#include "Circle.h"

class CGame {
public:
    static CScriptSystem        ScriptSystem;
    static CInterfaceSystem     InterfaceSystem;
    static CMessageSystem       MessageSystem;
    static CEntitySystem        EntitySystem;
    static CSceneManager        SceneManager;
    static CAssetManager        AssetManager;
    static CWorldManager        WorldManager;
    static CSpatialSystem       SpatialSystem;
    static CZBuffer             ZBuffer;
    static CAbilityManager      AbilityManager;
    static CEffectManager       EffectManager;
    static CUpgradeManager      UpgradeManager;

    static sf::RenderWindow     GameWindow;
    static sf::Clock            GameClock;

    static size_t               SceneID;

public:
    bool            init            ();
    static void     close           ();

    void            run             ();

    static void     changeScene     ();
    static size_t   getSceneID      ();

public:
    static int      luaCloseGame    ( lua_State* state );
    static int      luaGetWindowSize( lua_State* state );
    static int      luaGetGameTime  ( lua_State* state );
    static int      luaChangeScene  ( lua_State* state );
    static int      luaGetSceneID   ( lua_State* state );
    static int      luaLoadScript   ( lua_State* state );
    static int      luaSendMsg      ( lua_State* state );
    static int      luaSendDelayMsg ( lua_State* state );

private:
    void            setupWindow     ();

    void            handleInput     ();
    void            update          ();
    void            render          ();

private:
    sf::Clock       m_loopClock;

    CDisplayStats   m_displayStats;

    static bool     m_changeScene;

};

#endif // CGAME_H
