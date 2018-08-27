#ifndef CBUTTON_H
#define CBUTTON_H

#include "CWidget.h"

class CButton : public CWidget {
public:
                        CButton         ();

    virtual void        handleInput     ( sf::Event event );
    virtual void        update          ();

            void        addHotkey       ( const sf::Keyboard::Key& key );
            void        setMsg          ( const std::string& msg );
            void        setRMsg         ( const std::string& msg );
            void        setActive       ( bool active = true );

            bool        isActive        () const;
            bool        isMouseover     () const;
            bool        isPressed       () const;
            bool        isClicked       () const;
            bool        isRPressed      () const;
            bool        isRClicked      () const;

private:
            bool        compareKeyCode  ( const sf::Keyboard::Key& key );

protected:
    virtual void        onMouseover     () {}
    virtual void        onPress         () {}
    virtual void        onClick         () {}
    virtual void        onRPress        () {}
    virtual void        onRClick        () {}

protected:
    std::vector<sf::Keyboard::Key>   m_hotkeys;
    std::string         m_msg;
    std::string         m_rMsg;

    bool                m_active;
    bool                m_keySet;
    bool                m_mouseover;
    bool                m_pressed;
    bool                m_clicked;
    bool                m_rPressed;
    bool                m_rClicked;

};

#endif // CBUTTON_H
