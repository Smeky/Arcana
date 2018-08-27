#ifndef CAICOMPONENT_H
#define CAICOMPONENT_H

#include "SFML/System/Time.hpp"
#include <SFML/System/Vector2.hpp>

struct lua_State;
class CEntity;

class CAiComponent {
public:
                    CAiComponent    ();
    virtual        ~CAiComponent    () {}

    virtual void    update          ( const sf::Time& delta ) {}

    virtual void    loadDataFromLua ( lua_State* state, int index ) {}

    void            setParent       ( CEntity* parent );

protected:
    virtual void    onParentSet     () {}

protected:
    CEntity*        m_parent;

};

class CAiLoopMovement : public CAiComponent {
public:
                    CAiLoopMovement ();

            void    update          ( const sf::Time& delta );

            void    loadDataFromLua ( lua_State* state, int idnex );

private:
            void    onParentSet     ();

private:
    sf::Vector2f    m_firstPos;
    sf::Vector2f    m_secondPos;
    sf::Vector2f    m_offsetPos;
    bool            m_toFirst;      // Determine to which position object should be moving at the time
    float           m_smoothVel;
    float           m_smoothTime;
    float           m_maxDistance;
    float           m_curDistance;

};

#endif // CAICOMPONENT_H
