#ifndef CINVENTORY_H
#define CINVENTORY_H

#include "string"

#include "Item.h"
#include "CharacterStats.h"

struct lua_State;

class CInventory {
public:

                    CInventory          ();
                   ~CInventory          ();

    void            close               ();

    /** Items */
    int             addItem             ( Item* item );             // Return index of itemslot ( -1 in case of no slot was free )
    bool            delItem             ( size_t index );           // Delete item by index in items array
    bool            delItem             ( const std::string& ID );  // Delete item by its ID
    bool            moveItem            ( size_t from, size_t to );
    void            autoEquipItem       ( size_t from );
    const Item*     getItem             ( size_t index ) const;

    /** Runestones */
    void            setRunestones       ( size_t amount );
    void            addRunestones       ( size_t amount );
    bool            removeRunestones    ( size_t amount );
    size_t          getRunestones       () const;

    bool            buyUpgrade          ( const std::string& ID );

    bool            isFull              () const;

    Item            itemFromLuaTable    ( lua_State* state, int index );

public:
    /** Items */
    static int      luaAddItem          ( lua_State* state );
    static int      luaDelItem          ( lua_State* state );
    static int      luaHasItem          ( lua_State* state );
    static int      luaGetItemData      ( lua_State* state );
    static int      luaIsFull           ( lua_State* state );

    /** Runestones */
    static int      luaAddRunestones    ( lua_State* state );
    static int      luaGetRunestones    ( lua_State* state );
    static int      luaRemoveRunestones ( lua_State* state );

    static int      luaBuyUpgrade       ( lua_State* state );

private:
    bool            isEquipSlot         ( size_t index );
    void            addPlayerStats      ( size_t index );           // Adds stats from item at given index
    void            delPlayerStats      ( size_t index );           // removes stats from item at given index

private:
    static const size_t m_maxItems = 21 + SLOT_TYPECOUNT;

    Item*           m_items[ m_maxItems ];

    size_t          m_runestones;
};

#endif // CINVENTORY_H
