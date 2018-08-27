#ifndef CPLAYER_H
#define CPLAYER_H

#include "CCharacter.h"
#include "CInventory.h"

class CPlayer : public CCharacter {
public:
    void            update              ( const sf::Time& delta );

    void            moveLeft            ();
    void            moveRight           ();
    void            moveUp              ();
    void            moveDown            ();

    CInventory&     getInventory        ();

private:
    enum Direction {
        LEFT,
        UP,
        RIGHT,
        DOWN
    };

    typedef std::vector<size_t> Size_ts;

private:
    void            onStatChange        ();
    void            onNewAbility        ( size_t index );
    void            onAbilityCast       ( size_t index );

    void            updateDirOfAnimation( float* angle = nullptr );

private:
    CInventory      m_inventory;

    sf::Vector2f    m_moveDirSpeed;

    Size_ts         m_moveDirBuffer;
    bool            m_moveDirTrack[ 4 ];
    bool            m_prevMoveDirTrack[ 4 ];
};

#endif // CPLAYER_H
