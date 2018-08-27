#ifndef CLOOTSYSTEM_H
#define CLOOTSYSTEM_H

#include <LootTable.h>
#include <list>
#include <SFML/System/Vector2.hpp>

struct lua_State;
class CCharacter;

class CLootSystem {
public:
                    CLootSystem             ();

    lootTable       loadLootTableFromLua    ( lua_State* state, int intex );

    void            createLoot              ( const CCharacter* character );

private:
    ItemTable       goTroughTableTree       ( lua_State* state, int index );
    ItemTable       getItemFromLuaTable     ( lua_State* state, int index );

    void            spawnLoot               ( const std::vector<std::string>& itemsToDrop, const sf::Vector2f& pos );
    void            spawnRunestones         ( size_t amount, const sf::Vector2f& pos );

    size_t          rollForRunestones       ( const lootTable& table );

    // Returns ID of randomly chosen item out of all in the given vector
    std::string     rollForItem             ( const std::list<lootItem>& table, const std::vector<std::string>& itemsToDrop, size_t charLevel );

};

#endif // CLOOTSYSTEM_H
