#ifndef CPATHFINDER_H
#define CPATHFINDER_H

#include <SFML/System/Vector2.hpp>
#include <vector>

#include <CPathGrid.h>

typedef std::vector<sf::Vector2f>       VecVector2f;

class CPathFinder {
private:
    typedef std::vector<CPathNode>      VecNode;
    typedef std::vector<CPathNode*>     VecNodePtr;

public:
    void            setup               ();

    VecVector2f     findPath            ( const sf::Vector2f& start, const sf::Vector2f& end );

    void            render              ();

private:
    VecVector2f     retracePath         ( CPathNode* startNode, CPathNode* endNode );
    VecVector2f     simplifyPath        ( const VecNode& path );

    int             getDistance         ( const CPathNode* first, const CPathNode* second );
    bool            isCorner            ( const CPathNode* current, const CPathNode* neighbour );

    bool            contains            ( const VecNodePtr& vec, const CPathNode* node );

private:
    CPathGrid       m_grid;
};

#endif // CPATHFINDER_H
