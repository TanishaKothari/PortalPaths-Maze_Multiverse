#include "gameplay.h"
#include "maze.h"
#include "resources.h"

Particle::Particle() : lifetime(0), active(false) {}

std::vector<TimePowerup> timePowerups(MAX_TIME_POWERUPS);
std::vector<SpeedBoost> speedBoosts(5);  // 5 speed boosts max

TimePowerup::TimePowerup() : active(false), rotationAngle(0.0f) {}

SpeedBoost::SpeedBoost() : active(false) {}

DifficultySettings DifficultySettings::Get(Difficulty diff) {
    switch(diff) {
        case Easy:
            return {GAME_STATE.getScreenWidth()/48, 0.15f, 10.0f};
        case Medium:
            return {GAME_STATE.getScreenWidth()/64, 0.20f, 8.0f};
        case Hard:
            return {GAME_STATE.getScreenWidth()/80, 0.25f, 5.0f};
        default:
            return {GAME_STATE.getScreenWidth()/48, 0.15f, 10.0f};
    }
}

void ResetGame() {
    // Re-initialize grid with new cell size and related properties
    GAME_STATE.getGrid().clear();
    GAME_STATE.getGrid().resize(GAME_STATE.getRows(), std::vector<Cell>(GAME_STATE.getColumns(), Cell(0, 0)));
    
    // Reinitialize maze
    InitializeGrid();
    GenerateMaze();
    PlaceRandomExit();
    AddRandomObstacles(GAME_STATE.getExitX(), GAME_STATE.getExitY());

    // Reset speed boost variables
    GAME_STATE.setIsSpeedBoosted(false);
    GAME_STATE.setSpeedBoostTimeLeft(0.0f);
    
    // Reset time limit and respawn powerups
    if (GAME_STATE.getCurrentMode() == Timed) {
        GAME_STATE.setTimeLimit(120.0f);
        timePowerups.clear();
        timePowerups.resize(MAX_TIME_POWERUPS);
    }

    SpawnPowerups();
    
    GAME_STATE.setStartTime(GetTime());
    GAME_STATE.setTotalPausedTime(0.0f);
    GAME_STATE.setTimeElapsed(0.0f);
    
    // Reset player and target position and rotation
    GAME_STATE.setPlayerX(0); GAME_STATE.setPlayerY(0);
    GAME_STATE.setPlayerPosX(0); GAME_STATE.setPlayerPosY(0);
    GAME_STATE.setPlayerTargetX(0); GAME_STATE.setPlayerTargetY(0);
    GAME_STATE.setPlayerRotation(0); GAME_STATE.setTargetRotation(0);
    GAME_STATE.setIsMoving(false);

    GAME_STATE.setGameWon(false);
}

void InitializeGameWithDifficulty() {
    DifficultySettings settings = DifficultySettings::Get(GAME_STATE.getCurrentDifficulty());

    GAME_STATE.setTimeBonusAmount(settings.timeBonusAmount);
    GAME_STATE.setCellSize(settings.cellSize);
    GAME_STATE.setObstaclePercentage(settings.obstaclePercentage);
    GAME_STATE.setTotalPausedTime(0.0f);
    GAME_STATE.setTimeElapsed(0.0f);

    // Resize grid with new dimensions
    ResetGame();
}

// Maintain a list of all valid cells for powerup placement
void PrecomputeValidCells(std::vector<std::pair<int, int>>& validCells) {
    validCells.clear();
    for (int y = 0; y < GAME_STATE.getRows(); y++) {
        for (int x = 0; x < GAME_STATE.getColumns(); x++) {
            if (!GAME_STATE.getGrid()[y][x].isObstacle && !(x == 0 && y == 0) && !(x == GAME_STATE.getExitX() && y == GAME_STATE.getExitY())) {
                validCells.emplace_back(x, y);
            }
        }
    }
}

void SpawnPowerups() {
    std::vector<std::pair<int, int>> validCells;
    PrecomputeValidCells(validCells);

    if (GAME_STATE.getCurrentMode() == Timed) {
        // Spawn time power-ups
        for (int i = 0; i < MAX_TIME_POWERUPS && !validCells.empty(); i++) {
            int index = std::rand() % validCells.size();
            timePowerups[i].x = validCells[index].first;
            timePowerups[i].y = validCells[index].second;
            timePowerups[i].active = true;
            validCells.erase(validCells.begin() + index);
        }      
    }

    // Spawn speed boosts if applicable
    if (GAME_STATE.getCurrentDifficulty() != Difficulty::Easy) {
        speedBoosts.clear();
        speedBoosts.resize(5); // Limit to 5 boosts
        for (std::vector<SpeedBoost>::size_type i = 0; i < speedBoosts.size() && !validCells.empty(); i++) {
            int index = std::rand() % validCells.size();
            speedBoosts[i].x = validCells[index].first;
            speedBoosts[i].y = validCells[index].second;
            speedBoosts[i].active = true;
            validCells.erase(validCells.begin() + index);
        }
    }
}
void UpdateTimePowerups(float deltaTime) {
    for (auto& powerup : timePowerups) {
        if (powerup.active) {
            // Rotate powerup
            powerup.rotationAngle += 90.0f * deltaTime;
            
            // Check if player collected
            if (GAME_STATE.getPlayerX() == powerup.x && GAME_STATE.getPlayerY() == powerup.y) {
                PlaySound(GameResources::bonusSound);
                GAME_STATE.setTimeLimit(GAME_STATE.getTimeLimit() + GAME_STATE.getTimeBonusAmount()); // Add bonus time
                powerup.active = false;

                // Trigger feedback
                GAME_STATE.setShowTimeBonusFeedback(true);
                GAME_STATE.setTimeBonusFeedbackTimer(TIME_BONUS_FEEDBACK_DURATION);
            }
        }
    }

    // Update feedback timer
    if (GAME_STATE.getShowTimeBonusFeedback()) {
        GAME_STATE.setTimeBonusFeedbackTimer(GAME_STATE.getTimeBonusFeedbackTimer() - deltaTime);
        if (GAME_STATE.getTimeBonusFeedbackTimer() <= 0) {
            GAME_STATE.setShowTimeBonusFeedback(false);
        }
    }
}

void DrawTimePowerups() {
    for (const auto& powerup : timePowerups) {
        if (powerup.active) {
            // Calculate position
            float x = GAME_STATE.getMargin() + powerup.x * GAME_STATE.getCellSize() + GAME_STATE.getCellSize()/2.0f;
            float y = GAME_STATE.getMargin() + powerup.y * GAME_STATE.getCellSize() + GAME_STATE.getCellSize()/2.0f;
            
            // Draw with rotation
            DrawTexturePro(
                GameResources::timeBonusTexture,
                Rectangle{ 0, 0, (float)GameResources::timeBonusTexture.width, (float)GameResources::timeBonusTexture.height },
                Rectangle{ x, y, GAME_STATE.getCellSize() * 0.6f, GAME_STATE.getCellSize() * 0.6f },
                Vector2{ GAME_STATE.getCellSize() * 0.3f, GAME_STATE.getCellSize() * 0.3f },
                powerup.rotationAngle,
                WHITE
            );
        }
    }
}

void UpdateSpeedBoosts(float deltaTime) {
    // Update active speed boost duration
    if (GAME_STATE.getIsSpeedBoosted()) {
        GAME_STATE.setSpeedBoostTimeLeft(GAME_STATE.getSpeedBoostTimeLeft() - deltaTime);
        if (GAME_STATE.getSpeedBoostTimeLeft() <= 0) {
            GAME_STATE.setIsSpeedBoosted(false);
        }
    }

    // Update and rotate speed boost pickups
    for (auto& boost : speedBoosts) {
        if (boost.active) {
            // Check if player collected
            if (GAME_STATE.getPlayerX() == boost.x && GAME_STATE.getPlayerY() == boost.y) {
                PlaySound(GameResources::bonusSound);
                GAME_STATE.setSpeedBoostTimeLeft(SPEED_BOOST_DURATION);
                GAME_STATE.setIsSpeedBoosted(true);
                boost.active = false;
            }
        }
    }
}

void DrawSpeedBoosts() {
    for (const auto& boost : speedBoosts) {
        if (boost.active) {
            float x = GAME_STATE.getMargin() + boost.x * GAME_STATE.getCellSize() + GAME_STATE.getCellSize()/2.0f;
            float y = GAME_STATE.getMargin() + boost.y * GAME_STATE.getCellSize() + GAME_STATE.getCellSize()/2.0f;
            
            DrawTexturePro(
                GameResources::speedBoostTexture,
                Rectangle{ 0, 0, (float)GameResources::speedBoostTexture.width, (float)GameResources::speedBoostTexture.height },
                Rectangle{ x, y, GAME_STATE.getCellSize() * 0.5f, GAME_STATE.getCellSize() * 0.5f },
                Vector2{ GAME_STATE.getCellSize() * 0.2f, GAME_STATE.getCellSize() * 0.2f },
                0,
                WHITE
            );
        }
    }
}

bool CanMove(int direction) {
    Cell* current = &GAME_STATE.getGrid()[GAME_STATE.getPlayerY()][GAME_STATE.getPlayerX()];
    int newX = GAME_STATE.getPlayerX(), newY = GAME_STATE.getPlayerY();

    // Calculate new position based on direction
    switch(direction) {
        case 0: newY--; break; // Up
        case 1: newX++; break; // Right
        case 2: newY++; break; // Down
        case 3: newX--; break; // Left
    }

    // Check if the move is within bounds and not blocked by walls or obstacles
    return IsInBounds(newX, newY) && 
           !current->walls[direction] && 
           !GAME_STATE.getGrid()[newY][newX].isObstacle;
}

float Lerp(float start, float end, float amount) {
    return start + amount * (end - start);
}

void MovePlayer() {
    if (GAME_STATE.getIsMoving()) {
        float currentSpeed = GAME_STATE.getPlayerSpeed();
        if (GAME_STATE.getIsSpeedBoosted()) {
            currentSpeed *= SPEED_BOOST_MULTIPLIER;
        }

        // Update current position towards target
        float moveAmount = currentSpeed * GetFrameTime();
        GAME_STATE.setPlayerPosX(Lerp(GAME_STATE.getPlayerPosX(), GAME_STATE.getPlayerTargetX(), moveAmount));
        GAME_STATE.setPlayerPosY(Lerp(GAME_STATE.getPlayerPosY(), GAME_STATE.getPlayerTargetY(), moveAmount));

        // Check if we've essentially reached the target
        if (fabs(GAME_STATE.getPlayerPosX() - GAME_STATE.getPlayerTargetX()) < 0.01f && 
            fabs(GAME_STATE.getPlayerPosY() - GAME_STATE.getPlayerTargetY()) < 0.01f) {
            GAME_STATE.setPlayerPosX(GAME_STATE.getPlayerTargetX());
            GAME_STATE.setPlayerPosY(GAME_STATE.getPlayerTargetY());
            GAME_STATE.setIsMoving(false);
        }
    }

    // Check for new movement regardless of current movement
    int newX = GAME_STATE.getPlayerX();
    int newY = GAME_STATE.getPlayerY();
    bool shouldMove = false;

    if ((IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) && GAME_STATE.getPlayerY() > 0) {
        if (!GAME_STATE.getGrid()[GAME_STATE.getPlayerY()][GAME_STATE.getPlayerX()].walls[0] && GAME_STATE.getGrid()[GAME_STATE.getPlayerY()-1][GAME_STATE.getPlayerX()].isObstacle) {
            PlaySound(GameResources::blockedSound);
        }
        else if (CanMove(0)) {
            newY--;
            shouldMove = true;
        }
    }
    else if ((IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) && GAME_STATE.getPlayerX() < GAME_STATE.getColumns() - 1) {
        if (!GAME_STATE.getGrid()[GAME_STATE.getPlayerY()][GAME_STATE.getPlayerX()].walls[1] && GAME_STATE.getGrid()[GAME_STATE.getPlayerY()][GAME_STATE.getPlayerX()+1].isObstacle) {
            PlaySound(GameResources::blockedSound);
        }
        else if (CanMove(1)) {
            newX++;
            shouldMove = true;
        }
    }
    else if ((IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) && GAME_STATE.getPlayerY() < GAME_STATE.getRows() - 1) {
        if (!GAME_STATE.getGrid()[GAME_STATE.getPlayerY()][GAME_STATE.getPlayerX()].walls[2] && GAME_STATE.getGrid()[GAME_STATE.getPlayerY()+1][GAME_STATE.getPlayerX()].isObstacle) {
            PlaySound(GameResources::blockedSound);
        }
        else if (CanMove(2)) {
            newY++;
            shouldMove = true;
        }
    }
    else if ((IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) && GAME_STATE.getPlayerX() > 0) {
        if (!GAME_STATE.getGrid()[GAME_STATE.getPlayerY()][GAME_STATE.getPlayerX()].walls[3] && GAME_STATE.getGrid()[GAME_STATE.getPlayerY()][GAME_STATE.getPlayerX()-1].isObstacle) {
            PlaySound(GameResources::blockedSound);
        }
        else if (CanMove(3)) {
            newX--;
            shouldMove = true;
        }
    }

    if (shouldMove && (!GAME_STATE.getIsMoving() || (fabs(GAME_STATE.getPlayerPosX() - GAME_STATE.getPlayerTargetX()) < 0.5f && 
                                   fabs(GAME_STATE.getPlayerPosY() - GAME_STATE.getPlayerTargetY()) < 0.5f))) {
        GAME_STATE.setPlayerTargetX(static_cast<float>(newX));
        GAME_STATE.setPlayerTargetY(static_cast<float>(newY));
        GAME_STATE.setPlayerX(newX);
        GAME_STATE.setPlayerY(newY);
        GAME_STATE.setIsMoving(true);
    }
}

void UpdatePlayerRotation() {
    float diff{};
    switch (GAME_STATE.getCurrentTheme()) {
        case GameTheme::Space:
            // Space theme - rotation for all directions
            if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) GAME_STATE.setTargetRotation(-90.0f);
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) GAME_STATE.setTargetRotation(0.0f);
            if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) GAME_STATE.setTargetRotation(90.0f);
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) GAME_STATE.setTargetRotation(180.0f);
            
            // Smooth rotation
            diff = GAME_STATE.getTargetRotation() - GAME_STATE.getPlayerRotation();
            if (diff > 180.0f) diff -= 360.0f;
            if (diff < -180.0f) diff += 360.0f;
            GAME_STATE.setPlayerRotation(GAME_STATE.getPlayerRotation() + diff * GAME_STATE.getRotationSpeed() * GetFrameTime());
        
            break;

        case GameTheme::Jungle:
        case GameTheme::Desert: 
            // Jungle/Desert themes - no rotation (flip while drawing player)
            GAME_STATE.setPlayerRotation(0.0f);
            break;
    }
}