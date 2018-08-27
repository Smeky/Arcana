#include "CWindowInventory.h"

#include "CGame.h"
#include "CButtonImage.h"
#include "CInventory.h"
#include "CItemdrag.h"
#include "Utility.h"

CWindowInventory::CWindowInventory()
: m_itemdrag    ( nullptr )
{}

CWindowInventory::~CWindowInventory() {
    destroyItemdrag();

    while( !m_itemslots.empty() ) delete m_itemslots.back(), m_itemslots.pop_back();
}

void CWindowInventory::handleMsg( CMessage* msg ) {
    if( msg->getMsg() == "PlayerItemAdd" ) {
        size_t index = dynamic_cast<CTypedMessage<size_t>*>( msg )->getValue();

        addItem( index );
    }
    else if( msg->getMsg() == "PlayerItemDel" ) {
        size_t index = dynamic_cast<CTypedMessage<size_t>*>( msg )->getValue();

        delItem( index );
    }
    else if( msg->getMsg() == "PlayerItemMove" ) {

    }
}

void CWindowInventory::handleInput( sf::Event event ) {
    if( !m_shown ) return;

    CWindow::handleInput( event );

    for( size_t i = 0; i < m_itemslots.size(); i++ ) {
        m_itemslots[ i ]->handleInput( event );

        if( event.type == sf::Event::MouseButtonPressed ) {
            if( m_itemslots[ i ]->isPressed() ) {
                if( m_itemdrag == nullptr ) {
                    const Item* item = CGame::EntitySystem.getInventory().getItem( i );

                    if( item != nullptr ) {
                        sf::Vector2f mousePos   = Util::convertVector<float>( sf::Mouse::getPosition( CGame::GameWindow ) );
                        sf::Vector2f offset     = mousePos - m_itemslots[ i ]->getPos();

                        m_itemdrag = new CItemdrag( i, mousePos, offset, item->TextureID );

                        // Hide item in item slot ( so moving item around feels more smooth )
                        m_itemslots[ i ]->setTopDisplay( false );
                        m_itemslots[ i ]->setMidDisplay( false );
                    }
                }
            }
        }
        else if( event.type == sf::Event::MouseButtonReleased ) {
            if( m_itemdrag != nullptr ) {
                if( m_itemslots[ i ]->isMouseover() ) {
                    CGame::EntitySystem.getInventory().moveItem( m_itemdrag->getItemIndex(), i );

                    destroyItemdrag();
                }
            }
        }
    }

    if( event.type == sf::Event::MouseButtonReleased ) {
        if( m_itemdrag != nullptr ) {
            sf::Vector2i mousePos = sf::Mouse::getPosition( CGame::GameWindow );

            if( hasMouseover( mousePos.x, mousePos.y ) ) {
                destroyItemdrag();
            }
            else {
                CGame::ScriptSystem.fireEventWithInt( "InventoryDestroyItem", 1, m_itemdrag->getItemIndex() );

                destroyItemdrag();
            }
        }
    }
}

void CWindowInventory::update( const sf::Time& delta ) {
    if( !m_shown ) return;

    CWindow::update( delta );

    for( size_t i = 0; i < m_itemslots.size(); i++ ) {
        if( m_itemslots[ i ]->isRClicked() ) {
            CGame::EntitySystem.getInventory().autoEquipItem( i );
        }

        m_itemslots[ i ]->update( delta );
    }

    if( m_itemdrag != nullptr ) {
        m_itemdrag->update();
    }
}

void CWindowInventory::draw( sf::RenderTarget& window, sf::RenderStates states ) const {
    if( !m_shown ) return;

    CWindow::draw( window, states );

    for( auto it : m_itemslots ) {
        window.draw( *it, states );
    }

    if( m_itemdrag != nullptr ) {
        window.draw( *m_itemdrag, states );
    }
}

std::vector<size_t> CWindowInventory::getWidgetsID() const {
    std::vector<size_t> IDs;

    IDs = CWindow::getWidgetsID();

    for( auto& it : m_itemslots ) {
        IDs.push_back( it->getID() );
    }

    return IDs;
}

CWidget* CWindowInventory::getWidget( size_t ID ) {
    for( auto it : m_widgets ) {
        if( it->getID() == ID ) {
            return it;
        }
    }

    for( auto it : m_itemslots ) {
        if( it->getID() == ID ) {
            return it;
        }
    }

    std::cout << "Warning: Wrong ID of widget. ID: " << ID << std::endl;

    return nullptr;
}

void CWindowInventory::addItemslot( CButtonImage* itemslot ) {
    itemslot->setParent( this );

    m_itemslots.push_back( itemslot );

    updateItemShadow( m_itemslots.size() - 1 );
}

void CWindowInventory::addItem( size_t index ) {
    if( index >= m_itemslots.size() ) return;

    const Item* item = CGame::EntitySystem.getInventory().getItem( index );

    if( item != nullptr ) {
        m_itemslots[ index ]->setMiddleTexture( item->TextureID );

        switch( item->Rarity ) {
        case ItemRarity::RARITY_COMMON:     m_itemslots[ index ]->setFrontTexture( "GUI_ITEMSLOT_RARITY_COMMON" );      break;
        case ItemRarity::RARITY_RARE:       m_itemslots[ index ]->setFrontTexture( "GUI_ITEMSLOT_RARITY_RARE" );        break;
        case ItemRarity::RARITY_EPIC:       m_itemslots[ index ]->setFrontTexture( "GUI_ITEMSLOT_RARITY_EPIC" );        break;
        case ItemRarity::RARITY_LEGENDARY:  m_itemslots[ index ]->setFrontTexture( "GUI_ITEMSLOT_RARITY_LEGENDARY" );   break;
        default:                            m_itemslots[ index ]->setFrontTexture( "GUI_ITEMSLOT_BORDER" );             break;
        }
    }
}

void CWindowInventory::delItem( size_t index ) {
    if( index >= m_itemslots.size() ) return;

    m_itemslots[ index ]->setFrontTexture( "GUI_ITEMSLOT_BORDER" );

    updateItemShadow( index );
}

void CWindowInventory::destroyItemdrag() {
    if( m_itemdrag != nullptr ) {
        // Reset item display effect
        m_itemslots[ m_itemdrag->getItemIndex() ]->setTopDisplay( true );
        m_itemslots[ m_itemdrag->getItemIndex() ]->setMidDisplay( true );

        delete m_itemdrag;
        m_itemdrag = nullptr;
    }
}

void CWindowInventory::updateItemShadow( size_t index ) {
    if( index >= m_itemslots.size() ) return;

    switch( index ) {
    case Slottype::SLOT_HELM:     m_itemslots[ index ]->setMiddleTexture( "GUI_ITEMSLOT_SHADOW_HELM" ); break;
    case Slottype::SLOT_AMULET:   m_itemslots[ index ]->setMiddleTexture( "GUI_ITEMSLOT_SHADOW_AMULET" ); break;
    case Slottype::SLOT_CHEST:    m_itemslots[ index ]->setMiddleTexture( "GUI_ITEMSLOT_SHADOW_CHEST" ); break;
    case Slottype::SLOT_PANTS:    m_itemslots[ index ]->setMiddleTexture( "GUI_ITEMSLOT_SHADOW_PANTS" ); break;
    case Slottype::SLOT_GLOVES:   m_itemslots[ index ]->setMiddleTexture( "GUI_ITEMSLOT_SHADOW_GLOVES" ); break;
    case Slottype::SLOT_SHOES:    m_itemslots[ index ]->setMiddleTexture( "GUI_ITEMSLOT_SHADOW_SHOES" ); break;
    case Slottype::SLOT_MAINHAND: m_itemslots[ index ]->setMiddleTexture( "GUI_ITEMSLOT_SHADOW_MAINHAND" ); break;
    case Slottype::SLOT_OFFHAND:  m_itemslots[ index ]->setMiddleTexture( "GUI_ITEMSLOT_SHADOW_OFFHAND" ); break;
    default:                      m_itemslots[ index ]->setMiddleTexture( "GUI_ITEMSLOT_SHADOW_BAG" ); break;
    }
}
