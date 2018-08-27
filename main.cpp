#include "CGame.h"

int main() {
    CGame game;

    if( !game.init() ) {
        return 1;
    }

    game.run();

    return 0;
}
