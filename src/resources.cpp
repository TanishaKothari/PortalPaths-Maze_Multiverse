#include "resources.h"
#include "game_state.h"
#include <random>

namespace GameResources {
    // Global resource variables
    Texture2D spaceshipTexture;
    Texture2D portalPlaceholder;
    Texture2D asteroidSpritesheet;
    Music spaceAmbient;
    
    Texture2D monkeyTexture;
    Texture2D bananaTexture;
    Texture2D logsSpritesheet;
    Music jungleAmbient;
    
    Texture2D camelTexture;
    Texture2D oasisTexture;
    Texture2D cactusTexture;
    Texture2D tumbleweedTexture;
    Music desertAmbient;
    
    Texture2D timeBonusTexture;
    Texture2D speedBoostTexture;
    
    Sound startSound;
    Sound blockedSound;
    Sound winSound;
    Sound loseSound;
    Sound menuSound;
    Sound bonusSound;

    bool spaceResourcesLoaded = false;
    bool jungleResourcesLoaded = false;
    bool desertResourcesLoaded = false;
    bool powerupResourcesLoaded = false;
    bool generalSoundsLoaded = false;
    
    // Resource loading functions
    void LoadSpaceResources() {
        if (!spaceResourcesLoaded) {
            spaceshipTexture = LoadTexture("Assets/spaceship.png");
            asteroidSpritesheet = LoadTexture("Assets/asteroids_spritesheet.png");
            spaceAmbient = LoadMusicStream("sounds/space_ambient.wav");
            
            // Generate portal texture
            Image portalImage = GenImageColor(64, 64, BLANK);
            ImageDrawCircle(&portalImage, 32, 32, 30, SKYBLUE);
            portalPlaceholder = LoadTextureFromImage(portalImage);
            UnloadImage(portalImage);
            
            spaceResourcesLoaded = true;
        }
    }
    
    void UnloadSpaceResources() {
        if (spaceResourcesLoaded) {
            UnloadTexture(spaceshipTexture);
            UnloadTexture(portalPlaceholder);
            UnloadTexture(asteroidSpritesheet);
            UnloadMusicStream(spaceAmbient);

            spaceResourcesLoaded = false;
        }
    }
    
    void LoadJungleResources() {
        if (!jungleResourcesLoaded) {
            monkeyTexture = LoadTexture("Assets/monkey.png");
            bananaTexture = LoadTexture("Assets/bananas.png");
            logsSpritesheet = LoadTexture("Assets/logs_spritesheet.png");
            jungleAmbient = LoadMusicStream("sounds/jungle_ambient.wav");
            
            jungleResourcesLoaded = true;
        }
    }

    void UnloadJungleResources() {
        if (jungleResourcesLoaded) {
            UnloadTexture(monkeyTexture);
            UnloadTexture(bananaTexture);
            UnloadTexture(logsSpritesheet);
            UnloadMusicStream(jungleAmbient);

            jungleResourcesLoaded = false;
        }
    }

    void LoadDesertResources() {
        if (!desertResourcesLoaded) {
            camelTexture = LoadTexture("Assets/camel.png");
            oasisTexture = LoadTexture("Assets/oasis.png");
            cactusTexture = LoadTexture("Assets/cactus.png");
            tumbleweedTexture = LoadTexture("Assets/tumbleweed.png");
            desertAmbient = LoadMusicStream("sounds/desert_ambient.wav");
            
            desertResourcesLoaded = true;
        }
    }

    void UnloadDesertResources() {
        if (desertResourcesLoaded) {
            UnloadTexture(camelTexture);
            UnloadTexture(oasisTexture);
            UnloadTexture(cactusTexture);
            UnloadTexture(tumbleweedTexture);
            UnloadMusicStream(desertAmbient);

            desertResourcesLoaded = false;
        }
    }

    void LoadPowerupResources() {
        if (!powerupResourcesLoaded) {
            timeBonusTexture = LoadTexture("Assets/timeBonus.png");
            speedBoostTexture = LoadTexture("Assets/speedBoost.png");

            // Sound effects for powerups
            bonusSound = LoadSound("sounds/bonus.wav");

            powerupResourcesLoaded = true;
        }
    }
    
    void UnloadPowerupResources() {
        if (powerupResourcesLoaded) {
            UnloadTexture(timeBonusTexture);
            UnloadTexture(speedBoostTexture);

            UnloadSound(bonusSound);

            powerupResourcesLoaded = false;
        }
    }

    void LoadGeneralSounds() {
        if (!generalSoundsLoaded) {
            startSound = LoadSound("sounds/poweron.wav");
            blockedSound = LoadSound("sounds/blocked.wav");
            winSound = LoadSound("sounds/win.wav");
            loseSound = LoadSound("sounds/lose.wav");
            menuSound = LoadSound("sounds/select.wav");

            generalSoundsLoaded = true;
        }
    }

    void UnloadGeneralSounds() {
        if (generalSoundsLoaded) {
            UnloadSound(startSound);
            UnloadSound(blockedSound);
            UnloadSound(winSound);
            UnloadSound(loseSound);
            UnloadSound(menuSound);

            generalSoundsLoaded = false;
        }
    }
    void UnloadUnusedResources() {
        // Unload resources for themes that are not currently active
        switch (GAME_STATE.getCurrentTheme()) {
            case GameTheme::Space:
                UnloadJungleResources();
                UnloadDesertResources();
                break;
            case GameTheme::Jungle:
                UnloadSpaceResources();
                UnloadDesertResources();
                break;
            case GameTheme::Desert:
                UnloadSpaceResources();
                UnloadJungleResources();
                break;
        }
    }
    void UnloadAllResources() {
        UnloadSpaceResources();
        UnloadJungleResources();
        UnloadDesertResources();
        UnloadPowerupResources();
        UnloadGeneralSounds();
    }
}

// Theme colors definitions
const ThemeColors SPACE_COLORS = {
    {15, 15, 35, 255},      // Dark space background
    {100, 150, 255, 255},   // Blue spaceship tint
    {140, 100, 255, 210},   // Purple barriers
    {110, 75, 65, 255},     // Brown asteroids
    {0, 255, 255, 255}      // Cyan portal
};

const ThemeColors JUNGLE_COLORS = {
    {34, 51, 34, 255},      // Dark green background
    {255, 255, 255, 255},   // White monkey tint
    {101, 67, 33, 210},     // Brown walls
    {120, 60, 30, 255},     // Wood brown logs
    {255, 255, 0, 255}      // Yellow bananas
};

const ThemeColors DESERT_COLORS = {
    {255, 204, 102, 255},   // Sandy background
    {139, 69, 19, 255},     // Saddle brown camel tint
    {204, 119, 34, 210},    // Orange walls
    {86, 125, 70, 255},     // Green cacti/tumbleweed
    {0, 191, 255, 255}      // Blue oasis
};

void RandomizeTheme() {
    GAME_STATE.setCurrentTheme(static_cast<GameTheme>(rand() % 3));
    SetThemeResources(GAME_STATE.getCurrentTheme());
}

Texture2D GetPlayerTextureForTheme(GameTheme theme) {
    switch(theme) {
        case GameTheme::Space:
            return GameResources::spaceshipTexture;
        case GameTheme::Jungle:
            return GameResources::monkeyTexture;
        case GameTheme::Desert:
            return GameResources::camelTexture;
        default:
            return GameResources::spaceshipTexture;
    }
}

Texture2D GetGoalTextureForTheme(GameTheme theme) {
    switch(theme) {
        case GameTheme::Space:
            return GameResources::portalPlaceholder;
        case GameTheme::Jungle:
            return GameResources::bananaTexture;
        case GameTheme::Desert:
            return GameResources::oasisTexture;
        default:
            return GameResources::portalPlaceholder;
    }
}

void SetThemeResources(GameTheme theme) {
    // Unload unused resources
    GameResources::UnloadUnusedResources();

    // Load resources for the current theme
    switch (theme) {
        case GameTheme::Space:
            GameResources::LoadSpaceResources();
            GAME_STATE.setCurrentTheme(GameTheme::Space);
            if (GAME_STATE.getSoundSettings() != SoundSettings::MusicOff && GAME_STATE.getSoundSettings() != SoundSettings::AllOff) {
                PlayMusicStream(GameResources::spaceAmbient);
                SetMusicVolume(GameResources::spaceAmbient, 1.0f);
            }
            break;
        case GameTheme::Jungle:
            GameResources::LoadJungleResources();
            GAME_STATE.setCurrentTheme(GameTheme::Jungle);
            if (GAME_STATE.getSoundSettings() != SoundSettings::MusicOff && GAME_STATE.getSoundSettings() != SoundSettings::AllOff) {
                PlayMusicStream(GameResources::jungleAmbient);
                SetMusicVolume(GameResources::jungleAmbient, 1.0f);
            }
            break;
        case GameTheme::Desert:
            GameResources::LoadDesertResources();
            GAME_STATE.setCurrentTheme(GameTheme::Desert);
            if (GAME_STATE.getSoundSettings() != SoundSettings::MusicOff && GAME_STATE.getSoundSettings() != SoundSettings::AllOff) {
                PlayMusicStream(GameResources::desertAmbient);
                SetMusicVolume(GameResources::desertAmbient, 1.0f);
            }
            break;
    }
}

void StopCurrentThemeMusic() {
    switch(GAME_STATE.getCurrentTheme()) {
        case GameTheme::Space:
            StopMusicStream(GameResources::spaceAmbient);
            break;
        case GameTheme::Jungle:
            StopMusicStream(GameResources::jungleAmbient);
            break;
        case GameTheme::Desert:
            StopMusicStream(GameResources::desertAmbient);
            break;
    }
}

Texture2D CreateStarFieldTexture(int width, int height, int starCount) {
    Image starField = GenImageColor(width, height, BLANK);  // Create blank image

    for (int i = 0; i < starCount; i++) {
        int x = GetRandomValue(0, width - 1);  // Random x-coordinate
        int y = GetRandomValue(0, height - 1); // Random y-coordinate
        Color starColor = ColorAlpha(WHITE, GetRandomValue(50, 100) / 100.0f);  // Semi-transparent stars
        ImageDrawPixel(&starField, x, y, starColor);  // Draw star pixel
    }

    Texture2D starFieldTexture = LoadTextureFromImage(starField);  // Convert to texture
    UnloadImage(starField);  // Free image memory
    return starFieldTexture;
}