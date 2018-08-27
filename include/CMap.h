#ifndef CMAP_H
#define CMAP_H

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
#include <vector>

class CTileLayer;
class CDimensional;

typedef std::vector<const CDimensional*>    C_Dimensionals;

class CMap {
private:
    typedef std::vector<CTileLayer*>        Layers;

public:
                        CMap                ();
                       ~CMap                ();

    void                close               ();

    void                update              ( const sf::Time& delta );
    void                render              ( sf::RenderTarget& window, sf::RenderStates states ) const;
    void                renderTopLayer      ( sf::RenderTarget& window, sf::RenderStates states ) const;

    void                setID               ( size_t ID );
    size_t              getID               () const;

    void                addTileLayer        ( CTileLayer* layer );
    void                addTopTileLayer     ( CTileLayer* layer );

    void                setSize             ( const sf::Vector2f& size );
    sf::Vector2f        getSize             () const;
    sf::Vector2f        getSizeInTiles      () const;
    size_t              getTotalLayers      () const;

    C_Dimensionals      getNearby           ( const sf::Vector2f& pos, const sf::Vector2f& size );
    C_Dimensionals      getNearby           ( const CDimensional* dim );

    bool                debugDisplayGrid    ();
    void                debugRenderGrid     ( sf::RenderTarget& window, sf::RenderStates states, const sf::IntRect& indexBox ) const;

private:
    sf::IntRect         calcIndex           ( const sf::Vector2f& pos, const sf::Vector2f& size ) const;
    sf::IntRect         calcIndex           ( const CDimensional& dim ) const;

private:
    size_t              m_ID;

    Layers              m_layers;
    Layers              m_topLayers;

    sf::Vector2f        m_size;
    sf::Vector2f        m_tileSize;

    bool                m_debugGrid;
};

#endif // CMAP_H
