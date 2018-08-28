#ifndef CSOUNDEMITTER_H
#define CSOUNDEMITTER_H

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

class CEntity;

class CSoundEmitter {
public:
                    CSoundEmitter   ();
                   ~CSoundEmitter   ();

    void            update          ();

    void            setParent       ( const CEntity* entity );
    const CEntity*  getParent       () const;

    void            setBuffer       ( const sf::SoundBuffer& buffer );

    void            setVolume       ( float volume );

    bool            isActive        () const;

private:
    const CEntity*  m_parent;

    sf::Sound       m_sound;

    float           m_volume;
};

#endif // CSOUNDEMITTER_H
