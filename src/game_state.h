#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "raylib.h"
#include "resources.h"
#include <mutex>
#include <vector>
#include <stack>
#include "maze.h"

// Forward declaration of Cell struct
struct Cell;

class GameState {
public:
    // Singleton access method
    static GameState& getInstance() {
        static GameState instance;
        return instance;
    }

    // Delete copy constructor and assignment operator
    GameState(const GameState&) = delete;
    GameState& operator=(const GameState&) = delete;

    // Screen and Rendering Properties
    int getScreenWidth() const { return m_screenWidth; }
    int getScreenHeight() const { return m_screenHeight; }
    void setScreenDimensions(int width, int height) {
        m_screenWidth = width;
        m_screenHeight = height;
        updateDerivedValues();
    }

    // Game Theme Management
    GameTheme getCurrentTheme() const { return m_currentTheme; }
    void setCurrentTheme(GameTheme theme) { 
        m_currentTheme = theme; 
        updateThemeColors();
    }

    // Game State Properties
    GameState& setCurrentGameState(GameStates state) { 
        m_gameState = state; 
        return *this;
    }
    GameStates getCurrentGameState() const { return m_gameState; }

    // Difficulty and Mode
    Difficulty getCurrentDifficulty() const { return m_currentDifficulty; }
    void setCurrentDifficulty(Difficulty difficulty) { m_currentDifficulty = difficulty; }

    GameMode getCurrentMode() const { return m_currentMode; }
    void setCurrentMode(GameMode mode) { m_currentMode = mode; }

    // Menu State
    MenuState getMenuState() const { return m_menuState; }
    void setMenuState(MenuState state) { m_menuState = state; }

    // Sound Settings
    SoundSettings getSoundSettings() const { return m_soundSettings; }
    void setSoundSettings(SoundSettings settings) { m_soundSettings = settings; }

    // Theme Colors
    const ThemeColors& getCurrentColors() const { return m_currentColors; }

    // Maze Generation Parameters
    int getCellSize() const { return m_cellSize; }
    void setCellSize(int size) { m_cellSize = size; updateDerivedValues(); }
    int getMargin() const { return m_margin; }
    int getMazeWidth() const { return m_mazeWidth; }
    int getMazeHeight() const { return m_mazeHeight; }
    int getColumns() const { return m_cols; }
    int getRows() const { return m_rows; }
    int getExitX() const { return exitX; }
    int getExitY() const { return exitY; }
    void setExitX(int exitX) { this->exitX = exitX; }
    void setExitY(int exitY) { this->exitY = exitY; }
    float getObstaclePercentage() const { return m_obstaclePercentage; }
    void setObstaclePercentage(float percentage) { m_obstaclePercentage = percentage; }

    // Player Movement Parameters
    int getPlayerX() const { return playerX; }
    int getPlayerY() const { return playerY; }
    void setPlayerX(int playerX) { this->playerX = playerX; }
    void setPlayerY(int playerY) { this->playerY = playerY; }
    float getPlayerPosX() const { return playerPosX; }
    float getPlayerPosY() const { return playerPosY; }
    void setPlayerPosX(float playerPosX) { this->playerPosX = playerPosX; }
    void setPlayerPosY(float playerPosY) { this->playerPosY = playerPosY; }
    float getPlayerTargetX() const { return playerTargetX; }
    float getPlayerTargetY() const { return playerTargetY; }
    void setPlayerTargetX(float playerTargetX) { this->playerTargetX = playerTargetX; }
    void setPlayerTargetY(float playerTargetY) { this->playerTargetY = playerTargetY; }
    float getPlayerRotation() const { return playerRotation; }
    void setPlayerRotation(float rotation) { playerRotation = rotation; }
    float getTargetRotation() const { return targetRotation; }
    void setTargetRotation(float rotation) { targetRotation = rotation; }
    bool getIsMoving() const { return isMoving; }
    void setIsMoving(bool moving) { isMoving = moving; }
    float getPlayerSpeed() const { return playerSpeed; }
    float getRotationSpeed() const { return rotationSpeed; }

    // Timed Mode Parameters
    float getTimeLimit() const { return timeLimit; }
    void setTimeLimit(float limit) { timeLimit = limit; }
    float getStartTime() const { return startTime; }
    void setStartTime(float time) { startTime = time; }
    float getTotalPausedTime() const { return totalPausedTime; }
    void setTotalPausedTime(float time) { totalPausedTime = time; }
    float getTimeElapsed() const { return timeElapsed; }
    void setTimeElapsed(float time) { timeElapsed = time; }

    bool getGameWon() const { return gameWon; }
    void setGameWon(bool won) { gameWon = won; }

    float getTimeBonusAmount() const { return TIME_BONUS_AMOUNT; }
    void setTimeBonusAmount(float amount) { TIME_BONUS_AMOUNT = amount; }
    float getTimeBonusFeedbackTimer() const { return timeBonusFeedbackTimer; }
    void setTimeBonusFeedbackTimer(float timer) { timeBonusFeedbackTimer = timer; }
    bool getShowTimeBonusFeedback() const { return showTimeBonusFeedback; }
    void setShowTimeBonusFeedback(bool show) { showTimeBonusFeedback = show; }
    float getSpeedBoostTimeLeft() const { return speedBoostTimeLeft; }
    void setSpeedBoostTimeLeft(float time) { speedBoostTimeLeft = time; }
    bool getIsSpeedBoosted() const { return isSpeedBoosted; }
    void setIsSpeedBoosted(bool boosted) { isSpeedBoosted = boosted; }

    const char* getScoresFile() const { return SCORES_FILE; }

    std::vector<std::vector<Cell>>& getGrid() { return m_grid; }
    std::stack<Cell*>& getStack() { return m_stack; }

private:
    // Private constructor to prevent direct instantiation
    GameState() {
        m_cellSize = m_screenWidth / 48; // Default to Easy difficulty cell size
        updateDerivedValues();
        updateThemeColors();
    }

    void updateDerivedValues() {
        // Recalculate maze parameters based on screen dimensions
        m_margin = m_screenWidth / 64;
        m_mazeWidth = m_screenWidth - 2 * m_margin;
        m_mazeHeight = m_screenHeight - 2 * m_margin;
        m_cols = m_mazeWidth / m_cellSize;
        m_rows = m_mazeHeight / m_cellSize;
    }

    void updateThemeColors() {
        // Set theme-specific colors
        switch (m_currentTheme) {
            case GameTheme::Space:
                m_currentColors = SPACE_COLORS;
                break;
            case GameTheme::Jungle:
                m_currentColors = JUNGLE_COLORS;
                break;
            case GameTheme::Desert:
                m_currentColors = DESERT_COLORS;
                break;
            default:
                m_currentColors = SPACE_COLORS;
                break;
        }
    }

    // Screen Properties
    int m_screenWidth = 1920;
    int m_screenHeight = 1000;

    // Game State Variables
    GameTheme m_currentTheme = GameTheme::Space;
    GameStates m_gameState = StartMenu;
    Difficulty m_currentDifficulty = Difficulty::Easy;
    GameMode m_currentMode = GameMode::Untimed;
    MenuState m_menuState = MenuState::DifficultySelect;
    SoundSettings m_soundSettings = SoundSettings::AllOn;
    
    // Current Theme Colors
    ThemeColors m_currentColors;

    // Maze Generation Parameters
    int m_cellSize;
    int m_margin;
    int m_mazeWidth;
    int m_mazeHeight;
    int m_cols;
    int m_rows;
    int exitX;
    int exitY;
    float m_obstaclePercentage = 0.15f;


    // Player position and rotation
    int playerX = 0, playerY = 0;
    float playerTargetX = 0.0f;
    float playerTargetY = 0.0f;
    float playerPosX = 0.0f;
    float playerPosY = 0.0f;
    const float playerSpeed = 8.0f;  // Cells per second
    bool isMoving = false;
    float playerRotation = 0.0f;         // Current rotation angle
    float targetRotation = 0.0f;         // Target rotation angle
    const float rotationSpeed = 10.0f;   // Rotation speed multiplier

    int timeLimit = 120.0f; // Seconds
    float startTime = 0.0f;
    float totalPausedTime = 0.0f;
    float timeElapsed;

    bool gameWon = false;
    float TIME_BONUS_AMOUNT = 10.0f;
    float timeBonusFeedbackTimer = 0.0f;
    bool showTimeBonusFeedback = false;
    float speedBoostTimeLeft = 0.0f;
    bool isSpeedBoosted = false;
    static constexpr const char* SCORES_FILE = "highscores.dat";

    std::vector<std::vector<Cell>> m_grid;
    std::stack<Cell*> m_stack;
};

// Convenience macro for global access
#define GAME_STATE GameState::getInstance()

#endif // GAME_STATE_H