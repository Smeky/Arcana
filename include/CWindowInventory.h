#ifndef CWINDOWINVENTORY_H
#define CWINDOWINVENTORY_H

#include "CWindow.h"
#include "CMessageListener.h"

class CButtonImage;
class CItemdrag;

class CWindowInventory : public CWindow, public CMessageListener {
private:
    typedef std::vector<CButtonImage*>  ButtonImages;
    typedef std::vector<size_t>         Size_ts;

public:
                    CWindowInventory    ();
                   ~CWindowInventory    ();

    void            handleMsg           ( CMessage* msg );

    void            handleInput         ( sf::Event event );
    void            update              ( const sf::Time& delta );
    void            draw                ( sf::RenderTarget& window, sf::RenderStates states ) const;

    void            addItemslot         ( CButtonImage* itemslot );

    Size_ts         getWidgetsID        () const;
    CWidget*        getWidget           ( size_t ID );

private:
    void            addItem             ( size_t index );
    void            delItem             ( size_t index );

    void            destroyItemdrag     ();

    void            updateItemShadow    ( size_t index );

private:
    ButtonImages    m_itemslots;
    CItemdrag*      m_itemdrag;
};

#endif // CWINDOWINVENTORY_H
