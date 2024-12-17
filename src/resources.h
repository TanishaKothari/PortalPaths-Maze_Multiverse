#ifndef RESOURCES_H
#define RESOURCES_H

#include "raylib.h"

// Theme-specific colors
struct ThemeColors {
    Color background;
    Color playerTint;
    Color wallColor;
    Color obstacleColor;
    Color goalColor;
};

// Game themes
enum class GameTheme {
    Space,
    Jungle, 
    Desert
};

enum GameStates {
    StartMenu,
    Playing,
    EndMenu
};

// Game modes
enum GameMode {
    Timed,
    Untimed
};

// Difficulty levels
enum Difficulty {
    Easy,
    Medium,
    Hard
};

// Menu states
enum MenuState {
    DifficultySelect,
    ModeSelect
};

// Sound settings
enum class SoundSettings {
    AllOn,
    MusicOff,
    SoundOff,
    AllOff
};

namespace GameResources {
    // Space theme textures and music
    extern Texture2D spaceshipTexture;
    extern Texture2D portalPlaceholder;
    extern Texture2D asteroidSpritesheet;
    extern Music spaceAmbient;

    // Jungle theme textures and music
    extern Texture2D monkeyTexture;
    extern Texture2D bananaTexture;
    extern Texture2D logsSpritesheet;
    extern Music jungleAmbient;

    // Desert theme textures and music
    extern Texture2D camelTexture;
    extern Texture2D oasisTexture;
    extern Texture2D cactusTexture;
    extern Texture2D tumbleweedTexture;
    extern Music desertAmbient;

    extern Texture2D timeBonusTexture;
    extern Texture2D speedBoostTexture;

    extern Sound startSound;
    extern Sound blockedSound;
    extern Sound winSound;
    extern Sound loseSound;
    extern Sound menuSound;
    extern Sound portalSound;
    extern Sound bonusSound;

    void LoadSpaceResources();
    void UnloadSpaceResources();
    void LoadJungleResources();
    void UnloadJungleResources();
    void LoadDesertResources();
    void UnloadDesertResources();
    void LoadPowerupResources();
    void UnloadPowerupResources();
    void LoadGeneralSounds();
    void UnloadGeneralSounds();
    void UnloadUnusedResources();
    void UnloadAllResources();
}

extern const ThemeColors SPACE_COLORS;
extern const ThemeColors JUNGLE_COLORS;
extern const ThemeColors DESERT_COLORS;

void RandomizeTheme();
Texture2D GetPlayerTextureForTheme(GameTheme theme);
Texture2D GetGoalTextureForTheme(GameTheme theme);
void SetThemeResources(GameTheme theme);
void StopCurrentThemeMusic();
Texture2D CreateStarFieldTexture(int width, int height, int starCount);

#endif