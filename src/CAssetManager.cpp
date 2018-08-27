#include "CAssetManager.h"

#include "lua.hpp"
#include "assert.h"

#include "CGame.h"

#include "iostream"
using namespace std;

void CAssetManager::loadTexture( const std::string& filepath, const std::string& ID ) {
    assert( m_textures.loadFromFile( filepath, ID ) );
}

void CAssetManager::loadFont( const std::string& filepath, const std::string& ID ) {
    assert( m_fonts.loadFromFile( filepath, ID ) );
}

sf::Texture& CAssetManager::getTexture( const std::string& ID ) {
    return m_textures.getAsset( ID );
}

sf::Font& CAssetManager::getFont( const std::string& ID ) {
    return m_fonts.getAsset( ID );
}

sf::Vector2u CAssetManager::getTextureSize( const std::string& ID ) {
    return getTexture( ID ).getSize();
}

/** LUA FUNCTIONS */

int CAssetManager::luaLoadTexture( lua_State* state ) {
    int argc = lua_gettop( state );

    std::string folderpath = lua_tostring( state, lua_gettop( state ) - 1 );

    for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
        lua_getfield( state, - 1, "ID" );

        std::string ID          = lua_tostring( state, lua_gettop( state ) );

        lua_pop( state, 1 );

        lua_getfield( state, - 1, "File" );

        std::string filename    = lua_tostring( state, lua_gettop( state ) );

        lua_pop( state, 1 );

        std::string filepath = folderpath;
        filepath.append( filename );

        CGame::AssetManager.loadTexture( filepath, ID );
    }

    lua_pop( state, argc );

    return 0;
}

int CAssetManager::luaLoadFont( lua_State* state ) {
    int argc = lua_gettop( state );

    std::string folderpath = lua_tostring( state, lua_gettop( state ) - 1 );

    for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
        lua_getfield( state, - 1, "ID" );

        std::string ID          = lua_tostring( state, lua_gettop( state ) );

        lua_pop( state, 1 );

        lua_getfield( state, - 1, "File" );

        std::string filename    = lua_tostring( state, lua_gettop( state ) );

        lua_pop( state, 1 );

        std::string filepath = folderpath;
        filepath.append( filename );

        CGame::AssetManager.loadFont( filepath, ID );
    }

    lua_pop( state, argc );

    return 0;
}

int CAssetManager::luaGetTextureSize( lua_State* state ) {
    int argc = lua_gettop( state );

    std::string textureID = lua_tostring( state, - 1 );

    sf::Vector2u size = CGame::AssetManager.getTextureSize( textureID );

    lua_pop( state, argc );

    lua_pushinteger( state, size.x );
    lua_pushinteger( state, size.y );

    return 2;
}

/** LUA FUNCTIONS END */
