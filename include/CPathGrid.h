#ifndef CPATHGRID_H
#define CPATHGRID_H

#include <vector>
#include <cstddef>

#include <CPathNode.h>


class CPathGrid {
private:
    typedef std::vector<CPathNode>      VecNode;
    typedef std::vector<VecNode>        Vec2DNode;
    typedef std::vector<CPathNode*>     VecNodePtr;

public:
                    CPathGrid           ();

    void            reset               ();
    void            setup               ();

    size_t          getSize             ();

    CPathNode*      getNode             ( const sf::Vector2f& worldPos );
    CPathNode*      getNode             ( int x, int y );

    void            render              ();

    VecNodePtr      getNeighbours       ( const CPathNode* node );

private:
    int             m_nodeCounter;
    int             m_nodeSize;
    sf::Vector2i    m_gridSize;
    Vec2DNode       m_grid;
};

#endif // CPATHGRID_H
