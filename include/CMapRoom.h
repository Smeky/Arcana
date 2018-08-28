#ifndef CMAPROOM_H
#define CMAPROOM_H

#include <SFML/System/Vector2.hpp>
#include <vector>
#include <cstdlib>

class CMapRoom {
private:
    typedef std::vector<std::vector<bool>>      Vec2DBool;
    typedef std::vector<sf::Vector2i>           Vectors2i;
    typedef std::vector<CMapRoom*>              VecMapRoom;

public:
                        CMapRoom                ();
                        CMapRoom                ( const Vectors2i& tiles, const Vec2DBool& map );

    static  void        connect                 ( CMapRoom* first, CMapRoom* second );

            void        setAccessibleFromMain   ();

            bool        isConnected             ( CMapRoom* room );

public:
    Vectors2i           m_tiles;
    Vectors2i           m_edgeTiles;
    VecMapRoom          m_connectedRooms;
    size_t              m_size;
    bool                m_accessibleFromMain;
    bool                m_isMainRoom;
};

#endif // CMAPROOM_H
