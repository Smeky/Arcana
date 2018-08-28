#ifndef CDROPBOXELEMENT_H
#define CDROPBOXELEMENT_H

#include <CDimensional.h>
#include <CSpriteWrapper.h>

struct lua_State;

class CDropboxElement : public CDimensional {
public:
                            CDropboxElement ();

            void            draw            ( sf::RenderTarget& window, sf::RenderStates states ) const;

            void            updateSprite    ();

    static  CDropboxElement setupFromTable  ( lua_State* state, int index );

public:
    CSpriteWrapper  sprite;
    bool            selected;
    bool            mouseover;

};

#endif // CDROPBOXELEMENT_H
