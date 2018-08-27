#ifndef CZBUFFER_H
#define CZBUFFER_H

#ifndef ZLAYERS
#define ZLAYERS 20
#endif // ZLAYERS

#include <SFML/Graphics.hpp>
#include <list>

class CDimensional;

/**
    Note:   Try moving insert and sorting logic to update()
*/

class CZBuffer {
private:
    typedef std::list<const CDimensional*>      Dimensinals;

public:
    void                render          ( sf::RenderTarget& window, sf::RenderStates states );

    void                clean           ();
    void                insert          ( const CDimensional* dim );

private:
    void                sort            ();

private:
    Dimensinals         m_dims[ ZLAYERS ];
};

#endif // CZBUFFER_H
