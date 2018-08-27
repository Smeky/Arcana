#ifndef CANIMATION_H
#define CANIMATION_H

#include <SFML/Graphics.hpp>

#include <CTimer.h>
#include <CSpriteWrapper.h>

struct lua_State;

class CAnimation : public sf::Drawable {
public:
                    CAnimation          ();

    // Setup sprite from Lua table
    void            setupFromLuaTable   ( lua_State* state, int index, const sf::Vector2f& size );

    // Setup sprite texture without any clip animation
    void            setupTexture        ( const std::string& textureID, const sf::Vector2f& size );

    void            update              ( const sf::Time& delta );
    void            draw                ( sf::RenderTarget& window, sf::RenderStates state ) const;

    // Set sprite scale size
    void            setSize             ( const sf::Vector2f& size );
    // Set image clip for sprite
    void            setBaseClip         ( const sf::IntRect& clip );
    // Set scale of sprite
    void            setSpriteScale      ( const sf::Vector2f& size );
    // Set the sprite position on size / 2
    void            setCenterSprite     ( const sf::Vector2f& size );
    // Set origin of sprite texture to center
    void            setCenterOrigin     ();

    void            resetIndex          ();

    void            setFrameTime        ( float seconds );
    void            setFactorFrameTime  ( float factor );   // Add to currect delay factor

    void            setRowIndex         ( const size_t& index );
    size_t          getRowIndex         () const;
    size_t          getIndex            () const;

    bool            isDirectional       () const;

    void            setColor            ( const sf::Color& color );
    void            setColorModulation  ( const sf::Color& color );
    void            delColorModulation  ( const sf::Color& color );

    CSpriteWrapper& getSprite           ();

    const  CTimer&  getTimer            ();

private:
    void            storeDefaultColor   ();
    void            resetDefaultColor   ();

private:
    CSpriteWrapper  m_sprite;

    size_t          m_index;        // Currect clip index
    size_t          m_clipsPerRow;  // Amount of clips in a single row
    size_t          m_totalClips;   // Amount of total clips in the image
    size_t          m_rowIndex;     // Index of clip row

    CTimer          m_frameTimer;   // Delay between frames

    float           m_delayFactor;  // Factor used to change delay between frames

    sf::IntRect     m_baseClip;     // First clip in the image ( doesn't change )
    sf::IntRect     m_currClip;     // Updated clip based on timer ( and directions )

    bool            m_directional;
    bool            m_repeat;

    sf::Color       m_colorDefault;
    sf::Color       m_colorModulation;
};

#endif // CANIMATION_H
