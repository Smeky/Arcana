#include "CSpatialSystem.h"

#include "algorithm"
#include "math.h"
#include "cassert"

#include "CEntity.h"
#include "CGame.h"
#include "Utility.h"

#include "iostream"
using namespace std;

void CSpatialSystem::setup( const sf::Vector2f& gridSize, const sf::Vector2f& cellAmount ) {
    m_gridSize      = gridSize;
    m_cellAmount    = cellAmount;

    m_cellSize.x    = gridSize.x / cellAmount.x;
    m_cellSize.y    = gridSize.y / cellAmount.y;

    // Make new grid full of empty cells
    for( size_t i = 0; i < m_cellAmount.x * m_cellAmount.y; i++ ) {
        m_grid.push_back( Cell() );
    }
}

void CSpatialSystem::close() {
    // Remove everything in the grid
    m_grid.clear();
}

void CSpatialSystem::clearGrid() {
    for( auto& it : m_grid ) {
        it.clear();
    }
}

void CSpatialSystem::addEntity( CEntity* entity ) {
    // Vector of grid indexes that entity is in
    std::vector<size_t> cellIndex = getEntityCellIndex( entity );

    // Add entity to every cell that she collides with
    for( size_t i = 0; i < (size_t)cellIndex.size(); i++ ) {
        m_grid[ cellIndex[ i ] ].push_back( entity );
    }
}

std::vector<size_t> CSpatialSystem::getEntityCellIndex( const CDimensional* dim ) {
    // Return vector of grid indexes that entity is in
    std::vector<size_t> indexes;

    // Store entity dimension -> prevents too many function calls
    sf::Vector2f entityPos      = dim->getPos();
    sf::Vector2f entityPosSize  = dim->getPosSize();

    // Entity top-left corner
    sf::Vector2f eCornerTL( entityPos.x         , entityPos.y );
    // Entity top-right corner    sf::Vector2f eCornerTR( entityPosSize.x     , entityPos.y );
    // Entity bottom-left corner
    sf::Vector2f eCornerBL( entityPos.x         , entityPosSize.y );

//    // Adjust entity corners so it can be added even when part of it is outside the grid
//    if( eCornerTL.x < 0 ) { eCornerTL.x = 0; }
//    if( eCornerTL.y < 0 ) { eCornerTL.y = 0; }
//
//    if( eCornerTR.x > m_box.left + m_box.width ) { eCornerTR.x = m_box.left + m_box.width; }
//    if( eCornerTR.y < 0 ) { eCornerTR.y = 0; }
//
//    if( eCornerBL.x < 0 ) { eCornerBL.x = 0; }
//    if( eCornerBL.y > m_box.top + m_box.height ) { eCornerBL.y = m_box.top + m_box.height; }

    // Calculate corner indexes
    size_t iCornerTL = calcIndex( eCornerTL );
    size_t iCornerTR = calcIndex( eCornerTR );
    size_t iCornerBL = calcIndex( eCornerBL );

    // Calculate dimension indexes - used for loop below
    int y   = floorf( iCornerTL / m_cellAmount.x );
    int h   = ( iCornerBL - iCornerTL ) / m_cellAmount.x;
    int x   = ( iCornerTL - m_cellAmount.x * y );
    int w   = iCornerTR - iCornerTL;

    // Fix dimension indexes
    if( w < 0 ) { w = 0; }
    if( h < 0 ) { h = 0; }

    int xw  = x + w + 1;
    int yh  = y + h + 1;

    if( xw  > m_cellAmount.x ) { xw = m_cellAmount.x; }
    if( yh  > m_cellAmount.y ) { yh = m_cellAmount.y; }

    // Store entity indexes
    for( int i = y; i < yh; i++ ) {
        for( int j = x; j < xw; j++ ) {
            indexes.push_back( i * m_cellAmount.x + j );
        }
    }

    return indexes;
}

size_t CSpatialSystem::calcIndex( const sf::Vector2f& eCorner ) {
    // Return index of cell that entity corner is in
    return (size_t)( ( floorf( eCorner.x / m_cellSize.x ) ) + ( floorf( eCorner.y / m_cellSize.y ) ) * m_cellAmount.x );
}

CSpatialSystem::Cell CSpatialSystem::getNearby( const CDimensional* dim, CEntity* exclude ) {
    // Returned vector of entities
    Cell entities;

    // Get indexes of cells that entity is in
    std::vector<size_t> indexes = getEntityCellIndex( dim );

    // Go through vector of indexes
    for( size_t i = 0; i < indexes.size(); i++ ) {
        // Go through all entities that match cell index
        for( size_t j = 0; j < m_grid[ indexes[ i ] ].size(); j++ ) {
            // Temporary entity that will be added in to returned vector
            CEntity* entityToAdd = m_grid[ indexes[ i ] ][ j ];

            // If near by entity is not the main entity, add it to the returned vector
            if( entityToAdd != dim && entityToAdd != exclude && entityToAdd != nullptr ) {
                entities.push_back( entityToAdd );
            }
        }
    }


    return entities;
}

CSpatialSystem::Cell CSpatialSystem::getNearbySingle( const CDimensional* dim, CEntity* exclude ) {
    // Returned vector of entities
    Cell entities;

    // Get indexes of cells that entity is in
    std::vector<size_t> indexes = getEntityCellIndex( dim );

    // Go through vector of indexes
    for( size_t i = 0; i < indexes.size(); i++ ) {
        // Go through all entities that match cell index
        for( size_t j = 0; j < m_grid[ indexes[ i ] ].size(); j++ ) {
            // Temporary entity that will be added in to returned vector
            CEntity* entityToAdd = m_grid[ indexes[ i ] ][ j ];

            // If near by entity is not the main entity, add it to the returned vector
            if( entityToAdd != dim && entityToAdd != exclude && entityToAdd != nullptr ) {
                if( std::find( entities.begin(), entities.end(), entityToAdd ) == entities.end() ) {
                    entities.push_back( entityToAdd );
                }
            }
        }
    }


    return entities;
}

//CSpatialSystem::Cell CSpatialSystem::getNearby( CEntity* entity, EntityType type, CEntity* exclude ) {
//    // Returned vector of entities
//    Cell entities;
//
//    // If entity box collides with grid box
//    if( m_box.intersects( entity->getBox() ) ) {
//        // Get indexes of cells that entity is in
//        std::vector<int> indexes = getEntityCellIndex( entity );
//
//        // Go through vector of indexes
//        for( int i = 0; i < (int)indexes.size(); i++ ) {
//            // Go throu all entities that match cell index
//            for( int j = 0; j < (int)m_grid[ indexes[ i ] ].size(); j++ ) {
//                // Temporary entity that will be added in to returned vecotr
//                CEntity* entityToAdd = m_grid[ indexes[ i ] ][ j ];
//
//                // If near by entity is not the main entity, add it to the returned vector
//                if( entityToAdd != entity && entityToAdd != exclude && entityToAdd != nullptr && entityToAdd->getType() == type ) {
//                    if( std::find( entities.begin(), entities.end(), entityToAdd ) == entities.end() ) {
//                        entities.push_back( entityToAdd );
//                    }
//                }
//            }
//        }
//    }
//
//    return entities;
//}

void CSpatialSystem::drawCells( sf::RenderTarget& window, sf::RenderStates states ) const  {
    for( int y = 0; y < (int)m_cellAmount.y; y++ ) {
        for( int x = 0; x < (int)m_cellAmount.x; x++ ) {
            sf::RectangleShape shape;
            shape.setSize( m_cellSize );
            shape.setPosition( x * m_cellSize.x, y * m_cellSize.y );
            shape.setFillColor( sf::Color( 0, 0, 0, 0 ) );
            shape.setOutlineColor( sf::Color( 255, 0, 0, 100 ) );
            shape.setOutlineThickness( 1 );

            sf::Text cellIndex( Util::intToString( y * m_cellAmount.x + x ), CGame::AssetManager.getFont( "FONT_ARIAL" ), 22 );
            cellIndex.setColor( sf::Color::Red );
            cellIndex.setPosition( x * m_cellSize.x + 10, y * m_cellSize.y + 10 );

            window.draw( shape, states );
            window.draw( cellIndex, states );
        }
    }
}
