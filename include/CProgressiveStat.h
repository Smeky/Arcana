#ifndef CPROGRESSIVESTAT_H
#define CPROGRESSIVESTAT_H

/**
    Utility class that keeps track on progressive
    stats like health and resource to keep things
    clean and organized
*/

class CProgressiveStat {
public:
                    CProgressiveStat();

    bool            add             ( int amount );
    bool            remove          ( int amount );

    bool            canAdd          ( int amount ) const;
    bool            canRemove       ( int amount ) const;

    void            setMaximum      ( int maximum );
    void            setMaximumFactor( int maximum );
    void            setCurrent      ( int current );

    int             getMaximum      () const;
    int             getCurrent      () const;

    bool            isEmpty         () const;

private:
    int             m_maximum;
    int             m_current;

};

#endif // CPROGRESSIVESTAT_H
