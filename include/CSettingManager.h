#ifndef CSETTINGMANAGER_H
#define CSETTINGMANAGER_H

struct lua_State;

class CSettingManager {
public:
    enum AudioSettings {
        SET_AUDIO_MASTER,
        SET_AUDIO_MUSIC,
        SET_AUDIO_EFFECT,
        SET_AUDIO_VOICE,
        SET_AUDIO_AMBIENT,
        SET_AUDIO_INTERFACE,
        SET_AUDIO_TOTAL     // Keep last
    };

public:
                    CSettingManager     ();

    void            init                ();
    void            close               ();

    void            loadToTable         ( lua_State* state );
    void            settingsFromTable   ( lua_State* state, int index );

public:
    static  int     luaGetSettings      ( lua_State* state );
    static  int     luaUpdateSettings   ( lua_State* state );

private:
    void            loadFromFile        ();
    void            saveToFile          ();

    void            setupSettings       ();

    void            handleAudioEnable   ( bool* enable = nullptr );
    void            handleAudioVolume   ( float* volume = nullptr );

private:
    bool            m_audioEnable[ SET_AUDIO_TOTAL ];
    float           m_audioVolume[ SET_AUDIO_TOTAL ];
};

#endif // CSETTINGMANAGER_H
