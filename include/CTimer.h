#ifndef CTIMER_H
#define CTIMER_H

#include "SFML/System/Time.hpp"

/**
    Simple timer that checks if
    progress is more then limit.
*/

class CTimer {
public:
                CTimer      ();

    // Add time to progress time
    // Return: true in case progress timer is same or over the limit
    bool        update      ( const sf::Time& delta );

    // Set timer limit
    void        setLimit    ( const sf::Time& limit );
    void        setLimit    ( float limit );

    // Set timer limit without reseting progress timer
    void        setLimitOnly( const sf::Time& limit );
    void        setLimitOnly( float limit );

    // Set timer progress
    void        setProgress ( const sf::Time& progress );
    void        setProgress ( float progress );

    // Return remaining time for progress to reach the limit
    sf::Time    getRemaining() const;

    // Return timer limit
    sf::Time    getLimit    () const;

    // Return timer progress
    sf::Time    getProgress () const;

    // Reset timer progress to zero
    void        reset       ();

    // Return if proggress is more then limit
    bool        isTime      () const;

private:
    sf::Time    m_limit;
    sf::Time    m_progress;

};

#endif // CTIMER_H
