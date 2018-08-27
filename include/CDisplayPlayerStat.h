#ifndef CDISPLAYPLAYERSTAT_H
#define CDISPLAYPLAYERSTAT_H

#include "CWidget.h"
#include "CMessageListener.h"
#include "CTextWrapper.h"

class CDisplayPlayerStat : public CWidget, public CMessageListener {
public:
                    CDisplayPlayerStat  ();

    void            handleMsg           ( CMessage* msg );

    void            draw                ( sf::RenderTarget& window, sf::RenderStates states ) const;

private:
    void            updateStats         ();

private:
    CTextWrapper    m_textStatsName[ 16 ];
    CTextWrapper    m_textStatsValue[ 16 ];


};

#endif // CDISPLAYPLAYERSTAT_H
