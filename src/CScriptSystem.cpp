#include "CScriptSystem.h"

#include "cstdio"
#include "cstring"
#include "cstdlib"
#include "cstdarg"
#include "cmath"

#include "CGame.h"
#include "CInputSystem.h"
#include "CInventory.h"

#include <CGuiElement.h>
#include <CGuiButton.h>

#include "iostream"
using namespace std;

std::unordered_map<std::string,std::vector<size_t>> CScriptSystem::s_events;

void CScriptSystem::update() {
    if( m_reloadScrip ) {
        reloadLuaState();
    }
}

void CScriptSystem::prepareLuaState() {
    m_luaState = luaL_newstate();

	luaL_openlibs( m_luaState );

	static const luaL_Reg engineLibs[] =
	{
		{ "register",   registerEvent },
		{ "reload",     CGame::ScriptSystem.luaReloadLuaState },
		{ NULL, NULL }
	};

	const luaL_Reg *engineLibsPtr = engineLibs;

	for ( ; engineLibsPtr->func != NULL; engineLibsPtr++ ) {
		lua_register( m_luaState, engineLibsPtr->name, engineLibsPtr->func );
	}

	luaL_Reg coreFuncs[] =
	{
		{ "closeGame",          CGame::luaCloseGame },
		{ "getWindowSize",      CGame::luaGetWindowSize },
		{ "getGameTime",        CGame::luaGetGameTime },
		{ "changeScene",        CGame::luaChangeScene },
		{ "getSceneID",         CGame::luaGetSceneID },
		{ "loadScript",         CGame::luaLoadScript },
		{ "sendMessage",        CGame::luaSendMsg },
		{ "sendDelayedMessage", CGame::luaSendDelayMsg },

		{ NULL, NULL }
	};

	registerGlobalObject( "Core", coreFuncs );

	luaL_Reg assetFuncs[] =
	{
		{ "loadTexture",        CAssetManager::luaLoadTexture },
		{ "loadFont",           CAssetManager::luaLoadFont },
		{ "getTextureSize",     CAssetManager::luaGetTextureSize },

		{ NULL, NULL }
	};

	registerGlobalObject( "Assets", assetFuncs );

	luaL_Reg inputFuncs[] =
	{
		{ "isKeyPressed",       Input::luaIsKeyPressed },

		{ "isMousePressed",     Input::luaIsMousePressed },
		{ "getMousePos",        Input::luaGetMousePos },

		{ NULL, NULL }
	};

	registerGlobalObject( "Input", inputFuncs );

	luaL_Reg guiFuncs[] =
	{
		{ "close",              CGame::InterfaceSystem.luaClose },

		{ "createWindow",       CGame::InterfaceSystem.luaCreateWindow },
		{ "createSprite",       CGame::InterfaceSystem.luaCreateSprite },
		{ "createText",         CGame::InterfaceSystem.luaCreateText },
		{ "createInventory",    CGame::InterfaceSystem.luaCreateInventory },
		{ "createCastbar",      CGame::InterfaceSystem.luaCreateCastbar },

		{ "deleteSprite",       CGame::InterfaceSystem.luaDeleteSprite },
		{ "deleteText",         CGame::InterfaceSystem.luaDeleteText },

		{ "reverseWindowShown", CGame::InterfaceSystem.luaWindowReverseShown },
		{ "reverseWindowActive",CGame::InterfaceSystem.luaWindowReverseActive },
		{ "setWindowShown",     CGame::InterfaceSystem.luaSetWindowShown },
		{ "setWindowActive",    CGame::InterfaceSystem.luaSetWindowActive },

		{ "isWindowShown",      CGame::InterfaceSystem.luaIsWindowShown },
		{ "isWindowActive",     CGame::InterfaceSystem.luaIsWindowActive },

		{ "hasMousefocus",      CGame::InterfaceSystem.luaHasMousefocus },

		{ "getWindowWidgetsID", CGame::InterfaceSystem.luaGetWindowWidgetsID },
		{ "setWidgetBgTexture",     CGame::InterfaceSystem.luaSetWidgetBgTexture },
		{ "setWidgetMiddleTexture", CGame::InterfaceSystem.luaSetWidgetMiddleTexture },
		{ "setWidgetFrontTexture",  CGame::InterfaceSystem.luaSetWidgetFrontTexture },

		{ "getWidgetBox",       CGame::InterfaceSystem.luaGetWidgetBox },

        { "createTooltip",      CGame::InterfaceSystem.TooltipManager.luaCreateTooltip },
        { "destroyTooltip",     CGame::InterfaceSystem.TooltipManager.luaDestroyTooltip },
        { "hasTooltip",         CGame::InterfaceSystem.TooltipManager.luaHasTooltip },

        // New element, interface system
        { "newElement",         CGame::InterfaceSystem.luaNewElement },
        { "newButton",          CGame::InterfaceSystem.luaNewButton },

        // New element, element pos / size
        { "setPos",             CGuiElement::luaSetPos },
        { "getPos",             CGuiElement::luaGetPos },
        { "setCenter",          CGuiElement::luaSetCenter },
        { "getCenter",          CGuiElement::luaGetCenter },
        { "setSize",            CGuiElement::luaSetSize },
        { "getSize",            CGuiElement::luaGetSize },
        { "setPosSize",         CGuiElement::luaSetPosSize },
        { "getPosSize",         CGuiElement::luaGetPosSize },
        { "setBox",             CGuiElement::luaSetBox },
        { "getBox",             CGuiElement::luaGetBox },
        { "move",               CGuiElement::luaMove },
        { "resize",             CGuiElement::luaResize },

        // New element, rest
        { "addTexture",         CGuiElement::luaAddTexture },
        { "setTexture",         CGuiElement::luaSetTexture },
        { "addText",            CGuiElement::luaAddText },
        { "setText",            CGuiElement::luaSetText },
        { "getTexIDByIndex",    CGuiElement::luaGetTexIDByIndex },
        { "getTextIDByIndex",   CGuiElement::luaGetTextIDByIndex },
        { "getRelPos",          CGuiElement::luaGetRelativePos },
        { "setActive",          CGuiElement::luaSetActive },
        { "isActive",           CGuiElement::luaIsActive },
        { "reverseActive",      CGuiElement::luaReverseActive },
        { "setShown",           CGuiElement::luaSetShown },
        { "isShown",            CGuiElement::luaIsShown },
        { "reverseShown",       CGuiElement::luaReverseShown },

        // New element button
        { "addButtonTexture",   CGuiButton::luaAddButtonTexture },
        { "setButtonTexture",   CGuiButton::luaSetButtonTexture },
        { "addHotkey",          CGuiButton::luaAddHotkey },
        { "setMsg",             CGuiButton::luaSetMsg },
        { "setRightMsg",        CGuiButton::luaSetRMsg },
        { "setPressOffset",     CGuiButton::luaSetPressOffset },
        { "setLocked",          CGuiButton::luaSetLocked },
        { "isLocked",           CGuiButton::luaIsLocked },
        { "addButtonFunction",  CGuiButton::luaAddFunction },

		{ NULL, NULL }
	};

	registerGlobalObject( "Gui", guiFuncs );

	luaL_Reg worldFuncs[] =
	{
		{ "close",              CGame::WorldManager.luaClose },
		{ "setCameraSize",      CGame::WorldManager.luaSetCameraSize },
		{ "getCameraPos",       CGame::WorldManager.luaGetCameraPos },
		{ "createMap",          CGame::WorldManager.luaCreateMap },
		{ "enterMap",           CGame::WorldManager.luaEnterMap },
		{ "loadUpgrades",       CGame::UpgradeManager.luaLoad },
		{ "setupUpgrades",      CGame::UpgradeManager.luaSetup },

		{ "pauseGame",          CGame::WorldManager.luaPauseGame },
		{ "isGamePaused",       CGame::WorldManager.luaIsGamePaused },

		{ "displaySS",          CGame::WorldManager.luaDisplaySpatialSystem },      // Display spatial system
		{ "displayEntityBox",   CGame::EntitySystem.luaDisplayEntityBox },
		{ "displayEntityStat",  CGame::EntitySystem.luaDisplayEntityStat },

		{ "playerInteract",     CGame::EntitySystem.luaPlayerInteract },
		{ "interactWithObject", CGame::EntitySystem.luaInteractObject },

		{ "movePlayerLeft",     CGame::EntitySystem.luaPlayerMoveLeft },
		{ "movePlayerRight",    CGame::EntitySystem.luaPlayerMoveRight },
		{ "movePlayerUp",       CGame::EntitySystem.luaPlayerMoveUp },
		{ "movePlayerDown",     CGame::EntitySystem.luaPlayerMoveDown },
		{ "playerAttack",       CGame::EntitySystem.luaPlayerAttack },
		{ "playerCastAbility",  CGame::EntitySystem.luaPlayerCastAbility },

		{ "setPlayerPos",       CGame::EntitySystem.luaSetPlayerPos },
		{ "getPlayerPos",       CGame::EntitySystem.luaGetPlayerPos },
		{ "getPlayerSize",      CGame::EntitySystem.luaGetPlayerSize },
		{ "getPlayerCenter",    CGame::EntitySystem.luaGetPlayerCenter },

		{ "moveCameraTo",       CGame::WorldManager.luaCameraMoveTo },
		{ "moveCameraCenterTo", CGame::WorldManager.luaCameraMoveCenterTo },
		{ "getCameraOffset",    CGame::WorldManager.luaCameraGetOffset },

		{ "createPlayer",       CGame::EntitySystem.luaCreatePlayer },
		{ "createEnemy",        CGame::EntitySystem.luaCreateEnemy },
		{ "createObject",       CGame::EntitySystem.luaCreateObject },
		{ "createProjectile",   CGame::EntitySystem.luaCreateProjectile },

		{ "despawnEntity",      CGame::EntitySystem.luaDespawnEntity },
		{ "despawnEnemy",       CGame::EntitySystem.luaDespawnEnemy },
		{ "despawnObject",      CGame::EntitySystem.luaDespawnObject },
		{ "despawnProjectile",  CGame::EntitySystem.luaDespawnProjectile },

		{ "getEntityPos",       CGame::EntitySystem.luaGetEntityPos },
		{ "getEntitySize",      CGame::EntitySystem.luaGetEntitySize },
		{ "getEntityCenter",    CGame::EntitySystem.luaGetEntityCenter },

		{ "setEntityPos",       CGame::EntitySystem.luaSetEntityPos },
		{ "setEntitySize",      CGame::EntitySystem.luaSetEntitySize },
		{ "setEntityCenter",    CGame::EntitySystem.luaSetEntityCenter },

        /** Particles */
		{ "createEffect",       CGame::WorldManager.ParticleSystem.luaCreateEffect },
		{ "createParticle",     CGame::WorldManager.ParticleSystem.luaCreateParticle },

		/** Effects*/
		{ "addEffect",          CGame::EntitySystem.luaAddEffect },
		{ "addEffectToPlayer",  CGame::EntitySystem.luaAddEffectToPlayer },
		{ "delEffectFromPlayer",CGame::EntitySystem.luaDelEffectFromPlayer },

		/** Upgrades */
		{ "getUpgrades",        CGame::UpgradeManager.luaGetUpgrades },
		{ "getUpgrade",         CGame::UpgradeManager.luaGetUpgrade },

		{ NULL, NULL }
	};

	registerGlobalObject( "World", worldFuncs );

    luaL_Reg entityFuncs[] =
	{
        { "getLightUserData",   CGame::EntitySystem.luaGetLightUserData },      // Temporary

        /** Dimension set */
        { "setPos",             CGame::EntitySystem.luaSetPos },
        { "setCenter",          CGame::EntitySystem.luaSetCenter },
        { "setSize",            CGame::EntitySystem.luaSetSize },
        { "setPosSize",         CGame::EntitySystem.luaSetPosSize },

        /** Dimension get */
        { "getPos",             CGame::EntitySystem.luaGetPos },
        { "getCenter",          CGame::EntitySystem.luaGetCenter },
        { "getSize",            CGame::EntitySystem.luaGetSize },
        { "getPosSize",         CGame::EntitySystem.luaGetPosSize },

        /** Sprite */
        { "setClip",            CGame::EntitySystem.luaSetClip },
        { "getClip",            CGame::EntitySystem.luaGetClip },

        /** Characters */
        { "getHealth",          CGame::EntitySystem.luaGetHealth },
        { "getResource",        CGame::EntitySystem.luaGetResource },
        { "getCharacterStats",  CGame::EntitySystem.luaGetCharacterStats },
        { "addAbility",         CGame::EntitySystem.luaAddAbility },
        { "setAbility",         CGame::EntitySystem.luaSetAbility },
        { "getAbility",         CGame::EntitySystem.luaGetAbility },
        { "dealDamage",         CGame::EntitySystem.luaDealDamage },

        /** Effects */
        { "addEffect",          CGame::EntitySystem.luaAddEffect },

        /** Utility */
        { "getFaction",         CGame::EntitySystem.luaGetFaction },
        { "compareFaction",     CGame::EntitySystem.luaCompareFaction },
        { "getNearby",          CGame::EntitySystem.luaGetNearby },
        { "getNearbyChars",     CGame::EntitySystem.luaGetNearbyChars },
        { "isPosFree",          CGame::EntitySystem.luaIsPosFree },

        /** Colors */
        { "setColor",           CGame::EntitySystem.luaSetColor },
        { "getColor",           CGame::EntitySystem.luaGetColor },
        { "addColor",           CGame::EntitySystem.luaAddColor },
        { "delColor",           CGame::EntitySystem.luaDelColor },

		{ NULL, NULL }
	};

	registerGlobalObject( "EntitySystem", entityFuncs );

    luaL_Reg inventoryFuncs[] =
	{
		{ "addItem",            CGame::EntitySystem.getInventory().luaAddItem },
		{ "delItem",            CGame::EntitySystem.getInventory().luaDelItem },
		{ "hasItem",            CGame::EntitySystem.getInventory().luaHasItem },
		{ "getItemData",        CGame::EntitySystem.getInventory().luaGetItemData },
		{ "isInventoryFull",    CGame::EntitySystem.getInventory().luaIsFull },
		{ "addRunestones",      CGame::EntitySystem.getInventory().luaAddRunestones },
		{ "removeRunestones",   CGame::EntitySystem.getInventory().luaRemoveRunestones },
		{ "getRunestones",      CGame::EntitySystem.getInventory().luaGetRunestones },
		{ "buyUpgrade",         CGame::EntitySystem.getInventory().luaBuyUpgrade },

		{ NULL, NULL }
	};

	registerGlobalObject( "Inventory", inventoryFuncs );

    m_reloadScrip = false;

    loadScript( "media/scripts/scripts.lua" );

    fireEvent( "ReloadScript" );
}

void CScriptSystem::closeLuaState() {
    s_events.clear();

    lua_close( m_luaState );
}

void CScriptSystem::reloadLuaState() {
    closeLuaState();
    prepareLuaState();
}

int CScriptSystem::luaReloadLuaState( lua_State* luaState ) {
    int argc = lua_gettop( luaState );

    lua_pop( luaState, argc );

    CGame::ScriptSystem.m_reloadScrip = true;

    return 0;
}

void CScriptSystem::loadScript( const std::string& filePath ) {
	if( luaL_loadfile( m_luaState, filePath.c_str() ) == LUA_OK ) {
		// Run the script
		lua_pcall( m_luaState, 0, LUA_MULTRET, 0 );

		// Check if any values were returned */
		int argc = lua_gettop( m_luaState );
		if( argc != 0 ) {
			if( lua_istable( m_luaState, lua_gettop( m_luaState ) ) ) {
				goTroughTable();
			}
		}
	}
	else {
		std::cout << "Error: " << lua_tostring( m_luaState, lua_gettop( m_luaState ) ) << std::endl;

		lua_pop( m_luaState, 1 );
	}
}

void CScriptSystem::fireEvent( const std::string& eventID ) {
    if( !s_events[ eventID ].empty() ) {
		for ( size_t i = 0; i < s_events[ eventID ].size(); i++ ) {

			// Check Lua's registry for the function name
			lua_rawgeti( m_luaState, LUA_REGISTRYINDEX, s_events[ eventID ][ i ] );

			// If the top of the stack is a function
			if( lua_isfunction( m_luaState, lua_gettop( m_luaState ) ) ) {

				// Run function on top of stack
				int result = lua_pcall( m_luaState, 0, LUA_MULTRET, 0 );

                if( result != LUA_OK ) {
                    std::cout << "Error: " << lua_tostring( m_luaState, lua_gettop( m_luaState ) ) << std::endl;
                }
			}
			else {

				// Clean the stack
				lua_pop( m_luaState, 1 );
			}
		}
	}
}

void CScriptSystem::fireEventWithString( const std::string& eventID, int argc, ... ) {
    va_list valist;

    va_start( valist, argc );

    if( !s_events[ eventID ].empty() ) {
		for ( size_t i = 0; i < s_events[ eventID ].size(); i++ ) {

			// Check Lua's registry for the function name
			lua_rawgeti( m_luaState, LUA_REGISTRYINDEX, s_events[ eventID ][ i ] );

			// If the top of the stack is a function
			if( lua_isfunction( m_luaState, lua_gettop( m_luaState ) ) ) {

                for( int i = 0; i < argc; i++ ) {
                    const char* value = va_arg( valist, const char* );

                    lua_pushstring( m_luaState, value );
                }

				// Run function on top of stack
				int result = lua_pcall( m_luaState, argc, LUA_MULTRET, 0 );

                if( result != LUA_OK ) {
                    std::cout << "Error: " << lua_tostring( m_luaState, lua_gettop( m_luaState ) ) << std::endl;
                }
			}
			else {

				// Clean the stack
				lua_pop( m_luaState, 1 );
			}
		}
	}

    va_end( valist );
}

void CScriptSystem::fireEventWithInt( const std::string& eventID, int argc, ... ) {
    va_list valist;

    va_start( valist, argc );

    if( !s_events[ eventID ].empty() ) {
		for ( size_t i = 0; i < s_events[ eventID ].size(); i++ ) {

			// Check Lua's registry for the function name
			lua_rawgeti( m_luaState, LUA_REGISTRYINDEX, s_events[ eventID ][ i ] );

			// If the top of the stack is a function
			if( lua_isfunction( m_luaState, lua_gettop( m_luaState ) ) ) {

                for( int i = 0; i < argc; i++ ) {
                    int value = va_arg( valist, int );

                    lua_pushinteger( m_luaState, value );
                }

				// Run function on top of stack
				int result = lua_pcall( m_luaState, argc, LUA_MULTRET, 0 );

                if( result != LUA_OK ) {
                    std::cout << "Error: " << lua_tostring( m_luaState, lua_gettop( m_luaState ) ) << std::endl;
                }
			}
			else {

				// Clean the stack
				lua_pop( m_luaState, 1 );
			}
		}
	}

    va_end( valist );
}

void CScriptSystem::fireEventWithTable( const std::string& eventID, int index ) {
    if( !s_events[ eventID ].empty() ) {
		for ( size_t i = 0; i < s_events[ eventID ].size(); i++ ) {

			// Check Lua's registry for the function name
			lua_rawgeti( m_luaState, LUA_REGISTRYINDEX, s_events[ eventID ][ i ] );

			// If the top of the stack is a function
			if( lua_isfunction( m_luaState, - 1 ) ) {

                lua_pushvalue( m_luaState, index );

				// Run function on top of stack
				int result = lua_pcall( m_luaState, 1, LUA_MULTRET, 0 );

                if( result != LUA_OK ) {
                    std::cout << "Error: " << lua_tostring( m_luaState, - 1 ) << std::endl;
                }
			}
			else {
				// Clean the stack
				lua_pop( m_luaState, 1 );
			}
		}
	}
}

int CScriptSystem::registerEvent( lua_State* luaState ) {
    int argc = lua_gettop( luaState );

    // If failed hook due to incorrect argument count */
	if ( argc != 2 ) {
		lua_pop( luaState, argc ); // clean stack
		lua_pushnumber( luaState, -1 );

		return 0;
	}

	// Add the function on top of the stack to Lua's registry
	int functionID;
	functionID = luaL_ref( luaState, LUA_REGISTRYINDEX );

	// Grab the function name and clean the stack
	size_t len;

	const char *tempFunc = lua_tolstring( luaState, lua_gettop( luaState ), &len );

	std::string functionName( tempFunc, len );

	lua_pop( luaState, 1 );

	// Add the registry index ID at 'functionName' to a vector that contains a list of ids that have already registered
	s_events[ functionName ].push_back( functionID );

	// successful register
	lua_pushnumber( luaState, 1 );

	return 1;
}

void CScriptSystem::goTroughTable() {
    // stack now contains: -1 => table
	lua_pushnil( m_luaState );

	// stack now contains: -1 => nil; -2 => table
	while( lua_next( m_luaState, -2 ) ) {

		// stack now contains: -1 => value; -2 => key; -3 => table
		// copy the key so that lua_tostring does not modify the original
		lua_pushvalue( m_luaState, -2 );

		// stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
		const char *key = lua_tostring( m_luaState, -1 );

		std::cout << key << ":";

		if( lua_istable( m_luaState, -2 ) ) {
			lua_pushvalue( m_luaState, -2 );
			goTroughTable();
		}
		else {
			const char *value = lua_tostring( m_luaState, -2 );
			std::cout << value << std::endl;
		}

		// pop value + copy of key, leaving original key
		lua_pop( m_luaState, 2 );
		// stack now contains: -1 => key; -2 => table
	}

	// stack now contains: -1 => table (when lua_next returns 0 it pops the key
	// but does not push anything.)
	// Pop table
	lua_pop( m_luaState, 1 );
}

void CScriptSystem::getFieldsFromTable( lua_State *state, int index, const char *fields, ... ) {
    va_list arguments;

    // Allocate room for and make an editable copy of fields
    char* tempField = (char*)malloc( strlen( fields ) + 1 );
    strcpy( tempField, fields );

    const size_t MAX_TOKENS = 25;
    size_t count = 0;

    char* tokens[ MAX_TOKENS ];

    // Tokenise tempField by ' ' delimiter
    for( char* p = strtok( tempField, " " ); p != nullptr; p = strtok( NULL, " " ) ) {
            if( count >= MAX_TOKENS ) {
                break;
            }

            tokens[ count++ ] = p;
    }

    // Total amount of arguments calculated from string
    va_start( arguments, count );

    // Loop through fields
    for( size_t i = 0; i < count; i++ ) {
        // Push the field onto the stack
        lua_getfield( state, index, tokens[ i ] );

        // Get the type of the field on top of the stack */
        int fieldType = lua_type( state, - 1 );

        if( fieldType == LUA_TNUMBER ) {
            float number = lua_tonumber( state, - 1 );

            // Check for whole number
            if( fmod( number, 1 ) == 0 ) {
                *va_arg( arguments, int* ) = number;
            }
            else {
                *va_arg( arguments, float* ) = number;
                cout << "Util: " << number << endl;
            }
        }
        else if( fieldType == LUA_TBOOLEAN ) {
            *va_arg( arguments, bool* ) = lua_toboolean( state, - 1 );
        }
        else if( fieldType == LUA_TTABLE ) {
            *va_arg( arguments, int* ) = lua_gettop( state );
        }
        else if (fieldType == LUA_TSTRING) {
            *va_arg( arguments, const char** ) = lua_tostring( state, - 1 );
        }
        else if( fieldType == LUA_TNIL ) {
            va_arg( arguments, int );
        }

        // Keep tables on the stack so they can be called recursively
        if( fieldType != LUA_TTABLE ) {
            lua_pop( state, 1 );
        }
    }

    // Free previously allocated memory
    free(tempField);
}

lua_State* CScriptSystem::getState() {
    return m_luaState;
}

void CScriptSystem::registerGlobalObject( const std::string& objectName, luaL_Reg* regs ) {
    // Create a new object
	luaL_newmetatable( m_luaState, objectName.c_str() );

	// Duplicate the object on top of the stack
	lua_pushvalue( m_luaState, -1 );

	// Tell the object to point to itself
	lua_setfield( m_luaState, -2, "__index" );

	// Push the functions into the object
	luaL_setfuncs( m_luaState, regs, 0 );

	// Push the object into Lua's global space
	lua_setglobal( m_luaState, objectName.c_str() );
}
