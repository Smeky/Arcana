#ifndef CUPGRADEMANAGER_H
#define CUPGRADEMANAGER_H

#include <SFML/System/Vector2.hpp>
#include <string>
#include <vector>

struct lua_State;

struct UpgradeData {
    std::string         ID;
    std::string         Name;
    std::string         Description;
    std::string         TextureID;
    std::string         Unlocks[ 3 ];
    std::vector<size_t> Prices;
    sf::Vector2f        Pos;
    size_t              Level           = 0;
    size_t              LevelMax        = 0;
    bool                Locked          = true;
};

class CUpgradeManager {
private:
    typedef std::vector<UpgradeData>    Upgrades;

public:
            void            close           ();

            bool            save            ();
            bool            load            ( lua_State* state, int index );
            void            setup           ();

            bool            unlock          ( const std::string& ID );
            bool            canUnlock       ( const std::string& ID );
            size_t          getPrice        ( const std::string& ID );

            bool            loadFromSave    ();

            void            setupUpgTable   ( lua_State* state, const std::string& ID );
            void            setupUpgsTable  ( lua_State* state );

public:
    static  int             luaLoad         ( lua_State* state );
    static  int             luaSetup        ( lua_State* state );
    static  int             luaGetUpgrades  ( lua_State* state );
    static  int             luaGetUpgrade   ( lua_State* state );

private:
            std::string     getElement      ( const std::string& input, const std::string& name );

            UpgradeData*    getUpgrade      ( const std::string& ID );

            void            handleUpgrade   ( const UpgradeData* data, bool init = false );

private:
            Upgrades        m_upgrades;

};

#endif // CUPGRADEMANAGER_H
