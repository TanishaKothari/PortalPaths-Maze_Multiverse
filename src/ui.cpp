#include "ui.h"
#include "game_state.h"
#include "gameplay.h"
#include <sstream>
#include <fstream>
#include <iomanip>

std::vector<Score> highScores;


bool Score::operator<(const Score& rhs) const {
    return time < rhs.time;
}

Score::Score(float t, bool timed, Difficulty diff, std::time_t ts)
    : time(t), wasTimed(timed), difficulty(diff), timestamp(ts) {}

std::string Score::ToString() const {
    std::stringstream ss;
    
    // Format game completion time
    int minutes = static_cast<int>(time) / 60;
    float seconds = time - (minutes * 60);
    ss << std::setfill('0') << std::setw(2) << minutes << ":"
       << std::fixed << std::setprecision(2) << std::setfill('0') 
       << std::setw(5) << seconds;
    
    // Format difficulty
    std::string diffStr;
    switch(difficulty) {
        case Easy: diffStr = "Easy"; break;
        case Medium: diffStr = "Med"; break;
        case Hard: diffStr = "Hard"; break;
    }

    // Format UTC timestamp
    std::tm* tm = std::gmtime(&timestamp);
    if (tm != nullptr) {
        char timeStr[32];
        if (std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M UTC", tm)) {
            ss << " | " << (wasTimed ? "Timed" : "Untimed") 
               << " | " << diffStr
               << " | " << timeStr;
        } else {
            ss << " | " << (wasTimed ? "Timed" : "Untimed") 
               << " | " << diffStr
               << " | Invalid Time";
        }
    } else {
        ss << " | " << (wasTimed ? "Timed" : "Untimed") 
           << " | " << diffStr
           << " | Invalid Time";
    }

    return ss.str();
}

void LoadHighScores() {
    std::ifstream file(GAME_STATE.getScoresFile(), std::ios::binary);
    if (!file) return;

    highScores.clear();
    Score score;
    while (file.read(reinterpret_cast<char*>(&score), sizeof(Score))) {
        highScores.push_back(score);
    }
}

void SaveHighScores() {
    std::ofstream file(GAME_STATE.getScoresFile(), std::ios::binary);
    if (!file) return;

    for (const auto& score : highScores) {
        file.write(reinterpret_cast<const char*>(&score), sizeof(Score));
    }
}

void AddHighScore(float time, bool wasTimed) {
    // Don't add if game wasn't won
    if (!GAME_STATE.getGameWon()) return;
    
    // Create new score with current UTC timestamp
    Score newScore(time, wasTimed, GAME_STATE.getCurrentDifficulty(), std::time(nullptr));
    
    // Insert score in sorted order by completion time
    auto it = std::lower_bound(highScores.begin(), highScores.end(), newScore);
    
    highScores.insert(it, newScore);
    
    // Keep only top scores
    if (highScores.size() > MAX_HIGH_SCORES) {
        highScores.resize(MAX_HIGH_SCORES);
    }
    
    SaveHighScores();
}
void DrawDifficultyMenu() {
    DrawText("Select Difficulty", GAME_STATE.getScreenWidth()/2 - MeasureText("Select Difficulty", 20) / 2, GAME_STATE.getScreenHeight() / 2 - 100, 20, ORANGE);
    DrawText("1. Easy", GAME_STATE.getScreenWidth()/2 - MeasureText("1. Easy", 20) / 2, GAME_STATE.getScreenHeight() / 2 - 60, 20, GREEN);
    DrawText("2. Medium", GAME_STATE.getScreenWidth()/2 - MeasureText("2. Medium", 20) / 2, GAME_STATE.getScreenHeight() / 2 - 20, 20, YELLOW);
    DrawText("3. Hard", GAME_STATE.getScreenWidth()/2 - MeasureText("3. Hard", 20) / 2, GAME_STATE.getScreenHeight() / 2 + 20, 20, DARKBLUE);
    DrawText("4. Quit", GAME_STATE.getScreenWidth() / 2 - MeasureText("4. Quit", 20) / 2, GAME_STATE.getScreenHeight() / 2 + 60, 20, RED);
}
void DrawModeMenu() {
    // Game mode options
    DrawText("Select Game Mode", GAME_STATE.getScreenWidth() / 2 - MeasureText("Select Game Mode", 20) / 2, GAME_STATE.getScreenHeight() / 2 - 100, 20, ORANGE);
    DrawText("1. Timed", GAME_STATE.getScreenWidth() / 2 - MeasureText("1. Timed", 20) / 2, GAME_STATE.getScreenHeight() / 2 - 60, 20, ORANGE);
    DrawText("2. Untimed", GAME_STATE.getScreenWidth() / 2 - MeasureText("2. Untimed", 20) / 2, GAME_STATE.getScreenHeight() / 2 - 20, 20, ORANGE);
    DrawText("3. Change Difficulty", GAME_STATE.getScreenWidth() / 2 - MeasureText("3. Change Difficulty", 20) / 2, GAME_STATE.getScreenHeight() / 2 + 20, 20, ORANGE);
    DrawText("4. Quit", GAME_STATE.getScreenWidth() / 2 - MeasureText("4. Quit", 20) / 2, GAME_STATE.getScreenHeight() / 2 + 60, 20, RED);

    // Gameplay instructions
    DrawText("Game Controls:", GAME_STATE.getScreenWidth() / 2 - MeasureText("Game Controls:", 20) / 2, GAME_STATE.getScreenHeight() / 2 + 140, 20, LIGHTGRAY);
    DrawText("Arrow Keys/WASD - Move", GAME_STATE.getScreenWidth() / 2 - MeasureText("Arrow Keys/WASD - Move", 20) / 2, GAME_STATE.getScreenHeight() / 2 + 170, 20, LIGHTGRAY);
    DrawText("P - Pause Game", GAME_STATE.getScreenWidth() / 2 - MeasureText("P - Pause Game", 20) / 2, GAME_STATE.getScreenHeight() / 2 + 200, 20, LIGHTGRAY);
}
void DrawEndMenu(bool gameWon, float completionTime) {
    const int baseY = GAME_STATE.getScreenHeight() / 4;

    if (gameWon) {
        // Convert total time to hours, minutes, seconds
        int hours = static_cast<int>(completionTime) / 3600;
        int minutes = (static_cast<int>(completionTime) % 3600) / 60;
        float seconds = completionTime - (hours * 3600 + minutes * 60);

        // Create formatted time string
        char timeStr[50];
        if (hours > 0) {
            snprintf(timeStr, sizeof(timeStr), "Completion Time: %d:%02d:%05.2f", hours, minutes, seconds);
        } else if (minutes > 0) {
            snprintf(timeStr, sizeof(timeStr), "Completion Time: %d:%05.2f", minutes, seconds);
        } else {
            snprintf(timeStr, sizeof(timeStr), "Completion Time: %.2f seconds", seconds);
        }

        DrawText("You Win!", GAME_STATE.getScreenWidth() / 2 - MeasureText("You Win!", 50) / 2, baseY - 50, 50, DARKGREEN);
        DrawText(timeStr, GAME_STATE.getScreenWidth() / 2 - MeasureText(timeStr, 20) / 1.3, baseY, 30, DARKGREEN);
    } else {
        DrawText("You Lost", GAME_STATE.getScreenWidth() / 2 - MeasureText("You Lost", 40) / 2.2, baseY - 50, 40, RED);
        DrawText("Better Luck Next Time", GAME_STATE.getScreenWidth() / 2 - MeasureText("Better Luck Next Time", 20) / 1.3, baseY, 30, RED);
    }

    // Draw high scores table
    const int scoreStartY = baseY + 80;
    DrawText("HIGH SCORES", 
            GAME_STATE.getScreenWidth()/2 - MeasureText("HIGH SCORES", 25)/2,
            scoreStartY, 25, {0, 255, 255, 255});
    
    const int lineHeight = 25;
    for (size_t i = 0; i < highScores.size(); i++) {
        Color textColor = WHITE;
        // Highlight new score
        if (gameWon && highScores[i].time == completionTime && 
            highScores[i].wasTimed == (GAME_STATE.getCurrentMode() == Timed) &&
            highScores[i].difficulty == GAME_STATE.getCurrentDifficulty()) {
            textColor = YELLOW;
        }
        
        std::string scoreText = highScores[i].ToString();
        DrawText(scoreText.c_str(),
                GAME_STATE.getScreenWidth()/2 - MeasureText(scoreText.c_str(), 20)/2,
                scoreStartY + 40 + (i * lineHeight), 20, textColor);
    }
    
    // Draw end menu options at bottom
    const int optionsY = scoreStartY + 300;
    DrawText("1. Play Again", GAME_STATE.getScreenWidth() / 2 - MeasureText("1. Play Again", 20) / 2, optionsY, 20, ORANGE);
    DrawText("2. Change Game Mode", GAME_STATE.getScreenWidth() / 2 - MeasureText("2. Change Game Mode", 20) / 2, optionsY + 30, 20, ORANGE);
    DrawText("3. Quit", GAME_STATE.getScreenWidth() / 2 - MeasureText("3. Quit", 20) / 2, optionsY + 60, 20, RED);
}
void HandleMenuInput() {
    if (GAME_STATE.getCurrentGameState() == StartMenu) {
        if (GAME_STATE.getMenuState() == DifficultySelect) {
            if (IsKeyPressed(KEY_ONE)) {
                PlaySound(GameResources::menuSound);
                GAME_STATE.setCurrentDifficulty(Easy);
                GAME_STATE.setMenuState(ModeSelect);
            } else if (IsKeyPressed(KEY_TWO)) {
                PlaySound(GameResources::menuSound);
                GAME_STATE.setCurrentDifficulty(Medium);
                GAME_STATE.setMenuState(ModeSelect);
            } else if (IsKeyPressed(KEY_THREE)) {
                PlaySound(GameResources::menuSound);
                GAME_STATE.setCurrentDifficulty(Hard);
                GAME_STATE.setMenuState(ModeSelect);
            } else if (IsKeyPressed(KEY_FOUR)) {
                PlaySound(GameResources::menuSound);
                GameResources::UnloadAllResources();
                CloseWindow();
                exit(0);
            }
        } else if (GAME_STATE.getMenuState() == ModeSelect) {
            if (IsKeyPressed(KEY_ONE)) {
                PlaySound(GameResources::startSound);
                GAME_STATE.setCurrentMode(Timed);
                GAME_STATE.setCurrentGameState(Playing);
                InitializeGameWithDifficulty();
                GAME_STATE.setStartTime(GetTime());
            } else if (IsKeyPressed(KEY_TWO)) {
                PlaySound(GameResources::startSound);
                GAME_STATE.setCurrentMode(Untimed);
                GAME_STATE.setCurrentGameState(Playing);
                InitializeGameWithDifficulty();
                GAME_STATE.setStartTime(GetTime());
            } else if (IsKeyPressed(KEY_THREE)) {
                PlaySound(GameResources::menuSound);
                GAME_STATE.setMenuState(DifficultySelect);
            } else if (IsKeyPressed(KEY_FOUR)) {
                PlaySound(GameResources::menuSound);
                GameResources::UnloadAllResources();
                CloseWindow();
                exit(0);
            }
        }
    } else if (GAME_STATE.getCurrentGameState() == EndMenu) {
        if (IsKeyPressed(KEY_ONE)) {
            RandomizeTheme();
            PlaySound(GameResources::startSound);
            ResetGame();
            GAME_STATE.setCurrentGameState(Playing);
        } else if (IsKeyPressed(KEY_TWO)) {
            RandomizeTheme();
            PlaySound(GameResources::menuSound);
            GAME_STATE.setCurrentGameState(StartMenu);
        } else if (IsKeyPressed(KEY_THREE)) {
            PlaySound(GameResources::menuSound);
            GameResources::UnloadAllResources();
            CloseWindow();
            exit(0);
        }
    }
}
void UpdateSoundSettings() {
    switch(GAME_STATE.getSoundSettings()) {
        case SoundSettings::AllOn:
            switch(GAME_STATE.getCurrentTheme()) {
                case GameTheme::Space:
                    SetMusicVolume(GameResources::spaceAmbient, 1.0f);
                    break;
                case GameTheme::Jungle:
                    SetMusicVolume(GameResources::jungleAmbient, 1.0f);
                    break;
                case GameTheme::Desert:
                    SetMusicVolume(GameResources::desertAmbient, 1.0);
                    break;
            }
            SetSoundVolume(GameResources::startSound, 1.0f);
            SetSoundVolume(GameResources::blockedSound, 1.0f);
            SetSoundVolume(GameResources::winSound, 1.0f);
            SetSoundVolume(GameResources::loseSound, 1.0f);
            SetSoundVolume(GameResources::menuSound, 1.0f);
            SetSoundVolume(GameResources::bonusSound, 1.0f);
            break;
        case SoundSettings::MusicOff:
            SetMusicVolume(GameResources::spaceAmbient, 0.0f);
            SetMusicVolume(GameResources::jungleAmbient, 0.0f);
            SetMusicVolume(GameResources::desertAmbient, 0.0f);
            SetSoundVolume(GameResources::startSound, 1.0f);
            SetSoundVolume(GameResources::blockedSound, 1.0f);
            SetSoundVolume(GameResources::winSound, 1.0f);
            SetSoundVolume(GameResources::loseSound, 1.0f);
            SetSoundVolume(GameResources::menuSound, 1.0f);
            SetSoundVolume(GameResources::bonusSound, 1.0f);
            break;
        case SoundSettings::SoundOff:
            switch(GAME_STATE.getCurrentTheme()) {
                case GameTheme::Space:
                    SetMusicVolume(GameResources::spaceAmbient, 1.0f);
                    break;
                case GameTheme::Jungle:
                    SetMusicVolume(GameResources::jungleAmbient, 1.0f);
                    break;
                case GameTheme::Desert:
                    SetMusicVolume(GameResources::desertAmbient, 1.0);
                    break;
            }
            SetSoundVolume(GameResources::startSound, 0.0f);
            SetSoundVolume(GameResources::blockedSound, 0.0f);
            SetSoundVolume(GameResources::winSound, 0.0f);
            SetSoundVolume(GameResources::loseSound, 0.0f);
            SetSoundVolume(GameResources::menuSound, 0.0f);
            SetSoundVolume(GameResources::bonusSound, 0.0f);
            break;
        case SoundSettings::AllOff:
            SetMusicVolume(GameResources::spaceAmbient, 0.0f);
            SetMusicVolume(GameResources::jungleAmbient, 0.0f);
            SetMusicVolume(GameResources::desertAmbient, 0.0f);
            SetSoundVolume(GameResources::startSound, 0.0f);
            SetSoundVolume(GameResources::blockedSound, 0.0f);
            SetSoundVolume(GameResources::winSound, 0.0f);
            SetSoundVolume(GameResources::loseSound, 0.0f);
            SetSoundVolume(GameResources::menuSound, 0.0f);
            SetSoundVolume(GameResources::bonusSound, 0.0f);
            break;
    }
}
void DrawPauseMenu() {
    // Semi-transparent background
    DrawRectangle(0, 0, GAME_STATE.getScreenWidth(), GAME_STATE.getScreenHeight(), ColorAlpha(BLACK, 0.5f));
    
    // Draw settings menu
    DrawText("GAME PAUSED", GAME_STATE.getScreenWidth()/2 - MeasureText("GAME PAUSED", 40)/2, GAME_STATE.getScreenHeight()/2 - 150, 40, WHITE);
    
    // Sound settings
    DrawText("Sound Settings:", GAME_STATE.getScreenWidth()/2 - MeasureText("Sound Settings:", 20)/2, GAME_STATE.getScreenHeight()/2 - 50, 20, WHITE);
    DrawText("1. All Sounds ON", GAME_STATE.getScreenWidth()/2 - MeasureText("1. All Sounds ON", 20)/2, GAME_STATE.getScreenHeight()/2, 20, 
             GAME_STATE.getSoundSettings() == SoundSettings::AllOn ? GREEN : GRAY);
    DrawText("2. Music OFF", GAME_STATE.getScreenWidth()/2 - MeasureText("2. Music OFF", 20)/2, GAME_STATE.getScreenHeight()/2 + 30, 20,
             GAME_STATE.getSoundSettings() == SoundSettings::MusicOff ? GREEN : GRAY);
    DrawText("3. Sound Effects OFF", GAME_STATE.getScreenWidth()/2 - MeasureText("3. Sound Effects OFF", 20)/2, GAME_STATE.getScreenHeight()/2 + 60, 20,
             GAME_STATE.getSoundSettings() == SoundSettings::SoundOff ? GREEN : GRAY);
    DrawText("4. All Sounds OFF", GAME_STATE.getScreenWidth()/2 - MeasureText("4. All Sounds OFF", 20)/2, GAME_STATE.getScreenHeight()/2 + 90, 20,
             GAME_STATE.getSoundSettings() == SoundSettings::AllOff ? GREEN : GRAY);
    
    DrawText("P - Resume Game", GAME_STATE.getScreenWidth()/2 - MeasureText("P - Resume Game", 20)/2, GAME_STATE.getScreenHeight()/2 + 150, 20, LIGHTGRAY);
}