#ifndef CCOMBATTEXTSYSTEM_H
#define CCOMBATTEXTSYSTEM_H

#include "SFML/Graphics.hpp"
#include "SFML/System/Time.hpp"

class CCombatText;

enum class CombatTextType : int {
    COMBATTEXT_DMGNORMAL,
    COMBATTEXT_DMGCRIT,
    COMBATTEXT_HEALNORMAL,
    COMBATTEXT_HEALCRIT,
};

class CCombatTextSystem {
private:
    typedef std::vector<CCombatText*> CombatTexts;

public:
                    CCombatTextSystem   () {}
                   ~CCombatTextSystem   ();

    void            close               ();

    void            update              ( const sf::Time& delta );
    void            render              ( sf::RenderTarget& window, sf::RenderStates states ) const;

    void            createCombatText    ( const sf::Vector2f& pos, int number, CombatTextType type );

private:
    CombatTexts     m_combatTexts;

};

#endif // CCOMBATTEXTSYSTEM_H
