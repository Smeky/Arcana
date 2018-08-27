#ifndef CASSETMANAGER_H
#define CASSETMANAGER_H

#include "AssetHolder.h"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/Font.hpp"

struct lua_State;

class CAssetManager {
public:
    void            loadTexture         ( const std::string& filepath, const std::string& ID );
    void            loadFont            ( const std::string& filepath, const std::string& ID );

    sf::Texture&    getTexture          ( const std::string& ID );
    sf::Font&       getFont             ( const std::string& ID );

    sf::Vector2u    getTextureSize      ( const std::string& ID );

public:
    static int      luaLoadTexture      ( lua_State* state );
    static int      luaLoadFont         ( lua_State* state );
    static int      luaGetTextureSize   ( lua_State* state );

private:
    AssetHolder<sf::Texture>    m_textures;
    AssetHolder<sf::Font>       m_fonts;

};

#endif // CASSETMANAGER_H
