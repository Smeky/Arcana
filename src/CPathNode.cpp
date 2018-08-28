#include "CPathNode.h"

#include <Utility.h>

CPathNode::CPathNode( const sf::Vector2f _worldPos, bool _walkable, const sf::Vector2i& _gridPos )
: worldPos          ( _worldPos )
, gridPos           ( _gridPos )
, walkable          ( _walkable )
, index             ( 0 )
, gCost             ( 0 )
, hCost             ( 0 )
, parent            ( nullptr )
{}

int CPathNode::compareTo( const CPathNode* node ) {
    int compare = Util::compareTo( getFCost(), node->getFCost() );

    if( compare == 0 ) {
        compare = Util::compareTo( hCost, node->hCost);
    }

    return - compare;
}

int CPathNode::getFCost() const {
    return gCost + hCost;
}
