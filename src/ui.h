#ifndef UI_H
#define UI_H

#include <string>
#include <ctime>
#include "raylib.h"
#include "gameplay.h"

struct Score {
    float time;
    bool wasTimed;
    Difficulty difficulty;
    std::time_t timestamp;
    bool operator<(const Score& rhs) const;
    
    Score(float t = 0.0f, bool timed = false, 
          Difficulty diff = Easy, std::time_t ts = 0);
    std::string ToString() const;
};

// Leaderboard variables and file
extern std::vector<Score> highScores;
const int MAX_HIGH_SCORES = 10;

void LoadHighScores();
void SaveHighScores();
void AddHighScore(float time, bool wasTimed);
void DrawDifficultyMenu();
void DrawModeMenu();
void DrawEndMenu(bool gameWon, float completionTime);
void HandleMenuInput();
void UpdateSoundSettings();
void DrawPauseMenu();

#endif