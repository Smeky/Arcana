#ifndef CCHARSTAT_H
#define CCHARSTAT_H

#include "functional"
#include "vector"
#include "cstring"

/**
    Operation for calculations are used to determine
    what operation should be done with specified value
*/
enum class CalcOperation : int {
    CALC_ADD,
    CALC_REMOVE,
    CALC_MULTIPLY,
    CALC_DIVIDE,
    CALC_PERCENTADD,
    CALC_PERCENTREMOVE,
};

class CCharStat {
private:
    struct StatMod {
        CalcOperation   operation;
        float           value       = 0.0;
        size_t          ID          = 0;
    };

    typedef std::function<float( float value )>                 Func;
    typedef std::function<float( float value, float bonus )>    FuncWithBonus;
    typedef std::vector<StatMod>                                StatMods;

public:
                    CCharStat           ();

    void            setCalcFunction     ( Func calcFunc );
    void            setCalcFunctionBonus( FuncWithBonus calcFunc );

    size_t          addModifier         ( float value, CalcOperation operation );
    bool            delModifier         ( size_t ID );

    void            setBase             ( float base );
    float           getBase             ();
    void            setBonus            ( float bonus );
    float           getResult           ();

private:
    size_t          getNewID            ();

private:
    size_t          m_currentID;        // Used to track ID that should be given next
    float           m_base;             // Base value used in calculation with all modifiers
    float           m_bonus;

    Func            m_calcFunc;         // Function that calculates result
    FuncWithBonus   m_calcFuncWithBonus;// Function that calculates result
    StatMods        m_mods;             // Vector of all stat modifiers ( buffs, etc. )
};

#endif // CCHARSTAT_H
