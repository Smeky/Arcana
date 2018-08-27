#ifndef CSPATIALSYSTEM_H
#define CSPATIALSYSTEM_H

#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/System/Vector2.hpp"

#include "vector"

class CDimensional;
class CEntity;

class CSpatialSystem {
public:
    typedef std::vector<CEntity*>   Cell;        // Vector of entities
    typedef std::vector<Cell>       Grid;        // Vector of cells

public:
    // Setup Spatial grid
    void            setup           ( const sf::Vector2f& gridSize, const sf::Vector2f& cellAmount );

    void            close           ();

    // Clear the grid out of all entities
    void            clearGrid       ();

    // Add entity to the grid
    void            addEntity       ( CEntity* entity );

    // Get all entities in the grid that current entity is in
    // Used for collision checking
    Cell            getNearby       ( const CDimensional* dim, CEntity* exclude = nullptr );

    // Returns all nearby entities and makes sure that all of them are returned only once
    Cell            getNearbySingle ( const CDimensional* dim, CEntity* exclude = nullptr );

    // Get all nearby entities that match entity type
//    Cell            getNearby       ( Entity* entity, EntityType type, Entity* exclude = nullptr );

    // Draw border of all cells
    void            drawCells       ( sf::RenderTarget& window, sf::RenderStates states ) const;

private:
    // Calculates grid cell index
//    void            calcIndex        ( sf::Vector2f entityCorner, std::vector<int>& indexes );
    size_t          calcIndex        ( const sf::Vector2f& entityCorner );

    // Returns vector of indexes of cells that entity is in
    std::vector<size_t> getEntityCellIndex ( const CDimensional* dim );

private:
    sf::Vector2f    m_cellSize;     // Spatial grid cell size
    sf::Vector2f    m_gridSize;     // Size in cols and rows
    sf::Vector2f    m_cellAmount;   // Amount of cells in the grid
    Cell            m_cell;         // Vector of entities
    Grid            m_grid;         // Vector of cells
};

#endif // CSPATIALSYSTEM_H
