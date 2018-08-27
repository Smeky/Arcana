#include "CWindow.h"

#include "CGame.h"
#include "CWidget.h"
#include "Utility.h"

CWindow::CWindow()
: m_shown       ( true )
, m_active      ( true )
, m_mouseFocus  ( false )
, m_mouseover   ( false )
{}

CWindow::~CWindow() {
    while( !m_widgets.empty() ) delete m_widgets.back(), m_widgets.pop_back();
}

void CWindow::handleInput( sf::Event event ) {
    if( !m_shown || !m_active ) return;

    if( event.type == sf::Event::MouseMoved ) {
        if( getBox().contains( event.mouseMove.x, event.mouseMove.y ) ) {
            m_mouseover = true;
        }
        else {
            m_mouseover = false;
        }
    }
    else if( event.type == sf::Event::MouseButtonPressed ) {
        if( getBox().contains( event.mouseButton.x, event.mouseButton.y ) ) {
            m_mouseFocus = true;
        }
        else {
            m_mouseFocus = false;
        }
    }

    for( auto it : m_widgets ) {
        it->handleInput( event );
    }
}

void CWindow::update( const sf::Time& delta ) {
    if( !m_shown ) return;

    for( auto it = m_widgets.begin(); it != m_widgets.end(); ) {
        (*it)->update( delta );

        if( (*it)->shouldDelete() ) {
            delete *it;
            it = m_widgets.erase( it );
        }
        else {
            it++;
        }
    }
}

void CWindow::draw( sf::RenderTarget& window, sf::RenderStates states ) const {
    if( !m_shown ) return;

    sf::Transformable transform;

    transform.setPosition( m_pos );

    states.transform *= transform.getTransform();

    window.draw( m_bgSprite, states );

    for( auto it : m_widgets ) {
        window.draw( *it, states );
    }

    window.draw( m_frontSprite, states );
}

void CWindow::setID( size_t ID ) {
    m_ID = ID;
}

size_t CWindow::getID() const {
    return m_ID;
}

std::vector<size_t> CWindow::getWidgetsID() const {
    std::vector<size_t> IDs;

    for( auto it : m_widgets ) {
        IDs.push_back( it->getID() );
    }

    return IDs;
}

void CWindow::addWidget( CWidget* widget ) {
    widget->setParent( this );

    m_widgets.push_back( widget );
}

void CWindow::delWidget( CWidget* widget ) {
    for( auto it = m_widgets.begin(); it != m_widgets.end(); it++ ) {
        if( (*it) == widget ) {
            m_widgets.erase( it );
            break;
        }
    }
}

CWidget* CWindow::getWidget( size_t ID ) {
    for( auto it : m_widgets ) {
        if( it->getID() == ID ) {
            return it;
        }
    }

    std::cout << "Warning: Wrong ID of widget. ID: " << ID << std::endl;

    return nullptr;
}

void CWindow::setTexture( const std::string& textureID ) {
    m_bgSprite.setTexture( CGame::AssetManager.getTexture( textureID ) );
    Util::resizeSprite( m_bgSprite, m_size );
}

void CWindow::setFrontTexture( const std::string& textureID ) {
    m_frontSprite.setTexture( CGame::AssetManager.getTexture( textureID ) );
    Util::resizeSprite( m_frontSprite, m_size );
}

void CWindow::setWidgetBgTexture( size_t ID, const std::string& textureID ) {
    for( auto it : m_widgets ) {
        if( it->getID() == ID ) {
            it->setBgTexture( textureID );

            return;
        }
    }
}

void CWindow::setWidgetMiddleTexture( size_t ID, const std::string& textureID ) {
    for( auto it : m_widgets ) {
        if( it->getID() == ID ) {
            it->setMiddleTexture( textureID );

            return;
        }
    }
}

void CWindow::setWidgetFrontTexture( size_t ID, const std::string& textureID ) {
    for( auto it : m_widgets ) {
        if( it->getID() == ID ) {
            it->setFrontTexture( textureID );

            return;
        }
    }
}

void CWindow::setShown( bool shown ) {
    m_shown = shown;

    if( !m_shown ) {
        m_mouseFocus = false;
    }
}

void CWindow::reverseShown() {
    m_shown = !m_shown;

    if( !m_shown ) {
        m_mouseFocus = false;
    }
}

bool CWindow::isShown() const {
    return m_shown;
}

void CWindow::setActive( bool active ) {
    m_active = active;

    if( !m_active ) {
        m_mouseFocus = false;
    }
}

void CWindow::reverseActive() {
    m_active = !m_active;

    if( !m_active ) {
        m_mouseFocus = false;
    }
}

bool CWindow::isActive() const {
    return m_active;
}

bool CWindow::hasMouseFocus() const {
    return m_mouseFocus;
}

bool CWindow::hasMouseover( int x, int y ) {
    return getBox().contains( x, y );
}

bool CWindow::isMouseover() const {
    return m_mouseover;
}

void CWindow::onPosChange() {
    m_pos.x = (int)( m_pos.x + 0.5 );
    m_pos.y = (int)( m_pos.y + 0.5 );

    for( auto it : m_widgets ) {
        it->updatePos();
    }
}
