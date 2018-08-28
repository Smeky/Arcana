#ifndef CMAPGEN_H
#define CMAPGEN_H

#include <SFML/System/Vector2.hpp>
#include <string>
#include <vector>
#include <memory>

struct lua_State;

class CMap;
class CMapRoom;

class CMapGen {
private:
    struct MapLayer {
        float       chance      = 0.5;
        int         smoothing   = 5;
    };

    typedef std::vector<std::vector<bool>>  Vec2DBool;
    typedef std::vector<sf::Vector2i>       Vectors2i;
    typedef std::vector<Vectors2i>          VecOfVectors2i;
    typedef std::vector<Vec2DBool>          VecOf2DBool;
    typedef std::vector<MapLayer>           VecMapLayer;

public:
    static  CMap*           generateMap         ( lua_State* state, int index );

private:
    static  void            setupData           ( lua_State* state, int index );

    static  Vec2DBool       setupLayer          ( float spawnChance, int smoothing );
    static  void            smooth              ( Vec2DBool& layer );
    static  int             countNeighbours     ( Vec2DBool& layer, int xIndex, int yIndex );

    static  void            processMap          ();
    static  VecOfVectors2i  getRegions          ( bool type );
    static  Vectors2i       getRegion           ( int xStart, int yStart );

    static  void            connectRooms        ( std::vector<CMapRoom*> rooms, bool forceAccessibilityFromMain = false );
    static  void            createPassage       ( CMapRoom* aRoom, CMapRoom* bRoom, sf::Vector2i aTile, sf::Vector2i bTile );
    static  Vectors2i       getLine             ( const sf::Vector2i& from, const sf::Vector2i& to );
    static  void            makeCircle          ( const sf::Vector2i& tile, int radius );

    static  int             countBitmask        ( Vec2DBool& layer, const sf::Vector2i& tile );
    static  int             countBitmask        ( Vec2DBool& layer, const sf::Vector2i& tile, bool type );

    static  bool            isInMap             ( int x, int y );

private:
    static  Vec2DBool       m_empty;
    static  MapLayer        m_emptyData;
    static  int             m_mapWidth;
    static  int             m_mapHeight;
    static  int             m_emptyTresh;
    static  int             m_solidTresh;
    static  int             m_passageRad;
    static  std::string     m_tileset;
    static  VecMapLayer     m_emptyLayers;
    static  VecMapLayer     m_solidLayers;

};

#endif // CMAPGEN_H
