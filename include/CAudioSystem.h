#ifndef CAUDIOSYSTEM_H
#define CAUDIOSYSTEM_H

#define MAX_SOUNDS 128
#define MAX_EMITTERS 32

#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/System/Vector2.hpp>
#include <map>

struct lua_State;

class CSoundEmitter;
class CEntity;

class CAudioSystem {
public:
    enum AudioCategory {
        DEFAULT,    // Uncategorized, used mainly for setting master volume
        MUSIC,
        EFFECT,
        VOICE,
        AMBIENT,
        INTERFACE,
        TOTAL       // Keep Last
    };

private:
    typedef std::map<std::string,std::string>   MusicHash;
    typedef std::vector<CSoundEmitter*>         Emitters;

public:
                    CAudioSystem        ();

    void            update              ();

    /** Manage all */
    void            stopAll             ();
    void            pauseAll            ();
    void            resumeAll           ();

    /** Volume */
    void            setVolume           ( float volume, AudioCategory category );
    float           getVolume           ( AudioCategory category ) const;

    void            setEnabled          ( bool enabled, AudioCategory category );
    void            reverseEnabled      ( AudioCategory category );
    bool            isEnabled           ( AudioCategory category ) const;

    /** Sounds */
    void            playSound           ( const std::string& ID, AudioCategory category );
    void            playSoundRelative   ( const std::string& ID, AudioCategory category, const sf::Vector2f& pos );

    /** Music */
    void            playMusic           ( const std::string& ID = std::string() );
    void            stopMusic           ();
    void            pauseMusic          ();

    /** Emitter */
    void            registerEmitter     ( const CEntity* parent, const std::string& ID );
    void            destroyEmitter      ( const CEntity* parent );

    /** Loading */
    void            loadMusic           ( const std::string& filepath, const std::string& ID );

public:
    static  int     luaStopAll          ( lua_State* state );
    static  int     luaPauseAll         ( lua_State* state );
    static  int     luaResumeAll        ( lua_State* state );

    static  int     luaSetVolume        ( lua_State* state );
    static  int     luaGetVolume        ( lua_State* state );
    static  int     luaSetEnabled       ( lua_State* state );
    static  int     luaReverseEnabled   ( lua_State* state );
    static  int     luaIsEnabled        ( lua_State* state );

    static  int     luaPlaySound        ( lua_State* state );

    static  int     luaPlayMusic        ( lua_State* state );
    static  int     luaStopMusic        ( lua_State* state );
    static  int     luaPauseMusic       ( lua_State* state );

    static  int     luaLoadMusic        ( lua_State* state );

private:
    float           getVolume           ( AudioCategory category = AudioCategory::DEFAULT );
    void            updateMusic         ();

private:
    /** Sounds */
    sf::Sound       m_sounds[ MAX_SOUNDS ];
    size_t          m_activeCounter;

    /** Music */
    MusicHash       m_musicMap;
    sf::Music*      m_music;

    /** Volume */
    float           m_volume[ AudioCategory::TOTAL ];
    bool            m_enable[ AudioCategory::TOTAL ];

    /** Emitter */
    Emitters        m_emitters;
};

#endif // CAUDIOSYSTEM_H
