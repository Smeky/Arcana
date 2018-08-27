#ifndef ITEM_H
#define ITEM_H

#include "string"
#include "vector"

enum class StatIndex : int;
enum class CalcOperation : int;

enum class ItemRarity : int {
    RARITY_NORARITY,
    RARITY_COMMON,
    RARITY_RARE,
    RARITY_EPIC,
    RARITY_LEGENDARY,
};

enum Slottype {
    SLOT_UNKNOWN   = -1,
    SLOT_HELM      = 0,
    SLOT_AMULET,
    SLOT_CHEST,
    SLOT_PANTS,
    SLOT_GLOVES,
    SLOT_SHOES,
    SLOT_MAINHAND,
    SLOT_OFFHAND,

    SLOT_TYPECOUNT      // Keep last
};

struct ItemStat {
    StatIndex       statIndex;
    float           statValue   = 0.0;
    CalcOperation   statOperand;
    size_t          statModID   = 0;
};

struct Item {
    std::string             ID;
    std::string             Name;
    std::string             Description;
    std::string             TextureID;
    std::string             EffectID;
    ItemRarity              Rarity      = ItemRarity::RARITY_NORARITY;
    Slottype                Slot        = Slottype::SLOT_UNKNOWN;
//    CharacterStats  Stats;

    std::vector<ItemStat>   Stats;
    std::vector<size_t>     ModIDs;
};


#endif // ITEM_H
