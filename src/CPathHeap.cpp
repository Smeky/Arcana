#include "CPathHeap.h"

#include <CPathNode.h>

#include <iostream>
using namespace std;

CPathHeap::CPathHeap( size_t size )
: m_nodeCount       ( 0 )
{
    m_nodes.resize( size );
}

void CPathHeap::add( CPathNode* node ) {
    node->index = m_nodeCount;

    m_nodes[ m_nodeCount ]  = node;

    sortUp( node );

    m_nodeCount++;
}

CPathNode* CPathHeap::removeFirst() {
    CPathNode* firstNode    = m_nodes[ 0 ];

    m_nodeCount--;

    m_nodes[ 0 ]    = m_nodes[ m_nodeCount ];
    m_nodes[ 0 ]->index = 0;

    sortDown( m_nodes[ 0 ] );

    return firstNode;
}

void CPathHeap::updateNode( CPathNode* node ) {
    sortUp( node );
}

int CPathHeap::count() {
    return m_nodeCount;
}

bool CPathHeap::contains( CPathNode* node ) {
    return m_nodes[ node->index ] == node;
}

void CPathHeap::sortDown( CPathNode* node ) {
    while( true ) {
        int childIndexLeft  = node->index * 2 + 1;
        int childIndexRight = node->index * 2 + 2;
        int swapIndex       = 0;

        if( childIndexLeft < m_nodeCount ) {
            swapIndex   = childIndexLeft;

            if( childIndexRight < m_nodeCount ) {
                if( m_nodes[ childIndexLeft ]->compareTo( m_nodes[ childIndexRight ] ) < 0 ) {
                    swapIndex   = childIndexRight;
                }
            }

            if( node->compareTo( m_nodes[ swapIndex ] ) < 0 ) {
                swap( node, m_nodes[ swapIndex ] );
            }
            else {
                return;
            }
        }
        else {
            return;
        }
    }
}

void CPathHeap::sortUp( CPathNode* node ) {
    int parentIndex = ( node->index - 1 ) / 2;

    while( true ) {
        CPathNode* parentNode = m_nodes[ parentIndex ];

        if( node->compareTo( parentNode ) > 0 ) {
            swap( node, parentNode );
        }
        else {
            return;
        }

        parentIndex = ( node->index - 1 ) / 2;
    }
}

void CPathHeap::swap( CPathNode* first, CPathNode* second ) {
    m_nodes[ first->index ]     = second;
    m_nodes[ second->index ]    = first;

    int firstIndex  = first->index;

    first->index    = second->index;
    second->index   = firstIndex;
}
