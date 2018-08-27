#ifndef CMAPGENERATOR_H
#define CMAPGENERATOR_H

#include "SFML/Graphics.hpp"
#include "string"
#include "vector"
#include "list"

class CMap;
struct lua_State;

typedef std::vector<std::vector<bool>>  doubleVecBool;

class CMapGenerator {
public:
                    CMapGenerator       ();

    void            close               ();

    CMap*           createFromLuaTable  ( lua_State* state, int index );

    CMap*           loadMap             ( const std::string& tilesetID, const sf::Vector2f& mapSize, lua_State* state, int index );

    // Create new map based on its type ( grass, snow, etc. ) - return false on failure
    CMap*           generateMap         ( const std::string& tilesetID, const sf::Vector2f& mapSize );

private:
    // Load data from lua table and give it to Entity system so they can be created
    void            createObjects       ( lua_State* state, int index );

    // Count surrounding alive tiles
    int             countAlives         ( doubleVecBool alives, const sf::Vector2u& index, const sf::Vector2f& mapSize );

    // Count bitmask of surrounding alive tiles
    int             countBitmask        ( doubleVecBool alives, const sf::Vector2u& index, const sf::Vector2f& mapSize );

    // Do simulation step ( makes the map look better )
    void            doSimStep           ( doubleVecBool& alives, const sf::Vector2f& mapSize );

};

#endif // CMAPGENERATOR_H
