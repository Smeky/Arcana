#include "CLootSystem.h"

#include <lua.hpp>

#include <CGame.h>
#include <CCharacter.h>
#include <Utility.h>
#include <LuaWrapper.h>
#include <CRNG.h>

#include <iostream>
using namespace std;

CLootSystem::CLootSystem()
{
    //ctor
}

lootTable CLootSystem::loadLootTableFromLua( lua_State* state, int index ) {
    lootTable loot;

    lWrapper::valueFromLuaTable<size_t> ( state, index, "minItems",         loot.minItems );
    lWrapper::valueFromLuaTable<size_t> ( state, index, "maxItems",         loot.maxItems );
    lWrapper::valueFromLuaTable<size_t> ( state, index, "minRunestones",    loot.minRunestones );
    lWrapper::valueFromLuaTable<size_t> ( state, index, "maxRunestones",    loot.maxRunestones );
    lWrapper::valueFromLuaTable<float>  ( state, index, "runestoneChance",  loot.runestoneChance );

    // Load all items
    lua_getfield( state, index, "table" );
    if( !lua_isnil( state, - 1 ) ) {
        loot.itemTable  = goTroughTableTree( state, lua_gettop( state ) );
    }

    lua_pop( state, 1 );

    return loot;
}

void CLootSystem::createLoot( const CCharacter* character ) {
    lootTable table     = character->getLootTable();

    size_t  level       = character->getLevel();
    size_t  runestones  = rollForRunestones( table );
    size_t  itemAmount  = CRNG::iRandom( table.minItems, table.maxItems );

    // list that excludes all 100% drop chance items
    std::list<lootItem> fixedTable;

    // vector of all item IDs that should be dropped
    std::vector<std::string>    itemIDsToDrop;

    for( auto it : table.itemTable ) {
        if( it.Chance != 100 ) {
            fixedTable.push_back( it );
        }
        else {
            itemIDsToDrop.push_back( it.ID );
        }
    }

    // Sort items based on their chance so we can go trough the list from smallest to highest chance
    fixedTable.sort( []( const lootItem& first, const lootItem& second ){
        return first.Chance < second.Chance;
    } );

    if( itemAmount != 0 ) {
        for( size_t i = 0; i < itemAmount; i++ ) {
            std::string buffer = rollForItem( fixedTable, itemIDsToDrop, level );

            if( !buffer.empty() ) {
                itemIDsToDrop.push_back( buffer );
            }
        }
    }

    spawnLoot( itemIDsToDrop, character->getCenter() );

    if( runestones != 0 ) {
        spawnRunestones( runestones, character->getCenter() );
    }
}

/** PRIVATE */

ItemTable CLootSystem::goTroughTableTree( lua_State* state, int index ) {
    ItemTable table;

    if( lua_istable( state, index ) ) {
        for( lua_pushnil( state ); lua_next( state, index ) != 0; lua_pop( state, 1 ) ) {
            ItemTable temp;

            // Store pair key
            const char* key = lWrapper::keyFromLuaTable( state, lua_gettop( state ) - 1 );

            // Check if table has items in it or another sub table
            // and based on that load items into vector
            if( std::string( key ) != "items" ) {
                temp = goTroughTableTree( state, lua_gettop( state ) );
            }
            else {
                temp = getItemFromLuaTable( state, lua_gettop( state ) );
            }

            // Insert loaded items in to loot table vector
            table.insert( table.end(), temp.begin(), temp.end() );
        }
    }

    return table;
}

ItemTable CLootSystem::getItemFromLuaTable( lua_State* state, int index ) {
    ItemTable table;

    // Load all items from items table
    for( lua_pushnil( state ); lua_next( state, index ) != 0; lua_pop( state, 1 ) ) {
        lootItem item;

        lWrapper::stringFromLuaTable            ( state, lua_gettop( state ), "ID",     item.ID );
        lWrapper::valueFromLuaTable<size_t>     ( state, lua_gettop( state ), "level",  item.Level );
        lWrapper::valueFromLuaTable<float>      ( state, lua_gettop( state ), "chance", item.Chance );

        table.push_back( item );
    }

    return table;
}

void CLootSystem::spawnLoot( const std::vector<std::string>& itemsToDrop, const sf::Vector2f& pos ) {
    for( size_t i = 0; i < itemsToDrop.size(); i++ ) {
        sf::Vector2f posToDrop  = pos;

        if( i > 1 ) {
            posToDrop.x = CRNG::iRandom( pos.x - 10, pos.x + 10 );
            posToDrop.y = CRNG::iRandom( pos.y - 10, pos.y + 10 );
        }

        lua_State* state = CGame::ScriptSystem.getState();

        lua_createtable( state, 0, 3 );
        int index   = lua_gettop( state );

        lua_pushstring( state, itemsToDrop[ i ].c_str() );
        lua_setfield( state, index, "ID" );

        lua_pushnumber( state, posToDrop.x );
        lua_setfield( state, index, "posX" );

        lua_pushnumber( state, posToDrop.y );
        lua_setfield( state, index, "posY" );

        CGame::ScriptSystem.fireEventWithTable( "CreateLootBag", index );
    }
}

void CLootSystem::spawnRunestones( size_t amount, const sf::Vector2f& pos ) {
    lua_State* state = CGame::ScriptSystem.getState();

    lua_createtable( state, 0, 3 );
    int index   = lua_gettop( state );

    lua_pushinteger( state, amount );
    lua_setfield( state, index, "amount" );

    lua_pushnumber( state, pos.x );
    lua_setfield( state, index, "posX" );

    lua_pushnumber( state, pos.y );
    lua_setfield( state, index, "posY" );

    CGame::ScriptSystem.fireEventWithTable( "CreateRunestones", index );
}

size_t CLootSystem::rollForRunestones( const lootTable& table ) {
    if( table.minRunestones == 0 && table.maxRunestones == 0 ) {
        return 0;
    }

    size_t amount = 0;

    if( CRNG::percentRandom() < table.runestoneChance / 100 ) {
        amount = CRNG::iRandom( table.minRunestones , table.maxRunestones );
    }

    return amount;
}

std::string CLootSystem::rollForItem( const std::list<lootItem>& items, const std::vector<std::string>& itemsToDrop, size_t charLevel ) {
    for( auto it = items.begin(); it != items.end(); it++ ) {
        bool canRoll    = true;

        for( auto dropItem : itemsToDrop ) {
            if( dropItem == it->ID ) {
                canRoll = false;
            }
        }

        if( canRoll ) {
            int levelDifference = 5 - abs( (int)it->Level - (int)charLevel );

            float dropChance  = CRNG::percentRandom();

            if( dropChance < ( it->Chance / 100 ) * ( levelDifference * 20 ) / 100 ) {
                return it->ID;
            }
        }
    }

    // In case no item has been rolled, return empty string
    return std::string();
}
