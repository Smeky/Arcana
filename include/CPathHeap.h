#ifndef CPATHHEAP_H
#define CPATHHEAP_H

#include <vector>
#include <stddef.h>

class CPathNode;

class CPathHeap {
private:
    typedef std::vector<CPathNode*>     VecNodePtr;

public:
                    CPathHeap       ( size_t size );

    void            add             ( CPathNode* node );
    CPathNode*      removeFirst     ();

    void            updateNode      ( CPathNode* node );

    bool            contains        ( CPathNode* node );

    int             count           ();

private:
    void            sortDown        ( CPathNode* node );
    void            sortUp          ( CPathNode* node );
    void            swap            ( CPathNode* first, CPathNode* second );

private:
    VecNodePtr      m_nodes;
    int             m_nodeCount;

};

#endif // CPATHHEAP_H
