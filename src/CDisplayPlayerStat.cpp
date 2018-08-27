#include "CDisplayPlayerStat.h"

#include "CGame.h"
#include "CPlayer.h"
#include "Utility.h"

CDisplayPlayerStat::CDisplayPlayerStat() {
    CGame::MessageSystem.subscribe( this, "PlayerStatUpdate" );

    for( size_t i = 0; i < 16; i++ ) {
        float offset = 0;

             if( i <= 3 )    offset = 10;
        else if( i <= 6 )    offset = 20;
        else if( i <= 8 )    offset = 30;
        else if( i <= 11 )   offset = 40;
        else                 offset = 50;

        m_textStatsName[ i ].setFont( "FONT_ARIAL" );
        m_textStatsName[ i ].setCharSize( 14 );
        m_textStatsName[ i ].setColor( sf::Color::White );

        m_textStatsName[ i ].setPos( 230, offset + 15 + i * 16 );
    }

    for( size_t i = 0; i < 16; i++ ) {
        float offset = 0;

             if( i <= 3 )    offset = 10;
        else if( i <= 6 )    offset = 20;
        else if( i <= 8 )    offset = 30;
        else if( i <= 11 )   offset = 40;
        else                 offset = 50;

        m_textStatsValue[ i ].setFont( "FONT_ARIAL" );
        m_textStatsValue[ i ].setCharSize( 14 );
        m_textStatsValue[ i ].setColor( sf::Color::White );

        m_textStatsValue[ i ].setPos( 405, offset + 15 + i * 16 );
    }

    m_textStatsName[ 0 ].setString( "Strength:" );
    m_textStatsName[ 1 ].setString( "Dexterity:" );
    m_textStatsName[ 2 ].setString( "Intelligence:" );
    m_textStatsName[ 3 ].setString( "Vitality:" );
    m_textStatsName[ 4 ].setString( "Health:" );
    m_textStatsName[ 5 ].setString( "Resource:" );
    m_textStatsName[ 6 ].setString( "Armor:" );
    m_textStatsName[ 7 ].setString( "HP regen:" );
    m_textStatsName[ 8 ].setString( "RP regen:" );
    m_textStatsName[ 9 ].setString( "Movement spd.:" );
    m_textStatsName[ 10 ].setString( "Attack spd.:" );
    m_textStatsName[ 11 ].setString( "Casting spd.:" );
    m_textStatsName[ 12 ].setString( "Min dmg:" );    m_textStatsName[ 13 ].setString( "Max dmg:" );    m_textStatsName[ 14 ].setString( "Crit. chance:" );    m_textStatsName[ 15 ].setString( "Crit. multiplier:" );

    updateStats();}

void CDisplayPlayerStat::handleMsg( CMessage* msg ) {
    if( msg->getMsg() == "PlayerStatUpdate" ) {
        updateStats();
    }
}

void CDisplayPlayerStat::draw( sf::RenderTarget& window, sf::RenderStates states ) const {
    sf::Transformable transformable;

    transformable.setPosition( m_pos );

    states.transform *= transformable.getTransform();

    for( auto& it : m_textStatsName ) {
        window.draw( it, states );
    }

    for( auto& it : m_textStatsValue ) {
        window.draw( it, states );
    }
}

/** Private */

void CDisplayPlayerStat::updateStats() {
    CPlayer* player = CGame::EntitySystem.getPlayer();

    // Make sure not to use player that has not been created yet
    if( player == nullptr ) return;

    CharacterStats  playerStats     = player->getModStats();
    int             playerHealth    = player->getMaximumHealth();
    int             playerResource  = player->getMaximumResource();

    m_textStatsValue[ 0 ].setString(    Util::intToString(      playerStats.STR ) );
    m_textStatsValue[ 1 ].setString(    Util::intToString(      playerStats.DEX ) );
    m_textStatsValue[ 2 ].setString(    Util::intToString(      playerStats.INT ) );
    m_textStatsValue[ 3 ].setString(    Util::intToString(      playerStats.VIT ) );
    m_textStatsValue[ 4 ].setString(    Util::intToString(      playerHealth ) );
    m_textStatsValue[ 5 ].setString(    Util::intToString(      playerResource ) );
    m_textStatsValue[ 6 ].setString(    Util::intToString(      playerStats.Armor ) );
    m_textStatsValue[ 7 ].setString(    Util::intToString(      playerStats.regenHealth ) );
    m_textStatsValue[ 8 ].setString(    Util::intToString(      playerStats.regenResource ) );
    m_textStatsValue[ 9 ].setString(    Util::decimalToString(  playerStats.movementSpd, 1 ) );
    m_textStatsValue[ 10 ].setString(   Util::decimalToString(  playerStats.attackSpd, 1 ) );
    m_textStatsValue[ 11 ].setString(   Util::decimalToString(  playerStats.castingSpd, 1 ) );
    m_textStatsValue[ 12 ].setString(   Util::intToString(      playerStats.damageMin ) );
    m_textStatsValue[ 13 ].setString(   Util::intToString(      playerStats.damageMax ) );
    m_textStatsValue[ 14 ].setString(   Util::decimalToString(  playerStats.critChance, 1 ) );
    m_textStatsValue[ 15 ].setString(   Util::decimalToString(  playerStats.critMultip, 1 ) );

    // Update origin ( because text size has changed )
    for( auto& it : m_textStatsValue ) {
        it.setOrigin( OriginPosition::ORIGIN_RIGHT );
    }
}
