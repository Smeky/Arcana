#include "CPathGrid.h"

#include <CGame.h>
#include <CMap.h>

#include <Utility.h>

#include <iostream>
using namespace std;

CPathGrid::CPathGrid()
: m_nodeCounter         ( 0 )
, m_nodeSize            ( 64 )
{}

void CPathGrid::reset() {
    for( auto& x : m_grid ) {
        for( auto& y : x ) {
            y.gCost     = 0;
            y.hCost     = 0;
            y.parent    = nullptr;
        }
    }
}

void CPathGrid::setup() {
    const CMap* map = CGame::WorldManager.getCurrentlyUsedMap();

    if( map != nullptr ) {
        // Clean up
        m_nodeCounter   = 0;
        m_grid.clear();

        sf::Vector2f mapSize    = map->getSize();

        m_gridSize.x    = (int)( mapSize.x / m_nodeSize + 0.5 );
        m_gridSize.y    = (int)( mapSize.y / m_nodeSize + 0.5 );

        for( int x = 0; x < m_gridSize.x; x++ ) {
            m_grid.push_back( VecNode() );

            for( int y = 0; y < m_gridSize.y; y++ ) {
                sf::Vector2f worldPos;
                worldPos.x  = x * m_nodeSize + m_nodeSize / 2;
                worldPos.y  = y * m_nodeSize + m_nodeSize / 2;

                bool walkable   = CGame::EntitySystem.isPosFree( sf::Vector2f( x * m_nodeSize + 1, y * m_nodeSize + 1 ), sf::Vector2f( m_nodeSize - 2, m_nodeSize - 2 ) );

                m_grid[ x ].push_back( CPathNode( worldPos, walkable, sf::Vector2i( x, y ) ) );
            }
        }
    }
}

size_t CPathGrid::getSize() {
    return m_gridSize.x * m_gridSize.y;
}

CPathNode* CPathGrid::getNode( const sf::Vector2f& worldPos ) {
    sf::Vector2f percent;
    percent.x   = worldPos.x / ( m_gridSize.x * m_nodeSize );
    percent.y   = worldPos.y / ( m_gridSize.y * m_nodeSize );

    int x       = (int)( ( m_gridSize.x ) * percent.x );
    int y       = (int)( ( m_gridSize.y ) * percent.y );

//    // Correct rounding - Doing some bad stuff without this
//    if( percent.x >= 0.5 ) {
//        x       = (int)( ( m_gridSize.x ) * percent.x + 0.5 );
//    }
//    if( percent.y >= 0.5 ) {
//        y       = (int)( ( m_gridSize.y ) * percent.y + 0.5 );
//    }

    return &m_grid[ x ][ y ];
}

CPathNode* CPathGrid::getNode( int x, int y ) {
    if( x < 0 || x >= m_gridSize.x ||
        y < 0 || y >= m_gridSize.y )
    {
        return nullptr;
    }

    return &m_grid[ x ][ y ];
}

CPathGrid::VecNodePtr CPathGrid::getNeighbours( const CPathNode* node ) {
    VecNodePtr neighbours;

    for( int x = - 1; x <= 1; x++ ) {
        for( int y = - 1; y <= 1; y++ ) {
            if( x == 0 && y == 0 ) {
                continue;
            }

            sf::Vector2i check;

            check.x = node->gridPos.x + x;
            check.y = node->gridPos.y + y;

            if( check.x >= 0 && check.x < m_gridSize.x &&
                check.y >= 0 && check.y < m_gridSize.y )
            {
                neighbours.push_back( &m_grid[ check.x ][ check.y ] );
            }
        }
    }

    return neighbours;
}

void CPathGrid::render() {
    for( auto x : m_grid ) {
        for( auto y : x ) {
            sf::RectangleShape box;

            box.setPosition( y.gridPos.x * m_nodeSize, y.gridPos.y * m_nodeSize );
            box.setSize( sf::Vector2f( m_nodeSize, m_nodeSize ) );
            box.setFillColor( sf::Color::Transparent );
            box.setOutlineThickness( 1.0 );

            if( y.walkable ) {
                box.setOutlineColor( sf::Color::Red );
            }
            else {
                box.setOutlineColor( sf::Color::Blue );
            }

            sf::Text text;
            text.setFont( CGame::AssetManager.getFont( "FONT_ARIAL" ) );
            text.setString( Util::intToString( y.gridPos.x ) + " " + Util::intToString( y.gridPos.y ) );
            text.setCharacterSize( 14 );
            text.setColor( sf::Color::Red );
            text.setPosition( y.worldPos.x + 5, y.worldPos.y + 5 );

            CGame::GameWindow.draw( box, CGame::WorldManager.Camera.getTransform() );
            CGame::GameWindow.draw( text, CGame::WorldManager.Camera.getTransform() );
        }
    }
}
