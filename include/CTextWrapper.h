#ifndef CTEXTWRAPPER_H
#define CTEXTWRAPPER_H

#include <CDimensional.h>

struct lua_State;

class CTextWrapper : public CDimensional {
public:
                            CTextWrapper    ();

            void            draw            ( sf::RenderTarget& window, sf::RenderStates states ) const;

            void            setID           ( size_t ID );
            size_t          getID           () const;

            void            setString       ( const std::string& text );
            void            setFont         ( const std::string& fontID );
            void            setCharSize     ( size_t size );
            void            setStyle        ( const sf::Text::Style& style );
            void            setColor        ( const sf::Color& color );
            void            setOrigin       ( const OriginPosition& origin );

            std::string     getString       () const;
            sf::FloatRect   getLocalBounds  () const;

            void            fade            ( float amount );
            bool            isFaded         () const;

    static  CTextWrapper    loadFromTable   ( lua_State* state, int index, bool* result = nullptr );

private:
    sf::Text                m_text;

    size_t                  m_ID;

    OriginPosition          m_origin;

    float                   m_fadeAlpha;    // Used for correct fading
};

#endif // CTEXTWRAPPER_H
