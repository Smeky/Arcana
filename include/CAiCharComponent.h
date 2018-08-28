#ifndef CAICHARCOMPONENT_H
#define CAICHARCOMPONENT_H

#include "SFML/System/Time.hpp"
#include <SFML/System/Vector2.hpp>
#include <vector>

#include <CTimer.h>

class CCharacter;

class CAiCharComponent {
public:
                    CAiCharComponent();
    virtual        ~CAiCharComponent() {}

    virtual void    update          ( const sf::Time& delta ) {}

    void            setParent       ( CCharacter* parent );

protected:
    CCharacter*     m_parent;

};

class CAiRangerHuman : public CAiCharComponent {
public:
    virtual void    update          ( const sf::Time& delta );

private:
    std::vector<sf::Vector2f>   m_pathWaypoints;
    CTimer                      m_timerPathFind;
    bool                        m_seekTarget;
};

#endif // CAICHARCOMPONENT_H
