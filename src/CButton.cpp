#include "CButton.h"

#include "CGame.h"

#include "iostream"
using namespace std;

CButton::CButton()
: m_active      ( true )
, m_keySet      ( false )
, m_mouseover   ( false )
, m_pressed     ( false )
, m_clicked     ( false )
, m_rPressed    ( false )
, m_rClicked    ( false )
{}

void CButton::handleInput( sf::Event event ) {
    if( !m_active ) return;

    if( m_mouseover ) {
        if( event.type == sf::Event::MouseButtonPressed ) {
            switch( event.mouseButton.button ) {
            case sf::Mouse::Left:   m_pressed   = true; break;
            case sf::Mouse::Right:  m_rPressed  = true; break;
            default: break;
            }
        }
    }


    if( m_keySet ) {
        if( event.type == sf::Event::KeyPressed ) {
            if( compareKeyCode( event.key.code ) ) {
                m_pressed = true;
            }
        }
        else if( event.type == sf::Event::KeyReleased ) {
            if( compareKeyCode( event.key.code ) ) {
                m_pressed = false;
                m_clicked = true;
            }
        }
    }

    if( event.type == sf::Event::MouseMoved ) {
        bool prevMouseover = m_mouseover;

        if( getBox().contains( event.mouseMove.x, event.mouseMove.y ) ) {
            m_mouseover = true;
        }
        else {
            m_mouseover = false;
        }

        // If mouseover is now different ( mouse left or entered )
        if( prevMouseover != m_mouseover ) {
            if( !m_mouseover ) {
                CGame::ScriptSystem.fireEventWithInt( "ButtonMouseLeft", 1, m_ID );
            }
            else {
                CGame::ScriptSystem.fireEventWithInt( "ButtonMouseEnter", 1, m_ID );
            }
        }
    }
    else if( event.type == sf::Event::MouseButtonReleased ) {
        if( event.mouseButton.button == sf::Mouse::Left ) {
            if( m_pressed && m_mouseover ) {
                m_clicked = true;
            }

            m_pressed = false;
        }
        else if( event.mouseButton.button == sf::Mouse::Right ) {
            if( m_rPressed && m_mouseover ) {
                m_rClicked = true;
            }

            m_rPressed = false;
        }
    }
}

void CButton::update() {
    if( !m_active ) return;

    if( m_clicked ) {
        if( !m_msg.empty() ) {
            CGame::MessageSystem.broadcast( new CMessage( m_msg ) );
        }

        m_clicked = false;
    }
    if( m_rClicked ) {
        if( !m_rMsg.empty() ) {
            CGame::MessageSystem.broadcast( new CMessage( m_rMsg ) );
        }

        m_rClicked = false;
    }
}

void CButton::addHotkey( const sf::Keyboard::Key& key ) {
    if( !compareKeyCode( key ) ) {
        m_hotkeys.push_back( key );
    }
    else {
        cout << "Warning: adding hotkey ( KeyID: " << key << " ) to button that is already binded" << endl;
    }

    m_keySet = true;
}

void CButton::setMsg( const std::string& msg ) {
    m_msg   = msg;
}

void CButton::setRMsg( const std::string& msg ) {
    m_rMsg  = msg;
}

void CButton::setActive( bool active ) {
    m_active    = active;
}

bool CButton::isActive() const {
    return m_active;
}

bool CButton::isMouseover() const {
    return m_mouseover;
}

bool CButton::isPressed() const {
    return m_pressed;
}

bool CButton::isClicked() const {
    return m_clicked;
}

bool CButton::isRPressed() const {
    return m_rPressed;
}

bool CButton::isRClicked() const {
    return m_rClicked;
}

bool CButton::compareKeyCode( const sf::Keyboard::Key& key ) {
    for( auto& it : m_hotkeys ) {
        if( it == key ) {
            return true;
        }
    }

    return false;
}
