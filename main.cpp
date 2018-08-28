#include "CGame.h"

#include <SFML/Audio.hpp>

#include <iostream>
using namespace std;

int main() {
    CGame game;

    if( !game.init() ) {
        return 1;
    }

    game.run();

    return 0;
}
