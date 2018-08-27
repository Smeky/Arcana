#ifndef CRNG_H
#define CRNG_H

class CRNG {
public:
                    CRNG            ();

    static void     makeSeed        ();
    static int      iRandom         ( int min, int max );
    static float    percentRandom   ();

};

#endif // CRNG_H
