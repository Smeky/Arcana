#ifndef CCAMERA_H
#define CCAMERA_H

#include "CDimensional.h"

class CCamera : public CDimensional {
public:

    void            update          ( const sf::Time& delta );

    sf::Transform   getTransform    ();

    void            moveTo          ( const sf::Vector2f& destination, float factor );
    void            moveCenterTo    ( const sf::Vector2f& destination, float factor );

    sf::Vector2f    getOffset       ( const sf::Vector2f& pos );

    sf::Vector2f    getRelCenter    () const;

private:
    sf::Vector2f    m_destination;
    float           m_moveFactor;
};

#endif // CCAMERA_H
