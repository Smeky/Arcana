#include "CPathFinder.h"

#include <cstdlib>
#include <math.h>

#include <CPathHeap.h>

#include <iostream>
using namespace std;

void CPathFinder::setup() {
    m_grid.setup();
}

VecVector2f CPathFinder::findPath( const sf::Vector2f& start, const sf::Vector2f& end ) {
    m_grid.reset();

    CPathNode*  startNode   = m_grid.getNode( start );
    CPathNode*  endNode     = m_grid.getNode( end );
    startNode->parent   = startNode;

//    cout << endNode->gridPos.x << " " << endNode->gridPos.y << " " << endNode->walkable endl;

    if( endNode->walkable ) {
        CPathHeap   openNodes( m_grid.getSize() );
        VecNodePtr  closedNodes;

        openNodes.add( startNode );

        while( openNodes.count() > 0 ) {
            CPathNode* currentNode = openNodes.removeFirst();
            closedNodes.push_back( currentNode );

            if( *currentNode == *endNode ) {
                VecVector2f waypoints;

                waypoints.push_back( end );

                for( auto& it : retracePath( startNode, endNode ) ) {
                    waypoints.push_back( it );
                }

                return waypoints;
            }

//            cout << currentNode->gridPos.x << " " << currentNode->gridPos.y << endl;

            for( auto neighbour : m_grid.getNeighbours( currentNode ) ) {
                if( !neighbour->walkable || contains( closedNodes, neighbour ) ) {
                    continue;
                }

                int movementCost    = currentNode->gCost + getDistance( currentNode, neighbour );

                if( movementCost < neighbour->gCost || !openNodes.contains( neighbour ) ) {
                    neighbour->gCost    = movementCost;
                    neighbour->hCost    = getDistance( neighbour, endNode );
                    neighbour->parent   = currentNode;

                    if( !openNodes.contains( neighbour ) ) {
                        openNodes.add( neighbour );
                    }
                    else {
                        openNodes.updateNode( neighbour );
                    }
                }
            }
        }
    }

    return VecVector2f();
}

void CPathFinder::render() {
    m_grid.render();
}

/** PRIVATE */

VecVector2f CPathFinder::retracePath( CPathNode* startNode, CPathNode* endNode ) {
    VecNode     path;
    CPathNode   currentNode = *endNode;

    while( currentNode != *startNode ) {
        path.push_back( currentNode );

        currentNode = *currentNode.parent;
    }

    return simplifyPath( path );
}

VecVector2f CPathFinder::simplifyPath( const CPathFinder::VecNode& path ) {
    VecVector2f     waypoints;
    sf::Vector2f    oldDirection;

    for( size_t i = 1; i < path.size(); i++ ) {
        sf::Vector2f newDirection;

        newDirection.x  = path[ i - 1 ].gridPos.x - path[ i ].gridPos.x;
        newDirection.y  = path[ i - 1 ].gridPos.y - path[ i ].gridPos.y;

        if( newDirection != oldDirection ) {
            waypoints.push_back( path[ i ].worldPos );
        }

        oldDirection    = newDirection;
    }

    return waypoints;
}

int CPathFinder::getDistance( const CPathNode* first, const CPathNode* second ) {
    sf::Vector2i distance;

    distance.x  = abs( first->gridPos.x - second->gridPos.x );
    distance.y  = abs( first->gridPos.y - second->gridPos.y );

    if( distance.x > distance.y ) {
        return 14 * distance.y + 10 * ( distance.x - distance.y );
    }
    else {
        return 14 * distance.x + 10 * ( distance.y - distance.x );
    }
}

bool CPathFinder::isCorner( const CPathNode* current, const CPathNode* neighbour ) {
    sf::Vector2i currentPos     = current->gridPos;
    sf::Vector2i neighbourPos   = neighbour->gridPos;

    sf::Vector2i relGridPos     = neighbourPos - currentPos;

    CPathNode* firstAdjacent    = nullptr;
    CPathNode* secondAdjacent   = nullptr;


    // If neighbor is in cross position
    if( neighbourPos.x != currentPos.x && neighbourPos.y != currentPos.y ) {
        firstAdjacent    = m_grid.getNode( neighbourPos.x - relGridPos.x, neighbourPos.y );
        secondAdjacent   = m_grid.getNode( neighbourPos.x, neighbourPos.y - relGridPos.y );
    }
    // If neighbor is in plus position
    else {
        if( relGridPos.x == 0 ) {
            firstAdjacent   = m_grid.getNode( neighbourPos.x - 1, neighbourPos.y );
            secondAdjacent  = m_grid.getNode( neighbourPos.x + 1, neighbourPos.y );
        }
        else if( relGridPos.y == 0 ) {
            firstAdjacent   = m_grid.getNode( neighbourPos.x, neighbourPos.y - 1 );
            secondAdjacent  = m_grid.getNode( neighbourPos.x, neighbourPos.y + 1 );
        }
    }

    if( firstAdjacent != nullptr ) {
        if( !firstAdjacent->walkable ) {
            return true;
        }
    }
    if( secondAdjacent != nullptr ) {
        if( !secondAdjacent->walkable ) {
            return true;
        }
    }

    return false;
}

bool CPathFinder::contains( const CPathFinder::VecNodePtr& vec, const CPathNode* node ) {
    for( auto it : vec ) {
        if( *it == *node ) {
            return true;
        }
    }

    return false;
}
