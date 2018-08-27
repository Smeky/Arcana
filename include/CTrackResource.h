#ifndef CTRACKRESOURCE_H
#define CTRACKRESOURCE_H

#include "CWidget.h"
#include "CMessageListener.h"
#include "CTextWrapper.h"

class CTrackResource : public CWidget {
public:
                    CTrackResource  ();

    void            update          ( sf::Time delta );
    void            draw            ( sf::RenderTarget& window, sf::RenderStates states ) const;

    void            setBgTexture    ( const std::string& textureID );
    void            setProgTexture  ( const std::string& textureID );

private:
    void            onSizeChange    ();

    void            updateTracker   ( int current, int total );

private:
    CTextWrapper    m_text;
    sf::Sprite      m_bgSpite;
    sf::Sprite      m_progSprite;

    int             m_prevCurrent;
    int             m_prevTotal;
};


#endif // CTRACKRESOURCE_H
