#ifndef CAICHARCOMPONENT_H
#define CAICHARCOMPONENT_H

#include "SFML/System/Time.hpp"

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

};

#endif // CAICHARCOMPONENT_H
