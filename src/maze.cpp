#include "maze.h"
#include "gameplay.h"
#include <random>
#include <queue>

Cell::Cell(int x_, int y_) : x(x_), y(y_), visited(false), isObstacle(false),
    spriteRow(0), spriteCol(0), logSpriteRow(0), logSpriteCol(0),
    desertObstacleType(DesertObstacleType::Cactus) {
    for (int i = 0; i < 4; i++) walls[i] = true;
}

void Cell::Draw() const {
    int screenX = GAME_STATE.getMargin() + x * GAME_STATE.getCellSize();
    int screenY = GAME_STATE.getMargin() + y * GAME_STATE.getCellSize();
    
    // Draw walls
    if (walls[0]) DrawLine(screenX, screenY, screenX + GAME_STATE.getCellSize(), screenY, GAME_STATE.getCurrentColors().wallColor);
    if (walls[1]) DrawLine(screenX + GAME_STATE.getCellSize(), screenY, screenX + GAME_STATE.getCellSize(), screenY + GAME_STATE.getCellSize(), GAME_STATE.getCurrentColors().wallColor);
    if (walls[2]) DrawLine(screenX, screenY + GAME_STATE.getCellSize(), screenX + GAME_STATE.getCellSize(), screenY + GAME_STATE.getCellSize(), GAME_STATE.getCurrentColors().wallColor);
    if (walls[3]) DrawLine(screenX, screenY, screenX, screenY + GAME_STATE.getCellSize(), GAME_STATE.getCurrentColors().wallColor);
    
    // Draw obstacles based on current theme
    if (isObstacle) {
        float spriteWidth{};
        float spriteHeight{};

        switch (GAME_STATE.getCurrentTheme()) {
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
                        static_cast<float>(screenX) + GAME_STATE.getCellSize()/2,
                        static_cast<float>(screenY) + GAME_STATE.getCellSize()/2,
                        GAME_STATE.getCellSize() * 0.8f,
                        GAME_STATE.getCellSize() * 0.8f
                    },
                    Vector2{ GAME_STATE.getCellSize() * 0.4f, GAME_STATE.getCellSize() * 0.4f },
                    0.0f,
                    GAME_STATE.getCurrentColors().obstacleColor
                );
                break;

            case GameTheme::Jungle:
                // Draw log sprite
                spriteWidth = GameResources::logsSpritesheet.width / 3.0f;
                spriteHeight = GameResources::logsSpritesheet.height / 3.0f;

                DrawTexturePro(
                    GameResources::logsSpritesheet,
                    Rectangle{logSpriteCol * spriteWidth, logSpriteRow * spriteHeight, spriteWidth, spriteHeight},
                    Rectangle{static_cast<float>(screenX) + GAME_STATE.getCellSize()/2,
                            static_cast<float>(screenY) + GAME_STATE.getCellSize()/2,
                            GAME_STATE.getCellSize() * 0.8f,
                            GAME_STATE.getCellSize() * 0.8f},
                    Vector2{GAME_STATE.getCellSize() * 0.4f, GAME_STATE.getCellSize() * 0.4f},
                    0.0f,
                    GAME_STATE.getCurrentColors().obstacleColor
                );
                break;

            case GameTheme::Desert:
                if (desertObstacleType == DesertObstacleType::Cactus) {
                    // Draw cactus
                    DrawTexturePro(
                        GameResources::cactusTexture,
                        Rectangle{0, 0, (float)GameResources::cactusTexture.width,
                                (float)GameResources::cactusTexture.height},
                        Rectangle{static_cast<float>(screenX) + GAME_STATE.getCellSize()/2,
                                static_cast<float>(screenY) + GAME_STATE.getCellSize()/2,
                                GAME_STATE.getCellSize() * 0.8f,
                                GAME_STATE.getCellSize() * 0.8f},
                        Vector2{GAME_STATE.getCellSize() * 0.4f, GAME_STATE.getCellSize() * 0.4f},
                        0.0f,
                        GAME_STATE.getCurrentColors().obstacleColor
                    );
                } else {
                    // Draw tumbleweed with darker brown color
                    DrawTexturePro(
                        GameResources::tumbleweedTexture,
                        Rectangle{0, 0, (float)GameResources::tumbleweedTexture.width,
                                (float)GameResources::tumbleweedTexture.height},
                        Rectangle{static_cast<float>(screenX) + GAME_STATE.getCellSize()/2,
                                static_cast<float>(screenY) + GAME_STATE.getCellSize()/2,
                                GAME_STATE.getCellSize() * 0.8f,
                                GAME_STATE.getCellSize() * 0.8f},
                        Vector2{GAME_STATE.getCellSize() * 0.4f, GAME_STATE.getCellSize() * 0.4f},
                        0.0f,
                        Color{55, 41, 14, 255}  // #37290E
                    );
                }
                break;

            default:
                // Handle unexpected theme
                DrawRectangle(screenX + GAME_STATE.getCellSize()/4, screenY + GAME_STATE.getCellSize()/4, 
                            GAME_STATE.getCellSize()/2, GAME_STATE.getCellSize()/2, 
                            GAME_STATE.getCurrentColors().obstacleColor);
                break;
        }
    }
}

void InitializeGrid() {
    for (int y = 0; y < GAME_STATE.getRows(); y++) {
        for (int x = 0; x < GAME_STATE.getColumns(); x++) {
            GAME_STATE.getGrid()[y][x] = Cell(x, y);
        }
    }
}

bool IsInBounds(int x, int y) {
    return x >= 0 && x < GAME_STATE.getColumns() && y >= 0 && y < GAME_STATE.getRows();
}

Cell* GetUnvisitedNeighbor(Cell* current) {
    std::vector<int> neighbors;

    for (int i = 0; i < 4; i++) {
        int nx = current->x + directions[i][0];
        int ny = current->y + directions[i][1];

        if (IsInBounds(nx, ny) && !GAME_STATE.getGrid()[ny][nx].visited) {
            neighbors.push_back(i);
        }
    }

    if (!neighbors.empty()) {
        int randIndex = neighbors[std::rand() % neighbors.size()];
        return &GAME_STATE.getGrid()[current->y + directions[randIndex][1]][current->x + directions[randIndex][0]];
    }
    return nullptr;
}

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
    for (int y = 0; y < GAME_STATE.getRows(); ++y) {
        for (int x = 0; x < GAME_STATE.getColumns(); ++x) {
            GAME_STATE.getGrid()[y][x] = Cell(x, y);
        }
    }

    Cell* current = &GAME_STATE.getGrid()[0][0];
    current->visited = true;
    GAME_STATE.getStack().push(current);

    while (!GAME_STATE.getStack().empty()) {
        current = GAME_STATE.getStack().top();
        Cell* next = GetUnvisitedNeighbor(current);

        if (next) {
            next->visited = true;
            RemoveWalls(current, next);
            GAME_STATE.getStack().push(next);
        } else {
            GAME_STATE.getStack().pop();
        }
    }
}

void DrawBorder() {
    int borderThickness = 5; // Thickness of the border
    Color borderColor = GAME_STATE.getCurrentColors().wallColor;

    // Top border
    DrawRectangle(GAME_STATE.getMargin() - borderThickness, GAME_STATE.getMargin() - borderThickness, GAME_STATE.getMazeWidth() + 2 * borderThickness, borderThickness, borderColor);

    // Bottom border
    DrawRectangle(GAME_STATE.getMargin() - borderThickness, GAME_STATE.getMargin() + GAME_STATE.getMazeHeight(), GAME_STATE.getMazeWidth() + 2 * borderThickness, borderThickness, borderColor);

    // Left border
    DrawRectangle(GAME_STATE.getMargin() - borderThickness, GAME_STATE.getMargin() - borderThickness, borderThickness, GAME_STATE.getMazeHeight() + 2 * borderThickness, borderColor);

    // Right border
    DrawRectangle(GAME_STATE.getMargin() + GAME_STATE.getMazeWidth(), GAME_STATE.getMargin() - borderThickness, borderThickness, GAME_STATE.getMazeHeight() + 2 * borderThickness, borderColor);
}

void PlaceRandomExit() {
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Keep exit away from start position (minimum distance)
    const int MIN_DISTANCE = std::max(GAME_STATE.getColumns(), GAME_STATE.getRows()) / 2;
    
    while (true) {
        // Generate random position
        GAME_STATE.setExitX(std::uniform_int_distribution<>(0, GAME_STATE.getColumns()-1)(gen));
        GAME_STATE.setExitY(std::uniform_int_distribution<>(0, GAME_STATE.getRows()-1)(gen));
        
        // Calculate distance from start
        int distance = std::abs(GAME_STATE.getExitX() - 0) + std::abs(GAME_STATE.getExitY() - 0);
        
        // Check if position is valid
        if (distance >= MIN_DISTANCE && !GAME_STATE.getGrid()[GAME_STATE.getExitY()][GAME_STATE.getExitX()].isObstacle) {
            // Verify path exists from start to this exit
            if (PathExists(0, 0, GAME_STATE.getExitX(), GAME_STATE.getExitY())) {
                return;
            }
        }
    }
}

void DrawExit(int exitX, int exitY, float time) {
    static PortalEffect portalEffect;
    
    int exitPosX = GAME_STATE.getMargin() + exitX * GAME_STATE.getCellSize() + GAME_STATE.getCellSize()/2;
    int exitPosY = GAME_STATE.getMargin() + exitY * GAME_STATE.getCellSize() + GAME_STATE.getCellSize()/2;
    float radius = GAME_STATE.getCellSize()/2;

    float scale{};

    // Draw theme-specific goal
    switch(GAME_STATE.getCurrentTheme()) {
        case GameTheme::Space:
            // Update portal effect position
            portalEffect.SetPosition({static_cast<float>(exitPosX), static_cast<float>(exitPosY)}, radius);
            
            // Update and draw particles
            portalEffect.Update(GetFrameTime());

            // Draw space portal with special effects
            scale = 1.0f + 0.2f * sin(time * 5.0f);
            for(int i = 3; i > 0; i--) {
                DrawCircle(exitPosX, exitPosY, radius * scale * i/3, 
                          ColorAlpha(GAME_STATE.getCurrentColors().goalColor, (1.0f - (float)i/4.0f)));
            }
            portalEffect.Draw();
            break;
            
        case GameTheme::Jungle:
            // Rotating bananas
            DrawTexturePro(
                GetGoalTextureForTheme(GAME_STATE.getCurrentTheme()),
                Rectangle{ 0, 0, 
                          (float)GetGoalTextureForTheme(GAME_STATE.getCurrentTheme()).width,
                          (float)GetGoalTextureForTheme(GAME_STATE.getCurrentTheme()).height },
                Rectangle{ 
                    static_cast<float>(exitPosX),
                    static_cast<float>(exitPosY),
                    GAME_STATE.getCellSize() * 0.8f,
                    GAME_STATE.getCellSize() * 0.8f
                },
                Vector2{ GAME_STATE.getCellSize() * 0.4f, GAME_STATE.getCellSize() * 0.4f },
                time * 45.0f,  // Rotate bananas
                WHITE
            );
            break;

        case GameTheme::Desert:
            // Draw shadow underneath
            DrawCircleGradient(
                exitPosX,
                exitPosY + GAME_STATE.getCellSize() * 0.1f,
                GAME_STATE.getCellSize() * 0.7f,
                ColorAlpha(BLACK, 0.3f),
                ColorAlpha(BLACK, 0.0f)
            );

            // Draw glow effect
            for(int i = 3; i > 0; i--) {
                DrawCircle(exitPosX, exitPosY, 
                          GAME_STATE.getCellSize() * 1.2f * 0.6f * i/3,
                          ColorAlpha(SKYBLUE, 0.1f * (1.0f - (float)i/4.0f)));
            }

            // Draw oasis with slight vertical offset for 3D effect
            DrawTexturePro(
                GetGoalTextureForTheme(GAME_STATE.getCurrentTheme()),
                Rectangle{ 0, 0, 
                          (float)GetGoalTextureForTheme(GAME_STATE.getCurrentTheme()).width,
                          (float)GetGoalTextureForTheme(GAME_STATE.getCurrentTheme()).height },
                Rectangle{ 
                    static_cast<float>(exitPosX),
                    static_cast<float>(exitPosY - GAME_STATE.getCellSize() * 0.1f), // Slight vertical offset
                    GAME_STATE.getCellSize() * 1.2f,
                    GAME_STATE.getCellSize() * 1.2f
                },
                Vector2{ GAME_STATE.getCellSize() * 1.2f/2, GAME_STATE.getCellSize() * 1.2f/2 },
                0.0f,
                WHITE
            );

            // Add highlight/reflection effect
            DrawCircleGradient(
                exitPosX,
                exitPosY,
                GAME_STATE.getCellSize() * 0.4f,
                ColorAlpha(SKYBLUE, 0.2f),
                ColorAlpha(SKYBLUE, 0.0f)
            );
            break;
        default:
            DrawCircle(exitPosX, exitPosY, radius, GAME_STATE.getCurrentColors().goalColor);
            break;
    }
}

// Helper function to calculate Manhattan distance (heuristic)
int Heuristic(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

// Check if there is a path from start to end
bool PathExists(int startX, int startY, int endX, int endY) {
    // Priority queue for A* frontier (min-heap)
    using Node = std::pair<int, std::pair<int, int>>; // <cost, <x, y>>
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> frontier;

    // Costs and visited grids
    std::vector<std::vector<int>> cost(GAME_STATE.getRows(), std::vector<int>(GAME_STATE.getColumns(), std::numeric_limits<int>::max()));
    std::vector<std::vector<bool>> visited(GAME_STATE.getRows(), std::vector<bool>(GAME_STATE.getColumns(), false));

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
            if (nx >= 0 && nx < GAME_STATE.getColumns() && ny >= 0 && ny < GAME_STATE.getRows() &&
                !GAME_STATE.getGrid()[ny][nx].isObstacle && !visited[ny][nx] &&
                !GAME_STATE.getGrid()[y][x].walls[i]) {
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

void AddRandomObstacles(int exitX, int exitY) {
    std::random_device rd;
    std::mt19937 gen(rd());

    // Separate distributions for different sprite sheets
    std::uniform_int_distribution<> asteroidRowDist(0, 1);      // 2 GAME_STATE.getRows() for asteroids
    std::uniform_int_distribution<> asteroidColDist(0, 15);     // 16 columns for asteroids
    std::uniform_int_distribution<> logRowDist(0, 2);           // 3 GAME_STATE.getRows() for logs
    std::uniform_int_distribution<> logColDist(0, 2);           // 3 columns for logs
    std::uniform_int_distribution<> obstacleDist(0, 1);         // For desert obstacle type

    // Determine number of obstacles (10-15% of cells)
    int maxObstacles = std::max(1, static_cast<int>(GAME_STATE.getRows() * GAME_STATE.getColumns() * GAME_STATE.getObstaclePercentage()));
    int obstacleCount = 0;

    while (obstacleCount < maxObstacles) {
        int x = std::rand() % GAME_STATE.getColumns();
        int y = std::rand() % GAME_STATE.getRows();

        // Avoid placing obstacles at start, exit, or already obstacle cells
        if ((x == 0 && y == 0) || (x == exitX && y == exitY) || GAME_STATE.getGrid()[y][x].isObstacle) {
            continue;
        }

        // Temporarily set as obstacle
        GAME_STATE.getGrid()[y][x].isObstacle = true;

        // Set sprite coordinates based on theme
        if (GAME_STATE.getCurrentTheme() == GameTheme::Space) {
            GAME_STATE.getGrid()[y][x].spriteRow = asteroidRowDist(gen);
            GAME_STATE.getGrid()[y][x].spriteCol = asteroidColDist(gen);
        } else if (GAME_STATE.getCurrentTheme() == GameTheme::Jungle) {
            GAME_STATE.getGrid()[y][x].logSpriteRow = logRowDist(gen);
            GAME_STATE.getGrid()[y][x].logSpriteCol = logColDist(gen);
        } else if (GAME_STATE.getCurrentTheme() == GameTheme::Desert) {
            GAME_STATE.getGrid()[y][x].desertObstacleType = obstacleDist(gen) == 0 ? 
                DesertObstacleType::Cactus : DesertObstacleType::Tumbleweed;
        }

        // Check if maze is still solvable
        if (PathExists(0, 0, exitX, exitY)) {
            obstacleCount++;
        } else {
            // If not solvable, revert
            GAME_STATE.getGrid()[y][x].isObstacle = false;
        }
    }
}