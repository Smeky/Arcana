#ifndef CDIMENSIONAL_H
#define CDIMENSIONAL_H

#include "SFML/Graphics.hpp"

struct lua_State;

typedef enum {
    ORIGIN_CENTER,
    ORIGIN_LEFT,
    ORIGIN_RIGHT,
    ORIGIN_TOP,
    ORIGIN_BOTTOM
} OriginPosition;

class CDimensional : public sf::Drawable {
public:
                            CDimensional    ();
    virtual                ~CDimensional    () {}

    virtual void            draw            ( sf::RenderTarget& window, sf::RenderStates states ) const {}

            void            setPos          ( const sf::Vector2f& pos );
            void            setPos          ( float x, float y );
            sf::Vector2f    getPos          () const;

            // Used to set position as integer ( helps with correcting textures and texts )
            void            setPosRounded   ( const sf::Vector2f& pos );
            void            setPosRounded   ( float x, float y );

            // Set position based on center position ( m_pos - m_size / 2 )
            void            setCenter       ( const sf::Vector2f& center );
            void            setCenter       ( float x, float y );
            sf::Vector2f    getCenter       () const;

            void            setSize         ( const sf::Vector2f& size );
            void            setSize         ( float x, float y );
            sf::Vector2f    getSize         () const;

            // Pos size is position based on right and bottom border
            void            setPosSize      ( const sf::Vector2f& pos );
            void            setPosSize      ( float x, float );
            sf::Vector2f    getPosSize      () const;

            // Set m_pos and m_size at once
            void            setBox          ( const sf::FloatRect& box );
            void            setBox          ( const sf::Vector2f& pos, const sf::Vector2f& size );
            void            setBox          ( float x, float y, float w, float h );
            sf::FloatRect   getBox          () const;

            void            setRotation     ( float angle );
            float           getRotation     () const;

            // Reset collision box to 0, 0, size.x, size.y
            void            resetCollBox    ();

            // Set box for collision relative to m_pos
            void            setCollBox      ( const sf::FloatRect& box );
            void            setCollBox      ( const sf::Vector2f& pos, const sf::Vector2f& size );
            void            setCollBox      ( float x, float y, float w, float h );
            sf::FloatRect   getCollBox      () const;   // Return non-relative box ( = m_pos is included )
            sf::Vector2f    getCollBoxPos   () const;   // Return relative position to m_pos
            sf::Vector2f    getCollBoxSize  () const;

            // Add vector to m_pos
            void            move            ( const sf::Vector2f& movement );
            void            move            ( float x, float y );

            // Add vector to m_size
            void            resize          ( const sf::Vector2f& size );
            void            resize          ( float x, float y );

            void            rotate          ( float rotation );
            void            rotateCenter    ( float rotation );

            sf::Transform   getTransform    () const;

            // Z-buffer
            void            setZLayer       ( int layer );
            void            moveZLayer      ( int amount );
            int             getZLayer       () const;

protected:
    virtual void            onPosChange     () {}
    virtual void            onSizeChange    () {}

protected:
    sf::Vector2f            m_pos;
    sf::Vector2f            m_size;
    float                   m_rotAngle;     // Angle of rotation
    int                     m_zLayer;       // Z-buffer layer

    sf::FloatRect           m_collBox;      // Box relative to m_pos
};

#endif // CDIMENSIONAL_H
