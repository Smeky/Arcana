#include "CTooltip.h"

#include "sstream"

#include "CGame.h"

#include "iostream"
using namespace std;

CTooltip::CTooltip()
: m_textOffset      ( 0 )
{}

void CTooltip::draw( sf::RenderTarget& window, sf::RenderStates states ) const {
    sf::Transformable transformable;

    transformable.setPosition( m_pos );

    states.transform *= transformable.getTransform();

    window.draw( m_bgSprite, states );

    for( auto& it : m_texts ) {
        window.draw( it, states );
    }
}

void CTooltip::setWidth( float width ) {
    m_size.x = width;

    onSizeChange();
}

void CTooltip::setBgTexture( const std::string& textureID ) {
    m_bgSprite.setTexture( textureID );
    m_bgSprite.setSize( m_size );
}

void CTooltip::addText( const CTextWrapper& textWrapper ) {
    fitText( textWrapper );
}

void CTooltip::onSizeChange() {
    // If tooltip would be outside game window, adjust it's position
    sf::Vector2f posSize = getPosSize();

    if( posSize.x > CGame::GameWindow.getSize().x - 5 ) {
        setPosSize( CGame::GameWindow.getSize().x - 5, posSize.y );
    }
    else if( m_pos.x < 5 ) {
        setPos( 5, m_pos.y );
    }

    if( posSize.y > CGame::GameWindow.getSize().y - 5 ) {
        setPosSize( posSize.x, CGame::GameWindow.getSize().y - 5 );
    }
    else if( m_pos.y < 5 ) {
        setPos( m_pos.x, 5 );
    }

    m_bgSprite.setSize( m_size );
}

void CTooltip::pushText( const CTextWrapper& textWrapper ) {
    m_texts.push_back( textWrapper );

    updateHeight( textWrapper );
}

void CTooltip::fitText( const CTextWrapper& textWrapper ) {
    std::string         text    = textWrapper.getString();
    std::ostringstream  line;
    std::ostringstream  word;

    float maximumSize       = m_size.x - 40;
    int   lineCounter       = 0;

    sf::Vector2f textPos    = textWrapper.getPos();

    for( auto it = text.begin(); it != text.end(); it++ ) {
        word << *it;

        if( *it == ' ' ) {
            CTextWrapper temp = textWrapper;

            std::string lineString;
            lineString.append( line.str() );
            lineString.append( word.str() );

            temp.setString( lineString );

            if( temp.getLocalBounds().width > maximumSize ) {

                CTextWrapper newText = textWrapper;

                newText.setString( line.str() );

                newText.setPos( textPos );
                textPos.y += ( textWrapper.getLocalBounds().height + 5 );
                lineCounter++;

                m_textOffset += textWrapper.getLocalBounds().height + 5;

                pushText( newText );

                line.str( "" );
                line.clear();
            }

            line << word.str();

            word.str( "" );
            word.clear();
        }
        else if( it == text.end() - 1 ) {
            std::string wordString = word.str();

            if( wordString[ 0 ] == ' ' ) {
                wordString.erase( wordString.begin() );
            }

            CTextWrapper temp = textWrapper;

            std::string lineString;
            lineString.append( line.str() );
            lineString.append( wordString );

            temp.setString( lineString );

            if( temp.getLocalBounds().width > maximumSize ) {

                CTextWrapper newText = textWrapper;

                newText.setString( line.str() );

                // Set correct position with offset
                newText.setPos( textPos );
                textPos.y += ( textWrapper.getLocalBounds().height + 5 );
                lineCounter++;

                m_textOffset += textWrapper.getLocalBounds().height + 5;

                pushText( newText );

                line.str( "" );
                line.clear();
            }

            line << wordString;

            CTextWrapper newText = textWrapper;

            newText.setString( line.str() );

            if( lineCounter > 0 ) {
                textPos.y += 0;
            }
            else {
                textPos.y += m_textOffset;
            }
            newText.setPos( textPos );

            pushText( newText );
        }
    }
}

void CTooltip::updateHeight( const CTextWrapper& textWrapper ) {
    float textPos   = textWrapper.getPos().y;
    float textSize  = textWrapper.getLocalBounds().height;

    if( textPos + textSize > m_size.y - 15 ) {
        m_size.y = textPos + textSize + 15;
    }

    onSizeChange();
}
