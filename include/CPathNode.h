#ifndef CPATHNODE_H
#define CPATHNODE_H

#include <SFML/System/Vector2.hpp>

class CPathNode {
public:
    bool operator==( const CPathNode& node ) {
        return  this->worldPos      == node.worldPos &&
                this->gridPos       == node.gridPos &&
                this->walkable      == node.walkable &&
                this->index         == node.index;
    }
    bool operator!=( const CPathNode& node ) {
        return  this->worldPos      != node.worldPos ||
                this->gridPos       != node.gridPos ||
                this->walkable      != node.walkable ||
                this->index         != node.index;
    }

public:
                    CPathNode           ( const sf::Vector2f _worldPos, bool _walkable, const sf::Vector2i& _gridPos );

    int             compareTo           ( const CPathNode* node );

    int             getFCost            () const;

public:
    sf::Vector2f    worldPos;
    sf::Vector2i    gridPos;
    bool            walkable;
    int             index;
    int             gCost;
    int             hCost;
    CPathNode*      parent;
};

#endif // CPATHNODE_H
