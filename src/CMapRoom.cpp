#include "CMapRoom.h"

#include <algorithm>

CMapRoom::CMapRoom( const Vectors2i& tiles, const Vec2DBool& map ) {
    m_tiles                 = tiles;
    m_size                  = tiles.size();
    m_accessibleFromMain    = false;
    m_isMainRoom            = false;

    for( sf::Vector2i tile : m_tiles ) {
        for( int y = tile.y - 1; y <= tile.y + 1; y++ ) {
            for( int x = tile.x - 1; x <= tile.x + 1; x++ ) {
                if( x == tile.x || y == tile.y ) {
                    // If is not empty
                    if( !map[ y ][ x ] ) {
                        m_edgeTiles.push_back( tile );
                    }
                }
            }
        }
    }
}

void CMapRoom::connect( CMapRoom* first, CMapRoom* second ) {
    if( first->m_accessibleFromMain ) {
        second->m_accessibleFromMain = true;
    }
    else if( second->m_accessibleFromMain ) {
        first->m_accessibleFromMain = true;
    }

    first->m_connectedRooms.push_back( second );
    second->m_connectedRooms.push_back( first );
}

void CMapRoom::setAccessibleFromMain() {
    if( !m_accessibleFromMain ) {
        m_accessibleFromMain    = true;

        for( CMapRoom* room : m_connectedRooms ) {
            room->setAccessibleFromMain();
        }
    }
}

bool CMapRoom::isConnected( CMapRoom* room ) {
    return std::find( m_connectedRooms.begin(), m_connectedRooms.end(), room ) != m_connectedRooms.end();
}
