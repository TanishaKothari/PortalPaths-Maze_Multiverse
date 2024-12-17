#include "raylib.h"
#include "game_state.h"
#include "resources.h"
#include "maze.h"
#include "gameplay.h"
#include "ui.h"

bool isPaused = false;
float pauseStartTime = 0.0f;
RenderTexture2D mazeRenderTexture;
Texture2D starFieldTexture;

int main() {
    // Initialize window
    InitWindow(GAME_STATE.getScreenWidth(), GAME_STATE.getScreenHeight(), "Portal Paths: Maze Multiverse");
    SetTargetFPS(60);
    
    // Initialize audio
    InitAudioDevice();

    RandomizeTheme();
    
    // Load initial resources
    GameResources::LoadGeneralSounds();
    GameResources::LoadPowerupResources();
    SetThemeResources(GAME_STATE.getCurrentTheme());

    // Load star field texture
    if (GAME_STATE.getCurrentTheme() == GameTheme::Space) {
        starFieldTexture = CreateStarFieldTexture(GAME_STATE.getScreenWidth(), GAME_STATE.getScreenHeight(), 200);
    }
    
    // Load high scores
    LoadHighScores();

    // Main game loop
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GAME_STATE.getCurrentColors().background);

        // Draw star field background for space theme
        if (GAME_STATE.getCurrentTheme() == GameTheme::Space) {
            DrawTexture(starFieldTexture, 0, 0, WHITE);
        }

        // Update music streams
        if (GAME_STATE.getCurrentGameState() == StartMenu || GAME_STATE.getCurrentGameState() == Playing) {
            if (GAME_STATE.getSoundSettings() != SoundSettings::AllOff && GAME_STATE.getSoundSettings()!= SoundSettings::MusicOff) {
                switch(GAME_STATE.getCurrentTheme()) {
                    case GameTheme::Space:
                        if (!IsMusicStreamPlaying(GameResources::spaceAmbient)) {
                            PlayMusicStream(GameResources::spaceAmbient);
                        }
                        UpdateMusicStream(GameResources::spaceAmbient);
                        break;
                    case GameTheme::Jungle:
                        if (!IsMusicStreamPlaying(GameResources::jungleAmbient)) {
                            PlayMusicStream(GameResources::jungleAmbient);
                        }
                        UpdateMusicStream(GameResources::jungleAmbient);
                        break;
                    case GameTheme::Desert:
                        if (!IsMusicStreamPlaying(GameResources::desertAmbient)) {
                            PlayMusicStream(GameResources::desertAmbient);
                        }
                        UpdateMusicStream(GameResources::desertAmbient);
                        break;
                }
            }
        }

        // Handle player rotation based on theme
        switch(GAME_STATE.getCurrentTheme()) {
            case GameTheme::Space:
                UpdatePlayerRotation();
                break;
            case GameTheme::Jungle:
            case GameTheme::Desert:
                GAME_STATE.setPlayerRotation(0.0f);
                break;
        }

        // Game state machine
        switch (GAME_STATE.getCurrentGameState()) {
            case StartMenu:
                HandleMenuInput();
                break;

            case Playing:
                if (IsKeyPressed(KEY_P)) {
                    if (!isPaused) {
                        isPaused = true;
                        pauseStartTime = GetTime();
                        StopCurrentThemeMusic();
                    } else {
                        isPaused = false;
                        GAME_STATE.setTotalPausedTime(GAME_STATE.getTotalPausedTime() + GetTime() - pauseStartTime);
                        SetThemeResources(GAME_STATE.getCurrentTheme());
                    }
                }
                
                if (!isPaused) {
                    if (!GAME_STATE.getGameWon()) {
                        MovePlayer();
                        UpdatePlayerRotation();
                        GAME_STATE.setTimeElapsed(GetTime() - GAME_STATE.getStartTime() - GAME_STATE.getTotalPausedTime());

                        if (GAME_STATE.getCurrentMode() == Timed) {
                            UpdateTimePowerups(GetFrameTime());
                            DrawTimePowerups();
                            
                            if (GAME_STATE.getShowTimeBonusFeedback()) {
                                // Draw Time Left and Time Bonus at distinct positions
                                float timeLeft = GAME_STATE.getTimeLimit() - GAME_STATE.getTimeElapsed();
                                std::string timeText = TextFormat("Time Left: %.2f", timeLeft);
                                std::string bonusText = TextFormat(" + %.1f", GAME_STATE.getTimeBonusAmount());
                                
                                DrawText(timeText.c_str(), 10, 8, 20, WHITE);
                                DrawText(bonusText.c_str(), 10 + MeasureText(timeText.c_str(), 20), 8, 20, GREEN); // Positioned next the main timer
                            } else {
                                float timeLeft = GAME_STATE.getTimeLimit() - GAME_STATE.getTimeElapsed();
                                Color timeColor = (timeLeft < 10.0f) ? RED : WHITE;
                                std::string timeText = TextFormat("Time Left: %.2f", timeLeft);
                                DrawText(timeText.c_str(), 10, 8, 20, timeColor);
                            }
                        } else {
                            DrawText(TextFormat("Time: %.2f", GAME_STATE.getTimeElapsed()), 10, 8, 20, WHITE);  // Draw timer
                        }

                        UpdateSpeedBoosts(GetFrameTime());
                        DrawSpeedBoosts();

                        // Check win condition
                        if (GAME_STATE.getPlayerX() == GAME_STATE.getExitX() && GAME_STATE.getPlayerY() == GAME_STATE.getExitY()) {
                            GAME_STATE.setGameWon(true);
                            PlaySound(GameResources::winSound);
                            AddHighScore(GAME_STATE.getTimeElapsed(), GAME_STATE.getCurrentMode() == Timed);
                            GAME_STATE.setCurrentGameState(EndMenu);
                            StopCurrentThemeMusic();
                        }

                        // Check lose condition (timed mode)
                        if (GAME_STATE.getCurrentMode() == Timed && GAME_STATE.getTimeElapsed() >= GAME_STATE.getTimeLimit()) {
                            GAME_STATE.setGameWon(false);
                            PlaySound(GameResources::loseSound);
                            GAME_STATE.setCurrentGameState(EndMenu);
                            StopCurrentThemeMusic();
                        }
                    }
                } else {
                    // Handle pause menu input
                    if (IsKeyPressed(KEY_ONE)) GAME_STATE.setSoundSettings(SoundSettings::AllOn);
                    else if (IsKeyPressed(KEY_TWO)) GAME_STATE.setSoundSettings(SoundSettings::MusicOff);
                    else if (IsKeyPressed(KEY_THREE)) GAME_STATE.setSoundSettings(SoundSettings::SoundOff);
                    else if (IsKeyPressed(KEY_FOUR)) GAME_STATE.setSoundSettings(SoundSettings::AllOff);
                    
                    UpdateSoundSettings();
                }

                if (!isPaused) {
                    // Draw maze and border
                    DrawBorder();
                    for (const auto& row : GAME_STATE.getGrid()) {
                        for (const auto& cell : row) {
                            cell.Draw();
                        }
                    }

                    // Draw pause hint in top-right corner
                    const char* pauseHint = "P - Pause/Settings";
                    DrawText(pauseHint, 
                            GAME_STATE.getScreenWidth() - MeasureText(pauseHint, 20) - 10,
                            8, 20, LIGHTGRAY);

                    // Draw powerups
                    if (GAME_STATE.getCurrentMode() == Timed) {
                        DrawTimePowerups();
                        DrawSpeedBoosts();
                    }

                    // Draw exit
                    DrawExit(GAME_STATE.getExitX(), GAME_STATE.getExitY(), GetTime() - GAME_STATE.getStartTime());

                    // Draw player
                    int playerScreenX = GAME_STATE.getMargin() + static_cast<int>(GAME_STATE.getPlayerPosX() * GAME_STATE.getCellSize() + GAME_STATE.getCellSize()/2);
                    int playerScreenY = GAME_STATE.getMargin() + static_cast<int>(GAME_STATE.getPlayerPosY() * GAME_STATE.getCellSize() + GAME_STATE.getCellSize()/2);
                    
                    // Draw player based on theme
                    if (GAME_STATE.getCurrentTheme() == GameTheme::Space) {
                        // Space theme uses rotation
                        DrawTexturePro(
                            GetPlayerTextureForTheme(GAME_STATE.getCurrentTheme()),
                            Rectangle{ 0, 0,
                                (float)GetPlayerTextureForTheme(GAME_STATE.getCurrentTheme()).width,
                                (float)GetPlayerTextureForTheme(GAME_STATE.getCurrentTheme()).height
                            },
                            Rectangle{ 
                                static_cast<float>(GAME_STATE.getMargin()) + GAME_STATE.getPlayerPosX() * GAME_STATE.getCellSize() + GAME_STATE.getCellSize()/2,
                                static_cast<float>(GAME_STATE.getMargin()) + GAME_STATE.getPlayerPosY() * GAME_STATE.getCellSize() + GAME_STATE.getCellSize()/2,
                                GAME_STATE.getCellSize() * 0.8f,
                                GAME_STATE.getCellSize() * 0.8f
                            },
                            Vector2{ GAME_STATE.getCellSize() * 0.4f, GAME_STATE.getCellSize() * 0.4f },
                            GAME_STATE.getPlayerRotation(),
                            GAME_STATE.getCurrentColors().playerTint
                        );
                    } else {
                        // Jungle and Desert themes use horizontal flipping
                        DrawTexturePro(
                            GetPlayerTextureForTheme(GAME_STATE.getCurrentTheme()),
                            Rectangle{ 0, 0,
                                (float)GetPlayerTextureForTheme(GAME_STATE.getCurrentTheme()).width * (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A) ? -1.0f : 1.0f),  // Flip horizontally for left movement
                                (float)GetPlayerTextureForTheme(GAME_STATE.getCurrentTheme()).height
                            },
                            Rectangle{ 
                                static_cast<float>(GAME_STATE.getMargin()) + GAME_STATE.getPlayerPosX() * GAME_STATE.getCellSize() + GAME_STATE.getCellSize()/2,
                                static_cast<float>(GAME_STATE.getMargin()) + GAME_STATE.getPlayerPosY() * GAME_STATE.getCellSize() + GAME_STATE.getCellSize()/2,
                                GAME_STATE.getCellSize() * 0.8f,
                                GAME_STATE.getCellSize() * 0.8f
                            },
                            Vector2{ GAME_STATE.getCellSize() * 0.4f, GAME_STATE.getCellSize() * 0.4f },  // Origin at center
                            GAME_STATE.getPlayerRotation(),
                            GAME_STATE.getCurrentColors().playerTint
                        );
                    }

                    // Draw UI elements
                    if (GAME_STATE.getIsSpeedBoosted()) {
                        DrawText(TextFormat("Speed Boost: %.2fs", GAME_STATE.getSpeedBoostTimeLeft()), 
                                    900, 8, 20, YELLOW);
                    }
                } else {
                    DrawPauseMenu();
                }
                
                EndDrawing();
                break;

            case EndMenu:
                HandleMenuInput();
                break;
        }
            
            switch (GAME_STATE.getCurrentGameState()) {
                case StartMenu:
                    if (GAME_STATE.getMenuState() == DifficultySelect) {
                        DrawDifficultyMenu();
                    } else {
                        DrawModeMenu();
                    }
                    break;
                    
                case EndMenu:
                    DrawEndMenu(GAME_STATE.getGameWon(), GAME_STATE.getTimeElapsed());
                    break;
            }
        EndDrawing();
    }

    // Cleanup
    if (GAME_STATE.getCurrentTheme() == GameTheme::Space) {
        UnloadTexture(starFieldTexture);
    }
    GameResources::UnloadAllResources();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}