#ifndef CPARTICLEEFFECT_H
#define CPARTICLEEFFECT_H

#include <vector>
#include <cstddef>
#include <SFML/System/Time.hpp>

class CParticleEmitter;

class CParticleEffect {
private:
    typedef std::vector<CParticleEmitter*>  Emitters;

public:
                    CParticleEffect     ();
                   ~CParticleEffect     ();

    void            update              ( const sf::Time& delta );

    void            setID               ( size_t ID );
    size_t          getID               () const;

    bool            isActive            () const;

    void            addEmitter          ( CParticleEmitter* emitter );

private:
    size_t          m_ID;
    bool            m_active;

    Emitters        m_emitters;

};

#endif // CPARTICLEEFFECT_H
