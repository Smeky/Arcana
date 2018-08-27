#ifndef LOOTTABLE_H_INCLUDED
#define LOOTTABLE_H_INCLUDED

#include <string>
#include <vector>

struct lootItem {
    std::string     ID;
    size_t          Level   = 0;
    float           Chance  = 0.0;
};

typedef std::vector<lootItem>  ItemTable;

struct lootTable {
    size_t          minItems        = 0;
    size_t          maxItems        = 0;
    size_t          minRunestones   = 0;
    size_t          maxRunestones   = 0;
    float           runestoneChance = 0.0;
    ItemTable       itemTable;
};

#endif // LOOTTABLE_H_INCLUDED
