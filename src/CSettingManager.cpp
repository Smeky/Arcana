#include "CSettingManager.h"

#include <fstream>
#include <ostream>
#include <lua.hpp>

#include <CGame.h>
#include <Utility.h>
#include <LuaWrapper.h>

#include <iostream>
using namespace std;

CSettingManager::CSettingManager()
{
    for( bool& enable : m_audioEnable ) {
        enable = false;
    }

    for( float& volume : m_audioVolume ) {
        volume = 0.0;
    }
}

void CSettingManager::init() {
    loadFromFile();
}

void CSettingManager::close() {
    saveToFile();
}

void CSettingManager::loadToTable( lua_State* state ) {
    /** Main table */
    lua_createtable( state, 0, 1 );

    /** Audio */
    lua_createtable( state, 0, 1 );

    /** Audio - Enable */
    lua_createtable( state, 0, 6 );

    lua_pushboolean( state, m_audioEnable[ AudioSettings::SET_AUDIO_MASTER ] );
    lua_setfield( state, - 2, "master" );
    lua_pushboolean( state, m_audioEnable[ AudioSettings::SET_AUDIO_MUSIC ] );
    lua_setfield( state, - 2, "music" );
    lua_pushboolean( state, m_audioEnable[ AudioSettings::SET_AUDIO_EFFECT ] );
    lua_setfield( state, - 2, "effect" );
    lua_pushboolean( state, m_audioEnable[ AudioSettings::SET_AUDIO_VOICE ] );
    lua_setfield( state, - 2, "voice" );
    lua_pushboolean( state, m_audioEnable[ AudioSettings::SET_AUDIO_AMBIENT ] );
    lua_setfield( state, - 2, "ambient" );
    lua_pushboolean( state, m_audioEnable[ AudioSettings::SET_AUDIO_INTERFACE ] );
    lua_setfield( state, - 2, "interface" );

    /** Audio - Enable => Audio */
    lua_setfield( state, - 2, "enable" );

    /** Audio - Volume*/
    lua_createtable( state, 0, 6 );

    lua_pushnumber( state, m_audioVolume[ AudioSettings::SET_AUDIO_MASTER ] );
    lua_setfield( state, - 2, "master" );
    lua_pushnumber( state, m_audioVolume[ AudioSettings::SET_AUDIO_MUSIC ] );
    lua_setfield( state, - 2, "music" );
    lua_pushnumber( state, m_audioVolume[ AudioSettings::SET_AUDIO_EFFECT ] );
    lua_setfield( state, - 2, "effect" );
    lua_pushnumber( state, m_audioVolume[ AudioSettings::SET_AUDIO_VOICE ] );
    lua_setfield( state, - 2, "voice" );
    lua_pushnumber( state, m_audioVolume[ AudioSettings::SET_AUDIO_AMBIENT ] );
    lua_setfield( state, - 2, "ambient" );
    lua_pushnumber( state, m_audioVolume[ AudioSettings::SET_AUDIO_INTERFACE ] );
    lua_setfield( state, - 2, "interface" );

    /** Audio - Volume => Audio */
    lua_setfield( state, - 2, "volume" );

    /** Audio => Main */
    lua_setfield( state, - 2, "audio" );
}

void CSettingManager::settingsFromTable( lua_State* state, int index ) {
    lua_getfield( state, index, "audio" );
    if( lua_istable( state, - 1 ) ) {
        lua_getfield( state, - 1, "enable" );

        if( lua_istable( state, - 1 ) ) {
            int enableIndex = lua_gettop( state );

            bool enable[ AudioSettings::SET_AUDIO_TOTAL ];

            lWrapper::valueFromLuaTable<bool>( state, enableIndex, "master",    enable[ AudioSettings::SET_AUDIO_MASTER ] );
            lWrapper::valueFromLuaTable<bool>( state, enableIndex, "music",     enable[ AudioSettings::SET_AUDIO_MUSIC ] );
            lWrapper::valueFromLuaTable<bool>( state, enableIndex, "effect",    enable[ AudioSettings::SET_AUDIO_EFFECT ] );
            lWrapper::valueFromLuaTable<bool>( state, enableIndex, "voice",     enable[ AudioSettings::SET_AUDIO_VOICE ] );
            lWrapper::valueFromLuaTable<bool>( state, enableIndex, "ambient",   enable[ AudioSettings::SET_AUDIO_AMBIENT ] );
            lWrapper::valueFromLuaTable<bool>( state, enableIndex, "interface", enable[ AudioSettings::SET_AUDIO_INTERFACE ] );

            handleAudioEnable( enable );
        }

        lua_pop( state, 1 );

        lua_getfield( state, - 1, "volume" );

        if( lua_istable( state, - 1 ) ) {
            int volumeIndex = lua_gettop( state );

            float volume[ AudioSettings::SET_AUDIO_TOTAL ];

            lWrapper::valueFromLuaTable<float>( state, volumeIndex, "master",    volume[ AudioSettings::SET_AUDIO_MASTER ] );
            lWrapper::valueFromLuaTable<float>( state, volumeIndex, "music",     volume[ AudioSettings::SET_AUDIO_MUSIC ] );
            lWrapper::valueFromLuaTable<float>( state, volumeIndex, "effect",    volume[ AudioSettings::SET_AUDIO_EFFECT ] );
            lWrapper::valueFromLuaTable<float>( state, volumeIndex, "voice",     volume[ AudioSettings::SET_AUDIO_VOICE ] );
            lWrapper::valueFromLuaTable<float>( state, volumeIndex, "ambient",   volume[ AudioSettings::SET_AUDIO_AMBIENT ] );
            lWrapper::valueFromLuaTable<float>( state, volumeIndex, "interface", volume[ AudioSettings::SET_AUDIO_INTERFACE ] );

            handleAudioVolume( volume );
        }

        lua_pop( state, 1 );
    }

    saveToFile();
}

/** LUA */

int CSettingManager::luaGetSettings( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    CGame::SettingsManager.loadToTable( state );

    return 1;
}

int CSettingManager::luaUpdateSettings( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc == 2 ) {
        if( lua_istable( state, argc ) ) {
            CGame::SettingsManager.settingsFromTable( state, argc );
        }
        else {
            std::cout << "Error: Unable to update settings. Expected table, got " << lua_typename( state, lua_type( state, argc ) ) << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to update settings. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    return 0;
}

/** PRIVATE */

void CSettingManager::loadFromFile() {
    std::ifstream file;

    file.open( "settings.txt" );

    if( file.is_open() ) {
        std::string line;

        while( std::getline( file, line ) ) {
            size_t pos = line.find( ':' );

            // Make sure format is fine
            if( pos < line.size() ) {
                // Store variable name and value
                const std::string name  = line.substr( 0, pos );
                const std::string value = line.substr( pos + 2 );

                // Store value out of string based on its name
                /** Audio - Enable */
                     if( name == "audio_enable_master" )        m_audioEnable[ AudioSettings::SET_AUDIO_MASTER ]    = Util::stringToBool( value );
                else if( name == "audio_enable_music" )         m_audioEnable[ AudioSettings::SET_AUDIO_MUSIC ]     = Util::stringToBool( value );
                else if( name == "audio_enable_effect" )        m_audioEnable[ AudioSettings::SET_AUDIO_EFFECT ]    = Util::stringToBool( value );
                else if( name == "audio_enable_voice" )         m_audioEnable[ AudioSettings::SET_AUDIO_VOICE ]     = Util::stringToBool( value );
                else if( name == "audio_enable_ambient" )       m_audioEnable[ AudioSettings::SET_AUDIO_AMBIENT ]   = Util::stringToBool( value );
                else if( name == "audio_enable_interface" )     m_audioEnable[ AudioSettings::SET_AUDIO_INTERFACE ] = Util::stringToBool( value );

                /** Audio - Volume */
                else if( name == "audio_volume_master" )        m_audioVolume[ AudioSettings::SET_AUDIO_MASTER ]    = Util::stringToFloat( value );
                else if( name == "audio_volume_music" )         m_audioVolume[ AudioSettings::SET_AUDIO_MUSIC ]     = Util::stringToFloat( value );
                else if( name == "audio_volume_effect" )        m_audioVolume[ AudioSettings::SET_AUDIO_EFFECT ]    = Util::stringToFloat( value );
                else if( name == "audio_volume_voice" )         m_audioVolume[ AudioSettings::SET_AUDIO_VOICE ]     = Util::stringToFloat( value );
                else if( name == "audio_volume_ambient" )       m_audioVolume[ AudioSettings::SET_AUDIO_AMBIENT ]   = Util::stringToFloat( value );
                else if( name == "audio_volume_interface" )     m_audioVolume[ AudioSettings::SET_AUDIO_INTERFACE ] = Util::stringToFloat( value );
            }
        }

        file.close();
    }

    setupSettings();
}

void CSettingManager::saveToFile() {
    std::ofstream file;

    file.open( "settings.txt" );

    if( file.is_open() ) {
        /** Audio */
        /** Audio - Enable */
        file << "audio_enable_master: "     << Util::boolToString( m_audioEnable[ AudioSettings::SET_AUDIO_MASTER ] ) << std::endl;
        file << "audio_enable_music: "      << Util::boolToString( m_audioEnable[ AudioSettings::SET_AUDIO_MUSIC ] ) << std::endl;
        file << "audio_enable_effect: "     << Util::boolToString( m_audioEnable[ AudioSettings::SET_AUDIO_EFFECT ] ) << std::endl;
        file << "audio_enable_voice: "      << Util::boolToString( m_audioEnable[ AudioSettings::SET_AUDIO_VOICE ] ) << std::endl;
        file << "audio_enable_ambient: "    << Util::boolToString( m_audioEnable[ AudioSettings::SET_AUDIO_AMBIENT ] ) << std::endl;
        file << "audio_enable_interface: "  << Util::boolToString( m_audioEnable[ AudioSettings::SET_AUDIO_INTERFACE ] ) << std::endl;

        /** Audio - Volume */
        file << "audio_volume_master: "     << Util::floatToString( m_audioVolume[ AudioSettings::SET_AUDIO_MASTER ] ) << std::endl;
        file << "audio_volume_music: "      << Util::floatToString( m_audioVolume[ AudioSettings::SET_AUDIO_MUSIC ] ) << std::endl;
        file << "audio_volume_effect: "     << Util::floatToString( m_audioVolume[ AudioSettings::SET_AUDIO_EFFECT ] ) << std::endl;
        file << "audio_volume_voice: "      << Util::floatToString( m_audioVolume[ AudioSettings::SET_AUDIO_VOICE ] ) << std::endl;
        file << "audio_volume_ambient: "    << Util::floatToString( m_audioVolume[ AudioSettings::SET_AUDIO_AMBIENT ] ) << std::endl;
        file << "audio_volume_interface: "  << Util::floatToString( m_audioVolume[ AudioSettings::SET_AUDIO_INTERFACE ] ) << std::endl;

        file.close();
    }
}

void CSettingManager::setupSettings() {
    handleAudioEnable();
    handleAudioVolume();
}

void CSettingManager::handleAudioEnable( bool* enable ) {
    if( enable != nullptr ) {
        for( int i = 0; i < AudioSettings::SET_AUDIO_TOTAL; i++ ) {
            if( m_audioEnable[ i ] != enable[ i ] ) {
                CGame::AudioSystem.setEnabled( enable[ i ], (CAudioSystem::AudioCategory)i );

                m_audioEnable[ i ] = enable[ i ];
            }
        }
    }
    else {
        for( int i = 0; i < AudioSettings::SET_AUDIO_TOTAL; i++ ) {
            CGame::AudioSystem.setEnabled( m_audioEnable[ i ], (CAudioSystem::AudioCategory)i );
        }
    }
}

void CSettingManager::handleAudioVolume( float* volume ) {
    if( volume != nullptr ) {
        for( int i = 0; i < AudioSettings::SET_AUDIO_TOTAL; i++ ) {
            if( m_audioVolume[ i ] != volume[ i ] ) {
                CGame::AudioSystem.setVolume( volume[ i ] * 100, (CAudioSystem::AudioCategory)i );

                m_audioVolume[ i ] = volume[ i ];
            }
        }
    }
    else {
        for( int i = 0; i < AudioSettings::SET_AUDIO_TOTAL; i++ ) {
            CGame::AudioSystem.setVolume( m_audioVolume[ i ] * 100, (CAudioSystem::AudioCategory)i );
        }
    }
}
