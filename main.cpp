#include "raylib.h"
#include <stdio.h>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <random>
#include <fstream>
#include <iomanip>
#include <sstream>

// Space theme assets
const char* SPACESHIP_TEXTURE_PATH = "Assets/spaceship.png";
const char* ASTEROID_SPRITESHEET_PATH = "Assets/asteroids_spritesheet.png";
const char* SPACE_AMBIENT = "sounds/space_ambient.wav";

// Jungle theme assets
const char* MONKEY_TEXTURE_PATH = "Assets/monkey.png";
const char* BANANA_TEXTURE_PATH = "Assets/bananas.png";
const char* LOGS_SPRITESHEET_PATH = "Assets/logs_spritesheet.png";
const char* JUNGLE_AMBIENT = "sounds/jungle_ambient.wav";

// Desert theme assets
const char* CAMEL_TEXTURE_PATH = "Assets/camel.png";
const char* OASIS_TEXTURE_PATH = "Assets/oasis.png";
const char* CACTUS_TEXTURE_PATH = "Assets/cactus.png";
const char* TUMBLEWEED_TEXTURE_PATH = "Assets/tumbleweed.png";
const char* DESERT_AMBIENT = "sounds/desert_ambient.wav";

// Powerup assets
const char* TIME_BONUS_TEXTURE_PATH = "Assets/timeBonus.png";
const char* SPEED_BOOST_TEXTURE_PATH = "Assets/speedBoost.png";

// Sound file paths
const char* SOUND_START = "sounds/poweron.wav";
const char* SOUND_BLOCKED = "sounds/blocked.wav";
const char* SOUND_WIN = "sounds/win.wav";
const char* SOUND_LOSE = "sounds/lose.wav";
const char* SOUND_MENU_SELECT = "sounds/select.wav";
const char* SOUND_PORTAL = "sounds/portal.wav";
const char* SOUND_BONUS = "sounds/bonus.wav";

// Theme-specific colors
struct ThemeColors {
    Color background;
    Color playerTint;
    Color wallColor;
    Color obstacleColor;
    Color goalColor;
};

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
    {120, 60, 30, 255},    // Wood brown logs
    {255, 255, 0, 255}      // Yellow bananas
};

const ThemeColors DESERT_COLORS = {
    {255, 204, 102, 255},   // Sandy background
    {139, 69, 19, 255},     // Saddle brown camel tint
    {204, 119, 34, 210},    // Orange walls
    {86, 125, 70, 255},     // Green cacti/tumbleweed
    {0, 191, 255, 255}      // Blue oasis
};

// Game themes
enum class GameTheme {
    Space,
    Jungle, 
    Desert
};

// Obstacle types for desert theme
enum class DesertObstacleType {
    Cactus,
    Tumbleweed
};

// Game states
enum GameState {
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

// Global variables
GameState currentState = StartMenu;
GameMode currentMode = Untimed;
Difficulty currentDifficulty = Easy;
MenuState menuState = DifficultySelect;
GameTheme currentTheme = GameTheme::Space;
ThemeColors currentColors = SPACE_COLORS;

SoundSettings currentSoundSettings = SoundSettings::AllOn;
bool isPaused = false;
float pauseStartTime = 0.0f;  // Track when pause started
float totalPausedTime = 0.0f; // Track total time spent paused
Texture2D blurredMazeTexture; // For storing blurred maze screenshot
RenderTexture2D mazeRenderTexture; // For rendering maze to texture

// Screen dimensions
const int screenWidth = 1920;
const int screenHeight = 1000;

// Maze settings
int cellSize = screenWidth / 48;    // Size of each cell in pixels
const int margin = screenWidth / 64;      // Margin between the maze and the window edges
int mazeWidth = screenWidth - 2 * margin;  // Width of the maze
int mazeHeight = screenHeight - 2 * margin; // Height of the maze
int cols = mazeWidth / cellSize;   // Number of columns in the maze
int rows = mazeHeight / cellSize;  // Number of rows in the maze

// Particle settings
const int MAX_PORTAL_PARTICLES = 100;
const float PARTICLE_SPAWN_RATE = 0.016f; // Spawn every 16ms
const float PARTICLE_MAX_LIFETIME = 1.0f;
const float PARTICLE_SPEED = 25.0f;

// Powerup settings
const int MAX_TIME_POWERUPS = 5;
float TIME_BONUS_AMOUNT = 10.0f; // Seconds added to timer
float timeBonusFeedbackTimer = 0.0f;
const float TIME_BONUS_FEEDBACK_DURATION = 2.0f; // Show feedback for 2 seconds
bool showTimeBonusFeedback = false;

const float SPEED_BOOST_DURATION = 5.0f;  // 5 seconds
const float SPEED_BOOST_MULTIPLIER = 1.75f; // 75% speed boost

Texture2D starFieldTexture;  // Texture for star field background

float timeLimit = 120.f;  // 2 minute time limit for timed mode

// Directions for maze generation
const int directions[4][2] = {
    {0, -1},  // Up
    {1, 0},   // Right
    {0, 1},   // Down
    {-1, 0}   // Left
};

namespace GameResources {
    // Space theme textures and music
    Texture2D spaceshipTexture;
    Texture2D portalPlaceholder;  // This will be generated at runtime
    Texture2D asteroidSpritesheet;
    Music spaceAmbient;

    // Jungle theme textures and music
    Texture2D monkeyTexture;
    Texture2D bananaTexture;
    Texture2D logsSpritesheet;
    Music jungleAmbient;

    // Desert theme textures and music
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
    Sound portalSound;
    Sound bonusSound;

    // Flags to track loaded resources
    bool spaceResourcesLoaded = false;
    bool jungleResourcesLoaded = false;
    bool desertResourcesLoaded = false;
    bool powerupResourcesLoaded = false;
    bool generalSoundsLoaded = false;

    void LoadSpaceResources() {
        if (!spaceResourcesLoaded) {
            spaceshipTexture = LoadTexture(SPACESHIP_TEXTURE_PATH);
            Image portalImage = GenImageColor(64, 64, BLANK);
            ImageDrawCircle(&portalImage, 32, 32, 30, SKYBLUE);
            portalPlaceholder = LoadTextureFromImage(portalImage);
            UnloadImage(portalImage);
            asteroidSpritesheet = LoadTexture(ASTEROID_SPRITESHEET_PATH);
            spaceAmbient = LoadMusicStream(SPACE_AMBIENT);
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
            monkeyTexture = LoadTexture(MONKEY_TEXTURE_PATH);
            bananaTexture = LoadTexture(BANANA_TEXTURE_PATH);
            logsSpritesheet = LoadTexture(LOGS_SPRITESHEET_PATH);
            jungleAmbient = LoadMusicStream(JUNGLE_AMBIENT);
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
            camelTexture = LoadTexture(CAMEL_TEXTURE_PATH);
            oasisTexture = LoadTexture(OASIS_TEXTURE_PATH);
            cactusTexture = LoadTexture(CACTUS_TEXTURE_PATH);
            tumbleweedTexture = LoadTexture(TUMBLEWEED_TEXTURE_PATH);
            desertAmbient = LoadMusicStream(DESERT_AMBIENT);
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

    // Load powerup resources (common to all themes)
    void LoadPowerupResources() {
        if (!powerupResourcesLoaded) {
            timeBonusTexture = LoadTexture(TIME_BONUS_TEXTURE_PATH);
            speedBoostTexture = LoadTexture(SPEED_BOOST_TEXTURE_PATH);

            // Sound effects for powerups
            bonusSound = LoadSound(SOUND_BONUS);

            powerupResourcesLoaded = true;
        }
    }

    // Unload powerup resources
    void UnloadPowerupResources() {
        if (powerupResourcesLoaded) {
            UnloadTexture(timeBonusTexture);
            UnloadTexture(speedBoostTexture);

            UnloadSound(bonusSound);

            powerupResourcesLoaded = false;
        }
    }

    // Load general sounds (not tied to themes)
    void LoadGeneralSounds() {
        if (!generalSoundsLoaded) {
            startSound = LoadSound(SOUND_START);
            blockedSound = LoadSound(SOUND_BLOCKED);
            winSound = LoadSound(SOUND_WIN);
            loseSound = LoadSound(SOUND_LOSE);
            menuSound = LoadSound(SOUND_MENU_SELECT);
            portalSound = LoadSound(SOUND_PORTAL);

            generalSoundsLoaded = true;
        }
    }

    // Unload general sounds
    void UnloadGeneralSounds() {
        if (generalSoundsLoaded) {
            UnloadSound(startSound);
            UnloadSound(blockedSound);
            UnloadSound(winSound);
            UnloadSound(loseSound);
            UnloadSound(menuSound);
            UnloadSound(portalSound);

            generalSoundsLoaded = false;
        }
    }

    void UnloadUnusedResources() {
        // Unload resources for themes that are not currently active
        switch (currentTheme) {
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

    // Full unload function for game termination
    void UnloadAllResources() {
        UnloadSpaceResources();
        UnloadJungleResources();
        UnloadDesertResources();
        UnloadPowerupResources();
        UnloadGeneralSounds();
    }
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
            currentColors = SPACE_COLORS;
            if (currentSoundSettings != SoundSettings::MusicOff && currentSoundSettings != SoundSettings::AllOff) {
                PlayMusicStream(GameResources::spaceAmbient);
                SetMusicVolume(GameResources::spaceAmbient, 1.0f);
            }
            break;
        case GameTheme::Jungle:
            GameResources::LoadJungleResources();
            currentColors = JUNGLE_COLORS;
            if (currentSoundSettings != SoundSettings::MusicOff && currentSoundSettings != SoundSettings::AllOff) {
                PlayMusicStream(GameResources::jungleAmbient);
                SetMusicVolume(GameResources::jungleAmbient, 1.0f);
            }
            break;
        case GameTheme::Desert:
            GameResources::LoadDesertResources();
            currentColors = DESERT_COLORS;
            if (currentSoundSettings != SoundSettings::MusicOff && currentSoundSettings != SoundSettings::AllOff) {
                PlayMusicStream(GameResources::desertAmbient);
                SetMusicVolume(GameResources::desertAmbient, 1.0f);
            }
            break;
    }
}

void StopCurrentThemeMusic() {
    switch(currentTheme) {
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

void RandomizeTheme() {
    currentTheme = static_cast<GameTheme>(rand() % 3);
    SetThemeResources(currentTheme);
}

// Cell structure for the maze
struct Cell {
    int x, y;
    bool visited;
    bool walls[4]; // Top, Right, Bottom, Left
    bool isObstacle;

    // Sprite sheet coordinates for asteroids
    int spriteRow;    // 0 or 1
    int spriteCol;    // 0-15

    // Sprite sheet coordinates for logs (3x3 spritesheet)
    int logSpriteRow;    // 0-2
    int logSpriteCol;    // 0-2

    DesertObstacleType desertObstacleType;  // Cactus or tumbleweed

    Cell(int x_, int y_) : x(x_), y(y_), visited(false), isObstacle(false),
                        spriteRow(0), spriteCol(0), logSpriteRow(0), logSpriteCol(0),
                        desertObstacleType(DesertObstacleType::Cactus) {
        // Initialize all walls as present
        walls[0] = walls[1] = walls[2] = walls[3] = true;
    }

    // Draw the cell with its walls and obstacles
    void Draw() const {
        int xPos = margin + x * cellSize;
        int yPos = margin + y * cellSize;

        // Draw theme-specific walls
        Color wallColor = currentColors.wallColor;
        if (walls[0]) DrawLine(xPos, yPos, xPos + cellSize, yPos, wallColor);          // Top
        if (walls[1]) DrawLine(xPos + cellSize, yPos, xPos + cellSize, yPos + cellSize, wallColor); // Right
        if (walls[2]) DrawLine(xPos + cellSize, yPos + cellSize, xPos, yPos + cellSize, wallColor); // Bottom
        if (walls[3]) DrawLine(xPos, yPos + cellSize, xPos, yPos, wallColor);          // Left

        // Draw theme-specific obstacles
        if (isObstacle) {
            float spriteWidth{};
            float spriteHeight{};

            switch (currentTheme) {
                case GameTheme::Space:
                    // Draw asteroid sprite
                    spriteWidth = GameResources::asteroidSpritesheet.width / 16.0f;
                    spriteHeight = GameResources::asteroidSpritesheet.height / 2.0f;
                    
                    DrawTexturePro(
                        GameResources::asteroidSpritesheet,
                        Rectangle{ 
                            spriteCol * spriteWidth,
                            spriteRow * spriteHeight,
                            spriteWidth,
                            spriteHeight
                        },
                        Rectangle{
                            static_cast<float>(xPos) + cellSize/2,
                            static_cast<float>(yPos) + cellSize/2,
                            cellSize * 0.8f,
                            cellSize * 0.8f
                        },
                        Vector2{ cellSize * 0.4f, cellSize * 0.4f },
                        0.0f,
                        currentColors.obstacleColor
                    );
                    break;

                case GameTheme::Jungle:
                    // Draw log sprite
                    spriteWidth = GameResources::logsSpritesheet.width / 3.0f;
                    spriteHeight = GameResources::logsSpritesheet.height / 3.0f;

                    DrawTexturePro(
                        GameResources::logsSpritesheet,
                        Rectangle{logSpriteCol * spriteWidth, logSpriteRow * spriteHeight, spriteWidth, spriteHeight},
                        Rectangle{static_cast<float>(xPos) + cellSize/2,
                                static_cast<float>(yPos) + cellSize/2,
                                cellSize * 0.8f,
                                cellSize * 0.8f},
                        Vector2{cellSize * 0.4f, cellSize * 0.4f},
                        0.0f,
                        currentColors.obstacleColor
                    );
                    break;

                case GameTheme::Desert:
                    if (desertObstacleType == DesertObstacleType::Cactus) {
                        // Draw cactus
                        DrawTexturePro(
                            GameResources::cactusTexture,
                            Rectangle{0, 0, (float)GameResources::cactusTexture.width,
                                    (float)GameResources::cactusTexture.height},
                            Rectangle{static_cast<float>(xPos) + cellSize/2,
                                    static_cast<float>(yPos) + cellSize/2,
                                    cellSize * 0.8f,
                                    cellSize * 0.8f},
                            Vector2{cellSize * 0.4f, cellSize * 0.4f},
                            0.0f,
                            currentColors.obstacleColor
                        );
                    } else {
                        // Draw tumbleweed with darker brown color
                        DrawTexturePro(
                            GameResources::tumbleweedTexture,
                            Rectangle{0, 0, (float)GameResources::tumbleweedTexture.width,
                                    (float)GameResources::tumbleweedTexture.height},
                            Rectangle{static_cast<float>(xPos) + cellSize/2,
                                    static_cast<float>(yPos) + cellSize/2,
                                    cellSize * 0.8f,
                                    cellSize * 0.8f},
                            Vector2{cellSize * 0.4f, cellSize * 0.4f},
                            0.0f,
                            Color{55, 41, 14, 255}  // #37290E
                        );
                    }
                    break;

                default:
                    // Handle unexpected theme
                    DrawRectangle(xPos + cellSize/4, yPos + cellSize/4, 
                                cellSize/2, cellSize/2, 
                                currentColors.obstacleColor);
                    break;
            }
        }
    }
};

// Particle structure for portal effect
struct Particle {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float lifetime;
    bool active;
    
    Particle() : lifetime(0), active(false) {}
};

// Powerup structures
struct TimePowerup {
    int x, y;
    bool active;
    float rotationAngle;
    
    TimePowerup() : active(false), rotationAngle(0.0f) {}
};

struct SpeedBoost {
    int x, y;
    bool active;
    
    SpeedBoost() : active(false) {}
};

// Difficulty settings structure
struct DifficultySettings {
    int cellSize;                    // Size of maze cells
    float obstaclePercentage;        // % of cells that are obstacles
    float timeBonusAmount;          // Seconds added by time bonus
    bool hasSpeedBoosts;
    
    // Get settings based on difficulty level
    static DifficultySettings Get(Difficulty diff) {
        switch(diff) {
            case Easy:
                return {screenWidth / 48, 0.15f, 10.0f, false};
            case Medium:
                return {screenWidth / 64, 0.20f, 8.0f, true};
            case Hard:
                return {screenWidth / 80, 0.25f, 5.0f, true};
            default:
                return {screenWidth / 48, 0.15f, 10.0f, false};
        }
    }
};

struct Score {
    float time;             // Game completion time
    bool wasTimed;          // Game mode
    Difficulty difficulty;
    std::time_t timestamp;  // UTC timestamp when completed
    
    Score(float t = 0.0f, bool timed = false, 
          Difficulty diff = Easy, std::time_t ts = 0) 
        : time(t), wasTimed(timed), difficulty(diff), timestamp(ts) {}

    // Convert score to string for display
    std::string ToString() const {
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
        char timeStr[32];
        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M UTC", tm);
        
        ss << " | " << (wasTimed ? "Timed" : "Untimed") 
           << " | " << diffStr
           << " | " << timeStr;
        
        return ss.str();
    }
};

// Maze grid
std::vector<std::vector<Cell>> grid(rows, std::vector<Cell>(cols, Cell(0, 0)));

// Maze generation stack
std::stack<Cell*> stack;

// Time bonus powerups
std::vector<TimePowerup> powerups(MAX_TIME_POWERUPS); 

// Star field vector
std::vector<Vector2> stars(200);

// Speed boost powerups
std::vector<SpeedBoost> speedBoosts(5);  // 5 speed boosts max
float speedBoostTimeLeft = 0.0f;
bool isSpeedBoosted = false;

// Leaderboard variables and file
std::vector<Score> highScores;
const int MAX_HIGH_SCORES = 10;
const char* SCORES_FILE = "highscores.dat";

// Helper function to calculate Manhattan distance (heuristic)
int Heuristic(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

// Check if a path exists from start to end
bool PathExists(int startX, int startY, int endX, int endY) {
    // Priority queue for A* frontier (min-heap)
    using Node = std::pair<int, std::pair<int, int>>; // <cost, <x, y>>
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> frontier;

    // Costs and visited grids
    std::vector<std::vector<int>> cost(rows, std::vector<int>(cols, INT_MAX));
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));

    // Start point
    frontier.push({0, {startX, startY}});
    cost[startY][startX] = 0;

    while (!frontier.empty()) {
        auto [currentCost, current] = frontier.top();
        frontier.pop();

        int x = current.first;
        int y = current.second;

        // If goal is reached, return true
        if (x == endX && y == endY) {
            return true;
        }

        // Skip already visited nodes
        if (visited[y][x]) continue;
        visited[y][x] = true;

        // Explore neighbors
        for (int i = 0; i < 4; i++) {
            int nx = x + directions[i][0];
            int ny = y + directions[i][1];

            // Check bounds, obstacles, and walls
            if (nx >= 0 && nx < cols && ny >= 0 && ny < rows &&
                !grid[ny][nx].isObstacle && !visited[ny][nx] &&
                !grid[y][x].walls[i]) {
                // Calculate new cost
                int newCost = cost[y][x] + 1; // All movements cost 1
                if (newCost < cost[ny][nx]) {
                    cost[ny][nx] = newCost;
                    int priority = newCost + Heuristic(nx, ny, endX, endY);
                    frontier.push({priority, {nx, ny}});
                }
            }
        }
    }
    return false; // No path found
}

// Add random obstacles while ensuring solvability
void AddRandomObstacles(int exitX, int exitY) {
    std::random_device rd;
    std::mt19937 gen(rd());

    // Separate distributions for different sprite sheets
    std::uniform_int_distribution<> asteroidRowDist(0, 1);      // 2 rows for asteroids
    std::uniform_int_distribution<> asteroidColDist(0, 15);     // 16 columns for asteroids
    std::uniform_int_distribution<> logRowDist(0, 2);           // 3 rows for logs
    std::uniform_int_distribution<> logColDist(0, 2);           // 3 columns for logs
    std::uniform_int_distribution<> obstacleDist(0, 1);         // For desert obstacle type

    // Determine number of obstacles (10-15% of cells)
    int maxObstacles = std::max(1, static_cast<int>(rows * cols * 0.15));
    int obstacleCount = 0;

    while (obstacleCount < maxObstacles) {
        int x = std::rand() % cols;
        int y = std::rand() % rows;

        // Avoid placing obstacles at start, exit, or already obstacle cells
        if ((x == 0 && y == 0) || (x == exitX && y == exitY) || grid[y][x].isObstacle) {
            continue;
        }

        // Temporarily set as obstacle
        grid[y][x].isObstacle = true;

        // Set sprite coordinates based on theme
        if (currentTheme == GameTheme::Space) {
            grid[y][x].spriteRow = asteroidRowDist(gen);
            grid[y][x].spriteCol = asteroidColDist(gen);
        } else if (currentTheme == GameTheme::Jungle) {
            grid[y][x].logSpriteRow = logRowDist(gen);
            grid[y][x].logSpriteCol = logColDist(gen);
        } else if (currentTheme == GameTheme::Desert) {
            grid[y][x].desertObstacleType = obstacleDist(gen) == 0 ? 
                DesertObstacleType::Cactus : DesertObstacleType::Tumbleweed;
        }

        // Check if maze is still solvable
        if (PathExists(0, 0, exitX, exitY)) {
            obstacleCount++;
        } else {
            // If not solvable, revert
            grid[y][x].isObstacle = false;
        }
    }
}

// Initialize the maze grid
void InitializeGrid() {
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            grid[y][x] = Cell(x, y);
        }
    }
}

// Check if a cell is within bounds
bool IsInBounds(int x, int y) {
    return x >= 0 && x < cols && y >= 0 && y < rows;
}

// Get a random unvisited neighbor
Cell* GetUnvisitedNeighbor(Cell* current) {
    std::vector<int> neighbors;

    for (int i = 0; i < 4; i++) {
        int nx = current->x + directions[i][0];
        int ny = current->y + directions[i][1];

        if (IsInBounds(nx, ny) && !grid[ny][nx].visited) {
            neighbors.push_back(i);
        }
    }

    if (!neighbors.empty()) {
        int randIndex = neighbors[std::rand() % neighbors.size()];
        return &grid[current->y + directions[randIndex][1]][current->x + directions[randIndex][0]];
    }
    return nullptr;
}

// Remove walls between two cells
void RemoveWalls(Cell* current, Cell* next) {
    int dx = next->x - current->x;
    int dy = next->y - current->y;

    if (dx == 1) { // Next is to the right
        current->walls[1] = false;
        next->walls[3] = false;
    } else if (dx == -1) { // Next is to the left
        current->walls[3] = false;
        next->walls[1] = false;
    } else if (dy == 1) { // Next is below
        current->walls[2] = false;
        next->walls[0] = false;
    } else if (dy == -1) { // Next is above
        current->walls[0] = false;
        next->walls[2] = false;
    }
}

// Parallelized Maze Generation using OpenMP
void GenerateMaze() {
    #pragma omp parallel for
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            grid[y][x] = Cell(x, y);
        }
    }

    Cell* current = &grid[0][0];
    current->visited = true;
    stack.push(current);

    while (!stack.empty()) {
        current = stack.top();
        Cell* next = GetUnvisitedNeighbor(current);

        if (next) {
            next->visited = true;
            RemoveWalls(current, next);
            stack.push(next);
        } else {
            stack.pop();
        }
    }
}

// Draw the border around the maze
void DrawBorder() {
    int borderThickness = 5; // Thickness of the border
    Color borderColor = currentColors.wallColor;

    // Top border
    DrawRectangle(margin - borderThickness, margin - borderThickness, mazeWidth + 2 * borderThickness, borderThickness, borderColor);

    // Bottom border
    DrawRectangle(margin - borderThickness, margin + mazeHeight, mazeWidth + 2 * borderThickness, borderThickness, borderColor);

    // Left border
    DrawRectangle(margin - borderThickness, margin - borderThickness, borderThickness, mazeHeight + 2 * borderThickness, borderColor);

    // Right border
    DrawRectangle(margin + mazeWidth, margin - borderThickness, borderThickness, mazeHeight + 2 * borderThickness, borderColor);
}

// Player position and rotation
int playerX = 0, playerY = 0;
float playerTargetX = 0.0f;
float playerTargetY = 0.0f;
float playerPosX = 0.0f;
float playerPosY = 0.0f;
const float PLAYER_SPEED = 8.0f;  // Cells per second
bool isMoving = false;
float playerRotation = 0.0f;         // Current rotation angle
float targetRotation = 0.0f;         // Target rotation angle
const float ROTATION_SPEED = 10.0f;   // Rotation speed multiplier

int exitX, exitY;  // Exit position

// Track game start time and game state
float startTime = 0.0f;
bool gameWon = false;

// Check if the player can move in a given direction
bool CanMove(int direction) {
    Cell* current = &grid[playerY][playerX];
    int newX = playerX, newY = playerY;

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
           !grid[newY][newX].isObstacle;
}

// Linear interpolation function
float Lerp(float start, float end, float amount) {
    return start + amount * (end - start);
}

// Update player position based on input
void MovePlayer() {
    if (isMoving) {
        float currentSpeed = PLAYER_SPEED;
        if (isSpeedBoosted) {
            currentSpeed *= SPEED_BOOST_MULTIPLIER;
        }

        // Update current position towards target
        float moveAmount = currentSpeed * GetFrameTime();
        playerPosX = Lerp(playerPosX, playerTargetX, moveAmount);
        playerPosY = Lerp(playerPosY, playerTargetY, moveAmount);

        // Check if we've essentially reached the target
        if (fabs(playerPosX - playerTargetX) < 0.01f && 
            fabs(playerPosY - playerTargetY) < 0.01f) {
            playerPosX = playerTargetX;
            playerPosY = playerTargetY;
            isMoving = false;
        }
    }

    // Check for new movement regardless of current movement
    int newX = playerX;
    int newY = playerY;
    bool shouldMove = false;

    if ((IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) && playerY > 0) {
        if (!grid[playerY][playerX].walls[0] && grid[playerY-1][playerX].isObstacle) {
            PlaySound(GameResources::blockedSound);
        }
        else if (CanMove(0)) {
            newY--;
            shouldMove = true;
        }
    }
    else if ((IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) && playerX < cols - 1) {
        if (!grid[playerY][playerX].walls[1] && grid[playerY][playerX+1].isObstacle) {
            PlaySound(GameResources::blockedSound);
        }
        else if (CanMove(1)) {
            newX++;
            shouldMove = true;
        }
    }
    else if ((IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) && playerY < rows - 1) {
        if (!grid[playerY][playerX].walls[2] && grid[playerY+1][playerX].isObstacle) {
            PlaySound(GameResources::blockedSound);
        }
        else if (CanMove(2)) {
            newY++;
            shouldMove = true;
        }
    }
    else if ((IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) && playerX > 0) {
        if (!grid[playerY][playerX].walls[3] && grid[playerY][playerX-1].isObstacle) {
            PlaySound(GameResources::blockedSound);
        }
        else if (CanMove(3)) {
            newX--;
            shouldMove = true;
        }
    }

    if (shouldMove && (!isMoving || (fabs(playerPosX - playerTargetX) < 0.5f && 
                                   fabs(playerPosY - playerTargetY) < 0.5f))) {
        playerTargetX = static_cast<float>(newX);
        playerTargetY = static_cast<float>(newY);
        playerX = newX;
        playerY = newY;
        isMoving = true;
    }
}

void UpdatePlayerRotation() {
    float diff{};
    switch (currentTheme) {
        case GameTheme::Space:
            // Space theme - rotation for all directions
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) targetRotation = -90.0f;
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) targetRotation = 0.0f;
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) targetRotation = 90.0f;
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) targetRotation = 180.0f;
            
            // Smooth rotation
            diff = targetRotation - playerRotation;
            if (diff > 180.0f) diff -= 360.0f;
            if (diff < -180.0f) diff += 360.0f;
            playerRotation += diff * ROTATION_SPEED * GetFrameTime();
            
            break;

        case GameTheme::Jungle:
        case GameTheme::Desert: 
            // Jungle/Desert themes - no rotation (flip while drawing player)
            playerRotation = 0.0f;
            break;
    }
}

class PortalEffect {
private:
    std::vector<Particle> particles;  // Pre-allocated pool of particles
    float spawnTimer;
    Vector2 center;
    float radius;
    std::mt19937 rng;

public:
    PortalEffect(int maxParticles = MAX_PORTAL_PARTICLES) 
        : particles(maxParticles), spawnTimer(0), rng(std::random_device{}()) {}

    void SetPosition(Vector2 pos, float r) {
        center = pos;
        radius = r;
    }

    void Update(float deltaTime) {
        spawnTimer -= deltaTime;

        // Reuse inactive particles when spawning
        if (spawnTimer <= 0) {
            SpawnParticle();
            spawnTimer = PARTICLE_SPAWN_RATE;
        }

        // Update all active particles
        for (auto& p : particles) {
            if (!p.active) continue;

            p.lifetime -= deltaTime;
            if (p.lifetime <= 0) {
                p.active = false;  // Mark particle as inactive
                continue;
            }

            p.position.x += p.velocity.x * deltaTime;
            p.position.y += p.velocity.y * deltaTime;

            float alpha = p.lifetime / PARTICLE_MAX_LIFETIME;
            p.color.a = static_cast<unsigned char>(255.0f * alpha);
        }
    }

    void Draw() {
        for (const auto& p : particles) {
            if (p.active) {
                DrawCircleV(p.position, 2, p.color);  // Draw only active particles
            }
        }
    }

private:
    void SpawnParticle() {
        // Find an inactive particle from the pool
        for (auto& p : particles) {
            if (p.active) continue;  // Skip active particles

            // Random angle around circle
            std::uniform_real_distribution<float> angleDist(0, 2 * PI);
            float angle = angleDist(rng);

            // Random distance from center within radius
            std::uniform_real_distribution<float> radiusDist(0, radius);
            float distance = radiusDist(rng);

            // Set particle properties
            p.position.x = center.x + cosf(angle) * distance;
            p.position.y = center.y + sinf(angle) * distance;
            p.velocity.x = cosf(angle) * PARTICLE_SPEED;
            p.velocity.y = sinf(angle) * PARTICLE_SPEED;
            p.color = ColorAlpha(currentColors.goalColor, 1.0f);
            p.lifetime = PARTICLE_MAX_LIFETIME;
            p.active = true;
            break;  // Spawn only one particle at a time
        }
    }
};

// Maintain a list of all valid cells for powerup placement
void PrecomputeValidCells(std::vector<std::pair<int, int>>& validCells) {
    validCells.clear();
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            if (!grid[y][x].isObstacle && !(x == 0 && y == 0) && !(x == exitX && y == exitY)) {
                validCells.emplace_back(x, y);
            }
        }
    }
}

void SpawnPowerups() {
    std::vector<std::pair<int, int>> validCells;
    PrecomputeValidCells(validCells);

    if (currentMode == Timed) {
        // Spawn time power-ups
        for (int i = 0; i < MAX_TIME_POWERUPS && !validCells.empty(); i++) {
            int index = std::rand() % validCells.size();
            powerups[i].x = validCells[index].first;
            powerups[i].y = validCells[index].second;
            powerups[i].active = true;
            validCells.erase(validCells.begin() + index);
        }      
    }

    // Spawn speed boosts if applicable
    DifficultySettings settings = DifficultySettings::Get(currentDifficulty);
    if (settings.hasSpeedBoosts) {
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
    for (auto& powerup : powerups) {
        if (powerup.active) {
            // Rotate powerup
            powerup.rotationAngle += 90.0f * deltaTime;
            
            // Check if player collected
            if (playerX == powerup.x && playerY == powerup.y) {
                PlaySound(GameResources::bonusSound);
                timeLimit += TIME_BONUS_AMOUNT; // Add 10 seconds
                powerup.active = false;

                // Trigger feedback
                showTimeBonusFeedback = true;
                timeBonusFeedbackTimer = TIME_BONUS_FEEDBACK_DURATION;
            }
        }
    }

    // Update feedback timer
    if (showTimeBonusFeedback) {
        timeBonusFeedbackTimer -= deltaTime;
        if (timeBonusFeedbackTimer <= 0) {
            showTimeBonusFeedback = false;
        }
    }
}

void DrawTimePowerups() {
    for (const auto& powerup : powerups) {
        if (powerup.active) {
            // Calculate position
            float x = margin + powerup.x * cellSize + cellSize/2.0f;
            float y = margin + powerup.y * cellSize + cellSize/2.0f;
            
            // Draw with rotation
            DrawTexturePro(
                GameResources::timeBonusTexture,
                Rectangle{ 0, 0, (float)GameResources::timeBonusTexture.width, (float)GameResources::timeBonusTexture.height },
                Rectangle{ x, y, cellSize * 0.6f, cellSize * 0.6f },
                Vector2{ cellSize * 0.3f, cellSize * 0.3f },
                powerup.rotationAngle,
                WHITE
            );
        }
    }
}

void UpdateSpeedBoosts(float deltaTime) {
    // Update active speed boost duration
    if (isSpeedBoosted) {
        speedBoostTimeLeft -= deltaTime;
        if (speedBoostTimeLeft <= 0) {
            isSpeedBoosted = false;
        }
    }

    // Update and rotate speed boost pickups
    for (auto& boost : speedBoosts) {
        if (boost.active) {
            // Check if player collected
            if (playerX == boost.x && playerY == boost.y) {
                PlaySound(GameResources::bonusSound);
                speedBoostTimeLeft = SPEED_BOOST_DURATION;
                isSpeedBoosted = true;
                boost.active = false;
            }
        }
    }
}

// Add new function to draw speed boosts
void DrawSpeedBoosts() {
    for (const auto& boost : speedBoosts) {
        if (boost.active) {
            float x = margin + boost.x * cellSize + cellSize/2.0f;
            float y = margin + boost.y * cellSize + cellSize/2.0f;
            
            DrawTexturePro(
                GameResources::speedBoostTexture,
                Rectangle{ 0, 0, (float)GameResources::speedBoostTexture.width, (float)GameResources::speedBoostTexture.height },
                Rectangle{ x, y, cellSize * 0.6f, cellSize * 0.6f },
                Vector2{ cellSize * 0.3f, cellSize * 0.3f },
                0,
                WHITE
            );
        }
    }
}

// Randomly place the goal
void PlaceRandomExit() {
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Keep exit away from start position (minimum distance)
    const int MIN_DISTANCE = std::max(cols, rows) / 2;
    
    while (true) {
        // Generate random position
        exitX = std::uniform_int_distribution<>(0, cols-1)(gen);
        exitY = std::uniform_int_distribution<>(0, rows-1)(gen);
        
        // Calculate distance from start
        int distance = std::abs(exitX - 0) + std::abs(exitY - 0);
        
        // Check if position is valid
        if (distance >= MIN_DISTANCE && !grid[exitY][exitX].isObstacle) {
            // Verify path exists from start to this exit
            if (PathExists(0, 0, exitX, exitY)) {
                return;
            }
        }
    }
}

// Draw exit as a wormhole portal with particle effect
void DrawExit(int exitX, int exitY, float time) {
    static PortalEffect portalEffect;
    
    int exitPosX = margin + exitX * cellSize + cellSize/2;
    int exitPosY = margin + exitY * cellSize + cellSize/2;
    float radius = cellSize/2;

    float scale{};

    // Draw theme-specific goal
    switch(currentTheme) {
        case GameTheme::Space:
            // Update portal effect position
            portalEffect.SetPosition({static_cast<float>(exitPosX), static_cast<float>(exitPosY)}, radius);
            
            // Update and draw particles
            portalEffect.Update(GetFrameTime());

            // Draw space portal with special effects
            scale = 1.0f + 0.2f * sin(time * 5.0f);
            for(int i = 3; i > 0; i--) {
                DrawCircle(exitPosX, exitPosY, radius * scale * i/3, 
                          ColorAlpha(currentColors.goalColor, (1.0f - (float)i/4.0f)));
            }
            portalEffect.Draw();
            break;
            
        case GameTheme::Jungle:
            // Rotating bananas
            DrawTexturePro(
                GetGoalTextureForTheme(currentTheme),
                Rectangle{ 0, 0, 
                          (float)GetGoalTextureForTheme(currentTheme).width,
                          (float)GetGoalTextureForTheme(currentTheme).height },
                Rectangle{ 
                    static_cast<float>(exitPosX),
                    static_cast<float>(exitPosY),
                    cellSize * 0.8f,
                    cellSize * 0.8f
                },
                Vector2{ cellSize * 0.4f, cellSize * 0.4f },
                time * 45.0f,  // Rotate bananas
                WHITE
            );
            break;

        case GameTheme::Desert:
            // Draw shadow underneath
            DrawCircleGradient(
                exitPosX,
                exitPosY + cellSize * 0.1f,
                cellSize * 0.7f,
                ColorAlpha(BLACK, 0.3f),
                ColorAlpha(BLACK, 0.0f)
            );

            // Draw glow effect
            for(int i = 3; i > 0; i--) {
                DrawCircle(exitPosX, exitPosY, 
                          cellSize * 1.2f * 0.6f * i/3,
                          ColorAlpha(SKYBLUE, 0.1f * (1.0f - (float)i/4.0f)));
            }

            // Draw oasis with slight vertical offset for 3D effect
            DrawTexturePro(
                GetGoalTextureForTheme(currentTheme),
                Rectangle{ 0, 0, 
                          (float)GetGoalTextureForTheme(currentTheme).width,
                          (float)GetGoalTextureForTheme(currentTheme).height },
                Rectangle{ 
                    static_cast<float>(exitPosX),
                    static_cast<float>(exitPosY - cellSize * 0.1f), // Slight vertical offset
                    cellSize * 1.2f,
                    cellSize * 1.2f
                },
                Vector2{ cellSize * 1.2f/2, cellSize * 1.2f/2 },
                0.0f,
                WHITE
            );

            // Add highlight/reflection effect
            DrawCircleGradient(
                exitPosX,
                exitPosY,
                cellSize * 0.4f,
                ColorAlpha(SKYBLUE, 0.2f),
                ColorAlpha(SKYBLUE, 0.0f)
            );
            break;
        default:
            DrawCircle(exitPosX, exitPosY, radius, currentColors.goalColor);
            break;
    }
}

void ResetGame()
{
    // Re-initialize grid with new cell size and related properties
    grid.clear();
    grid.resize(rows, std::vector<Cell>(cols, Cell(0, 0)));
    
    // Reinitialize maze
    InitializeGrid();
    GenerateMaze();
    PlaceRandomExit();
    AddRandomObstacles(exitX, exitY);

    // Reset speed boost variables
    isSpeedBoosted = false;
    speedBoostTimeLeft = 0.0f;
    
    // Reset time limit and respawn powerups
    if (currentMode == Timed) {
        timeLimit = 120.f;
        powerups.clear();
        powerups.resize(MAX_TIME_POWERUPS);
    }

    SpawnPowerups();
    
    startTime = GetTime();
    
    // Reset player and target position and rotation
    playerX = playerY = 0;
    playerPosX = playerPosY = 0;
    playerTargetX = playerTargetY = 0;
    playerRotation = 0;
    targetRotation = 0;

    isMoving = false;
    gameWon = false;
}

void InitializeGameWithDifficulty() {
    DifficultySettings settings = DifficultySettings::Get(currentDifficulty);
    
    // Update game settings
    cellSize = settings.cellSize;
    TIME_BONUS_AMOUNT = settings.timeBonusAmount;

    // Recalculate maze dimensions based on new cell size
    mazeWidth = screenWidth - 2 * margin;
    mazeHeight = screenHeight - 2 * margin;
    cols = mazeWidth / cellSize;
    rows = mazeHeight / cellSize;

    // Resize grid with new dimensions
    ResetGame();
}

void LoadHighScores() {
    std::ifstream file(SCORES_FILE, std::ios::binary);
    if (!file) return;

    highScores.clear();
    Score score;
    while (file.read(reinterpret_cast<char*>(&score), sizeof(Score))) {
        highScores.push_back(score);
    }
    file.close();
}

void SaveHighScores() {
    std::ofstream file(SCORES_FILE, std::ios::binary);
    if (!file) return;

    for (const auto& score : highScores) {
        file.write(reinterpret_cast<const char*>(&score), sizeof(Score));
    }
    file.close();
}

void AddHighScore(float time, bool wasTimed) {
    // Don't add if game wasn't won
    if (!gameWon) return;
    
    // Create new score with current UTC timestamp
    Score newScore(time, wasTimed, currentDifficulty, std::time(nullptr));
    
    // Insert score in sorted order by completion time
    auto it = std::lower_bound(highScores.begin(), highScores.end(), newScore,
        [](const Score& a, const Score& b) { return a.time < b.time; });
    
    highScores.insert(it, newScore);
    
    // Keep only top scores
    if (highScores.size() > MAX_HIGH_SCORES) {
        highScores.resize(MAX_HIGH_SCORES);
    }
    
    SaveHighScores();
}

void DrawDifficultyMenu() {
    DrawText("Select Difficulty", screenWidth/2 - MeasureText("Select Difficulty", 20)/2, screenHeight/2 - 80, 20, ORANGE);
    DrawText("1. Easy", screenWidth/2 - MeasureText("1. Easy", 20)/2, screenHeight/2 - 40, 20, GREEN);
    DrawText("2. Medium", screenWidth/2 - MeasureText("2. Medium", 20)/2, screenHeight/2, 20, YELLOW);
    DrawText("3. Hard", screenWidth/2 - MeasureText("3. Hard", 20)/2, screenHeight/2 + 40, 20, RED);
}

void DrawModeMenu() {
    // Game mode options
    DrawText("Select Game Mode", screenWidth / 2 - MeasureText("Select Game Mode", 20) / 2, screenHeight / 2 - 60, 20, ORANGE);
    DrawText("1. Timed", screenWidth / 2 - MeasureText("1. Timed", 20) / 2, screenHeight / 2 - 20, 20, ORANGE);
    DrawText("2. Untimed", screenWidth / 2 - MeasureText("2. Untimed", 20) / 2, screenHeight / 2 + 20, 20, ORANGE);
    DrawText("3. Change Difficulty", screenWidth / 2 - MeasureText("3. Change Difficulty", 20) / 2, screenHeight / 2 + 60, 20, ORANGE);
    DrawText("4. Quit", screenWidth / 2 - MeasureText("4. Quit", 20) / 2, screenHeight / 2 + 100, 20, RED);

    // Gameplay instructions
    DrawText("Game Controls:", screenWidth / 2 - MeasureText("Game Controls:", 20) / 2, screenHeight / 2 + 140, 20, LIGHTGRAY);
    DrawText("Arrow Keys/WASD - Move", screenWidth / 2 - MeasureText("Arrow Keys/WASD - Move", 20) / 2, screenHeight / 2 + 170, 20, LIGHTGRAY);
    DrawText("P - Pause Game", screenWidth / 2 - MeasureText("P - Pause Game", 20) / 2, screenHeight / 2 + 200, 20, LIGHTGRAY);
}

void DrawEndMenu(bool gameWon, float completionTime) {
    const int baseY = screenHeight / 4;

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

        DrawText("You Win!", screenWidth / 2 - MeasureText("You Win!", 50) / 2, baseY - 50, 50, DARKGREEN);
        DrawText(timeStr, screenWidth / 2 - MeasureText(timeStr, 20) / 1.3, baseY, 30, DARKGREEN);
    } else {
        DrawText("You Lost", screenWidth / 2 - MeasureText("You Lost", 40) / 2.2, baseY - 50, 40, RED);
        DrawText("Better Luck Next Time", screenWidth / 2 - MeasureText("Better Luck Next Time", 20) / 1.3, baseY, 30, RED);
    }

    // Draw high scores table
    const int scoreStartY = baseY + 80;
    DrawText("HIGH SCORES", 
            screenWidth/2 - MeasureText("HIGH SCORES", 25)/2,
            scoreStartY, 25, {0, 255, 255, 255});
    
    const int lineHeight = 25;
    for (size_t i = 0; i < highScores.size(); i++) {
        Color textColor = WHITE;
        // Highlight new score
        if (gameWon && highScores[i].time == completionTime && 
            highScores[i].wasTimed == (currentMode == Timed) &&
            highScores[i].difficulty == currentDifficulty) {
            textColor = YELLOW;
        }
        
        std::string scoreText = highScores[i].ToString();
        DrawText(scoreText.c_str(),
                screenWidth/2 - MeasureText(scoreText.c_str(), 20)/2,
                scoreStartY + 40 + (i * lineHeight), 20, textColor);
    }
    
    // Draw end menu options at bottom
    const int optionsY = scoreStartY + 300;
    DrawText("1. Play Again", screenWidth / 2 - MeasureText("1. Play Again", 20) / 2, optionsY, 20, ORANGE);
    DrawText("2. Change Game Mode", screenWidth / 2 - MeasureText("2. Change Game Mode", 20) / 2, optionsY + 30, 20, ORANGE);
    DrawText("3. Quit", screenWidth / 2 - MeasureText("3. Quit", 20) / 2, optionsY + 60, 20, RED);
}

void HandleMenuInput() {
    if (menuState == DifficultySelect) {
        if (IsKeyPressed(KEY_ONE)) {
            PlaySound(GameResources::menuSound);
            currentDifficulty = Easy;
            menuState = ModeSelect;
        } else if (IsKeyPressed(KEY_TWO)) {
            PlaySound(GameResources::menuSound);
            currentDifficulty = Medium;
            menuState = ModeSelect;
        } else if (IsKeyPressed(KEY_THREE)) {
            PlaySound(GameResources::menuSound);
            currentDifficulty = Hard;
            menuState = ModeSelect;
        }
    } else if (menuState == ModeSelect) {
        if (IsKeyPressed(KEY_ONE)) {
            PlaySound(GameResources::startSound);
            currentMode = Timed;
            currentState = Playing;
            InitializeGameWithDifficulty();
        } else if (IsKeyPressed(KEY_TWO)) {
            PlaySound(GameResources::startSound);
            currentMode = Untimed;
            currentState = Playing;
            InitializeGameWithDifficulty();
        } else if (IsKeyPressed(KEY_THREE)) {
            PlaySound(GameResources::menuSound);
            menuState = DifficultySelect;
        } else if (IsKeyPressed(KEY_FOUR)) {
            PlaySound(GameResources::menuSound);
            GameResources::UnloadAllResources();
            CloseWindow();
            exit(0);
        }
    }
}

void HandleEndMenuInput() {
    if (IsKeyPressed(KEY_ONE)) {
        RandomizeTheme();
        PlaySound(GameResources::startSound);
        ResetGame();
        currentState = Playing;
    } else if (IsKeyPressed(KEY_TWO)) {
        RandomizeTheme();
        PlaySound(GameResources::menuSound);
        currentState = StartMenu;
    } else if (IsKeyPressed(KEY_THREE)) {
        PlaySound(GameResources::menuSound);
        GameResources::UnloadAllResources();
        CloseWindow();
        exit(0);
    }
}

void UpdateSoundSettings() {
    switch(currentSoundSettings) {
        case SoundSettings::AllOn:
            switch(currentTheme) {
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
            SetSoundVolume(GameResources::portalSound, 1.0f);
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
            SetSoundVolume(GameResources::portalSound, 1.0f);
            SetSoundVolume(GameResources::bonusSound, 1.0f);
            break;
        case SoundSettings::SoundOff:
            switch(currentTheme) {
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
            SetSoundVolume(GameResources::portalSound, 0.0f);
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
            SetSoundVolume(GameResources::portalSound, 0.0f);
            SetSoundVolume(GameResources::bonusSound, 0.0f);
            break;
    }
}

void DrawPauseMenu() {
    // Semi-transparent background
    DrawRectangle(0, 0, screenWidth, screenHeight, ColorAlpha(BLACK, 0.7f));
    
    // Draw settings menu
    DrawText("GAME PAUSED", screenWidth/2 - MeasureText("GAME PAUSED", 40)/2, screenHeight/2 - 150, 40, WHITE);
    
    // Sound settings
    DrawText("Sound Settings:", screenWidth/2 - MeasureText("Sound Settings:", 20)/2, screenHeight/2 - 50, 20, WHITE);
    DrawText("1. All Sounds ON", screenWidth/2 - MeasureText("1. All Sounds ON", 20)/2, screenHeight/2, 20, 
             currentSoundSettings == SoundSettings::AllOn ? GREEN : GRAY);
    DrawText("2. Music OFF", screenWidth/2 - MeasureText("2. Music OFF", 20)/2, screenHeight/2 + 30, 20,
             currentSoundSettings == SoundSettings::MusicOff ? GREEN : GRAY);
    DrawText("3. Sound Effects OFF", screenWidth/2 - MeasureText("3. Sound Effects OFF", 20)/2, screenHeight/2 + 60, 20,
             currentSoundSettings == SoundSettings::SoundOff ? GREEN : GRAY);
    DrawText("4. All Sounds OFF", screenWidth/2 - MeasureText("4. All Sounds OFF", 20)/2, screenHeight/2 + 90, 20,
             currentSoundSettings == SoundSettings::AllOff ? GREEN : GRAY);
    
    DrawText("P - Resume Game", screenWidth/2 - MeasureText("P - Resume Game", 20)/2, screenHeight/2 + 150, 20, LIGHTGRAY);
}


int main() {
    InitWindow(screenWidth, screenHeight, "PortalPaths: Maze Multiverse");
    LoadHighScores();
    mazeRenderTexture = LoadRenderTexture(screenWidth, screenHeight);
    InitAudioDevice();
    GameResources::LoadGeneralSounds();
    GameResources::LoadPowerupResources();
    SetTargetFPS(60);

    // Randomize theme at startup
    RandomizeTheme();

    // Seed random number generator
    std::srand(std::time(nullptr));

    ResetGame();

    if (currentTheme == GameTheme::Space) {
        starFieldTexture = CreateStarFieldTexture(screenWidth, screenHeight, 200);  // Precompute starfield
    }

    float timeElapsed = 0.0f;

    while (!WindowShouldClose()) {
        if (currentState == StartMenu || currentState == Playing) {
            switch(currentTheme) {
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

        BeginDrawing();
        ClearBackground(currentColors.background);

        // Draw theme-specific background effects
        switch(currentTheme) {
            case GameTheme::Space:
                // Draw the precomputed starfield texture
                DrawTexture(starFieldTexture, 0, 0, WHITE);
                break;

            case GameTheme::Jungle:            
            case GameTheme::Desert:
                break;
        }

        switch (currentState) {
            case StartMenu:
                if (menuState == DifficultySelect) {
                    DrawDifficultyMenu();
                } else {
                    DrawModeMenu();
                }
                HandleMenuInput();
                break;
            case Playing:
                if (IsKeyPressed(KEY_P)) {
                    if (!isPaused) {
                        pauseStartTime = GetTime();  // Store when we paused
                    } else {
                        totalPausedTime += GetTime() - pauseStartTime;  // Add paused duration
                    }
                    isPaused = !isPaused;
                }
                
                if (!isPaused) {
                    if (!gameWon) {
                        MovePlayer();
                        UpdatePlayerRotation();
                        timeElapsed = GetTime() - startTime - totalPausedTime;  // Update elapsed time

                        if (currentMode == Timed) {
                            UpdateTimePowerups(GetFrameTime());
                            DrawTimePowerups();
                            float timeLeft = timeLimit - timeElapsed;

                            // Draw time with bonus feedback
                            if (showTimeBonusFeedback) {
                                const char* timeText = TextFormat("Time Left: %.2f", timeLeft);
                                DrawText(timeText, 10, 8, 20, WHITE);

                                int timeWidth = MeasureText(timeText, 20);
                                DrawText(TextFormat(" + %.1f", TIME_BONUS_AMOUNT), 10 + timeWidth, 8, 20, GREEN);
                            } else {
                                DrawText(TextFormat("Time Left: %.2f", timeLeft), 
                                        10, 8, 20, WHITE);
                            }
                        } else {
                            DrawText(TextFormat("Time: %.2f", timeElapsed), 10, 8, 20, WHITE);  // Draw timer
                        }
                        
                        // Check if the player has reached the exit
                        if (playerX == exitX && playerY == exitY) {
                            PlaySound(GameResources::winSound);
                            StopCurrentThemeMusic();
                            gameWon = true;
                            AddHighScore(timeElapsed, currentMode == Timed);
                            currentState = EndMenu;
                        }

                        // Check if time limit is exceeded in timed mode
                        if (currentMode == Timed && timeElapsed >= timeLimit) {
                            PlaySound(GameResources::loseSound);
                            StopCurrentThemeMusic();
                            gameWon = false;
                            currentState = EndMenu;
                        }

                        UpdateSpeedBoosts(GetFrameTime());
                        DrawSpeedBoosts();
                        
                        // Draw speed boost indicator if active
                        if (isSpeedBoosted) {
                            DrawText(TextFormat("Speed Boost: %.1fs", speedBoostTimeLeft), 
                                    900, 8, 20, YELLOW);
                        }
                    }
                } else {
                    // Handle pause menu input
                    if (IsKeyPressed(KEY_ONE)) {
                        currentSoundSettings = SoundSettings::AllOn;
                        UpdateSoundSettings();
                    } else if (IsKeyPressed(KEY_TWO)) {
                        currentSoundSettings = SoundSettings::MusicOff;
                        UpdateSoundSettings();
                    } else if (IsKeyPressed(KEY_THREE)) {
                        currentSoundSettings = SoundSettings::SoundOff;
                        UpdateSoundSettings();
                    } else if (IsKeyPressed(KEY_FOUR)) {
                        currentSoundSettings = SoundSettings::AllOff;
                        UpdateSoundSettings();
                    }
                }

                if (!isPaused) {
                    // Draw the maze
                    for (int y = 0; y < rows; y++) {
                        for (int x = 0; x < cols; x++) {
                            grid[y][x].Draw();
                        }
                    }

                    if (currentTheme == GameTheme::Space) {
                        // Space theme uses rotation
                        DrawTexturePro(
                            GetPlayerTextureForTheme(currentTheme),
                            Rectangle{ 0, 0,
                                (float)GetPlayerTextureForTheme(currentTheme).width,
                                (float)GetPlayerTextureForTheme(currentTheme).height
                            },
                            Rectangle{ 
                                static_cast<float>(margin) + playerPosX * cellSize + cellSize/2,
                                static_cast<float>(margin) + playerPosY * cellSize + cellSize/2,
                                cellSize * 0.8f,
                                cellSize * 0.8f
                            },
                            Vector2{ cellSize * 0.4f, cellSize * 0.4f },
                            playerRotation,
                            currentColors.playerTint
                        );
                    } else {
                        // Jungle and Desert themes use horizontal flipping
                        DrawTexturePro(
                            GetPlayerTextureForTheme(currentTheme),
                            Rectangle{ 0, 0,
                                (float)GetPlayerTextureForTheme(currentTheme).width * (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A) ? -1.0f : 1.0f),  // Flip horizontally for left movement
                                (float)GetPlayerTextureForTheme(currentTheme).height
                            },
                            Rectangle{ 
                                static_cast<float>(margin) + playerPosX * cellSize + cellSize/2,
                                static_cast<float>(margin) + playerPosY * cellSize + cellSize/2,
                                cellSize * 0.8f,
                                cellSize * 0.8f
                            },
                            Vector2{ cellSize * 0.4f, cellSize * 0.4f },  // Origin at center
                            playerRotation,
                            currentColors.playerTint
                        );
                    }

                    // Draw the exit wormhole
                    DrawExit(exitX, exitY, timeElapsed);

                    // Draw the border
                    DrawBorder();

                    // Draw pause hint in top-right corner
                    const char* pauseHint = "P - Pause/Settings";
                    DrawText(pauseHint, 
                            screenWidth - MeasureText(pauseHint, 20) - 10,
                            8, 20, LIGHTGRAY);
                } else {
                    // Draw blurred background (existing game state)
                    DrawRectangle(0, 0, screenWidth, screenHeight, ColorAlpha(BLACK, 0.5f));
                    DrawPauseMenu();
                }
                break;
            case EndMenu:
                DrawEndMenu(gameWon, timeElapsed);
                HandleEndMenuInput();
                break;
        }

        EndDrawing();
    }

    // Unload sounds
    GameResources::UnloadAllResources();
    CloseAudioDevice();
    UnloadRenderTexture(mazeRenderTexture);
    UnloadTexture(starFieldTexture);

    // Close Raylib
    CloseWindow();
}