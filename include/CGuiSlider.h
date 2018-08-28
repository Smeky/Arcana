#ifndef CGUISLIDER_H
#define CGUISLIDER_H

#include <CGuiElement.h>

class CGuiSlider : public CGuiElement {
public:
    /** Slider orientation */
    enum SliderOrint {
        HORIZONTAL,
        VERTICAL
    };

    enum ButtonSoundType {
        ONCLICK,
        ONMOUSEOVER,
        TOTAL           // Keep last
    };

    enum SliderFunction {
        SFUNC_ONMOVE,
        SFUNC_TOTAL           // Keep last
    };

public:
                            CGuiSlider          ();

            void            handleInput         ( sf::Event event );
            void            update              ( const sf::Time& delta );
            void            draw                ( sf::RenderTarget& window, sf::RenderStates states ) const;

            /** Functions */
            void            registerFunction    ( SliderFunction Type, size_t ID );

            /** Slider texture */
            size_t          addSliderTexture    ( lua_State* state, int index, bool* result = nullptr );
            bool            setSliderTexture    ( size_t ID );

            void            setSliderSize       ( const sf::Vector2f& size );

            /** Orientation */
            void            setOrientation      ( SliderOrint orientation );

            /** Range */
            void            setRange            ( const sf::Vector2f& range );

            /** Value */
            void            setValue            ( float value );
            float           getValue            () const;

            /** Sound */
            void            setSound            ( ButtonSoundType type, const std::string& ID );

            /** Locked */
            void            setLocked           ( bool locked = true );
            bool            isLocked            () const;

            bool            isMouseover         () const;

    static  CGuiSlider*     setupFromTable      ( lua_State* state, int index, bool* result = nullptr, const CGuiElement* parent = nullptr );

public:
    static  int             luaAddSliderTexture ( lua_State* state );
    static  int             luaSetSliderTexture ( lua_State* state );
    static  int             luaSetSliderSize    ( lua_State* state );
    static  int             luaSetOrientation   ( lua_State* state );
    static  int             luaSetRange         ( lua_State* state );
    static  int             luaSetValue         ( lua_State* state );
    static  int             luaGetValue         ( lua_State* state );
    static  int             luaSetSound         ( lua_State* state );
    static  int             luaSetLocked        ( lua_State* state );
    static  int             luaIsLocked         ( lua_State* state );

private:
            CSpriteWrapper* getSprite           ( size_t ID );

            void            moveSlider          ( sf::Vector2i mousePos );
            void            updateSlider        ();

            void            playSound           ( ButtonSoundType type );

private:
    size_t              m_spriteSliderID;
    sf::Vector2u        m_buttonClipSize;
    sf::FloatRect       m_sliderBox;

    SliderOrint         m_orientation;
    float               m_value;
    sf::Vector2f        m_range;

    size_t              m_luaFuncIDs[ SliderFunction::SFUNC_TOTAL ];

    std::string         m_sounds[ ButtonSoundType::TOTAL ];

    bool                m_locked;
    bool                m_mouseover;
    bool                m_pressed;
    bool                m_clicked;
    bool                m_checked;

    bool                m_wasPressed;
};

#endif // CGUISLIDER_H
