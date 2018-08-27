#ifndef CDISPLAYSTATS_H
#define CDISPLAYSTATS_H

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/RenderStates.hpp"
#include "SFML/System/Time.hpp"

#include "CTextWrapper.h"
#include "CTimer.h"

class CDisplayStats {
public:
    void            init            ();

    void            update          ( const sf::Time& delta );
    void            render          ( sf::RenderTarget& window, sf::RenderStates states ) const;

private:
    void            updateFPS       ();
    void            updateLuaMemory ();

private:
    CTimer          m_updateTimer;  // Keeps track when all texts should be updated

    CTextWrapper    m_FPS;
    float           m_FPSAverage;

    CTextWrapper    m_LuaMemory;

};

#endif // CDISPLAYSTATS_H
