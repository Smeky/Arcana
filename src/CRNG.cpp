#include "CRNG.h"

#include "stdlib.h"
#include "time.h"

CRNG::CRNG() {
    srand( time( nullptr ) );
}

void CRNG::makeSeed() {
    srand( time( nullptr ) );
}

int CRNG::iRandom( int min, int max ) {
    return min + rand() % ( max - min + 1 ) ;
}

float CRNG::percentRandom() {
    return static_cast<float>( rand() ) / static_cast<float>( RAND_MAX );
}
