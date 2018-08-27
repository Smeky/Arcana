#include "CCharStat.h"

#include "iostream"
using namespace std;

CCharStat::CCharStat()
: m_currentID           ( 1 )
, m_base                ( 0 )
, m_bonus               ( 0 )
, m_calcFunc            ( nullptr )
, m_calcFuncWithBonus   ( nullptr )
{}

void CCharStat::setCalcFunction( std::function<float( float value )> func ) {
    m_calcFunc  = func;
}

void CCharStat::setCalcFunctionBonus( std::function<float( float value, float bonus )> func ) {
    m_calcFuncWithBonus  = func;
}

size_t CCharStat::addModifier( float value, CalcOperation operation ) {
    StatMod mod;

    mod.operation   = operation;
    mod.value       = value;
    mod.ID          = getNewID();

    m_mods.push_back( mod );

    return mod.ID;
}

bool CCharStat::delModifier( size_t ID ) {
    if( ID == 0 ) return false; // Wrong ID

    for( auto it = m_mods.begin(); it != m_mods.end(); it++ ) {
        if( it->ID == ID ) {
            m_mods.erase( it );

            return true;
        }
    }

    return false;
}

void CCharStat::setBase( float base ) {
    m_base  = base;
}

float CCharStat::getBase() {
    return m_base;
}

void CCharStat::setBonus( float bonus ) {
    m_bonus = bonus;
}

float CCharStat::getResult() {
    float calculation = m_base;

    // Calculate modifier based on its operand
    for( auto it : m_mods ) {
        switch( it.operation ) {
        case CalcOperation::CALC_ADD:           calculation += it.value; break;
        case CalcOperation::CALC_REMOVE:        calculation -= it.value; break;
        case CalcOperation::CALC_MULTIPLY:      calculation *= it.value; break;
        case CalcOperation::CALC_DIVIDE:        calculation /= it.value; break;
        case CalcOperation::CALC_PERCENTADD:    calculation += ( calculation / 100 ) * it.value; break;
        case CalcOperation::CALC_PERCENTREMOVE: calculation -= ( calculation / 100 ) * it.value; break;
        default: break;
        }
    }

    // Return result that has been calculated based on the formula
    if( m_calcFuncWithBonus != nullptr ) {
        return m_calcFuncWithBonus( calculation, m_bonus );
    }
    else {
        return m_calcFunc( calculation );
    }
}

size_t CCharStat::getNewID() {
    m_currentID++;

    return m_currentID - 1;
}
