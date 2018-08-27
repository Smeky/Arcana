#include "CInventory.h"

#include <cassert>
#include <lua.hpp>

#include <CGame.h>
#include <CPlayer.h>
#include <LuaWrapper.h>

#include <iostream>
using namespace std;

CInventory::CInventory()
: m_runestones      ( 0 )
{
    // Initialize all items as nullptr
    for( size_t i = 0; i < m_maxItems; i++ ) {
        m_items[ i ] = nullptr;
    }
}

CInventory::~CInventory() {
    close();
}

void CInventory::close() {
    // Destroy all items and free the memory
    for( size_t i = 0; i < m_maxItems; i++ ) {
        if( m_items[ i ] != nullptr ) {
            delete m_items[ i ];
            m_items[ i ] = nullptr;
        }
    }
}

int CInventory::addItem( Item* item ) {
    Slottype itemSlot   = item->Slot;

    // Try to auto-equip the item
    if( itemSlot < Slottype::SLOT_TYPECOUNT ) {
        // If there is empty equip slot for the item
        if( m_items[ itemSlot ] == nullptr ) {
            m_items[ itemSlot ] = item;

            // Send message about adding item to inventory ( used by gui )
            CGame::MessageSystem.broadcast( new CTypedMessage<size_t>( "PlayerItemAdd", itemSlot ) );

            // Add items stats to player
            addPlayerStats( itemSlot );

            return itemSlot;
        }
    }

    // Find free item slot
    for( size_t i = SLOT_TYPECOUNT; i < m_maxItems; i++ ) {
        if( m_items[ i ] == nullptr ) {
            // Put item in to free item slot
            m_items[ i ] = item;

            // Send message about adding item to inventory ( used by gui )
            CGame::MessageSystem.broadcast( new CTypedMessage<size_t>( "PlayerItemAdd", i ) );

            // Return index of the item slot
            return i;
        }
    }


    // Item was not added to inventory so free the memory
    delete item;
    item = nullptr;

    return - 1;
}

bool CInventory::delItem( size_t index ) {
    if( index >= m_maxItems ) return false;

    if( m_items[ index ] != nullptr ) {
        // In case item is in equip slot, remove stats from player
        if( isEquipSlot( index ) ) {
//            CGame::EntitySystem.getPlayer()->removeBaseStats( m_items[ index ]->Stats );

            delPlayerStats( index );
        }

        // Destroy the item
        delete m_items[ index ];
        m_items[ index ] = nullptr;

        // Send message about removing item from inventory ( used by gui )
        CGame::MessageSystem.broadcast( new CTypedMessage<size_t>( "PlayerItemDel", index ) );

        return true;
    }

    return false;
}

bool CInventory::delItem( const std::string& ID ) {
    for( size_t i = 0; i < m_maxItems; i++ ) {
        if( m_items[ i ] != nullptr ) {
            if( m_items[ i ]->ID == ID ) {
                return delItem( i );
            }
        }
    }

    return false;
}

bool CInventory::moveItem( size_t from, size_t to ) {
    if( from >= m_maxItems || to >= m_maxItems ) return false;

    // If destination itemslot has item
    if( m_items[ to ] != nullptr ) {
        // In case items are the same
        if( m_items[ from ]->ID == m_items[ to ]->ID ) {
            return true;
        }

        // Store items type to keep things clear
        Slottype fromItemType   = m_items[ from ]->Slot;
        Slottype toItemType     = m_items[ to ]->Slot;

        // Check if right clicked item can be equipped
        if( isEquipSlot( to ) ) {
            if( fromItemType != toItemType ) {
                return false;
            }
        }

        // Store item from destination to buffer it can be swapped
        Item* buffer = m_items[ to ];

        // Send message about removing item from inventory ( used by gui )
        CGame::MessageSystem.broadcast( new CTypedMessage<size_t>( "PlayerItemDel", from ) );
        CGame::MessageSystem.broadcast( new CTypedMessage<size_t>( "PlayerItemDel", to ) );

        std::string toEffectID      = m_items[ to ]->EffectID;
        std::string fromEffectID    = m_items[ from ]->EffectID;

        // Update players stats
        if( isEquipSlot( to ) ) {
            delPlayerStats( to );
            addPlayerStats( from );

            if( !toEffectID.empty() ) {
                CGame::EntitySystem.getPlayer()->delEffect( toEffectID );
            }
            if( !fromEffectID.empty() ) {
                CGame::ScriptSystem.fireEventWithString( "AddEffectToPlayer", 1, fromEffectID.c_str() );
            }
        }
        else if( isEquipSlot( from ) ) {
            delPlayerStats( from );
            addPlayerStats( to );

            if( !toEffectID.empty() ) {
                CGame::ScriptSystem.fireEventWithString( "AddEffectToPlayer", 1, toEffectID.c_str() );
            }
            if( !fromEffectID.empty() ) {
                CGame::EntitySystem.getPlayer()->delEffect( fromEffectID );
            }
        }

        // Swap items
        m_items[ to ]   = m_items[ from ];
        m_items[ from ] = buffer;

        // Send message about removing item from inventory ( used by gui )
        CGame::MessageSystem.broadcast( new CTypedMessage<size_t>( "PlayerItemAdd", from ) );
        CGame::MessageSystem.broadcast( new CTypedMessage<size_t>( "PlayerItemAdd", to ) );
        CGame::ScriptSystem.fireEventWithInt( "PlayerItemMove", 1, to );
    }
    else {
        Slottype itemSlot   = m_items[ from ]->Slot;

        // In case destination itemslot is equip slot
        if( to < Slottype::SLOT_TYPECOUNT ) {
            // If type of item doesn't match the item slot
            if( itemSlot != (int)to ) {
                return false;
            }
        }

        // Send message about removing item from inventory ( used by gui )
        CGame::MessageSystem.broadcast( new CTypedMessage<size_t>( "PlayerItemDel", from ) );

        std::string fromEffectID    = m_items[ from ]->EffectID;

        if( isEquipSlot( to ) ) {
            addPlayerStats( from );

            if( !fromEffectID.empty() ) {
                CGame::ScriptSystem.fireEventWithString( "AddEffectToPlayer", 1, fromEffectID.c_str() );
            }
        }
        else if( isEquipSlot( from ) ) {
            delPlayerStats( from );

            if( !fromEffectID.empty() ) {
                CGame::EntitySystem.getPlayer()->delEffect( fromEffectID );
                CGame::EntitySystem.getPlayer()->delEffect( fromEffectID );
            }
        }

        // Move item
        m_items[ to ]   = m_items[ from ];
        m_items[ from ] = nullptr;

        // Send message about removing item from inventory ( used by gui )
        CGame::MessageSystem.broadcast( new CTypedMessage<size_t>( "PlayerItemAdd", to ) );
        CGame::ScriptSystem.fireEventWithInt( "PlayerItemMove", 1, to );
    }

    return true;
}

void CInventory::autoEquipItem( size_t from ) {
         if( from >= m_maxItems )           return;
    else if( m_items[ from ] == nullptr )   return;

    Slottype fromItemType   = CGame::EntitySystem.getInventory().getItem( from )->Slot;

    // If item can be equipped
    if( isEquipSlot( fromItemType ) ) {
        // Check if item is not equipped already
        if( (int)from != fromItemType ) {
            // Move the item
            moveItem( from, fromItemType );
        }
    }
}

void CInventory::setRunestones( size_t amount ) {
    m_runestones = amount;
}

void CInventory::addRunestones( size_t amount ) {
    m_runestones += amount;
}

bool CInventory::removeRunestones( size_t amount ) {
    if( (int)m_runestones - (int)amount < 0 ) {
        return false;
    }

    m_runestones -= amount;

    return true;
}

size_t CInventory::getRunestones() const {
    return m_runestones;
}

bool CInventory::buyUpgrade( const std::string& ID ) {
    if( CGame::UpgradeManager.canUnlock( ID ) ) {
        if( removeRunestones( CGame::UpgradeManager.getPrice( ID ) ) ) {
            CGame::UpgradeManager.unlock( ID );

            return true;
        }
    }

    return false;
}

bool CInventory::isFull() const {
    for( auto it : m_items ) {
        if( it == nullptr ) {
            return false;
        }
    }

    return true;
}

const Item* CInventory::getItem( size_t index ) const {
    assert( index < m_maxItems && "Wrong item index" );

    return m_items[ index ];
}

Item CInventory::itemFromLuaTable( lua_State* state, int index ) {
    Item item;

    // If the ID is missing, return empty item
    lua_getfield( state, index, "ID" );
    if( lua_isnil( state, - 1 ) ) {
        std::cout << "Error: Unable to add item to inventory. ID is missing!" << std::endl;

        lua_pop( state, 1 );

        return item;
    }
    else {
        item.ID = lua_tostring( state, - 1 );
    }

    item.Name       = "Unknown";
    lWrapper::stringFromLuaTable( state, index, "name",         item.Name );

    lWrapper::stringFromLuaTable( state, index, "description",  item.Description );

    item.TextureID  = "GUI_ITEM_MISSING";
    lWrapper::stringFromLuaTable( state, index, "textureID",    item.TextureID );

    lWrapper::valueFromLuaTable<ItemRarity> ( state, index, "rarity",   item.Rarity );
    lWrapper::valueFromLuaTable<Slottype>   ( state, index, "slot",     item.Slot );

    lua_getfield( state, index, "stats" );

    if( lua_istable( state, - 1 ) ) {
        for( lua_pushnil( state ); lua_next( state, - 2 ) != 0; lua_pop( state, 1 ) ) {
            // Store index of stat table
            int tableIndex  = lua_gettop( state );

            // Initialize item stats
            ItemStat    stat;

            lWrapper::valueFromLuaTable<StatIndex>      ( state, tableIndex, "stat",     stat.statIndex );
            lWrapper::valueFromLuaTable<float>          ( state, tableIndex, "value",    stat.statValue );
            lWrapper::valueFromLuaTable<CalcOperation>  ( state, tableIndex, "operand",  stat.statOperand );

            item.Stats.push_back( stat );
        }
    }

    lWrapper::stringFromLuaTable( state, index, "effectID",    item.EffectID );

    lua_pop( state, 2 );

    return item;
}

/** LUA FUNCTIONS */

int CInventory::luaAddItem( lua_State* state ) {
    int argc = lua_gettop( state );

    int result  = - 1;

    if( argc == 2 ) {
        Item temp = CGame::EntitySystem.getInventory().itemFromLuaTable( state, argc );

        if( !temp.ID.empty() ) {
            Item* item = new Item( temp );

            result = CGame::EntitySystem.getInventory().addItem( item );
        }
    }
    else if( argc == 3 ) {
        Item    temp    = CGame::EntitySystem.getInventory().itemFromLuaTable( state, argc - 1 );
        size_t  amount  = lua_tointeger( state, argc );

        if( !temp.ID.empty() ) {
            for( ; amount > 0; amount-- ) {
                Item* item = new Item( temp );

                result = CGame::EntitySystem.getInventory().addItem( item );
            }
        }
    }
    else {
        std::cout << "Error: Unable to add item to inventory. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    lua_pushinteger( state, result );

    return 1;
}

int CInventory::luaDelItem( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc != 2 ) {
        std::cout << "Error: Unable to delete item from inventory. Wrong amount of arguments!" << std::endl;

        lua_pushboolean( state, false );

        return 1;
    }

    bool success = false;

    if( lua_isinteger( state, - 1 ) ) {
        size_t index    = lua_tonumber( state, - 1 );

        success = CGame::EntitySystem.getInventory().delItem( index );
    }
    else {
        std::string ID  = lua_tostring( state, - 1 );

        success = CGame::EntitySystem.getInventory().delItem( ID );
    }

    lua_pop( state, argc );

    lua_pushboolean( state, success );

    // Return if deleting was successful
    return 1;
}

int CInventory::luaHasItem( lua_State* state ) {
    int argc = lua_gettop( state );

    if( argc != 2 ) {
        std::cout << "Error: Unable to check if inventory has item. Wrong amount of arguments!" << std::endl;

        lua_pop( state, argc );

        lua_pushboolean( state, false );

        return 1;
    }

    size_t index = lua_tointeger( state, argc );

    lua_pop( state, argc );

    lua_pushboolean( state, CGame::EntitySystem.getInventory().getItem( index ) != nullptr );

    return 1;
}

int CInventory::luaGetItemData( lua_State* state ) {
    int argc = lua_gettop( state );

    // Check for wrong amount of arguments
    assert( argc == 2 && "Wrong amount of arguments" );

    // Store item index
    size_t index = lua_tointeger( state, argc );

    // Clean the stack
    lua_pop( state, argc );

    // Check if the index is not wrong
    assert( index < m_maxItems && "Wrong item index" );

    // Store the item
    const Item* item = CGame::EntitySystem.getInventory().getItem( index );

    // Check if the item is nullptr
    assert( item != nullptr && "Item slot is empty" );

    // Create item table
    lua_createtable( state, 0, 5 );

    // Set item stats
    lua_pushstring( state, item->Name.c_str() );
    lua_setfield( state, - 2, "name" );

    lua_pushstring( state, item->Description.c_str() );
    lua_setfield( state, - 2, "description" );

    lua_pushstring( state, item->TextureID.c_str() );
    lua_setfield( state, - 2, "icon" );

    lua_pushinteger( state, (int)item->Rarity );
    lua_setfield( state, - 2, "rarity" );

    // Create stats table
    lua_createtable( state, 0, item->Stats.size() );
    for( size_t i = 0; i < item->Stats.size(); i++ ) {
        lua_createtable( state, 0, 3 );

        lua_pushinteger( state, (int)item->Stats[ i ].statIndex );
        lua_setfield( state, - 2, "index" );

        lua_pushnumber( state, item->Stats[ i ].statValue );
        lua_setfield( state, - 2, "value" );

        lua_pushinteger( state, (int)item->Stats[ i ].statOperand );
        lua_setfield( state, - 2, "operand" );

        // Put a single stat in to stats table
        lua_rawseti( state, - 2, i + 1 );
    }

    // Put stats table in to item table
    lua_setfield( state, - 2, "stats" );

    return 1;
}

int CInventory::luaIsFull( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    lua_pushboolean( state, CGame::EntitySystem.getInventory().isFull() );

    return 1;
}

int CInventory::luaAddRunestones( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 2 && "Wrong amount of arguments" );

    size_t amount   = lua_tointeger( state, argc );

    CGame::EntitySystem.getInventory().addRunestones( amount );

    lua_pop( state, argc );

    return 0;
}

int CInventory::luaRemoveRunestones( lua_State* state ) {
    int argc = lua_gettop( state );

    assert( argc == 2 && "Wrong amount of arguments" );

    size_t amount   = lua_tointeger( state, argc );

    bool success    = CGame::EntitySystem.getInventory().removeRunestones( amount );

    lua_pop( state, argc );

    lua_pushboolean( state, success );

    return 1;
}

int CInventory::luaGetRunestones( lua_State* state ) {
    int argc = lua_gettop( state );

    lua_pop( state, argc );

    lua_pushinteger( state, CGame::EntitySystem.getInventory().getRunestones() );

    return 1;
}

int CInventory::luaBuyUpgrade( lua_State* state ) {
    int argc = lua_gettop( state );

    bool success = false;

    if( argc == 2 ) {
        if( lua_isstring( state, argc ) ) {
            std::string upgradeID = lua_tostring( state, argc );

            success = CGame::EntitySystem.getInventory().buyUpgrade( upgradeID );
        }
        else {
            std::cout << "Error: Unable to purchase upgrade. Expected string, got " << lua_typename( state, lua_type( state, argc ) ) << std::endl;
        }
    }
    else {
        std::cout << "Error: Unable to purchase upgrade. Wrong amount of arguments" << std::endl;
    }

    lua_pop( state, argc );

    lua_pushboolean( state, success );

    return 1;
}

/** PRIVATE */

bool CInventory::isEquipSlot( size_t index ) {
    return index < Slottype::SLOT_TYPECOUNT;
}

void CInventory::addPlayerStats( size_t index ) {
    Item* item = m_items[ index ];

    for( auto& it : item->Stats ) {
        size_t modID = CGame::EntitySystem.getPlayer()->addStat( it.statIndex, it.statValue, it.statOperand );

        it.statModID = modID;
    }

    CGame::EntitySystem.getPlayer()->updateStats();
}

void CInventory::delPlayerStats( size_t index ) {
    Item* item = m_items[ index ];

    for( auto& it : item->Stats ) {
        CGame::EntitySystem.getPlayer()->delStat( it.statIndex, it.statModID );
    }

    CGame::EntitySystem.getPlayer()->updateStats();
}
