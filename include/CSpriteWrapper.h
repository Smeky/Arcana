#ifndef CSPRITEWRAPPER_H
#define CSPRITEWRAPPER_H

#include "CDimensional.h"

class CSpriteWrapper : public CDimensional {
private:
    typedef std::vector<sf::Color>    Colors;

public:
                            CSpriteWrapper  ();
                            CSpriteWrapper  ( const std::string& textureID );

            void            draw            ( sf::RenderTarget& window, sf::RenderStates states ) const;

            /** Texture */
            void            setTexture      ( const std::string& texuteID );
    const   sf::Texture*    getTexture      () const;

            /** Clip */
            void            setClip         ( const sf::IntRect& clip );
            void            setClip         ( const sf::FloatRect& clip );
            sf::IntRect     getClip         () const;
            void            resetClip       ();

            /** ID */
            void            setID           ( size_t ID );
            size_t          getID           () const;

            /** Origin */
            void            setOrigin       ( const sf::Vector2f& origin );
            void            setOrigin       ( float x, float y );

            /** Color */
            void            setColor        ( const sf::Color& color );
            sf::Color       getColor        () const;
            // Adds new color modulation to sprite
            void            addColorModul   ( const sf::Color& color );
            // Looks for color modulation and in case there is one, remove it
            void            delColorModul   ( const sf::Color& color );

            /** Bounds */
            sf::FloatRect   getLocalBounds  () const;
            sf::FloatRect   getGlobalBounds () const;

    static  CSpriteWrapper  loadFromTable   ( lua_State* state, int index, bool* result = nullptr );

private:
            void            onSizeChange    ();

            void            updateColor     ();

private:
    sf::Sprite              m_sprite;

    size_t                  m_ID;

    sf::Color               m_colorBase;
    Colors                  m_colorModuls;
};

#endif // CSPRITEWRAPPER_H
